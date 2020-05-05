#include "Castor3D/Render/Technique/Opaque/DeferredRendering.hpp"

#include "Castor3D/Cache/SceneCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/PluginCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/Ssgi/SsgiConfig.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

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

		TextureLayoutSPtr createTexture( Engine & engine
			, String const & name
			, TextureLayout const & source )
		{
			auto & renderSystem = *engine.getRenderSystem();
			ashes::ImageCreateInfo image
			{
				0u,
				source.getType(),
				source.getPixelFormat(),
				source.getDimensions(),
				source.getMipmapCount(),
				source.getLayersCount(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			};
			auto texture = std::make_shared< TextureLayout >( renderSystem
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, name );
			texture->getDefaultImage().initialiseSource();
			texture->initialise();
			return texture;
		}

		size_t getIndex( SsaoConfig const & ssaoConfig
			, Scene const & scene )
		{
			return 2u * ( ssaoConfig.enabled ? 1u : 0u )
				+ ( scene.needsSubsurfaceScattering() ? 1u : 0u );
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
		, SsaoConfig & ssaoConfig
		, SsgiConfig & ssgiConfig )
		: m_engine{ engine }
		, m_ssaoConfig{ ssaoConfig }
		, m_ssgiConfig{ ssgiConfig }
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
		, m_ssgi{ std::make_unique< SsgiPass >( m_engine
			, makeExtent2D( m_size )
			, m_ssaoConfig
			, m_ssgiConfig
			, *m_linearisePass->getResult().getTexture()
			, resultTexture ) }
	{
		//SSAO Off
		//	SSSSS Off
		m_resolve.emplace_back( std::make_unique< OpaqueResolvePass >( m_engine
			, scene
			, m_geometryPassResult
			, m_lightingPass->getDiffuse().getTexture()->getDefaultView()
			, m_lightingPass->getSpecular().getTexture()->getDefaultView()
			, resultTexture->getDefaultView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo
			, hdrConfigUbo
			, nullptr ) );
		//	SSSSS On
		m_resolve.emplace_back( std::make_unique< OpaqueResolvePass >( m_engine
			, scene
			, m_geometryPassResult
			, m_subsurfaceScattering->getResult().getTexture()->getDefaultView()
			, m_lightingPass->getSpecular().getTexture()->getDefaultView()
			, resultTexture->getDefaultView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo
			, hdrConfigUbo
			, nullptr ) );
		//SSAO On
		//	SSSSS Off
		m_resolve.emplace_back( std::make_unique< OpaqueResolvePass >( m_engine
			, scene
			, m_geometryPassResult
			, m_lightingPass->getDiffuse().getTexture()->getDefaultView()
			, m_lightingPass->getSpecular().getTexture()->getDefaultView()
			, resultTexture->getDefaultView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo
			, hdrConfigUbo
			, &m_ssao->getResult().getTexture()->getDefaultView() ) );
		//	SSSSS On
		m_resolve.emplace_back( std::make_unique< OpaqueResolvePass >( m_engine
			, scene
			, m_geometryPassResult
			, m_subsurfaceScattering->getResult().getTexture()->getDefaultView()
			, m_lightingPass->getSpecular().getTexture()->getDefaultView()
			, resultTexture->getDefaultView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo
			, hdrConfigUbo
			, &m_ssao->getResult().getTexture()->getDefaultView() ) );
		m_opaquePass.initialiseRenderPass( m_geometryPassResult );
	}

	DeferredRendering::~DeferredRendering()
	{
		m_results.clear();
		m_resolve.clear();
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

		if ( m_ssaoConfig.enabled
			|| m_ssgiConfig.enabled )
		{
			m_linearisePass->update( camera.getViewport() );
		}

		m_lightingPass->update( info, scene, camera, jitter );

		if ( m_ssaoConfig.enabled )
		{
			m_ssao->update( camera );
		}

		auto index = getIndex( m_ssaoConfig, scene );
		m_resolve[index]->update( camera );

		if ( m_ssgiConfig.enabled )
		{
			m_ssgi->update( camera );
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
			|| m_ssgiConfig.enabled )
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
		}

		auto index = getIndex( m_ssaoConfig, scene );
		result = &m_resolve[index]->render( *result );

		if ( m_ssgiConfig.enabled )
		{
			result = &m_ssgi->render( *result );
		}

		return *result;
	}

	void DeferredRendering::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Opaque Data1", m_geometryPassResult.getViews()[size_t( DsTexture::eData1 )] );
		visitor.visit( "Opaque Data2", m_geometryPassResult.getViews()[size_t( DsTexture::eData2 )] );
		visitor.visit( "Opaque Data3", m_geometryPassResult.getViews()[size_t( DsTexture::eData3 )] );
		visitor.visit( "Opaque Data4", m_geometryPassResult.getViews()[size_t( DsTexture::eData4 )] );
		visitor.visit( "Opaque Data5", m_geometryPassResult.getViews()[size_t( DsTexture::eData5 )] );

		m_opaquePass.accept( visitor );
		m_lightingPass->accept( visitor );

		if ( m_ssaoConfig.enabled
			|| m_ssgiConfig.enabled
			|| visitor.forceSubPassesVisit )
		{
			m_linearisePass->accept( visitor );
		}

		if ( m_ssaoConfig.enabled
			|| visitor.forceSubPassesVisit )
		{
			m_ssao->accept( visitor );
		}

		if ( visitor.getScene().needsSubsurfaceScattering()
			|| visitor.forceSubPassesVisit )
		{
			m_subsurfaceScattering->accept( visitor );
		}

		auto index = getIndex( m_ssaoConfig, visitor.getScene() );
		m_resolve[index]->accept( visitor );

		if ( m_ssgiConfig.enabled
			|| visitor.forceSubPassesVisit )
		{
			m_ssgi->accept( visitor );
		}
	}
}
