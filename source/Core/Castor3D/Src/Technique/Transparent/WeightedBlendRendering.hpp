/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WeightedBlendRendering_H___
#define ___C3D_WeightedBlendRendering_H___

#include "Technique/Transparent/FinalCombinePass.hpp"
#include "Technique/Transparent/TransparentPass.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Handles the weighted blend rendering.
	\~french
	\brief		Gère le rendu du weighted blend.
	*/
	class WeightedBlendRendering
	{
	public:
		/**
		 *\~english
		 *\brief		Initialises deferred rendering related stuff.
		 *\param[in]	engine			The engine.
		 *\param[in]	transparentPass	The transparent nodes render pass.
		 *\param[in]	frameBuffer		The target frame buffer.
		 *\param[in]	depthAttach		The attach between depth buffer and the frame buffer.
		 *\param[in]	size			The render dimensions.
		 *\param[in]	scene			The rendered scene.
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	transparentPass	La passe de rendu des noeuds transparents.
		 *\param[in]	frameBuffer		Le tampon d'image cible.
		 *\param[in]	depthAttach		L'attache entre le tampon de profondeur et le tampon d'image.
		 *\param[in]	size			Les dimensions du rendu.
		 *\param[in]	scene			La scène rendue.
		 */
		WeightedBlendRendering( Engine & engine
			, TransparentPass & transparentPass
			, FrameBuffer & frameBuffer
			, TextureAttachment & depthAttach
			, castor::Size const & size
			, Scene const & scene );
		/**
		 *\~english
		 *\brief		Destroys deferred rendering related stuff.
		 *\~french
		 *\brief		Détruit les données liées au deferred rendering.
		 */
		~WeightedBlendRendering();
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
		void debugDisplay();
		/**
		 *\~english
		 *\return		The intermediate framebuffer.
		 *\~french
		 *\return		Le tampon d'image intermédiaire.
		 */
		inline FrameBuffer & getFbo()
		{
			return *m_weightedBlendPassFrameBuffer;
		}

	private:
		using WeightedBlendTextures = std::array< TextureUnitUPtr, size_t( WbTexture::eCount ) >;
		using WeightedBlendAttachs = std::array< TextureAttachmentSPtr, size_t( WbTexture::eCount ) >;

		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	The pass used to render transparent nodes.
		//!\~french		La passe utilisée pour dessiner les noeuds transparents.
		TransparentPass & m_transparentPass;
		//!\~english	The target framebuffer.
		//!\~french		Le tampon d'image ciblé.
		FrameBuffer & m_frameBuffer;
		//!\~english	The render area dimension.
		//!\~french		Les dimensions de l'aire de rendu.
		castor::Size m_size;
		//!\~english	The opaque and transparent passes combination pass.
		//!\~french		La passe de combinaison des passes opaque et transparente.
		std::unique_ptr< FinalCombinePass > m_finalCombinePass;
		//!\~english	The various textures for weighted blend.
		//!\~french		Les diverses textures pour le weighted blend.
		WeightedBlendTextures m_weightedBlendPassResult;
		//!\~english	The weighted blend frame buffer.
		//!\~french		Le tampon d'image pour le weighted blend.
		FrameBufferSPtr m_weightedBlendPassFrameBuffer;
		//!\~english	The attachments between textures and weighted blend frame buffer.
		//!\~french		Les attaches entre les textures et le tampon weighted blend.
		WeightedBlendAttachs m_weightedBlendPassTexAttachs;
	};
}

#endif
