/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelBufferToTexture_H___
#define ___C3D_VoxelBufferToTexture_H___

#include "VoxelizeModule.hpp"

#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

#include <RenderGraph/RunnablePass.hpp>

namespace castor3d
{
	class VoxelBufferToTexture
		: public crg::RunnablePass
	{
	public:
		struct Pipeline
		{
			ShaderModule shader;
			ashes::ComputePipelinePtr pipeline{};
		};

	public:
		/**
		 *\~english
		 *\param[in]	pass		The parent frame pass.
		 *\param[in]	context		The rendering context.
		 *\param[in]	graph		The runnable graph.
		 *\param[in]	device		The GPU device.
		 *\param[in]	vctConfig	The voxelizer configuration.
		 *\~french
		 *\param[in]	pass		La frame pass parente.
		 *\param[in]	context		Le contexte de rendu.
		 *\param[in]	graph		Le runnable graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	vctConfig	La configuration du voxelizer.
		 */
		C3D_API VoxelBufferToTexture( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, VoxelSceneData const & vctConfig );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );

	private:
		void doInitialise();
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );
		VkPipelineStageFlags doGetSemaphoreWaitFlags()const;
		uint32_t doGetPassIndex()const;
		bool doIsComputePass()const;

	private:
		RenderDevice const & m_device;
		VoxelSceneData const & m_vctConfig;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		std::array< Pipeline, 4u > m_pipelines;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		ashes::DescriptorSetPtr m_descriptorSet;
	};
}

#endif
