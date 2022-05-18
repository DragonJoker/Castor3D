#include "Castor3D/Render/Technique/Opaque/LightingPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
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
		, crg::FramePass const *& previousPass
		, RenderDevice const & device
		, ProgressBar * progress
		, Scene & scene
		, Texture const & depth
		, OpaquePassResult const & gpResult
		, ShadowMapResult const & smDirectionalResult
		, ShadowMapResult const & smPointResult
		, ShadowMapResult const & smSpotResult
		, LightPassResult const & lpResult
		, SceneUbo const & sceneUbo
		, GpInfoUbo const & gpInfoUbo )
		: m_device{ device }
		, m_depth{ depth }
		, m_gpResult{ gpResult }
		, m_smDirectionalResult{ smDirectionalResult }
		, m_smPointResult{ smPointResult }
		, m_smSpotResult{ smSpotResult }
		, m_lpResult{ lpResult }
		, m_sceneUbo{ sceneUbo }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_group{ graph.createPassGroup( "DirectLighting" ) }
		, m_size{ makeSize( lpResult[LpTexture::eDiffuse].getExtent() ) }
	{
		previousPass = &doCreateDepthBlitPass( m_group
			, *previousPass
			, progress );
		previousPass = &doCreateLightingPass( m_group
			, *previousPass
			, scene
			, progress );
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

			m_lightPass->resetCommandBuffer();
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
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Light Specular"
			, m_lpResult[LpTexture::eSpecular]
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
			else
			{
				m_lightPass->disableLight( camera, *light );
			}
		}
	}

	crg::FramePass const & LightingPass::doCreateDepthBlitPass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, ProgressBar * progress )
	{
		stepProgressBar( progress, "Creating depth blit pass" );
		auto & engine = *m_device.renderSystem.getEngine();
		auto srcSize = m_depth.getExtent();
		auto dstSize = makeExtent3D( m_size );
		auto & pass = graph.createPass( "DepthBlit"
			, [this, progress, srcSize, dstSize, &engine]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				stepProgressBar( progress, "Initialising depth blit pass" );
				auto result = std::make_unique< crg::ImageBlit >( framePass
					, context
					, runGraph
					, VkOffset3D{}
					, srcSize
					, VkOffset3D{}
					, dstSize
					, VK_FILTER_NEAREST
					, crg::ru::Config{}
					, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
					, crg::RunnablePass::IsEnabledCallback( [this](){ return isEnabled(); } ) );
				engine.registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( previousPass );
		pass.addTransferInputView( m_depth.wholeViewId );
		pass.addTransferOutputView( m_lpResult[LpTexture::eDepth].wholeViewId );
		return pass;
	}

	crg::FramePass const & LightingPass::doCreateLightingPass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
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
					, m_smSpotResult );
				engine.registerTimer( framePass.getFullName()
					, result->getTimer() );
				m_lightPass = result.get();
				return result;
			} );
		pass.addDependency( previousPass );
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
		pass.addSampledView( m_gpResult[DsTexture::eData0].sampledViewId
			, uint32_t( LightPassIdx::eData0 ) );
		pass.addSampledView( m_gpResult[DsTexture::eData1].sampledViewId
			, uint32_t( LightPassIdx::eData1 ) );
		pass.addSampledView( m_gpResult[DsTexture::eData2].sampledViewId
			, uint32_t( LightPassIdx::eData2 ) );
		pass.addSampledView( m_gpResult[DsTexture::eData3].sampledViewId
			, uint32_t( LightPassIdx::eData3 ) );
		pass.addSampledView( m_gpResult[DsTexture::eData4].sampledViewId
			, uint32_t( LightPassIdx::eData4 ) );

		pass.addInOutDepthStencilView( m_lpResult[LpTexture::eDepth].targetViewId );
		pass.addOutputColourView( m_lpResult[LpTexture::eDiffuse].targetViewId );
		pass.addOutputColourView( m_lpResult[LpTexture::eSpecular].targetViewId );
		return pass;
	}
}
