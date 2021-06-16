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
			ashes::ComputePipelinePtr pipeline;
		};

	public:
		/**
		 *\~english
		 *\param[in]	device		The GPU device.
		 *\param[in]	vctConfig	The voxelizer configuration.
		 *\param[in]	voxels		The voxels buffer.
		 *\param[in]	result		The resulting texture.
		 *\~french
		 *\param[in]	device		Le device GPU.
		 *\param[in]	vctConfig	La configuration du voxelizer.
		 *\param[in]	voxels		Le tampon de voxels.
		 *\param[in]	result		La texture résultante.
		 */
		C3D_API VoxelBufferToTexture( crg::FramePass const & pass
			, crg::GraphContext const & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, VoxelSceneData const & vctConfig );
		C3D_API ~VoxelBufferToTexture();
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );

	private:
		void doInitialise( uint32_t index )override;
		void doRecordInto( VkCommandBuffer commandBuffer
			, uint32_t index )override;
		VkPipelineStageFlags doGetSemaphoreWaitFlags()const override;
		uint32_t doGetPassIndex()const override;
		bool doIsComputePass()const override;

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
