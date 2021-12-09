/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredRendering_H___
#define ___C3D_DeferredRendering_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/Ssao/SsaoModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"

namespace castor3d
{
	class DeferredRendering
	{
	public:
		/**
		 *\~english
		 *\brief		Initialises deferred rendering related stuff.
		 *\param[in]	engine				The engine.
		 *\param[in]	device				The GPU device.
		 *\param[in]	opaquePass			The opaque nodes render pass.
		 *\param[in]	depthTexture		The render target depth texture.
		 *\param[in]	velocityTexture		The render target velocity texture.
		 *\param[in]	resultTexture		The render technique texture.
		 *\param[in]	smDirectionalResult	The directional lights shadow map.
		 *\param[in]	smPointResult		The point lights shadow map.
		 *\param[in]	smSpotResult		The spot lights shadow map.
		 *\param[in]	lpvResult			The LPV result.
		 *\param[in]	llpvResult			The Layered LPV result.
		 *\param[in]	vctFirstBounce		The VCT first bounce result.
		 *\param[in]	vctSecondaryBounce	The VCT secondary bounce result.
		 *\param[in]	size				The render dimensions.
		 *\param[in]	scene				The rendered scene.
		 *\param[in]	hdrConfigUbo		The HDR configuration UBO.
		 *\param[in]	gpInfoUbo			The GBuffer configuration UBO.
		 *\param[in]	lpvConfigUbo		The LPV configuration UBO.
		 *\param[in]	llpvConfigUbo		The Layered LPV configuration UBO.
		 *\param[in]	vctConfigUbo		The VCT configuration UBO.
		 *\param[in]	ssaoConfig			The SSAO configuration.
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\param[in]	engine				Le moteur.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	opaquePass			La passe de rendu des noeuds opaques.
		 *\param[in]	depthTexture		La texture de profondeur de la cible de rendu.
		 *\param[in]	velocityTexture		La texture de vélocité de la cible de rendu.
		 *\param[in]	resultTexture		La texture de la technique de rendu.
		 *\param[in]	smDirectionalResult	La shadow map des source lumineuses directionnelles.
		 *\param[in]	smPointResult		La shadow map des source lumineuses omnidirectionnelles.
		 *\param[in]	smSpotResult		La shadow map des source lumineuses projecteurs.
		 *\param[in]	lpvResult			Le résultat du LPV.
		 *\param[in]	llpvResult			Le résultat du Layered LPV.
		 *\param[in]	vctFirstBounce		Le résultat du premier rebond de VCT.
		 *\param[in]	vctSecondaryBounce	Le résultat du second rebond de VCT.
		 *\param[in]	size				Les dimensions du rendu.
		 *\param[in]	scene				La scène rendue.
		 *\param[in]	hdrConfigUbo		L'UBO de configuration HDR.
		 *\param[in]	gpInfoUbo			L'UBO de configuration du GBuffer.
		 *\param[in]	lpvConfigUbo		L'UBO de configuration des LPV.
		 *\param[in]	llpvConfigUbo		L'UBO de configuration des Layered LPV.
		 *\param[in]	vctConfigUbo		L'UBO de configuration du VCT.
		 *\param[in]	ssaoConfig			La configuration du SSAO.
		 */
		DeferredRendering( crg::FrameGraph & graph
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
			, SsaoConfig & ssaoConfig );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Visitor acceptance function.
		 *\param		visitor	The ... visitor.
		 *\~french
		 *\brief		Fonction d'acceptation de visiteur.
		 *\param		visitor	Le ... visiteur.
		 */
		void accept( RenderTechniqueVisitor & visitor );

		crg::ImageViewId const & getLightDepthImgView()const;

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
		RenderDevice const & m_device;
		crg::FramePass const & m_opaquePass;
		crg::FramePass const * m_lastPass{};
		OpaquePassResult const & m_opaquePassResult;
		GpInfoUbo const & m_gpInfoUbo;
		castor::Size m_size;
		LightPassResult m_lightPassResult;
		LightingPassUPtr m_lightingPass;
		IndirectLightingPassUPtr m_indirectLightingPass;
		SubsurfaceScatteringPassUPtr m_subsurfaceScattering;
		OpaqueResolvePassUPtr m_resolve;
		std::vector< ashes::ImagePtr > m_results;
	};
}

#endif
