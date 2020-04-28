#include "Castor3D/Render/Technique/Opaque/DeferredRendering.hpp"

#include "Castor3D/Cache/SceneCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/PluginCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/Technique/LineariseDepthPass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"

#include <ashespp/RenderPass/FrameBuffer.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name )
		{
			SamplerSPtr result;
			auto & cache = engine.getSamplerCache();

			if ( cache.has( name ) )
			{
				result = cache.find( name );
			}
			else
			{
				result = engine.getSamplerCache().add( cuT( "TextureProjection" ) );
				result->setMinFilter( VK_FILTER_NEAREST );
				result->setMagFilter( VK_FILTER_NEAREST );
				result->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result->initialise();
			}

			return result;
		}
	}

	//*********************************************************************************************

	DeferredRendering::DeferredRendering( Engine & engine
		, OpaquePass & opaquePass
		, TextureLayoutSPtr depthTexture
		, TextureLayoutSPtr velocityTexture
		, TextureLayoutSPtr resultTexture
		, Size const & size
		, Scene & scene
		, HdrConfigUbo & hdrConfigUbo
		, SsaoConfig & config )
		: m_engine{ engine }
		, m_ssaoConfig{ config }
		, m_opaquePass{ opaquePass }
		, m_size{ size }
		, m_gpInfoUbo{ m_engine }
		, m_geometryPassResult{ m_engine, depthTexture->getTexture(), velocityTexture->getTexture() }
		, m_linearisePass{ std::make_unique< LineariseDepthPass >( m_engine
			, cuT( "Deferred" )
			, makeExtent2D( m_size )
			, m_geometryPassResult.getViews()[size_t( DsTexture::eDepth )] ) }
		, m_lightingPass{ std::make_unique< LightingPass >( m_engine
			, m_size
			, scene
			, m_geometryPassResult
			, depthTexture->getDefaultView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo ) }
		, m_ssao{ std::make_unique< SsaoPass >( m_engine
			, makeExtent2D( m_size )
			, m_ssaoConfig
			, m_linearisePass->getResult()
			, m_geometryPassResult ) }
		, m_subsurfaceScattering{ std::make_unique< SubsurfaceScatteringPass >( m_engine
			, m_gpInfoUbo
			, m_opaquePass.getSceneUbo()
			, m_size
			, m_geometryPassResult
			, m_lightingPass->getDiffuse() ) }
	{
		m_resolve.emplace_back( std::make_unique< ReflectionPass >( m_engine
			, scene
			, m_geometryPassResult
			, m_lightingPass->getDiffuse().getTexture()->getDefaultView()
			, m_lightingPass->getSpecular().getTexture()->getDefaultView()
			, resultTexture->getDefaultView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo
			, hdrConfigUbo
			, m_ssaoConfig.enabled ? &m_ssao->getResult().getTexture()->getDefaultView() : nullptr ) );
		m_resolve.emplace_back( std::make_unique< ReflectionPass >( m_engine
			, scene
			, m_geometryPassResult
			, m_subsurfaceScattering->getResult().getTexture()->getDefaultView()
			, m_lightingPass->getSpecular().getTexture()->getDefaultView()
			, resultTexture->getDefaultView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo
			, hdrConfigUbo
			, m_ssaoConfig.enabled ? &m_ssao->getResult().getTexture()->getDefaultView() : nullptr ) );
	}

	DeferredRendering::~DeferredRendering()
	{
		m_results.clear();
		m_reflection.clear();
		m_subsurfaceScattering.reset();
		m_lightingPass.reset();
	}

	void DeferredRendering::update( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, castor::Point2f const & jitter )
	{
		auto invView = camera.getView().getInverse().getTransposed();
		auto invProj = camera.getProjection().getInverse();
		auto invViewProj = ( camera.getProjection() * camera.getView() ).getInverse();
		m_gpInfoUbo.update( m_size
			, camera
			, invViewProj
			, invView
			, invProj );
		m_opaquePass.getSceneUbo().update( scene, &camera );
		m_opaquePass.update( info, jitter );
			|| m_ssgiConfig.enabled )
		m_lightingPass->update( info, scene, camera, jitter );

		if ( m_ssaoConfig.enabled )
		{
			m_ssao->update( camera );
		}

		if ( scene.needsSubsurfaceScattering() )
		{
			m_reflection[1]->update( camera );
		}
		else
		{
			m_reflection[0]->update( camera );
		}
	}

	ashes::Semaphore const & DeferredRendering::render( Scene const & scene
		, Camera const & camera
		, ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;
		m_engine.setPerObjectLighting( false );
		result = &m_opaquePass.render( *result );
		result = &m_lightingPass->render( scene
			, camera
			, m_geometryPassResult
			, *result );

		if ( m_ssaoConfig.enabled
		{
			result = &m_linearisePass->linearise( *result );
		}

		if ( m_ssaoConfig.enabled )
		{
			result = &m_ssao->render( *result );
		}

		if ( scene.needsSubsurfaceScattering() )
		{
			result = &m_subsurfaceScattering->render( *result );
			result = &m_reflection[1]->render( *result );
		}
		else
		{
			result = &m_reflection[0]->render( *result );
		}

		return *result;
	}

	void DeferredRendering::debugDisplay( ashes::RenderPass const & renderPass
		, ashes::FrameBuffer const & frameBuffer )const
	{
		//auto count = 8 + ( m_ssaoConfig.m_enabled ? 1 : 0 );
		//int width = int( m_size.getWidth() ) / count;
		//int height = int( m_size.getHeight() ) / count;
		//auto size = Size( width, height );
		//auto & context = *m_engine.getRenderSystem()->getCurrentContext();
		//auto index = 0;
		//context.renderDepth( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eDepth )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData1 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData2 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData3 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData4 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData5 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_lightingPass->getDiffuse().getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_lightingPass->getSpecular().getTexture() );

		//if ( m_ssaoConfig.m_enabled )
		//{
		//	context.renderTexture( Position{ width * ( index++ ), 0 }, size, m_reflection->getSsao() );
		//}

		//m_subsurfaceScattering->debugDisplay( m_size );
	}

	void DeferredRendering::accept( RenderTechniqueVisitor & visitor )
	{
		m_opaquePass.accept( visitor );
		m_lightingPass->accept( visitor );

		if ( m_ssaoConfig.enabled
		{
			m_linearisePass->accept( visitor );
		}

		if ( m_ssaoConfig.enabled )
		{
			m_ssao->accept( visitor );
		}

		if ( visitor.getScene().needsSubsurfaceScattering() )
		{
			m_subsurfaceScattering->accept( visitor );
			m_reflection[1]->accept( visitor );
		}
		else
		{
			m_reflection[0]->accept( visitor );
		}
	}
}
