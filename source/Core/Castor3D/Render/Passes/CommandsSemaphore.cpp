#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

namespace castor3d
{
	CommandsSemaphore::CommandsSemaphore( ashes::CommandBufferPtr commandBuffer
		, ashes::SemaphorePtr semaphore )
		: commandBuffer{ std::move( commandBuffer ) }
		, semaphore{ std::move( semaphore ) }
	{
	}

	CommandsSemaphore::CommandsSemaphore( RenderDevice const & device
		, QueueData const & queueData
		, castor::String const & name
		, VkCommandBufferLevel level )
		: CommandsSemaphore{ queueData.commandPool->createCommandBuffer( name, level )
			, device->createSemaphore( name ) }
	{
	}

	void CommandsSemaphore::submit( ashes::Queue const & queue
		, VkPipelineStageFlags stage )const
	{
		queue.submit( ashes::VkCommandBufferArray{ *commandBuffer }
			, ashes::VkSemaphoreArray{}
			, { stage }
			, ashes::VkSemaphoreArray{} );
	}

	ashes::Semaphore const & CommandsSemaphore::submit( ashes::Queue const & queue
		, ashes::Semaphore const & toWait
		, VkPipelineStageFlags stage )const
	{
		return submit( queue, { toWait, stage } );
	}

	ashes::Semaphore const & CommandsSemaphore::submit( ashes::Queue const & queue
		, crg::SemaphoreWait const & toWait )const
	{
		queue.submit( *commandBuffer
			, toWait.semaphore
			, toWait.dstStageMask
			, *semaphore );
		return *semaphore;
	}

	void CommandsSemaphore::submit( ashes::Queue const & queue
		, ashes::VkSemaphoreArray & semaphores
		, ashes::VkPipelineStageFlagsArray & stages )
	{
		queue.submit( *commandBuffer
			, semaphores
			, stages
			, *semaphore );
		semaphores = { *semaphore };
		stages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	}
}
