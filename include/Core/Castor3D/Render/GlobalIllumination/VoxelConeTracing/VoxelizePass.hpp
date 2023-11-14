/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelizePass_H___
#define ___C3D_VoxelizePass_H___

#include "VoxelizeModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

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
		 *\param[in]	pass			The parent frame pass.
		 *\param[in]	context			The rendering context.
		 *\param[in]	graph			The runnable graph.
		 *\param[in]	device			The GPU device.
		 *\param[in]	cameraUbo		The scene matrices UBO.
		 *\param[in]	sceneUbo		The scene UBO.
		 *\param[in]	camera			The camera for this pass.
		 *\param[in]	culler			The culler for this pass.
		 *\param[in]	voxelizerUbo	The voxelizer configuration UBO.
		 *\param[in]	voxels			The voxels buffer.
		 *\param[in]	voxelConfig		The voxelizer configuration.
		 *\param[in]	isStatic		Tells if this pass is for static nodes.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass			La frame pass parente.
		 *\param[in]	context			Le contexte de rendu.
		 *\param[in]	graph			Le runnable graph.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	cameraUbo		L'UBO de matrices de la scène.
		 *\param[in]	sceneUbo		L'UBO de scène.
		 *\param[in]	camera			La caméra pour cette passe.
		 *\param[in]	culler			Le culler pour cette passe.
		 *\param[in]	voxelizerUbo	L'UBO de configuration du voxelizer.
		 *\param[in]	voxels			Le tampon de voxels.
		 *\param[in]	voxelConfig		La configuration du voxelizer.
		 *\param[in]	isStatic		Dit si cette passe est pour les noeuds statiques.
		 */
		C3D_API VoxelizePass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, CameraUbo const & cameraUbo
			, SceneUbo const & sceneUbo
			, Camera const & camera
			, SceneCuller & culler
			, VoxelizerUbo const & voxelizerUbo
			, ashes::Buffer< Voxel > const & voxels
			, VctConfig const & voxelConfig
			, bool isStatic );
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
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API bool isPassEnabled()const override;
		C3D_API ShaderFlags getShaderFlags()const override;
		/**@}*/

		C3D_API void setUpToDate();

	private:
		using RenderNodesPass::update;

		void doFillAdditionalBindings( PipelineFlags const & flags
			, ashes::VkDescriptorSetLayoutBindingArray & bindings )const override;
		ashes::PipelineDepthStencilStateCreateInfo doCreateDepthStencilState( PipelineFlags const & flags )const override;
		ashes::PipelineColorBlendStateCreateInfo doCreateBlendState( PipelineFlags const & flags )const override;
		void doFillAdditionalDescriptor( PipelineFlags const & flags
			, ashes::WriteDescriptorSetArray & descriptorWrites
			, ShadowMapLightTypeArray const & shadowMaps
			, castor3d::ShadowBuffer const * shadowBuffer )override;
		SubmeshComponentCombine doAdjustSubmeshComponents( SubmeshComponentCombine submeshCombine )const override;
		ProgramFlags doAdjustProgramFlags( ProgramFlags flags )const override;
		SceneFlags doAdjustSceneFlags( SceneFlags flags )const override;
		void doGetVertexShaderSource( PipelineFlags const & flags
			, ast::ShaderBuilder & builder )const override;
		void doGetBillboardShaderSource( PipelineFlags const & flags
			, ast::ShaderBuilder & builder )const override;
		void doGetGeometryShaderSource( PipelineFlags const & flags
			, ast::ShaderBuilder & builder )const override;
		void doGetPixelShaderSource( PipelineFlags const & flags
			, ast::ShaderBuilder & builder )const override;

	public:
		C3D_API static castor::String const Type;

	private:
		Scene const & m_scene;
		ashes::Buffer< Voxel > const & m_voxels;
		VoxelizerUbo const & m_voxelizerUbo;
		VctConfig const & m_voxelConfig;
		bool m_outOfDate{ true };
	};
}

#endif
