/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VisibilityPass_H___
#define ___C3D_VisibilityPass_H___

#include "PrepassModule.hpp"

#include "Castor3D/Render/RenderTechniquePass.hpp"

namespace castor3d
{
	class VisibilityPass
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
		 *\param[in]	renderPassDesc		The scene render pass construction data.
		 *\param[in]	techniquePassDesc	The technique render pass construction data.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent				La technique parente.
		 *\param[in]	pass				La frame pass parente.
		 *\param[in]	context				Le contexte de rendu.
		 *\param[in]	graph				Le runnable graph.
		 *\param[in]	device				Le device GPU.
		 *\param[in]	renderPassDesc		Les données de construction de passe de rendu de scène.
		 *\param[in]	techniquePassDesc	Les données de construction de passe de rendu de technique.
		 */
		C3D_API VisibilityPass( RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, RenderNodesPassDesc const & renderPassDesc
			, RenderTechniquePassDesc const & techniquePassDesc );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\copydoc		castor3d::RenderNodesPass::getComponentsMask
		 */
		C3D_API ComponentModeFlags getComponentsMask()const override;
		/**
		 *\copydoc		castor3d::RenderNodesPass::getShaderFlags
		 */
		C3D_API ShaderFlags getShaderFlags()const override;

	private:
		ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const override;
		SceneFlags doAdjustSceneFlags( SceneFlags flags )const override;
		void doFillAdditionalBindings( PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings )const override;
		void doFillAdditionalDescriptor( PipelineFlags const & flags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps )override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const override;

	public:
		C3D_API static castor::String const Type;
	};
}

#endif
