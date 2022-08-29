/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelSecondaryBounce_H___
#define ___C3D_VoxelSecondaryBounce_H___

#include "VoxelizeModule.hpp"

#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

#include <RenderGraph/RunnablePass.hpp>

namespace castor3d
{
	class VoxelSecondaryBounce
		: public crg::RunnablePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	pass			The parent frame pass.
		 *\param[in]	context			The rendering context.
		 *\param[in]	graph			The runnable graph.
		 *\param[in]	device			The GPU device.
		 *\param[in]	voxelConfig		The voxelizer configuration.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	pass			La frame pass parente.
		 *\param[in]	context			Le contexte de rendu.
		 *\param[in]	graph			Le runnable graph.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	voxelConfig		La configuration du voxelizer.
		 */
		C3D_API VoxelSecondaryBounce( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, VoxelSceneData const & voxelConfig );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );

	private:
		void doInitialise();
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );
		bool doIsComputePass()const;

	private:
		VoxelSceneData const & m_vctConfig;
		ShaderModule m_shader;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::ComputePipelinePtr m_pipeline;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		ashes::DescriptorSetPtr m_descriptorSet;
	};
}

#endif
