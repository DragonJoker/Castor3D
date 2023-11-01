/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelRenderer_H___
#define ___C3D_VoxelRenderer_H___

#include "RenderToTextureModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

#include <RenderGraph/FrameGraphPrerequisites.hpp>

namespace castor3d
{
	struct Texture3DTo2DData
	{
		castor::Point4f gridCenterCellSize;
		uint32_t gridSize;
	};

	class Texture3DTo2D
	{
	public:
		C3D_API Texture3DTo2D( RenderDevice const & device
			, crg::ResourcesCache & resources
			, VkExtent2D const & size
			, CameraUbo const & cameraUbo );
		C3D_API ~Texture3DTo2D();

		C3D_API void createPasses( QueueData const & queueData
			, IntermediateViewArray intermediates );
		C3D_API void initialise( QueueData const & queueData );
		C3D_API void update( CpuUpdater & updater );
		C3D_API crg::SemaphoreWait render( ashes::Queue const & queue
			, crg::SemaphoreWait const & toWait );
		C3D_API void render( ashes::Queue const & queue
			, ashes::VkSemaphoreArray & semaphores
			, ashes::VkPipelineStageFlagsArray & stages );

		Texture const & getTarget()
		{
			return m_target;
		}

	private:
		RenderDevice const & m_device;
		crg::ResourcesCache & m_resources;
		CameraUbo const & m_cameraUbo;
		Texture m_target;
		Texture m_depthBuffer;
		UniformBufferOffsetT< Texture3DTo2DData > m_uniformBuffer;
		ashes::RenderPassPtr m_renderPass;
		ashes::FrameBufferPtr m_frameBuffer;
		SamplerUPtr m_sampler;

		ashes::DescriptorSetLayoutPtr m_descriptorSetLayoutVolume;
		ashes::PipelineLayoutPtr m_pipelineLayoutVolume;
		ProgramModule m_shaderVolume;
		ashes::GraphicsPipelinePtr m_pipelineVolume;
		ashes::DescriptorSetPoolPtr m_descriptorSetPoolVolume;

		ashes::DescriptorSetLayoutPtr m_descriptorSetLayoutSlice;
		ashes::PipelineLayoutPtr m_pipelineLayoutSlice;
		ProgramModule m_shaderSlice;
		ashes::GraphicsPipelinePtr m_pipelineSlice;
		ashes::DescriptorSetPoolPtr m_descriptorSetPoolSlice;

		struct Texture3DToScreen
		{
			Texture3DToScreen();
			Texture3DToScreen( RenderDevice const & device
				, QueueData const & queueData
				, crg::ResourcesCache & resources
				, UniformBufferOffsetT< Texture3DTo2DData > const & uniformBuffer
				, CameraUbo const & cameraUbo
				, IntermediateView const & texture3D
				, ashes::RenderPass const & renderPass
				, ashes::DescriptorSetPool const & descriptorSetPool
				, ashes::FrameBuffer const & frameBuffer
				, ashes::PipelineLayout const & pipelineLayout
				, ashes::GraphicsPipeline const & pipeline
				, SamplerRPtr sampler );

			ashes::DescriptorSetPtr descriptorSet;
			CommandsSemaphore commands;
		};

		IntermediateViewArray m_textures;
		std::vector< Texture3DToScreen > m_texture3DToScreen;
		uint32_t m_index{ 0u };
	};
}

#endif
