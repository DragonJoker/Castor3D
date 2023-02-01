/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WeightedBlendRendering_H___
#define ___C3D_WeightedBlendRendering_H___

#include "TransparentModule.hpp"

#include "Castor3D/Render/Transparent/TransparentPass.hpp"

namespace castor3d
{
	class WeightedBlendRendering
	{
	public:
		/**
		 *\~english
		 *\brief		Initialises deferred rendering related stuff.
		 *\param[in]	graph					The frame graph.
		 *\param[in]	device					The GPU device.
		 *\param[in]	progress				The progress bar.
		 *\param[in]	enabled					The pass' enabled status.
		 *\param[in]	transparentPassDesc		The transparent nodes render pass.
		 *\param[in]	depth					The scene depth image.
		 *\param[in]	transparentPassResult	The Weighted Blended OIT result.
		 *\param[in]	targetColourView		The target colour buffer.
		 *\param[in]	size					The render dimensions.
		 *\param[in]	sceneUbo				The scene UBO.
		 *\param[in]	hdrConfigUbo			The HDR configuration UBO.
		 *\param[in]	gpInfoUbo				The geometry pass UBO.
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\param[in]	graph					Le frame graph.
		 *\param[in]	device					Le device GPU.
		 *\param[in]	progress				La barre de progression.
		 *\param[in]	enabled					L'état d'activation de la passe.
		 *\param[in]	transparentPassDesc		La passe de rendu des noeuds transparents.
		 *\param[in]	depth					L'image de profondeur de la scène.
		 *\param[in]	transparentPassResult	Le résultat du Weighted Blended OIT.
		 *\param[in]	targetColourView		Le tampon de couleurs cible.
		 *\param[in]	size					Les dimensions du rendu.
		 *\param[in]	sceneUbo				L'UBO de scène.
		 *\param[in]	hdrConfigUbo			L'UBO de configuration HDR.
		 *\param[in]	gpInfoUbo				L'UBO de la passe géométrique.
		 */
		WeightedBlendRendering( crg::FramePassGroup & graph
			, RenderDevice const & device
			, ProgressBar * progress
			, bool & enabled
			, crg::FramePass const & transparentPassDesc
			, Texture const & depthObj
			, TransparentPassResult const & transparentPassResult
			, crg::ImageViewIdArray const & targetColourView
			, castor::Size const & size
			, CameraUbo const & cameraUbo
			, SceneUbo const & sceneUbo
			, HdrConfigUbo const & hdrConfigUbo
			, GpInfoUbo const & gpInfoUbo );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
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
		crg::FramePass const & getLastPass()const
		{
			return m_finalCombinePassDesc;
		}

		static uint32_t countInitialisationSteps()
		{
			uint32_t result = 0u;
			result += 2;// m_finalCombinePassDesc;
			return result;
		}
		/**@}*/

	private:
		crg::FramePass & doCreateFinalCombine( crg::FramePassGroup & graph
			, crg::FramePass const & transparentPassDesc
			, crg::ImageViewId const & depthObj
			, crg::ImageViewIdArray const & targetColourView
			, CameraUbo const & cameraUbo
			, SceneUbo const & sceneUbo
			, HdrConfigUbo const & hdrConfigUbo
			, GpInfoUbo const & gpInfoUbo
			, ProgressBar * progress );

	private:
		RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		bool & m_enabled;
		TransparentPassResult const & m_transparentPassResult;
		castor::Size m_size;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass & m_finalCombinePassDesc;
	};
}

#endif
