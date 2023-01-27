#include "Castor3D/Render/Opaque/DeferredRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/PluginCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Opaque/IndirectLightingPass.hpp"
#include "Castor3D/Render/Opaque/LightingPass.hpp"
#include "Castor3D/Render/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Opaque/OpaqueResolvePass.hpp"
#include "Castor3D/Render/Opaque/Lighting/SubsurfaceScatteringPass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Shader/LightingModelFactory.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>

CU_ImplementCUSmartPtr( castor3d, DeferredRendering )

namespace castor3d
{
	DeferredRendering::DeferredRendering( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, crg::FramePass const & ssaoPass
		, RenderDevice const & device
		, ProgressBar * progress
		, Texture const & brdf
		, Texture const & depth
		, Texture const & depthObj
		, OpaquePassResult const & opaquePassResult
		, crg::ImageViewIdArray resultTexture
		, ShadowMapResult const & smDirectionalResult
		, ShadowMapResult const & smPointResult
		, ShadowMapResult const & smSpotResult
		, LightVolumePassResult const & lpvResult
		, LightVolumePassResultArray const & llpvResult
		, Texture const & vctFirstBounce
		, Texture const & vctSecondaryBounce
		, Texture const & ssao
		, castor::Size const & size
		, RenderTechnique const & technique
		, SceneUbo const & sceneUbo
		, HdrConfigUbo const & hdrConfigUbo
		, GpInfoUbo const & gpInfoUbo
		, LpvGridConfigUbo const & lpvConfigUbo
		, LayeredLpvGridConfigUbo const & llpvConfigUbo
		, VoxelizerUbo const & vctConfigUbo
		, SsaoConfig & ssaoConfig
		, crg::RunnablePass::IsEnabledCallback const & opaquePassEnabled )
		: m_device{ device }
		, m_opaquePassResult{ opaquePassResult }
		, m_technique{ technique }
		, m_lightingGpInfoUbo{ device }
		, m_size{ size }
		, m_lightPassResult{ *depth.resources, device, m_size }
		, m_lightingPass{ castor::makeUnique< LightingPass >( graph
			, previousPasses
			, m_device
			, progress
			, *m_technique.getRenderTarget().getScene()
			, depth
			, depthObj
			, m_opaquePassResult
			, smDirectionalResult
			, smPointResult
			, smSpotResult
			, m_lightPassResult
			, resultTexture
			, sceneUbo
			, m_lightingGpInfoUbo ) }
		, m_indirectLightingPass{ castor::makeUnique< IndirectLightingPass >( m_device
			, progress
			, *m_technique.getRenderTarget().getScene()
			, graph
			, m_lightingPass->getLastPass()
			, brdf
			, depthObj
			, m_opaquePassResult
			, m_lightPassResult
			, lpvResult
			, llpvResult
			, vctFirstBounce
			, vctSecondaryBounce
			, sceneUbo
			, m_lightingGpInfoUbo
			, lpvConfigUbo
			, llpvConfigUbo
			, vctConfigUbo ) }
		, m_subsurfaceScattering{ castor::makeUnique< SubsurfaceScatteringPass >( graph
			, m_lightingPass->getLastPass()
			, m_device
			, progress
			, *m_technique.getRenderTarget().getScene()
			, m_lightingGpInfoUbo
			, sceneUbo
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
			, sceneUbo
			, hdrConfigUbo
			, gpInfoUbo
			, ssaoConfig
			, opaquePassEnabled );
		m_lightPassResult.create();
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

		auto & camera = *updater.camera;
		m_lightingGpInfoUbo.cpuUpdate( makeSize( m_lightPassResult[LpTexture::eDiffuse].getExtent() )
			, camera );
	}

	void DeferredRendering::update( GpuUpdater & updater )
	{
		m_lightingPass->update( updater );
	}

	void DeferredRendering::accept( RenderTechniqueVisitor & visitor )
	{
		for ( auto texture = DsTexture::eMin; texture < DsTexture::eCount; texture = DsTexture( size_t( texture ) + 1u ) )
		{
			visitor.visit( "Opaque " + getTexName( texture )
				, m_opaquePassResult[texture].sampledViewId
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				, TextureFactors{}.invert( true ) );
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

	crg::ImageViewId const & DeferredRendering::getLightDepthImgView()const
	{
		return m_lightPassResult[LpTexture::eDepth].targetViewId;
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
		, SceneUbo const & sceneUbo
		, HdrConfigUbo const & hdrConfigUbo
		, GpInfoUbo const & gpInfoUbo
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
				, sceneUbo
				, gpInfoUbo
				, hdrConfigUbo
				, lightingModelId
				, scene.getBackgroundModelId()
				, opaquePassEnabled );
			m_lastPass = &m_resolves[lightingModelId - 1u]->getLastPass();
			previousPasses = { m_lastPass };
		}
	}
}
