/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredRendering_H___
#define ___C3D_DeferredRendering_H___

#include "Castor3D/Technique/Opaque/GeometryPassResult.hpp"
#include "Castor3D/Technique/Opaque/LightingPass.hpp"
#include "Castor3D/Technique/Opaque/ReflectionPass.hpp"
#include "Castor3D/Technique/Opaque/SsaoPass.hpp"
#include "Castor3D/Technique/Opaque/SubsurfaceScatteringPass.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		The deferred rendering class.
	\~french
	\brief		Classe de rendu différé.
	*/
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
			, TextureLayoutSPtr depthTexture
			, TextureLayoutSPtr velocityTexture
			, TextureLayoutSPtr resultTexture
			, castor::Size const & size
			, Scene & scene
			, HdrConfigUbo & hdrConfigUbo
			, SsaoConfig & config );
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
		 *\param[out]	info		Receives the render informations.
		 *\param[out]	scene		The rendered scene.
		 *\param[out]	camera		The viewer camera.
		 *\param[out]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour la passe opaque.
		 *\param[out]	info		Reçoit les informations de rendu.
		 *\param[out]	scene		La scène rendue.
		 *\param[out]	camera		La caméra par laquelle la scène est rendue.
		 *\param[out]	jitter		La valeur de jittering.
		 */
		void update( RenderInfo & info
			, Scene const & scene
			, Camera const & camera
			, castor::Point2f const & jitter );
		/**
		 *\~english
		 *\brief		Renders opaque nodes.
		 *\param[out]	info		Receives the render informations.
		 *\param[out]	scene		The rendered scene.
		 *\param[out]	camera		The viewer camera.
		 *\param[in]	toWait		The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine les noeuds opaques.
		 *\param[out]	info		Reçoit les informations de rendu.
		 *\param[out]	scene		La scène rendue.
		 *\param[out]	camera		La caméra par laquelle la scène est rendue.
		 *\param[in]	toWait		Le sémaphore de la passe de rendu précédente.
		 */
		ashes::Semaphore const & render( RenderInfo & info
			, Scene const & scene
			, Camera const & camera
			, ashes::Semaphore const & toWait );
		/**
		 *\~english
		 *\brief		Displays debug data on screen.
		 *\~french
		 *\brief		Dessine les données de débogage sur l'écran.
		 */
		void debugDisplay( ashes::RenderPass const & renderPass
			, ashes::FrameBuffer const & frameBuffer )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( RenderTechniqueVisitor & visitor );

	private:
		Engine & m_engine;
		SsaoConfig & m_ssaoConfig;
		OpaquePass & m_opaquePass;
		castor::Size m_size;
		GpInfoUbo m_gpInfoUbo;
		std::unique_ptr< LightingPass > m_lightingPass;
		std::unique_ptr< SsaoPass > m_ssao;
		std::unique_ptr< SubsurfaceScatteringPass > m_subsurfaceScattering;
		std::vector< std::unique_ptr< ReflectionPass > > m_reflection;
		GeometryPassResult m_geometryPassResult;
		std::vector< ashes::ImagePtr > m_results;
	};
}

#endif
