/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CommandsSemaphore_H___
#define ___C3D_CommandsSemaphore_H___

#include "PassesModule.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <RenderGraph/FrameGraphPrerequisites.hpp>

namespace c3d
{
	struct CommandsSemaphore
	{
		C3D_API explicit CommandsSemaphore( ashes::CommandBufferPtr commandBuffer = nullptr
			, ashes::SemaphorePtr semaphore = nullptr );
		C3D_API CommandsSemaphore( RenderDevice const & device
			, QueueData const & queueData
			, String const & name
			, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY );

		C3D_API void submit( ashes::Queue const & queue
			, PipelineStageFlags stage = PipelineStageFlags::eColorAttachmentOutput )const;
		C3D_API ashes::Semaphore const & submit( ashes::Queue const & queue
			, ashes::Semaphore const & toWait
			, PipelineStageFlags stage = PipelineStageFlags::eColorAttachmentOutput )const;
		C3D_API ashes::Semaphore const & submit( ashes::Queue const & queue
			, SemaphoreWait const & toWait )const;
		C3D_API ashes::Semaphore const & submit( ashes::Queue const & queue
			, SemaphoreWaitArray const & toWait )const;
		C3D_API void submit( ashes::Queue const & queue
			, ashes::VkSemaphoreArray & semaphores
			, ashes::VkPipelineStageFlagsArray & stages )const;

		ashes::CommandBufferPtr commandBuffer;
		ashes::SemaphorePtr semaphore;
	};
}

#endif
