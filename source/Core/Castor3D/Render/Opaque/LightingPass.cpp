#include "Castor3D/Render/Opaque/LightingPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ImageBlit.hpp>

CU_ImplementCUSmartPtr( castor3d, LightingPass )

namespace castor3d
{
	LightingPass::LightingPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, RenderDevice const & device
		, ProgressBar * progress
		, RenderTechnique const & technique
		, Texture const & depthObj
		, OpaquePassResult const & gpResult
		, ShadowMapResult const & smDirectionalResult
		, ShadowMapResult const & smPointResult
		, ShadowMapResult const & smSpotResult
		, LightPassResult const & lpResult
		, crg::ImageViewIdArray const & targetColourResult
		, crg::ImageViewIdArray const & targetDepthResult
		, SceneUbo const & sceneUbo
		, GpInfoUbo const & gpInfoUbo )
		: m_device{ device }
		, m_technique{ technique }
		, m_depthObj{ depthObj }
		, m_gpResult{ gpResult }
		, m_smDirectionalResult{ smDirectionalResult }
		, m_smPointResult{ smPointResult }
		, m_smSpotResult{ smSpotResult }
		, m_lpResult{ lpResult }
		, m_targetColourResult{ targetColourResult }
		, m_targetDepthResult{ targetDepthResult }
		, m_sceneUbo{ sceneUbo }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_group{ graph.createPassGroup( "DirectLighting" ) }
		, m_size{ makeSize( lpResult[LpTexture::eDiffuse].getExtent() ) }
		, m_lastPass{ &doCreateLightingPass( m_group
			, previousPasses
			, *technique.getRenderTarget().getScene()
			, progress ) }
	{
		m_group.addOutput( m_lpResult[LpTexture::eDiffuse].wholeViewId );
		m_group.addOutput( m_lpResult[LpTexture::eSpecular].wholeViewId );
		m_group.addOutput( m_lpResult[LpTexture::eScattering].wholeViewId );
		m_group.addOutput( m_lpResult[LpTexture::eCoatingSpecular].wholeViewId );
		m_group.addOutput( m_lpResult[LpTexture::eSheen].wholeViewId );
	}

	void LightingPass::update( CpuUpdater & updater )
	{
		if ( m_lightPass )
		{
			auto & scene = *updater.camera->getScene();
			auto & cache = scene.getLightCache();

			m_lightPass->clear();

			if ( !cache.isEmpty() )
			{
				doUpdateLightPasses( updater, LightType::eDirectional );
				doUpdateLightPasses( updater, LightType::ePoint );
				doUpdateLightPasses( updater, LightType::eSpot );
			}

			m_lightPass->resetCommandBuffer( m_technique.getTargetResult().front() );
		}
	}

	void LightingPass::update( GpuUpdater & updater )
	{
		updater.info.visibleLightsCount += m_lightPass->getEnabledLightsCount();
	}

	void LightingPass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( "Light Diffuse"
			, m_lpResult[LpTexture::eDiffuse]
			, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Light Specular"
			, m_lpResult[LpTexture::eSpecular]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Light Scattering"
			, m_lpResult[LpTexture::eScattering]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Light Clearcoat"
			, m_lpResult[LpTexture::eCoatingSpecular]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Light Sheen"
			, m_lpResult[LpTexture::eSheen]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
	}

	void LightingPass::doUpdateLightPasses( CpuUpdater & updater
		, LightType lightType )
	{
		auto & camera = *updater.camera;
		auto & scene = *updater.camera->getScene();
		auto & cache = scene.getLightCache();

		for ( auto & light : cache.getLights( lightType ) )
		{
			if ( light->getLightType() == LightType::eDirectional
				|| camera.isVisible( light->getBoundingBox(), light->getParent()->getDerivedTransformationMatrix() ) )
			{
				m_lightPass->enableLight( camera, *light );
			}
		}
	}

	crg::FramePass const & LightingPass::doCreateLightingPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, Scene const & scene
		, ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating lighting pass" );
		auto & engine = *m_device.renderSystem.getEngine();
		auto & modelBuffer = scene.getModelBuffer().getBuffer();
		auto & pass = graph.createPass( "Lighting"
			, [this, progress, &engine, &scene]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising lighting pass" );
				auto result = std::make_unique< RunnableLightingPass >( *this
					, framePass
					, context
					, runnableGraph
					, m_device
					, scene
					, m_lpResult
					, m_smDirectionalResult
					, m_smPointResult
					, m_smSpotResult
					, m_targetColourResult
					, m_targetDepthResult );
				engine.registerTimer( framePass.getFullName()
					, result->getTimer() );
				m_lightPass = result.get();
				return result;
			} );
		pass.addDependencies( previousPasses );
		engine.getMaterialCache().getPassBuffer().createPassBinding( pass
			, uint32_t( LightPassIdx::eMaterials ) );
		engine.getMaterialCache().getSssProfileBuffer().createPassBinding( pass
			, uint32_t( LightPassIdx::eSssProfiles ) );
		pass.addInputStorageBuffer( { modelBuffer, "Models" }
			, uint32_t( LightPassIdx::eModels )
			, 0u
			, uint32_t( modelBuffer.getSize() ) );
		m_gpInfoUbo.createPassBinding( pass
			, uint32_t( LightPassIdx::eGpInfo ) );
		m_sceneUbo.createPassBinding( pass
			, uint32_t( LightPassIdx::eScene ) );
		pass.addSampledView( m_depthObj.sampledViewId
			, uint32_t( LightPassIdx::eDepthObj ) );
		pass.addSampledView( m_gpResult[DsTexture::eNmlOcc].sampledViewId
			, uint32_t( LightPassIdx::eNmlOcc ) );
		pass.addSampledView( m_gpResult[DsTexture::eColMtl].sampledViewId
			, uint32_t( LightPassIdx::eColMtl ) );
		pass.addSampledView( m_gpResult[DsTexture::eSpcRgh].sampledViewId
			, uint32_t( LightPassIdx::eSpcRgh ) );
		pass.addSampledView( m_gpResult[DsTexture::eEmsTrn].sampledViewId
			, uint32_t( LightPassIdx::eEmsTrn ) );
		pass.addSampledView( m_gpResult[DsTexture::eClrCot].sampledViewId
			, uint32_t( LightPassIdx::eClrCot ) );
		pass.addSampledView( m_gpResult[DsTexture::eCrTsIr].sampledViewId
			, uint32_t( LightPassIdx::eCrTsIr ) );
		pass.addSampledView( m_gpResult[DsTexture::eSheen].sampledViewId
			, uint32_t( LightPassIdx::eSheen ) );
		auto index = uint32_t( LightPassIdx::eCount );
		engine.createSpecificsBuffersPassBindings( pass, index );

		pass.addInOutDepthStencilView( m_targetDepthResult );
		pass.addOutputColourView( m_lpResult[LpTexture::eDiffuse].targetViewId );
		pass.addOutputColourView( m_lpResult[LpTexture::eSpecular].targetViewId );
		pass.addOutputColourView( m_lpResult[LpTexture::eScattering].targetViewId );
		pass.addOutputColourView( m_lpResult[LpTexture::eCoatingSpecular].targetViewId );
		pass.addOutputColourView( m_lpResult[LpTexture::eSheen].targetViewId );

		pass.addImplicitColourView( m_smDirectionalResult[SmTexture::eLinearDepth].wholeViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		pass.addImplicitColourView( m_smDirectionalResult[SmTexture::eVariance].wholeViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		pass.addImplicitColourView( m_smPointResult[SmTexture::eLinearDepth].wholeViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		pass.addImplicitColourView( m_smPointResult[SmTexture::eVariance].wholeViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		pass.addImplicitColourView( m_smSpotResult[SmTexture::eLinearDepth].wholeViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		pass.addImplicitColourView( m_smSpotResult[SmTexture::eVariance].wholeViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		return pass;
	}
}
