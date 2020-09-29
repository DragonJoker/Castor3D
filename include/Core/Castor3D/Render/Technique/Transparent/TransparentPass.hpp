/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DeferredTransparentPass_H___
#define ___C3D_DeferredTransparentPass_H___

#include "TransparentModule.hpp"

#include "Castor3D/Render/ShadowMap/ShadowMapDirectional.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapPoint.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapSpot.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"

namespace castor3d
{
	class TransparentPass
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
		C3D_API TransparentPass( MatrixUbo & matrixUbo
			, SceneCuller & culler
			, SsaoConfig const & config );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~TransparentPass();
		/**
		 *\~english
		 *\brief		Initialises the render pass.
		 *\param[in]	wbpResult	The accumulation pass buffers.
		 *\~french
		 *\brief		Initialise la passe de rendu.
		 *\param[in]	wbpResult	Les tampons de la passe d'accumulation.
		 */
		C3D_API void initialiseRenderPass( RenderDevice const & device
			, TransparentPassResult const & wbpResult );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor )override;
		/**
		 *\~english
		 *\brief		Renders transparent nodes.
		 *\param[in]	toWait	The semaphore to wait for.
		 *\~french
		 *\brief		Dessine les noeuds transparents.
		 *\param[in]	toWait	Le sémaphore à attendre.
		 */
		C3D_API ashes::Semaphore const & render( RenderDevice const & device
			, ashes::Semaphore const & toWait );

		C3D_API TextureFlags getTexturesMask()const override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline void setDepthFormat( VkFormat value )
		{
			m_depthFormat = value;
		}
		/**@}*/

	private:
		/**
		 *\copydoc		castor3d::RenderPass::doInitialise
		 */
		bool doInitialise( RenderDevice const & device
			, castor::Size const & size )override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateDepthStencilState
		 */
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateBlendState
		 */
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		void doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		void doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateTextureBindings
		 */
		ashes::VkDescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetVertexShaderSource
		 */
		ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
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
		/**
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		void doUpdatePipeline( RenderPipeline & pipeline )override;

	private:
		VkFormat m_depthFormat{};
		ashes::CommandBufferPtr m_nodesCommands;
		ashes::FrameBufferPtr m_frameBuffer;
	};
}

#endif
