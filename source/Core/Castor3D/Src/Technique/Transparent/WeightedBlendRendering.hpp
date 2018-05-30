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
			, renderer::TextureView const & depthView
			, renderer::TextureView const & colourView
			, TextureLayoutSPtr velocityTexture
			, castor::Size const & size
			, Scene const & scene );
		/**
		 *\~english
		 *\brief		Renders opaque nodes.
		 *\param[out]	info		Receives the render informations.
		 *\param[out]	scene		The rendered scene.
		 *\param[out]	camera		The viewer camera.
		 *\param[out]	jitter		The jittering value.
		 *\~french
		 *\brief		Dessine les noeuds opaques.
		 *\param[out]	info		Reçoit les informations de rendu.
		 *\param[out]	scene		La scène rendue.
		 *\param[out]	camera		La caméra par laquelle la scène est rendue.
		 *\param[out]	jitter		La valeur de jittering.
		 */
		void update( RenderInfo & info
			, Scene const & scene
			, Camera const & camera
			, castor::Point2r const & jitter );
		/**
		 *\~english
		 *\brief		Renders opaque nodes.
		 *\param[out]	info	Receives the render informations.
		 *\param[out]	scene	The rendered scene.
		 *\~french
		 *\brief		Dessine les noeuds opaques.
		 *\param[out]	info	Reçoit les informations de rendu.
		 *\param[out]	scene	La scène rendue.
		 */
		renderer::Semaphore const & render( RenderInfo & info
			, Scene const & scene
			, renderer::Semaphore const & toWait );
		/**
		 *\~english
		 *\brief		Displays debug data on screen.
		 *\~french
		 *\brief		Dessine les données de débogage sur l'écran.
		 */
		void debugDisplay();
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( RenderTechniqueVisitor & visitor );

	private:
		Engine & m_engine;
		TransparentPass & m_transparentPass;
		castor::Size m_size;
		renderer::TextureViewPtr m_depthView;
		renderer::TexturePtr m_accumulation;
		renderer::TextureViewPtr m_accumulationView;
		renderer::TexturePtr m_revealage;
		renderer::TextureViewPtr m_revealageView;
		WeightedBlendTextures m_weightedBlendPassResult;
		renderer::FrameBufferPtr m_frameBuffer;
		FinalCombinePass m_finalCombinePass;
	};
}

#endif
