#include "Castor3D/Render/Opaque/DeferredRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/PluginCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Opaque/IndirectLightingPass.hpp"
#include "Castor3D/Render/Opaque/LightingPass.hpp"
#include "Castor3D/Render/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Opaque/OpaqueResolvePass.hpp"
#include "Castor3D/Render/Opaque/Lighting/SubsurfaceScatteringPass.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
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
		, Texture const & resultTexture
		, ShadowMapResult const & smDirectionalResult
		, ShadowMapResult const & smPointResult
		, ShadowMapResult const & smSpotResult
		, LightVolumePassResult const & lpvResult
		, LightVolumePassResultArray const & llpvResult
		, Texture const & vctFirstBounce
		, Texture const & vctSecondaryBounce
		, Texture const & ssao
		, castor::Size const & size
		, Scene & scene
		, SceneUbo const & sceneUbo
		, HdrConfigUbo const & hdrConfigUbo
		, GpInfoUbo const & gpInfoUbo
		, LpvGridConfigUbo const & lpvConfigUbo
		, LayeredLpvGridConfigUbo const & llpvConfigUbo
		, VoxelizerUbo const & vctConfigUbo
		, SsaoConfig & ssaoConfig )
		: m_device{ device }
		, m_opaquePassResult{ opaquePassResult }
		, m_lightingGpInfoUbo{ device }
		, m_size{ size }
		, m_lightPassResult{ scene.getOwner()->getGraphResourceHandler(), device, m_size }
		, m_lightingPass{ castor::makeUnique< LightingPass >( graph
			, previousPasses
			, m_device
			, progress
			, scene
			, depth
			, depthObj
			, m_opaquePassResult
			, smDirectionalResult
			, smPointResult
			, smSpotResult
			, m_lightPassResult
			, resultTexture.imageId
			, sceneUbo
			, m_lightingGpInfoUbo ) }
		, m_indirectLightingPass{ castor::makeUnique< IndirectLightingPass >( m_device
			, progress
			, scene
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
			, scene
			, m_lightingGpInfoUbo
			, sceneUbo
			, depthObj
			, m_opaquePassResult
			, m_lightPassResult ) }
		, m_resolve{ castor::makeUnique< OpaqueResolvePass >( graph
			, crg::FramePassArray{ &m_indirectLightingPass->getLastPass(), &m_subsurfaceScattering->getLastPass(), &ssaoPass }
			, m_device
			, progress
			, scene
			, depthObj
			, opaquePassResult
			, ssaoConfig
			, ssao
			, m_subsurfaceScattering->getResult()
			, m_lightPassResult[LpTexture::eDiffuse]
			, m_lightPassResult[LpTexture::eSpecular]
			, m_lightPassResult[LpTexture::eScattering]
			, m_lightPassResult[LpTexture::eIndirectDiffuse]
			, m_lightPassResult[LpTexture::eIndirectSpecular]
			, resultTexture
			, sceneUbo
			, gpInfoUbo
			, hdrConfigUbo ) }
	{
		m_lastPass = &m_resolve->getLastPass();
		m_lightPassResult.create();
	}

	void DeferredRendering::update( CpuUpdater & updater )
	{
		m_subsurfaceScattering->update( updater );
		m_lightingPass->update( updater );
		m_indirectLightingPass->update( updater );
		m_resolve->update( updater );
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
		m_resolve->accept( visitor );
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
}