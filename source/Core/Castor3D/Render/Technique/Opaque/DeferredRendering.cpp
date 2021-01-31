#include "Castor3D/Render/Technique/Opaque/DeferredRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SceneCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/PluginCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

#include <ashespp/RenderPass/FrameBuffer.hpp>

CU_ImplementCUSmartPtr( castor3d, DeferredRendering )

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		SamplerSPtr doCreateSampler( Engine & engine
			, RenderDevice const & device
			, castor::String const & name )
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
				result->initialise( device );
			}

			return result;
		}

		TextureLayoutSPtr createTexture( Engine & engine
			, RenderDevice const & device
			, castor::String const & name
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
			texture->initialise( device );
			return texture;
		}
	}

	//*********************************************************************************************

	DeferredRendering::DeferredRendering( Engine & engine
		, RenderDevice const & device
		, OpaquePass & opaquePass
		, TextureUnit const & depthTexture
		, TextureUnit const & velocityTexture
		, TextureUnit const & resultTexture
		, ShadowMapResult const & smDirectionalResult
		, ShadowMapResult const & smPointResult
		, ShadowMapResult const & smSpotResult
		, LightVolumePassResult const & lpvResult
		, LightVolumePassResultArray const & llpvResult
		, TextureUnit const & voxels
		, castor::Size const & size
		, Scene & scene
		, HdrConfigUbo const & hdrConfigUbo
		, GpInfoUbo const & gpInfoUbo
		, LpvGridConfigUbo const & lpvConfigUbo
		, LayeredLpvGridConfigUbo const & llpvConfigUbo
		, VoxelizerUbo const & vctConfigUbo
		, SsaoConfig & ssaoConfig )
		: m_engine{ engine }
		, m_scene{ scene }
		, m_device{ device }
		, m_ssaoConfig{ ssaoConfig }
		, m_opaquePass{ opaquePass }
		, m_size{ size }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_opaquePassResult{ m_engine
			, device
			, depthTexture
			, velocityTexture }
		, m_linearisePass{ castor::makeUnique< LineariseDepthPass >( m_engine
			, cuT( "Deferred" )
			, m_size
			, depthTexture.getTexture()->getDefaultView().getSampledView() ) }
		, m_ssao{ castor::makeUnique< SsaoPass >( m_engine
			, m_size
			, m_ssaoConfig
			, m_linearisePass->getResult()
			, m_opaquePassResult
			, m_gpInfoUbo ) }
		, m_lightingPass{ std::make_unique< LightingPass >( m_engine
			, m_device
			, m_size
			, scene
			, m_opaquePassResult
			, smDirectionalResult
			, smPointResult
			, smSpotResult
			, lpvResult
			, llpvResult
			, voxels
			, depthTexture.getTexture()->getDefaultView().getTargetView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo
			, lpvConfigUbo
			, llpvConfigUbo
			, vctConfigUbo ) }
		, m_subsurfaceScattering{ castor::makeUnique< SubsurfaceScatteringPass >( m_engine
			, m_device
			, m_gpInfoUbo
			, m_opaquePass.getSceneUbo()
			, m_size
			, m_opaquePassResult
			, m_lightingPass->getResult() ) }
		, m_resolve{ castor::makeUnique< OpaqueResolvePass >( m_engine
			, m_device
			, scene
			, m_opaquePassResult
			, *m_ssao
			, m_subsurfaceScattering->getResult()
			, m_lightingPass->getResult()[LpTexture::eDiffuse]
			, m_lightingPass->getResult()[LpTexture::eSpecular]
			, m_lightingPass->getResult()[LpTexture::eIndirectDiffuse]
			, m_lightingPass->getResult()[LpTexture::eIndirectSpecular]
			, resultTexture
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo
			, hdrConfigUbo ) }
	{
		m_opaquePass.initialiseRenderPass( device, m_opaquePassResult );

		if ( m_ssaoConfig.enabled )
		{
			m_linearisePass->initialise( m_device );
			m_ssao->initialise( m_device );
		}

		if ( scene.needsSubsurfaceScattering() )
		{
			m_subsurfaceScattering->initialise( m_device );
		}

		m_resolve->initialise();
	}

	DeferredRendering::~DeferredRendering()
	{
		m_resolve->cleanup();
		m_subsurfaceScattering->cleanup( m_device );
		m_ssao->cleanup( m_device );
		m_linearisePass->cleanup( m_device );
	}

	void DeferredRendering::update( CpuUpdater & updater )
	{
		m_opaquePass.update( updater );
		m_lightingPass->update( updater );

		if ( m_ssaoConfig.enabled )
		{
			m_linearisePass->update( updater );
		}

		if ( m_ssaoConfig.enabled )
		{
			m_ssao->update( updater );
		}
	}

	void DeferredRendering::update( GpuUpdater & updater )
	{
		if ( m_ssaoConfig.enabled )
		{
			m_linearisePass->initialise( m_device );
			m_ssao->initialise( m_device );
		}

		if ( m_scene.needsSubsurfaceScattering() )
		{
			m_subsurfaceScattering->initialise( m_device );
		}

		m_opaquePass.update( updater );
		m_lightingPass->update( updater );
		m_resolve->update( updater );

		if ( m_ssaoConfig.enabled )
		{
			m_linearisePass->update( updater );
		}
	}

	ashes::Semaphore const & DeferredRendering::render( RenderDevice const & device
		, Scene const & scene
		, Camera const & camera
		, ashes::Semaphore const & toWait )
	{
		ashes::Semaphore const * result = &toWait;
		result = &m_opaquePass.render( device, *result );

		if ( m_ssaoConfig.enabled )
		{
			result = &m_linearisePass->linearise( device, *result );
		}

		if ( m_ssaoConfig.enabled )
		{
			result = &m_ssao->render( *result );
		}

		result = &m_lightingPass->render( scene
			, camera
			, m_opaquePassResult
			, *result );

		if ( scene.needsSubsurfaceScattering() )
		{
			result = &m_subsurfaceScattering->render( device, *result );
		}

		result = &m_resolve->render( *result );
		return *result;
	}

	void DeferredRendering::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Opaque Data1"
			, m_opaquePassResult[DsTexture::eData1].getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		visitor.visit( "Opaque Data2"
			, m_opaquePassResult[DsTexture::eData2].getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		visitor.visit( "Opaque Data3"
			, m_opaquePassResult[DsTexture::eData3].getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		visitor.visit( "Opaque Data4"
			, m_opaquePassResult[DsTexture::eData4].getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		visitor.visit( "Opaque Data5"
			, m_opaquePassResult[DsTexture::eData5].getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

		m_opaquePass.accept( visitor );

		if ( m_ssaoConfig.enabled
			|| visitor.config.forceSubPassesVisit )
		{
			m_linearisePass->accept( visitor );
			m_ssao->accept( visitor );
		}

		m_lightingPass->accept( visitor );

		if ( visitor.getScene().needsSubsurfaceScattering()
			|| visitor.config.forceSubPassesVisit )
		{
			m_subsurfaceScattering->accept( visitor );
		}

		m_resolve->accept( visitor );
	}
}
