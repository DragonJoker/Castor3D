#include "Castor3D/Technique/Opaque/DeferredRendering.hpp"

#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Texture/Sampler.hpp"

#include <Ashes/RenderPass/FrameBuffer.hpp>
#include <Ashes/RenderPass/FrameBufferAttachment.hpp>

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
				result->setMinFilter( ashes::Filter::eNearest );
				result->setMagFilter( ashes::Filter::eNearest );
				result->setWrapS( ashes::WrapMode::eClampToEdge );
				result->setWrapT( ashes::WrapMode::eClampToEdge );
				result->setWrapR( ashes::WrapMode::eClampToEdge );
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
		, m_gpInfoUbo{ engine }
		, m_geometryPassResult{ engine, depthTexture->getTexture(), velocityTexture->getTexture() }
	{
		auto & renderSystem = *engine.getRenderSystem();
		m_opaquePass.initialiseRenderPass( m_geometryPassResult );

		m_lightingPass = std::make_unique< LightingPass >( engine
			, m_size
			, scene
			, m_geometryPassResult
			, m_opaquePass
			, depthTexture->getDefaultView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo );
		m_ssao = std::make_unique< SsaoPass >( engine
			, ashes::Extent2D{ m_size.getWidth(), m_size.getHeight() }
			, m_ssaoConfig
			, m_geometryPassResult );
		m_subsurfaceScattering = std::make_unique< SubsurfaceScatteringPass >( engine
			, m_gpInfoUbo
			, m_opaquePass.getSceneUbo()
			, m_size
			, m_geometryPassResult
			, m_lightingPass->getDiffuse() );
		m_reflection.emplace_back( std::make_unique< ReflectionPass >( engine
			, scene
			, m_geometryPassResult
			, m_lightingPass->getDiffuse().getTexture()->getDefaultView()
			, m_lightingPass->getSpecular().getTexture()->getDefaultView()
			, resultTexture->getDefaultView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo
			, hdrConfigUbo
			, m_ssaoConfig.enabled ? &m_ssao->getResult().getTexture()->getDefaultView() : nullptr ) );
		m_reflection.emplace_back( std::make_unique< ReflectionPass >( engine
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
		, Point2r const & jitter )
	{
		auto invView = camera.getView().getInverse().getTransposed();
		auto invProj = camera.getProjection().getInverse();
		auto invViewProj = ( camera.getProjection() * camera.getView() ).getInverse();
		m_opaquePass.getSceneUbo().update( scene, &camera );
		m_gpInfoUbo.update( m_size
			, camera
			, invViewProj
			, invView
			, invProj );
		m_opaquePass.update( info
			, jitter );

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

	ashes::Semaphore const & DeferredRendering::render( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;
		m_engine.setPerObjectLighting( false );
		result = &m_opaquePass.render( *result );
		result = &m_lightingPass->render( scene
			, camera
			, m_geometryPassResult
			, *result
			, info );

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
