/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredRendering_H___
#define ___C3D_DeferredRendering_H___

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
			, FrameBuffer & frameBuffer
			, TextureAttachment & depthAttach
			, castor::Size const & size
			, Scene const & scene
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
		 *\brief		Renders opaque nodes.
		 *\param[out]	info		Receives the render informations.
		 *\param[out]	scene		The rendered scene.
		 *\param[out]	camera		The viewer camera.
		 *\param[out]	shadowMaps	The shadow maps.
		 *\param[out]	jitter		The jittering value.
		 *\param[out]	velocity	The velocity texture.
		 *\~french
		 *\brief		Dessine les noeuds opaques.
		 *\param[out]	info		Reçoit les informations de rendu.
		 *\param[out]	scene		La scène rendue.
		 *\param[out]	camera		La caméra par laquelle la scène est rendue.
		 *\param[out]	shadowMaps	Les textures d'ombres.
		 *\param[out]	jitter		La valeur de jittering.
		 *\param[out]	velocity	La texture de vélocité.
		 */
		void render( RenderInfo & info
			, Scene const & scene
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps
			, castor::Point2r const & jitter
			, TextureUnit const & velocity );
		/**
		 *\~english
		 *\brief		Displays debug data on screen.
		 *\~french
		 *\brief		Dessine les données de débogage sur l'écran.
		 */
		void debugDisplay()const;

	private:
		using GeometryBufferTextures = std::array< TextureUnitUPtr, size_t( DsTexture::eCount ) >;
		using GeometryBufferAttachs = std::array< TextureAttachmentSPtr, size_t( DsTexture::eCount ) >;

		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	The SSAO configuration.
		//!\~french		La configuration du SSAO.
		SsaoConfig const & m_ssaoConfig;
		//!\~english	The pass used to render opaque nodes.
		//!\~french		La passe utilisée pour dessiner les noeuds opaques.
		OpaquePass & m_opaquePass;
		//!\~english	The target framebuffer.
		//!\~french		Le tampon d'image ciblé.
		FrameBuffer & m_frameBuffer;
		//!\~english	The target framebuffer's depth attachment.
		//!\~french		L'attache profondeur du tampon d'image ciblé.
		TextureAttachment & m_depthAttach;
		//!\~english	The render area dimension.
		//!\~french		Les dimensions de l'aire de rendu.
		castor::Size m_size;
		//!\~english	The geometry pass informations.
		//!\~french		Les informations de la passe de géométrie.
		GpInfoUbo m_gpInfoUbo;
		//!\~english	The fog pass.
		//!\~french		La passe de brouillard.
		std::unique_ptr< LightingPass > m_lightingPass;
		//!\~english	The reflection pass.
		//!\~french		La passe de réflexion.
		std::unique_ptr< ReflectionPass > m_reflection;
		//!\~english	The reflection pass.
		//!\~french		La passe de réflexion.
		std::unique_ptr< SubsurfaceScatteringPass > m_subsurfaceScattering;
		//!\~english	The various textures for deferred shading.
		//!\~french		Les diverses textures pour le deferred shading.
		GeometryBufferTextures m_geometryPassResult;
		//!\~english	The deferred shading frame buffer.
		//!\~french		Le tampon d'image pour le deferred shading.
		FrameBufferSPtr m_geometryPassFrameBuffer;
		//!\~english	The attachments between textures and deferred shading frame buffer.
		//!\~french		Les attaches entre les textures et le tampon deferred shading.
		GeometryBufferAttachs m_geometryPassTexAttachs;
	};
}

#endif
