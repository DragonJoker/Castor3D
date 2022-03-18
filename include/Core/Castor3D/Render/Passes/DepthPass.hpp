/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DepthPass_H___
#define ___C3D_DepthPass_H___

#include "PassesModule.hpp"

#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"

namespace castor3d
{
	class DepthPass
		: public RenderTechniquePass
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
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parent			La technique parente.
		 *\param[in]	pass			La frame pass parente.
		 *\param[in]	context			Le contexte de rendu.
		 *\param[in]	graph			Le runnable graph.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	renderPassDesc	La description de la passe de rendu.
		 */
		C3D_API DepthPass( RenderTechnique * parent
			, crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, SsaoConfig const & ssaoConfig
			, RenderNodesPassDesc const & renderPassDesc );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::getTexturesMask
		 */
		C3D_API TextureFlags getTexturesMask()const override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::getShaderFlags
		 */
		C3D_API ShaderFlags getShaderFlags()const override
		{
			return ShaderFlag::eWorldSpace
				| ShaderFlag::eTangentSpace
				| ShaderFlag::eVelocity;
		}

	private:
		PassFlags doAdjustPassFlags( PassFlags flags )const override;
		ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const override;
		SceneFlags doAdjustSceneFlags( SceneFlags flags )const override;
		void doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const override
		{
		}
		void doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps )override
		{
		}
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const override;

	public:
		C3D_API static castor::String const Type;
	};
}

#endif
