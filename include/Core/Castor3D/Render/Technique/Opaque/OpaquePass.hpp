/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredOpaquePass_H___
#define ___C3D_DeferredOpaquePass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"

namespace castor3d
{
	class OpaquePass
		: public RenderTechniquePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\param[in]	config		The SSAO configuration.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 *\param[in]	config		La configuration du SSAO.
		 */
		C3D_API OpaquePass( MatrixUbo & matrixUbo
			, SceneCuller & culler
			, SsaoConfig const & config );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~OpaquePass();
		/**
		 *\~english
		 *\brief		Initialises the render pass.
		 *\param[in]	device		The GPU device.
		 *\param[in]	gpResult	The geometry pass buffers.
		 *\~french
		 *\brief		Initialise la passe de rendu.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	gpResult	Les tampons de la geometry pass.
		 */
		C3D_API void initialiseRenderPass( RenderDevice const & device
			, OpaquePassResult const & gpResult );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\~english
		 *\brief		Renders transparent nodes.
		 *\param[in]	device	The GPU device.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine les noeuds transparents.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		C3D_API ashes::Semaphore const & render( RenderDevice const & device
			, ashes::Semaphore const & toWait );

		C3D_API TextureFlags getTexturesMask()const override;

	private:
		void doUpdateFlags( PipelineFlags & flags )const override;
		void doUpdatePipeline( RenderPipeline & pipeline )override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
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
		ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const override;

	private:
		static castor::String const Output1;
		static castor::String const Output2;
		static castor::String const Output3;
		static castor::String const Output4;
		static castor::String const Output5;

		ashes::FrameBufferPtr m_frameBuffer;
		ashes::CommandBufferPtr m_nodesCommands;
	};
}

#endif
