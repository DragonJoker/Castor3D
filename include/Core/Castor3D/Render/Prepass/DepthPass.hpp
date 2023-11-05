/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DepthPass_H___
#define ___C3D_DepthPass_H___

#include "PrepassModule.hpp"

#include "Castor3D/Render/RenderTechniquePass.hpp"

namespace castor3d
{
	class DepthPass
		: public RenderTechniqueNodesPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	parent			The parent technique
		 *\param[in]	pass			The parent frame pass.
		 *\param[in]	context			The rendering context.
		 *\param[in]	graph			The runnable graph.
		 *\param		device			The GPU device.
		 *\param		targetDepth		The target depth textures.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	renderPassDesc	The render pass description.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parent			La technique parente.
		 *\param[in]	pass			La frame pass parente.
		 *\param[in]	context			Le contexte de rendu.
		 *\param[in]	graph			Le runnable graph.
		 *\param[in]	device			Le device GPU.
		 *\param		targetDepth		Les textures profondeur cibles.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	renderPassDesc	La description de la passe de rendu.
		 */
		C3D_API DepthPass( RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, crg::ImageViewIdArray targetDepth
			, SsaoConfig const & ssaoConfig
			, RenderNodesPassDesc const & renderPassDesc );
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
			, ShadowMapLightTypeArray const & shadowMaps
			, ShadowBuffer const * shadowBuffer )override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		void doGetGeometryShaderSource( PipelineFlags const & flags
			, ast::ShaderBuilder & builder )const override;
		void doGetPixelShaderSource( PipelineFlags const & flags
			, ast::ShaderBuilder & builder )const override;

	public:
		C3D_API static castor::String const Type;
	};
}

#endif
