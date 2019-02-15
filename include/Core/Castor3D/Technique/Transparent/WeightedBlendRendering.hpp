/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WeightedBlendRendering_H___
#define ___C3D_WeightedBlendRendering_H___

#include "Castor3D/Technique/Transparent/FinalCombinePass.hpp"
#include "Castor3D/Technique/Transparent/TransparentPass.hpp"

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
		 *\param[in]	depthView		The target depth buffer.
		 *\param[in]	colourView		The target colour buffer.
		 *\param[in]	velocityTexture	The velocity buffer.
		 *\param[in]	size			The render dimensions.
		 *\param[in]	scene			The rendered scene.
		 *\param[in]	hdrConfigUbo	The HDR configuration UBO.
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	transparentPass	La passe de rendu des noeuds transparents.
		 *\param[in]	depthView		Le tampon de profondeur cible.
		 *\param[in]	colourView		Le tampon de couleurs cible.
		 *\param[in]	velocityTexture	Le tampon de vélocité.
		 *\param[in]	size			Les dimensions du rendu.
		 *\param[in]	scene			La scène rendue.
		 *\param[in]	hdrConfigUbo	L'UBO de configuration HDR.
		 */
		WeightedBlendRendering( Engine & engine
			, TransparentPass & transparentPass
			, ashes::TextureView const & depthView
			, ashes::TextureView const & colourView
			, TextureLayoutSPtr velocityTexture
			, castor::Size const & size
			, Scene const & scene
			, HdrConfigUbo & hdrConfigUbo );
		/**
		 *\~english
		 *\brief		Renders opaque nodes.
		 *\param[out]	info		Receives the render informations.
		 *\param[in]	scene		The rendered scene.
		 *\param[in]	camera		The viewer camera.
		 *\param[in]	jitter		The jittering value.
		 *\~french
		 *\brief		Dessine les noeuds opaques.
		 *\param[out]	info		Reçoit les informations de rendu.
		 *\param[in]	scene		La scène rendue.
		 *\param[in]	camera		La caméra par laquelle la scène est rendue.
		 *\param[in]	jitter		La valeur de jittering.
		 */
		void update( RenderInfo & info
			, Scene const & scene
			, Camera const & camera
			, castor::Point2r const & jitter );
		/**
		 *\~english
		 *\brief		Renders opaque nodes.
		 *\param[out]	info	Receives the render informations.
		 *\param[in]	scene	The rendered scene.
		 *\param[in]	toWait	The semaphore to wait for.
		 *\~french
		 *\brief		Dessine les noeuds opaques.
		 *\param[out]	info	Reçoit les informations de rendu.
		 *\param[in]	scene	La scène rendue.
		 *\param[in]	toWait	Le sémaphore à attendre.
		 */
		ashes::Semaphore const & render( RenderInfo & info
			, Scene const & scene
			, ashes::Semaphore const & toWait );
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
		ashes::TextureViewPtr m_depthView;
		ashes::TexturePtr m_accumulation;
		ashes::TextureViewPtr m_accumulationView;
		ashes::TexturePtr m_revealage;
		ashes::TextureViewPtr m_revealageView;
		WeightedBlendTextures m_weightedBlendPassResult;
		FinalCombinePass m_finalCombinePass;
	};
}

#endif
