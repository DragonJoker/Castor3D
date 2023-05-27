#include "Castor3D/Render/Opaque/DeferredRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/PluginCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Opaque/IndirectLightingPass.hpp"
#include "Castor3D/Render/Opaque/LightingPass.hpp"
#include "Castor3D/Render/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Opaque/OpaqueResolvePass.hpp"
#include "Castor3D/Render/Opaque/Lighting/SubsurfaceScatteringPass.hpp"
#include "Castor3D/Render/Passes/ForwardRenderTechniquePass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/LightingModelFactory.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>

CU_ImplementSmartPtr( castor3d, DeferredRendering )

namespace castor3d
{
	DeferredRendering::DeferredRendering( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, crg::FramePass const & ssaoPass
		, RenderDevice const & device
		, ProgressBar * progress
		, Texture const & brdf
		, crg::ImageViewIdArray const & resultDepth
		, Texture const & depthObj
		, OpaquePassResult const & opaquePassResult
		, crg::ImageViewIdArray resultTexture
		, ShadowMapResult const & smDirectionalResult
		, ShadowMapResult const & smPointResult
		, ShadowMapResult const & smSpotResult
		, IndirectLightingData const & indirect
		, Texture const & ssao
		, castor::Size const & size
		, RenderTechnique & technique
		, CameraUbo const & cameraUbo
		, SceneUbo const & sceneUbo
		, HdrConfigUbo const & hdrConfigUbo
		, SsaoConfig & ssaoConfig
		, crg::RunnablePass::IsEnabledCallback const & opaquePassEnabled )
		: m_device{ device }
		, m_opaquePassResult{ opaquePassResult }
		, m_technique{ technique }
		, m_size{ size }
		, m_lightPassResult{ *depthObj.resources, device, m_size }
		, m_lightingPass{ castor::makeUnique< LightingPass >( graph
			, previousPasses
			, m_device
			, progress
			, m_technique
			, depthObj
			, m_opaquePassResult
			, smDirectionalResult
			, smPointResult
			, smSpotResult
			, m_lightPassResult
			, resultTexture
			, resultDepth
			, cameraUbo ) }
		, m_indirectLightingPass{ castor::makeUnique< IndirectLightingPass >( m_device
			, progress
			, *m_technique.getRenderTarget().getScene()
			, m_technique.getDebugConfig()
			, graph
			, m_lightingPass->getLastPass()
			, brdf
			, depthObj
			, m_opaquePassResult
			, m_lightPassResult
			, indirect
			, cameraUbo ) }
		, m_subsurfaceScattering{ castor::makeUnique< SubsurfaceScatteringPass >( graph
			, m_lightingPass->getLastPass()
			, m_device
			, progress
			, *m_technique.getRenderTarget().getScene()
			, cameraUbo
			, depthObj
			, m_opaquePassResult
			, m_lightPassResult ) }
	{
		m_resolves.resize( MaxLightingModels );
		doCreateResolvePasses( graph
			, crg::FramePassArray{ &m_indirectLightingPass->getLastPass()
				, &m_subsurfaceScattering->getLastPass()
				, &ssaoPass }
			, progress
			, depthObj
			, resultTexture
			, ssao
			, cameraUbo
			, sceneUbo
			, hdrConfigUbo
			, ssaoConfig
			, opaquePassEnabled );
		m_lightPassResult.create();
		m_additionalPassDesc = &doCreateDeferredAdditionalPass( graph, progress, *m_lastPass );
		m_additionalPassEnabled = crg::RunnablePass::IsEnabledCallback{ [this]() { return doIsAdditionalPassEnabled(); } };
		m_lastPass = m_additionalPassDesc;
	}

	void DeferredRendering::update( CpuUpdater & updater )
	{
		m_subsurfaceScattering->update( updater );
		m_lightingPass->update( updater );
		m_indirectLightingPass->update( updater );

		for ( auto & resolve : castor::makeArrayView( m_resolves.begin(), m_index ) )
		{
			CU_Require( resolve );
			resolve->update( updater );
		}

		m_additionalPass->update( updater );
	}

	void DeferredRendering::update( GpuUpdater & updater )
	{
		m_lightingPass->update( updater );
		m_additionalPass->countNodes( updater.info );
	}

	void DeferredRendering::accept( RenderTechniqueVisitor & visitor )
	{
		for ( auto texture = DsTexture::eMin; texture < DsTexture::eCount; texture = DsTexture( size_t( texture ) + 1u ) )
		{
			if ( texture == DsTexture::eNmlOcc )
			{
				visitor.visit( "Opaque " + getTexName( texture )
					, m_opaquePassResult[texture].sampledViewId
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, TextureFactors::tex2D( { 0.5, 0.5f, 0.5 }, { 0.5, 0.5, 0.5 } ).invert( true ) );
			}
			else
			{
				visitor.visit( "Opaque " + getTexName( texture )
					, m_opaquePassResult[texture].sampledViewId
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, TextureFactors::tex2D().invert( true ) );
			}
		}

		m_lightingPass->accept( visitor );
		m_indirectLightingPass->accept( visitor );
		m_subsurfaceScattering->accept( visitor );

		for ( auto & resolve : castor::makeArrayView( m_resolves.begin(), m_index ) )
		{
			CU_Require( resolve );
			resolve->accept( visitor );
		}
	}

	Texture const & DeferredRendering::getLightDiffuse()
	{
		m_lightPassResult.create();
		return m_lightPassResult[LpTexture::eDiffuse];
	}

	Texture const & DeferredRendering::getLightScattering()
	{
		m_lightPassResult.create();
		return m_lightPassResult[LpTexture::eScattering];
	}

	void DeferredRendering::doCreateResolvePasses( crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses
		, ProgressBar * progress
		, Texture const & depthObj
		, crg::ImageViewIdArray resultTexture
		, Texture const & ssao
		, CameraUbo const & cameraUbo
		, SceneUbo const & sceneUbo
		, HdrConfigUbo const & hdrConfigUbo
		, SsaoConfig & ssaoConfig
		, crg::RunnablePass::IsEnabledCallback const & opaquePassEnabled )
	{
		m_index = 0u;
		auto & scene = *m_technique.getRenderTarget().getScene();

		for ( auto lightingModelId : scene.getEngine()->getLightingModelFactory().getLightingModelsID() )
		{
			m_index = std::max( m_index, uint32_t( lightingModelId ) );
			m_resolves[lightingModelId - 1u] = castor::makeUnique< OpaqueResolvePass >( graph
				, std::move( previousPasses )
				, m_device
				, progress
				, m_technique
				, depthObj
				, m_opaquePassResult
				, ssaoConfig
				, ssao
				, m_subsurfaceScattering->getResult()
				, m_lightPassResult
				, resultTexture
				, cameraUbo
				, sceneUbo
				, hdrConfigUbo
				, lightingModelId
				, scene.getBackgroundModelId()
				, opaquePassEnabled );
			m_lastPass = &m_resolves[lightingModelId - 1u]->getLastPass();
			previousPasses = { m_lastPass };
		}
	}

	crg::FramePass & DeferredRendering::doCreateDeferredAdditionalPass( crg::FramePassGroup & graph
		, ProgressBar * progress
		, crg::FramePass const & lastPass )
	{
		stepProgressBar( progress, "Creating additional opaque pass" );
		auto targetResult = m_technique.getTargetResult();
		auto targetDepth = m_technique.getTargetDepth();
		auto & result = graph.createPass( "AdditionalPass"
			, [this, progress, targetResult, targetDepth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				stepProgressBar( progress, "Initialising additional opaque pass" );
				RenderTechniquePassDesc techniquePassDesc{ false, m_technique.getSsaoConfig() };
				RenderNodesPassDesc renderPassDesc{ m_technique.getTargetExtent()
					, m_technique.getCameraUbo()
					, m_technique.getSceneUbo()
					, m_technique.getRenderTarget().getCuller() };
				renderPassDesc.safeBand( true )
					.meshShading( true )
					.allowClusteredLighting()
					.componentModeFlags( ForwardRenderTechniquePass::DefaultComponentFlags
						| ComponentModeFlag::eForward );
				techniquePassDesc.ssao( m_technique.getSsaoResult() )
					.indirect( m_technique.getIndirectLighting() )
					.hasVelocity( true );
				auto res = std::make_unique< ForwardRenderTechniquePass >( &m_technique
					, framePass
					, context
					, runnableGraph
					, m_device
					, "c3d.deferred.additional"
					, cuT( "Default" )
					, targetResult
					, targetDepth
					, std::move( renderPassDesc )
					, std::move( techniquePassDesc ) );
				m_additionalPass = res.get();
				m_technique.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependency( lastPass );
		result.addInOutDepthStencilView( targetDepth );
		result.addImplicitColourView( m_technique.getSsaoResult().wholeViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		result.addInOutColourView( targetResult );
		result.addInOutColourView( m_technique.getRenderTarget().getVelocity()->targetViewId );
		return result;
	}

	bool DeferredRendering::doIsAdditionalPassEnabled()const
	{
		CU_Require( m_additionalPass );
		return m_additionalPass->isPassEnabled();
	}
}
