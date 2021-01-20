/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelizePass_H___
#define ___C3D_VoxelizePass_H___

#include "VoxelizeModule.hpp"
#include "Castor3D/Render/Technique/TechniqueModule.hpp"

#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

namespace castor3d
{
	class VoxelizePass
		: public RenderPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 */
		C3D_API VoxelizePass( Engine & engine
			, RenderDevice const & device
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, UniformBufferOffsetT< VoxelizerUboConfiguration > const & voxelizerUbo
			, TextureUnit const & result
			, uint32_t voxelGridSize );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~VoxelizePass();
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::update
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Renders nodes.
		 *\param[out]	toWait	The semaphore to wait for.
		 *\~french
		 *\brief		Dessine les noeuds.
		 *\param[out]	toWait	Le sémaphore à attendre.
		 */
		ashes::Semaphore const & render( RenderDevice const & device
			, ashes::Semaphore const & toWait );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline ashes::Semaphore const & getSemaphore()const
		{
			CU_Require( m_commands.semaphore );
			return *m_commands.semaphore;
		}
		/**@}*/

		using RenderPass::update;

	private:
		/**
		 *\copydoc		castor3d::RenderPass::doCreateUboBindings
		 */
		C3D_API ashes::VkDescriptorSetLayoutBindingArray doCreateUboBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateTextureBindings
		 */
		C3D_API ashes::VkDescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateDepthStencilState
		 */
		C3D_API ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doCreateBlendState
		 */
		C3D_API ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::doCleanup
		 */
		C3D_API void doCleanup( RenderDevice const & device )override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::doInitialise
		 */
		C3D_API bool doInitialise( RenderDevice const & device
			, castor::Size const & size )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillUboDescriptor
		 */
		C3D_API void doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
			, BillboardListRenderNode & node )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillUboDescriptor
		 */
		C3D_API void doFillUboDescriptor( ashes::DescriptorSetLayout const & layout
			, SubmeshRenderNode & node )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		C3D_API void doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		/**
		 *\copydoc		castor3d::RenderPass::doFillTextureDescriptor
		 */
		C3D_API void doFillTextureDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdateFlags
		 */
		C3D_API void doUpdateFlags( PipelineFlags & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doUpdatePipeline
		 */
		C3D_API void doUpdatePipeline( RenderPipeline & pipeline )override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetVertexShaderSource
		 */
		C3D_API ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetBillboardShaderSource
		 */
		C3D_API ShaderPtr doGetBillboardShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetGeometryShaderSource
		 */
		C3D_API ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPhongPixelShaderSource
		 */
		C3D_API ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrMRPixelShaderSource
		 */
		C3D_API ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const override;
		/**
		 *\copydoc		castor3d::RenderPass::doGetPbrSGPixelShaderSource
		 */
		C3D_API ShaderPtr doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const override;

	private:
		Scene const & m_scene;
		Camera const & m_camera;
		TextureUnit const & m_result;
		CommandsSemaphore m_commands;
		ashes::FrameBufferPtr m_frameBuffer;
		uint32_t m_voxelGridSize;
		UniformBufferOffsetT< VoxelizerUboConfiguration > const & m_voxelizerUbo;
	};
}

#endif
