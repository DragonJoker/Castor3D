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
		: public SceneRenderPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	culler			The culler for this pass.
		 *\param[in]	voxelizerUbo	The voxelizer configuration UBO.
		 *\param[in]	voxels			The voxels buffer.
		 *\param[in]	voxelConfig		The voxelizer configuration.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	matrixUbo		L'UBO de matrices de la scène.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	voxelizerUbo	L'UBO de configuration du voxelizer.
		 *\param[in]	voxels			Le tampon de voxels.
		 *\param[in]	voxelConfig		La configuration du voxelizer.
		 */
		C3D_API VoxelizePass( RenderDevice const & device
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, VoxelizerUbo const & voxelizerUbo
			, ashes::Buffer< Voxel > const & voxels
			, VoxelSceneData const & voxelConfig );
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
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Renders nodes.
		 *\param[in]	device	The GPU device.
		 *\param[out]	toWait	The semaphore to wait for.
		 *\~french
		 *\brief		Dessine les noeuds.
		 *\param[in]	device	Le device GPU.
		 *\param[out]	toWait	Le sémaphore à attendre.
		 */
		ashes::Semaphore const & render( ashes::Semaphore const & toWait );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		ashes::Semaphore const & getSemaphore()const
		{
			CU_Require( m_commands.semaphore );
			return *m_commands.semaphore;
		}

		C3D_API ShaderFlags getShaderFlags()const override
		{
			return ShaderFlag::eWorldSpace
				| ShaderFlag::eNormal;
		}
		/**@}*/

		using SceneRenderPass::update;

	private:
		C3D_API ashes::VkDescriptorSetLayoutBindingArray doCreateUboBindings( PipelineFlags const & flags )const override;
		C3D_API ashes::VkDescriptorSetLayoutBindingArray doCreateTextureBindings( PipelineFlags const & flags )const override;
		C3D_API ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		C3D_API ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		C3D_API void doCleanup()override;
		C3D_API bool doInitialise( castor::Size const & size )override;
		C3D_API void doFillUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, BillboardListRenderNode & node )override;
		C3D_API void doFillUboDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, SubmeshRenderNode & node )override;
		C3D_API void doFillTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, BillboardListRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		C3D_API void doFillTextureDescriptor( RenderPipeline const & pipeline
			, ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, SubmeshRenderNode & nodes
			, ShadowMapLightTypeArray const & shadowMaps )override;
		C3D_API void doUpdateFlags( PipelineFlags & flags )const override;
		C3D_API void doUpdatePipeline( RenderPipeline & pipeline )override;
		C3D_API ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		C3D_API ShaderPtr doGetBillboardShaderSource( PipelineFlags const & flags )const override;
		C3D_API ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		C3D_API ShaderPtr doGetPhongPixelShaderSource( PipelineFlags const & flags )const override;
		C3D_API ShaderPtr doGetPbrMRPixelShaderSource( PipelineFlags const & flags )const override;
		C3D_API ShaderPtr doGetPbrSGPixelShaderSource( PipelineFlags const & flags )const override;

	private:
		Scene const & m_scene;
		Camera const & m_camera;
		ashes::Buffer< Voxel > const & m_voxels;
		CommandsSemaphore m_commands;
		ashes::FrameBufferPtr m_frameBuffer;
		VoxelizerUbo const & m_voxelizerUbo;
		VoxelSceneData const & m_voxelConfig;
	};
}

#endif
