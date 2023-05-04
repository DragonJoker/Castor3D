#include "Castor3D/Render/Opaque/LightingPass.hpp"

#include "Castor3D/Config.hpp"
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
#include "Castor3D/Render/Clustered/FrustumClusters.hpp"
#include "Castor3D/Render/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ImageBlit.hpp>

CU_ImplementSmartPtr( castor3d, LightingPass )

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
		, CameraUbo const & cameraUbo )
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
		, m_cameraUbo{ cameraUbo }
		, m_group{ graph.createPassGroup( "DirectLighting" ) }
		, m_size{ makeSize( lpResult[LpTexture::eDiffuse].getExtent() ) }
		, m_lastPass{ &doCreateLightingPass( m_group
			, previousPasses
			, *technique.getRenderTarget().getScene()
			, progress ) }
	{
		m_group.addGroupOutput( m_lpResult[LpTexture::eDiffuse].wholeViewId );
		m_group.addGroupOutput( m_lpResult[LpTexture::eSpecular].wholeViewId );
		m_group.addGroupOutput( m_lpResult[LpTexture::eScattering].wholeViewId );
	}

	void LightingPass::update( CpuUpdater & updater )
	{
		if ( m_lightPass && m_technique.isOpaqueEnabled() )
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
	}

	bool LightingPass::isEnabled()const
	{
		return m_lightPass
			&& m_lightPass->hasEnabledLights()
			&& m_technique.isOpaqueEnabled();
	}

	void LightingPass::doUpdateLightPasses( CpuUpdater & updater
		, LightType lightType )
	{
		auto & camera = *updater.camera;
		auto & scene = *updater.camera->getScene();
		auto & cache = scene.getLightCache();
		m_lightPass->updateCamera( camera );

		for ( auto & light : cache.getLights( lightType ) )
		{
			if ( light->getLightType() == LightType::eDirectional
				|| camera.isVisible( light->getBoundingBox(), light->getParent()->getDerivedTransformationMatrix() ) )
			{
				m_lightPass->enableLight( *light );
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
			, [this, progress, &engine]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising lighting pass" );
				auto result = std::make_unique< RunnableLightingPass >( *this
					, framePass
					, context
					, runnableGraph
					, m_device
					, m_technique.getRenderTarget()
					, m_lpResult
					, m_smDirectionalResult
					, m_smPointResult
					, m_smSpotResult
					, m_targetColourResult
					, m_targetDepthResult
					, C3D_UseClusteredRendering != 0 );
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
		pass.addInputStorageBuffer( { modelBuffer, "C3D_Models" }
			, uint32_t( LightPassIdx::eModels )
			, 0u
			, uint32_t( modelBuffer.getSize() ) );
		m_cameraUbo.createPassBinding( pass
			, uint32_t( LightPassIdx::eCamera ) );
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
		auto index = uint32_t( LightPassIdx::eCount );
		engine.createSpecificsBuffersPassBindings( pass, index );

		pass.addInOutDepthStencilView( m_targetDepthResult );
		pass.addOutputColourView( m_lpResult[LpTexture::eDiffuse].targetViewId );
		pass.addOutputColourView( m_lpResult[LpTexture::eSpecular].targetViewId );
		pass.addOutputColourView( m_lpResult[LpTexture::eScattering].targetViewId );

		return pass;
	}
}
