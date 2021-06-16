/*
See LICENSE file in root folder
*/
#ifndef ___C3D_WeightedBlendRendering_H___
#define ___C3D_WeightedBlendRendering_H___

#include "TransparentModule.hpp"

#include "Castor3D/Render/Technique/Transparent/TransparentPass.hpp"

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
		 *\param[in]	transparentPassDesc		The transparent nodes render pass.
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
		 *\param[in]	transparentPassDesc		La passe de rendu des noeuds transparents.
		 *\param[in]	transparentPassResult	Le résultat du Weighted Blended OIT.
		 *\param[in]	targetColourView		Le tampon de couleurs cible.
		 *\param[in]	size					Les dimensions du rendu.
		 *\param[in]	sceneUbo				L'UBO de scène.
		 *\param[in]	hdrConfigUbo			L'UBO de configuration HDR.
		 *\param[in]	gpInfoUbo				L'UBO de la passe géométrique.
		 */
		WeightedBlendRendering( crg::FrameGraph & graph
			, RenderDevice const & device
			, crg::FramePass const & transparentPassDesc
			, TransparentPassResult const & transparentPassResult
			, crg::ImageViewId const & targetColourView
			, castor::Size const & size
			, SceneUbo & sceneUbo
			, HdrConfigUbo const & hdrConfigUbo
			, GpInfoUbo const & gpInfoUbo );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( RenderTechniqueVisitor & visitor );

		crg::FramePass const & getLastPass()const
		{
			return m_finalCombinePassDesc;
		}

	private:
		crg::FramePass & doCreateFinalCombine( crg::FrameGraph & graph
			, crg::FramePass const & transparentPassDesc
			, crg::ImageViewId const & targetColourView
			, SceneUbo & sceneUbo
			, HdrConfigUbo const & hdrConfigUbo
			, GpInfoUbo const & gpInfoUbo );

	private:
		RenderDevice const & m_device;
		TransparentPassResult const & m_transparentPassResult;
		crg::ImageViewId m_depthOnlyView;
		castor::Size m_size;
		ShaderModule m_vertexShader;
		ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass & m_finalCombinePassDesc;
	};
}

#endif
