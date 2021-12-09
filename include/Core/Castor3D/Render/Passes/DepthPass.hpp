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
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	prefix		This pass name's prefix.
		 *\param		device		The GPU device.
		 *\param[in]	matrixUbo	The scene matrix UBO.
		 *\param[in]	culler		The render pass culler.
		 *\param[in]	ssaoConfig	The SSAO configuration.
		 *\param[in]	depthBuffer	The target depth buffer.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	prefix		Le préfixe du nom de cette passe.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	ssaoConfig	La configuration du SSAO.
		 *\param[in]	depthBuffer	Le tampon de profondeur cible.
		 */
		C3D_API DepthPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, SsaoConfig const & ssaoConfig
			, SceneRenderPassDesc const & renderPassDesc );

		C3D_API TextureFlags getTexturesMask()const override;

		C3D_API ShaderFlags getShaderFlags()const override
		{
			return ShaderFlag::eWorldSpace
				| ShaderFlag::eTangentSpace
				| ShaderFlag::eVelocity
				| ShaderFlag::eNodeId;
		}

	private:
		void doUpdateFlags( PipelineFlags & flags )const override;
		void doUpdatePipeline( RenderPipeline & pipeline)override;
		void doFillAdditionalBindings( PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings )const override
		{
		}
		void doFillAdditionalDescriptor( RenderPipeline const & pipeline
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, BillboardRenderNode & node
			, ShadowMapLightTypeArray const & shadowMaps )override
		{
		}
		void doFillAdditionalDescriptor( RenderPipeline const & pipeline
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, SubmeshRenderNode & node
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
