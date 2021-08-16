/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CommandsSemaphore_H___
#define ___C3D_CommandsSemaphore_H___

#include "PassesModule.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <RenderGraph/FrameGraphPrerequisites.hpp>

namespace castor3d
{
	struct CommandsSemaphore
	{
		C3D_API CommandsSemaphore( ashes::CommandBufferPtr commandBuffer = nullptr
			, ashes::SemaphorePtr semaphore = nullptr );
		C3D_API CommandsSemaphore( RenderDevice const & device
			, QueueData const & queueData
			, castor::String const & name );

		C3D_API void submit( ashes::Queue const & queue
			, VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT )const;
		C3D_API ashes::Semaphore const & submit( ashes::Queue const & queue
			, ashes::Semaphore const & toWait
			, VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT )const;
		C3D_API ashes::Semaphore const & submit( ashes::Queue const & queue
			, crg::SemaphoreWait const & toWait )const;
		C3D_API void submit( ashes::Queue const & queue
			, ashes::VkSemaphoreArray & semaphores
			, ashes::VkPipelineStageFlagsArray & stages );

		ashes::CommandBufferPtr commandBuffer;
		ashes::SemaphorePtr semaphore;
	};
}

#endif
