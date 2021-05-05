/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WeightedBlendRendering_H___
#define ___C3D_WeightedBlendRendering_H___

#include "TransparentModule.hpp"

#include "Castor3D/Render/Technique/Transparent/TransparentPass.hpp"
#include "Castor3D/Render/Technique/Transparent/TransparentResolvePass.hpp"

namespace castor3d
{
	class WeightedBlendRendering
	{
	public:
		/**
		 *\~english
		 *\brief		Initialises deferred rendering related stuff.
		 *\param[in]	device			The GPU device.
		 *\param[in]	transparentPass	The transparent nodes render pass.
		 *\param[in]	depthView		The target depth buffer.
		 *\param[in]	colourView		The target colour buffer.
		 *\param[in]	velocityTexture	The velocity buffer.
		 *\param[in]	size			The render dimensions.
		 *\param[in]	scene			The rendered scene.
		 *\param[in]	hdrConfigUbo	The HDR configuration UBO.
		 *\param[in]	gpInfoUbo		The geometry pass UBO.
		 *\param[in]	lpvResult		The LPV result.
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	transparentPass	La passe de rendu des noeuds transparents.
		 *\param[in]	depthView		Le tampon de profondeur cible.
		 *\param[in]	colourView		Le tampon de couleurs cible.
		 *\param[in]	velocityTexture	Le tampon de vélocité.
		 *\param[in]	size			Les dimensions du rendu.
		 *\param[in]	scene			La scène rendue.
		 *\param[in]	hdrConfigUbo	L'UBO de configuration HDR.
		 *\param[in]	gpInfoUbo		L'UBO de la passe géométrique.
		 *\param[in]	lpvResult		Le résultat du LPV.
		 */
		WeightedBlendRendering( RenderDevice const & device
			, TransparentPass & transparentPass
			, TransparentPassResult const & transparentPassResult
			, ashes::ImageView const & colourView
			, castor::Size const & size
			, Scene const & scene
			, HdrConfigUbo const & hdrConfigUbo
			, GpInfoUbo const & gpInfoUbo
			, LightVolumePassResult const & lpvResult );
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
		 *\brief		Renders opaque nodes.
		 *\param[in]	device	The GPU device.
		 *\param[in]	scene	The rendered scene.
		 *\param[in]	toWait	The semaphore to wait for.
		 *\~french
		 *\brief		Dessine les noeuds opaques.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	scene	La scène rendue.
		 *\param[in]	toWait	Le sémaphore à attendre.
		 */
		ashes::Semaphore const & render( Scene const & scene
			, ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( RenderTechniqueVisitor & visitor );

	private:
		RenderDevice const & m_device;
		TransparentPass & m_transparentPass;
		TransparentPassResult const & m_transparentPassResult;
		castor::Size m_size;
		TransparentResolvePass m_finalCombinePass;
	};
}

#endif
