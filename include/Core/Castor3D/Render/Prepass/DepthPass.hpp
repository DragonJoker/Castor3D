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
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	renderPassDesc	The render pass description.
		 *\param[in]	deferred		\p true if the opaque pass is using deferred rendering.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parent			La technique parente.
		 *\param[in]	pass			La frame pass parente.
		 *\param[in]	context			Le contexte de rendu.
		 *\param[in]	graph			Le runnable graph.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	renderPassDesc	La description de la passe de rendu.
		 *\param[in]	deferred		\p true si la passe opaque utilise le deferred rendering.
		 */
		C3D_API DepthPass( RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, SsaoConfig const & ssaoConfig
			, RenderNodesPassDesc const & renderPassDesc
			, bool deferred );
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
			, ashes::VkDescriptorSetLayoutBindingArray & bindings )const override
		{
		}
		void doFillAdditionalDescriptor( PipelineFlags const & flags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, uint32_t passIndex )override
		{
		}
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const override;

	public:
		C3D_API static castor::String const Type;
		bool m_deferred{ false };
	};
}

#endif
