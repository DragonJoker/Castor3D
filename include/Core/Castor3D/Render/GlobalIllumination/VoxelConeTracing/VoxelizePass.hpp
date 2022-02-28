/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelizePass_H___
#define ___C3D_VoxelizePass_H___

#include "VoxelizeModule.hpp"
#include "Castor3D/Render/Technique/TechniqueModule.hpp"

#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

namespace castor3d
{
	class VoxelizePass
		: public RenderNodesPass
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
		C3D_API VoxelizePass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, MatrixUbo & matrixUbo
			, SceneCuller & culler
			, VoxelizerUbo const & voxelizerUbo
			, ashes::Buffer< Voxel > const & voxels
			, VoxelSceneData const & voxelConfig );
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
		C3D_API void update( CpuUpdater & updater )override;
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater )override;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API ShaderFlags getShaderFlags()const override
		{
			return ShaderFlag::eWorldSpace
				| ShaderFlag::eNormal;
		}
		/**@}*/

	private:
		using RenderNodesPass::update;

		void doFillAdditionalBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings )const override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		void doFillAdditionalDescriptor( ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps )override;
		PassFlags doAdjustPassFlags( PassFlags flags )const override;
		ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const override;
		SceneFlags doAdjustSceneFlags( SceneFlags flags )const override;
		void doUpdatePipeline( RenderPipeline & pipeline )override;
		ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetBillboardShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetHullShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetDomainShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const override;
		ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const override;

	public:
		C3D_API static castor::String const Type;

	private:
		Scene const & m_scene;
		Camera const & m_camera;
		ashes::Buffer< Voxel > const & m_voxels;
		VoxelizerUbo const & m_voxelizerUbo;
		VoxelSceneData const & m_voxelConfig;
	};
}

#endif
