#include "Castor3D/Render/Technique/Opaque/DeferredRendering.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/PluginCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Technique/Opaque/IndirectLightingPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightingPass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueResolvePass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/SubsurfaceScatteringPass.hpp"
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
		, crg::FramePass const & opaquePass
		, RenderDevice const & device
		, ProgressBar * progress
		, Texture const & depth
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
		, m_opaquePass{ opaquePass }
		, m_lastPass{ &m_opaquePass }
		, m_opaquePassResult{ opaquePassResult }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_size{ size }
		, m_lightPassResult{ scene.getOwner()->getGraphResourceHandler(), device, size }
		, m_lightingPass{ castor::makeUnique< LightingPass >( graph
			, m_lastPass
			, m_device
			, progress
			, m_size
			, scene
			, depth
			, m_opaquePassResult
			, smDirectionalResult
			, smPointResult
			, smSpotResult
			, m_lightPassResult
			, sceneUbo
			, m_gpInfoUbo ) }
		, m_indirectLightingPass{ castor::makeUnique< IndirectLightingPass >( m_device
			, progress
			, scene
			, graph
			, m_lastPass
			, m_opaquePassResult
			, m_lightPassResult
			, lpvResult
			, llpvResult
			, vctFirstBounce
			, vctSecondaryBounce
			, sceneUbo
			, m_gpInfoUbo
			, lpvConfigUbo
			, llpvConfigUbo
			, vctConfigUbo ) }
		, m_subsurfaceScattering{ castor::makeUnique< SubsurfaceScatteringPass >( graph
			, m_lastPass
			, m_device
			, progress
			, scene
			, m_gpInfoUbo
			, sceneUbo
			, m_size
			, m_opaquePassResult
			, m_lightPassResult ) }
		, m_resolve{ castor::makeUnique< OpaqueResolvePass >( graph
			, m_lastPass
			, m_device
			, progress
			, scene
			, opaquePassResult
			, ssaoConfig
			, ssao
			, m_subsurfaceScattering->getResult()
			, m_lightPassResult[LpTexture::eDiffuse]
			, m_lightPassResult[LpTexture::eSpecular]
			, m_lightPassResult[LpTexture::eIndirectDiffuse]
			, m_lightPassResult[LpTexture::eIndirectSpecular]
			, resultTexture
			, sceneUbo
			, m_gpInfoUbo
			, hdrConfigUbo ) }
	{
		m_lightPassResult.create();
	}

	void DeferredRendering::update( CpuUpdater & updater )
	{
		m_subsurfaceScattering->update( updater );
		m_lightingPass->update( updater );
		m_indirectLightingPass->update( updater );
		m_resolve->update( updater );
	}

	void DeferredRendering::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( "Opaque Data1"
			, m_opaquePassResult[DsTexture::eData1].sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Opaque Data2"
			, m_opaquePassResult[DsTexture::eData2].sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Opaque Data3"
			, m_opaquePassResult[DsTexture::eData3].sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Opaque Data4"
			, m_opaquePassResult[DsTexture::eData4].sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Opaque Data5"
			, m_opaquePassResult[DsTexture::eData5].sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );

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
