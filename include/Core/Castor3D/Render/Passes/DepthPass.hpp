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
		C3D_API DepthPass( castor::String const & prefix
			, RenderDevice const & device
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, SsaoConfig const & ssaoConfig
			, TextureLayoutSPtr depthBuffer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~DepthPass();
		/**
		 *\~english
		 *\brief		Renders nodes.
		 *\param		device		The GPU device.
		 *\param[out]	semaphores	The semaphores to wait for.
		 *\~french
		 *\brief		Dessine les noeuds.
		 *\param[in]	device		Le device GPU.
		 *\param[out]	semaphores	Les sémaphores à attendre.
		 */
		C3D_API ashes::Semaphore const & render( ashes::SemaphoreCRefArray const & semaphores );

		C3D_API TextureFlags getTexturesMask()const override;

		C3D_API ShaderFlags getShaderFlags()const override
		{
			return ShaderFlag::eWorldSpace
				| ShaderFlag::eTangentSpace
				| ShaderFlag::eVelocity;
		}

	public:
		using RenderTechniquePass::update;

	protected:
		/**
		 *\copydoc		castor3d::RenderTechniquePass::doCleanup
		 */
		C3D_API void doCleanup()override;

	private:
		void doUpdateFlags( PipelineFlags & flags )const override;
		void doFillTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		void doFillTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		ashes::VkDescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const override;
		void doUpdatePipeline( RenderPipeline & pipeline)override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const;

	private:
		ashes::FrameBufferPtr m_frameBuffer;
		ashes::CommandBufferPtr m_nodesCommands;
	};
}

#endif
