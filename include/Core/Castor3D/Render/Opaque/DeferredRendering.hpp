/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredRendering_H___
#define ___C3D_DeferredRendering_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightingModule.hpp"
#include "Castor3D/Render/Ssao/SsaoModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Render/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"

#include <RenderGraph/RunnablePass.hpp>

namespace castor3d
{
	class DeferredRendering
	{
	public:
		/**
		 *\~english
		 *\brief		Initialises deferred rendering related stuff.
		 *\param[in]	graph				The graph passes group.
		 *\param[in]	previousPasses		The previous frame passes this pass depends on.
		 *\param[in]	ssaoPass			The SSAO frame pass.
		 *\param[in]	device				The GPU device.
		 *\param[in]	progress			The progress status.
		 *\param[in]	brdf				The prefiltered BRDF map.
		 *\param[in]	depth				The depth texture.
		 *\param[in]	depthObj			The depths and objects texture.
		 *\param[in]	opaquePassResult	The geometry pass buffers.
		 *\param[in]	resultTexture		The render technique texture.
		 *\param[in]	smDirectionalResult	The directional lights shadow map.
		 *\param[in]	smPointResult		The point lights shadow map.
		 *\param[in]	smSpotResult		The spot lights shadow map.
		 *\param[in]	lpvResult			The LPV result.
		 *\param[in]	llpvResult			The Layered LPV result.
		 *\param[in]	vctFirstBounce		The VCT first bounce result.
		 *\param[in]	vctSecondaryBounce	The VCT secondary bounce result.
		 *\param[in]	ssao				The SSAO result.
		 *\param[in]	size				The render dimensions.
		 *\param[in]	scene				The rendered scene.
		 *\param[in]	sceneUbo			The scene configuration UBO.
		 *\param[in]	hdrConfigUbo		The HDR configuration UBO.
		 *\param[in]	gpInfoUbo			The GBuffer configuration UBO.
		 *\param[in]	lpvConfigUbo		The LPV configuration UBO.
		 *\param[in]	llpvConfigUbo		The Layered LPV configuration UBO.
		 *\param[in]	vctConfigUbo		The VCT configuration UBO.
		 *\param[in]	ssaoConfig			The SSAO configuration.
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\param[in]	graph				Le groupe de passes du graphe.
		 *\param[in]	previousPasses		Les passes de frame dont cette passe dépend.
		 *\param[in]	ssaoPass			La passe de SSAO.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	progress			Le statut de progression.
		 *\param[in]	brdf				La texture de BRDF.
		 *\param[in]	depth				La texture de profondeur.
		 *\param[in]	depthObj			La texture de profondeurs et d'objets.
		 *\param[in]	opaquePassResult	Le résultat de la geometry pass.
		 *\param[in]	resultTexture		La texture de la technique de rendu.
		 *\param[in]	smDirectionalResult	La shadow map des source lumineuses directionnelles.
		 *\param[in]	smPointResult		La shadow map des source lumineuses omnidirectionnelles.
		 *\param[in]	smSpotResult		La shadow map des source lumineuses projecteurs.
		 *\param[in]	lpvResult			Le résultat du LPV.
		 *\param[in]	llpvResult			Le résultat du Layered LPV.
		 *\param[in]	vctFirstBounce		Le résultat du premier rebond de VCT.
		 *\param[in]	vctSecondaryBounce	Le résultat du second rebond de VCT.
		 *\param[in]	ssao				Le résultat du SSAO.
		 *\param[in]	size				Les dimensions du rendu.
		 *\param[in]	scene				La scène rendue.
		 *\param[in]	sceneUbo			L'UBO de configuration de la scène
		 *\param[in]	hdrConfigUbo		L'UBO de configuration HDR.
		 *\param[in]	gpInfoUbo			L'UBO de configuration du GBuffer.
		 *\param[in]	lpvConfigUbo		L'UBO de configuration des LPV.
		 *\param[in]	llpvConfigUbo		L'UBO de configuration des Layered LPV.
		 *\param[in]	vctConfigUbo		L'UBO de configuration du VCT.
		 *\param[in]	ssaoConfig			La configuration du SSAO.
		 */
		C3D_API DeferredRendering( crg::FramePassGroup & graph
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
			, crg::RunnablePass::IsEnabledCallback const & opaquePassEnabled );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Visitor acceptance function.
		 *\param		visitor	The ... visitor.
		 *\~french
		 *\brief		Fonction d'acceptation de visiteur.
		 *\param		visitor	Le ... visiteur.
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );

		C3D_API crg::ImageViewId const & getLightDepthImgView()const;
		C3D_API Texture const & getLightDiffuse();
		C3D_API Texture const & getLightScattering();

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

		static uint32_t countInitialisationSteps()
		{
			uint32_t result = 0u;
			result += 4;// m_lightingPass;
			result += 2;// m_indirectLightingPass;
			result += 2;// m_subsurfaceScattering;
			result += 2;// m_resolve;
			return result;
		}

	private:
		void doCreateResolvePasses( crg::FramePassGroup & graph
			, crg::FramePassArray previousPasses
			, ProgressBar * progress
			, Texture const & depthObj
			, crg::ImageViewIdArray resultTexture
			, Texture const & ssao
			, SceneUbo const & sceneUbo
			, HdrConfigUbo const & hdrConfigUbo
			, GpInfoUbo const & gpInfoUbo
			, SsaoConfig & ssaoConfig
			, crg::RunnablePass::IsEnabledCallback const & opaquePassEnabled );

	private:
		RenderDevice const & m_device;
		OpaquePassResult const & m_opaquePassResult;
		RenderTechnique const & m_technique;
		crg::FramePass const * m_lastPass{};
		GpInfoUbo m_lightingGpInfoUbo;
		castor::Size m_size;
		LightPassResult m_lightPassResult;
		LightingPassUPtr m_lightingPass;
		IndirectLightingPassUPtr m_indirectLightingPass;
		SubsurfaceScatteringPassUPtr m_subsurfaceScattering;
		std::vector< OpaqueResolvePassUPtr > m_resolves;
		uint32_t m_index{};
		std::vector< ashes::ImagePtr > m_results;
	};
}

#endif
