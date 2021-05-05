/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredRendering_H___
#define ___C3D_DeferredRendering_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueResolvePass.hpp"
#include "Castor3D/Render/Technique/Opaque/SsaoPass.hpp"
#include "Castor3D/Render/Technique/Opaque/LightingPass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/SubsurfaceScatteringPass.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"

#include <CastorUtils/Design/DelayedInitialiser.hpp>

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
		DeferredRendering( Engine & engine
			, RenderDevice const & device
			, OpaquePass & opaquePass
			, OpaquePassResult const & opaquePassResult
			, TextureUnit const & resultTexture
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult
			, LightVolumePassResult const & lpvResult
			, LightVolumePassResultArray const & llpvResult
			, TextureUnit const & vctFirstBounce
			, TextureUnit const & vctSecondaryBounce
			, castor::Size const & size
			, Scene & scene
			, HdrConfigUbo const & hdrConfigUbo
			, GpInfoUbo const & gpInfoUbo
			, LpvGridConfigUbo const & lpvConfigUbo
			, LayeredLpvGridConfigUbo const & llpvConfigUbo
			, VoxelizerUbo const & vctConfigUbo
			, SsaoConfig & ssaoConfig );
		/**
		 *\~english
		 *\brief		Destroys deferred rendering related stuff.
		 *\~french
		 *\brief		Détruit les données liées au deferred rendering.
		 */
		~DeferredRendering();
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
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Renders the light passes on currently bound framebuffer.
		 *\param[in]	device	The GPU device.
		 *\param[in]	scene	The scene.
		 *\param[in]	camera	The viewing camera.
		 *\param[out]	toWait	The semaphore from previous render pass.
		 *\~french
		 *\brief		Dessine les passes d'éclairage sur le tampon d'image donné.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	scene	La scène.
		 *\param[in]	camera	La caméra.
		 *\param[out]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		ashes::Semaphore const & render( Scene const & scene
			, Camera const & camera
			, ashes::Semaphore const & toWait );
		/**
		 *\~english
		 *\brief		Visitor acceptance function.
		 *\param		visitor	The ... visitor.
		 *\~french
		 *\brief		Fonction d'acceptation de visiteur.
		 *\param		visitor	Le ... visiteur.
		 */
		void accept( RenderTechniqueVisitor & visitor );

	private:
		Engine & m_engine;
		Scene const & m_scene;
		RenderDevice const & m_device;
		SsaoConfig & m_ssaoConfig;
		OpaquePass & m_opaquePass;
		OpaquePassResult const & m_opaquePassResult;
		GpInfoUbo const & m_gpInfoUbo;
		castor::Size m_size;
		LineariseDepthPassUPtr m_linearisePass;
		SsaoPassUPtr m_ssao;
		std::unique_ptr< LightingPass > m_lightingPass;
		SubsurfaceScatteringPassUPtr m_subsurfaceScattering;
		OpaqueResolvePassUPtr m_resolve;
		std::vector< ashes::ImagePtr > m_results;
	};
}

#endif
