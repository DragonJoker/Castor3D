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
		OpaquePass( MatrixUbo & matrixUbo
			, SceneCuller & culler
			, SsaoConfig const & config );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OpaquePass();
		/**
		 *\~english
		 *\brief		Initialises the render pass.
		 *\param[in]	gpResult	The geometry pass buffers.
		 *\~french
		 *\brief		Initialise la passe de rendu.
		 *\param[in]	gpResult	Les tampons de la geometry pass.
		 */
		void initialiseRenderPass( OpaquePassResult const & gpResult );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\~english
		 *\brief		Renders transparent nodes.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine les noeuds transparents.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		ashes::Semaphore const & render( ashes::Semaphore const & toWait );

		C3D_API TextureFlags getTexturesMask()const override;

	private:
		/**
		*\copydoc		castor3d::RenderPass::doCreateUboBindings
		*/
		ashes::VkDescriptorSetLayoutBindingArray doCreateUboBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdateFlags
		 */
		void doUpdateFlags( PipelineFlags & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		void doUpdatePipeline( RenderPipeline & pipeline )override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateDepthStencilState
		 */
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateBlendState
		 */
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetVertexShaderSource
		 */
		ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetGeometryShaderSource
		 */
		ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPhongPixelShaderSource
		 */
		ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrSGPixelShaderSource
		 */
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
