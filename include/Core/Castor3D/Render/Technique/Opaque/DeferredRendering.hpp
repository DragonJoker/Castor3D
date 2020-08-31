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
#include "Castor3D/Render/Technique/Opaque/Lighting/LightingPass.hpp"
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
		 *\param[in]	engine			The engine.
		 *\param[in]	opaquePass		The opaque nodes render pass.
		 *\param[in]	depthTexture	The render target depth texture.
		 *\param[in]	velocityTexture	The render target velocity texture.
		 *\param[in]	resultTexture	The render technique texture.
		 *\param[in]	size			The render dimensions.
		 *\param[in]	scene			The rendered scene.
		 *\param[in]	hdrConfigUbo	The HDR configuration UBO.
		 *\param[in]	config			The SSAO configuration.
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	opaquePass		La passe de rendu des noeuds opaques.
		 *\param[in]	depthTexture	La texture de profondeur de la cible de rendu.
		 *\param[in]	velocityTexture	La texture de vélocité de la cible de rendu.
		 *\param[in]	resultTexture	La texture de la technique de rendu.
		 *\param[in]	size			Les dimensions du rendu.
		 *\param[in]	scene			La scène rendue.
		 *\param[in]	hdrConfigUbo	L'UBO de configuration HDR.
		 *\param[in]	config			La configuration du SSAO.
		 */
		DeferredRendering( Engine & engine
			, OpaquePass & opaquePass
			, TextureUnit const & depthTexture
			, TextureUnit const & velocityTexture
			, TextureUnit const & resultTexture
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult
			, castor::Size const & size
			, Scene & scene
			, HdrConfigUbo & hdrConfigUbo
			, GpInfoUbo const & gpInfoUbo
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
		 *\brief		Updates opaque pass.
		 *\param[out]	info	Receives the render informations.
		 *\param[in]	scene	The rendered scene.
		 *\param[in]	camera	The viewer camera.
		 *\param[in]	jitter	The jittering value.
		 *\~french
		 *\brief		Met à jour la passe opaque.
		 *\param[out]	info	Reçoit les informations de rendu.
		 *\param[in]	scene	La scène rendue.
		 *\param[in]	camera	La caméra par laquelle la scène est rendue.
		 *\param[in]	jitter	La valeur de jittering.
		 */
		void update( RenderInfo & info
			, Scene const & scene
			, Camera const & camera
			, castor::Point2f const & jitter );
		/**
		 *\~english
		 *\brief		Renders opaque nodes.
		 *\param[out]	scene		The rendered scene.
		 *\param[out]	camera		The viewer camera.
		 *\param[in]	toWait		The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine les noeuds opaques.
		 *\param[out]	scene		La scène rendue.
		 *\param[out]	camera		La caméra par laquelle la scène est rendue.
		 *\param[in]	toWait		Le sémaphore de la passe de rendu précédente.
		 */
		ashes::Semaphore const & render( Scene const & scene
			, Camera const & camera
			, ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( RenderTechniqueVisitor & visitor );

	private:
		Engine & m_engine;
		SsaoConfig & m_ssaoConfig;
		OpaquePass & m_opaquePass;
		GpInfoUbo const & m_gpInfoUbo;
		castor::Size m_size;
		OpaquePassResult m_opaquePassResult;
		castor::DelayedInitialiserT< LineariseDepthPass > m_linearisePass;
		std::unique_ptr< LightingPass > m_lightingPass;
		castor::DelayedInitialiserT< SsaoPass > m_ssao;
		castor::DelayedInitialiserT< SubsurfaceScatteringPass > m_subsurfaceScattering;
		std::vector< castor::DelayedInitialiserT< OpaqueResolvePass > > m_resolve;
		std::vector< ashes::ImagePtr > m_results;
	};
}

#endif
