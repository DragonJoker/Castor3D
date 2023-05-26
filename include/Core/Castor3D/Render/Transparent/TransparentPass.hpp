/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredTransparentPass_H___
#define ___C3D_DeferredTransparentPass_H___

#include "TransparentModule.hpp"

#include "Castor3D/Render/RenderTechniquePass.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"

namespace castor3d
{
	class TransparentPass
		: public RenderTechniqueNodesPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	parent				The parent technique.
		 *\param[in]	pass				The parent frame pass.
		 *\param[in]	context				The rendering context.
		 *\param[in]	graph				The runnable graph.
		 *\param[in]	device				The GPU device.
		 *\param[in]	sceneImage			The current render result.
		 *\param[in]	targetImage			The image this pass renders to.
		 *\param[in]	targetDepth			The depth image this pass renders to.
		 *\param[in]	renderPassDesc		The scene render pass construction data.
		 *\param[in]	techniquePassDesc	The technique render pass construction data.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent				La technique parente.
		 *\param[in]	pass				La frame pass parente.
		 *\param[in]	context				Le contexte de rendu.
		 *\param[in]	graph				Le runnable graph.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	sceneImage			Le résultat actuel du rendu.
		 *\param[in]	targetImage			L'image dans laquelle cette passe fait son rendu.
		 *\param[in]	targetDepth			L'image de profondeur dans laquelle cette passe fait son rendu.
		 *\param[in]	renderPassDesc		Les données de construction de passe de rendu de scène.
		 *\param[in]	techniquePassDesc	Les données de construction de passe de rendu de technique.
		 */
		C3D_API TransparentPass( RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, Texture const & sceneImage
			, crg::ImageViewIdArray targetImage
			, crg::ImageViewIdArray targetDepth
			, RenderNodesPassDesc const & renderPassDesc
			, RenderTechniquePassDesc const & techniquePassDesc );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		void setDepthFormat( VkFormat value )
		{
			m_depthFormat = value;
		}
		/**@}*/

	private:
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		void doFillAdditionalBindings( PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings )const override;
		void doFillAdditionalDescriptor( PipelineFlags const & flags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, castor3d::ShadowMapLightTypeArray const & shadowMaps )override;
		ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const override;

	public:
		C3D_API static castor::String const Type;

	private:
		VkFormat m_depthFormat{};
		Texture const & m_sceneImage;
		ashes::CommandBufferPtr m_nodesCommands;
		ashes::FrameBufferPtr m_frameBuffer;
	};
}

#endif
