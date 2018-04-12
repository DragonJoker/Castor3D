/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredRendering_H___
#define ___C3D_DeferredRendering_H___

#include "Technique/Opaque/GeometryPassResult.hpp"
#include "Technique/Opaque/LightingPass.hpp"
#include "Technique/Opaque/ReflectionPass.hpp"
#include "Technique/Opaque/SubsurfaceScatteringPass.hpp"

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
		 *\param[in]	engine		The engine.
		 *\param[in]	opaquePass	The opaque nodes render pass.
		 *\param[in]	frameBuffer	The target frame buffer.
		 *\param[in]	depthAttach	The attach between depth buffer and the frame buffer.
		 *\param[in]	size		The render dimensions.
		 *\param[in]	scene		The rendered scene.
		 *\param[in]	config		The SSAO configuration.
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	opaquePass	La passe de rendu des noeuds opaques.
		 *\param[in]	frameBuffer	Le tampon d'image cible.
		 *\param[in]	depthAttach	L'attache entre le tampon de profondeur et le tampon d'image.
		 *\param[in]	size		Les dimensions du rendu.
		 *\param[in]	scene		La scène rendue.
		 *\param[in]	config		La configuration du SSAO.
		 */
		DeferredRendering( Engine & engine
			, OpaquePass & opaquePass
			, TextureLayoutSPtr depthTexture
			, TextureLayoutSPtr velocityTexture
			, TextureLayoutSPtr resultTexture
			, castor::Size const & size
			, Scene & scene
			, Viewport const & viewport
			, SsaoConfig const & config );
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
		 *\param[out]	shadowMaps	The shadow maps.
		 *\param[out]	jitter		The jittering value.
		 *\~french
		 *\brief		Met à jour la passe opaque.
		 *\param[out]	info		Reçoit les informations de rendu.
		 *\param[out]	scene		La scène rendue.
		 *\param[out]	camera		La caméra par laquelle la scène est rendue.
		 *\param[out]	shadowMaps	Les textures d'ombres.
		 *\param[out]	jitter		La valeur de jittering.
		 */
		void update( RenderInfo & info
			, Scene const & scene
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps
			, castor::Point2r const & jitter );
		/**
		 *\~english
		 *\brief		Renders opaque nodes.
		 *\param[out]	info		Receives the render informations.
		 *\param[out]	scene		The rendered scene.
		 *\param[out]	camera		The viewer camera.
		 *\~french
		 *\brief		Dessine les noeuds opaques.
		 *\param[out]	info		Reçoit les informations de rendu.
		 *\param[out]	scene		La scène rendue.
		 *\param[out]	camera		La caméra par laquelle la scène est rendue.
		 */
		renderer::Semaphore const & render( RenderInfo & info
			, Scene const & scene
			, Camera const & camera
			, renderer::Semaphore const & toWait );
		/**
		 *\~english
		 *\brief		Displays debug data on screen.
		 *\~french
		 *\brief		Dessine les données de débogage sur l'écran.
		 */
		void debugDisplay()const;

	private:
		Engine & m_engine;
		SsaoConfig const & m_ssaoConfig;
		OpaquePass & m_opaquePass;
		castor::Size m_size;
		GpInfoUbo m_gpInfoUbo;
		std::unique_ptr< LightingPass > m_lightingPass;
		std::unique_ptr< ReflectionPass > m_reflection;
		std::unique_ptr< SubsurfaceScatteringPass > m_subsurfaceScattering;
		GeometryPassResult m_geometryPassResult;
		std::vector< renderer::TexturePtr > m_results;
		renderer::CommandBufferPtr m_nodesCommands;
	};
}

#endif
