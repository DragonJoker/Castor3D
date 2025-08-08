/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WeightedBlendRendering_H___
#define ___C3D_WeightedBlendRendering_H___

#include "TransparentModule.hpp"

#include "Castor3D/Render/Transparent/TransparentPass.hpp"

namespace c3d
{
	class WeightedBlendRendering
	{
	public:
		/**
		 *\~english
		 *\brief		Initialises weighted blended rendering related stuff.
		 *\param[in]	graph					The frame graph.
		 *\param[in]	device					The GPU device.
		 *\param[in]	progress				The progress bar.
		 *\param[in]	enabled					The pass' enabled status.
		 *\param[in]	depthObj				The scene depth and objects image.
		 *\param[in]	transparentPassResult	The Weighted Blended OIT result.
		 *\param[in]	targetColour			The target colour buffer.
		 *\param[in]	size					The render dimensions.
		 *\param[in]	sceneUbo				The scene UBO.
		 *\param[in]	renderUbo				The render configuration UBO.
		 *\param[in]	cameraUbo				The camera UBO.
		 *\~french
		 *\brief		Initialise les données liées au weighted blended rendering.
		 *\param[in]	graph					Le frame graph.
		 *\param[in]	device					Le device GPU.
		 *\param[in]	progress				La barre de progression.
		 *\param[in]	enabled					L'état d'activation de la passe.
		 *\param[in]	depthObj				L'image de profondeur et d'objets de la scène.
		 *\param[in]	transparentPassResult	Le résultat du Weighted Blended OIT.
		 *\param[in]	targetColour			Le tampon de couleurs cible.
		 *\param[in]	size					Les dimensions du rendu.
		 *\param[in]	sceneUbo				L'UBO de scène.
		 *\param[in]	renderUbo				L'UBO de configuration du rendu.
		 *\param[in]	cameraUbo				L'UBO de la caméra.
		 */
		WeightedBlendRendering( crg::FramePassGroup & graph
			, RenderDevice const & device
			, ProgressBar * progress
			, bool & enabled
			, Texture const & depthObj
			, TransparentPassResult const & transparentPassResult
			, Texture & targetColour
			, Size const & size
			, CameraUbo const & cameraUbo
			, SceneUbo const & sceneUbo
			, RenderUbo const & renderUbo );
		/**
		 *\copydoc		RenderTechniquePass::accept
		 */
		void accept( RenderTechniqueVisitor & visitor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		static uint32_t countInitialisationSteps()noexcept;
		/**@}*/

	private:
		RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		bool & m_enabled;
		TransparentPassResult const & m_transparentPassResult;
		Size m_size;
		ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
