#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

namespace castor3d
{
	CommandsSemaphore::CommandsSemaphore( ashes::CommandBufferPtr commandBuffer
		, ashes::SemaphorePtr semaphore )
		: commandBuffer{ castor::move( commandBuffer ) }
		, semaphore{ castor::move( semaphore ) }
	{
	}

	CommandsSemaphore::CommandsSemaphore( RenderDevice const & device
		, QueueData const & queueData
		, castor::String const & name
		, VkCommandBufferLevel level )
		: CommandsSemaphore{ queueData.commandPool->createCommandBuffer( castor::toUtf8( name ), level )
			, device->createSemaphore( castor::toUtf8( name ) ) }
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

	ashes::Semaphore const & CommandsSemaphore::submit( ashes::Queue const & queue
		, crg::SemaphoreWaitArray const & toWait )const
	{
		ashes::VkSemaphoreArray semaphores;
		ashes::VkPipelineStageFlagsArray stages;
		crg::convert( toWait, semaphores, stages );
		queue.submit( *commandBuffer
			, semaphores
			, stages
			, *semaphore );
		return *semaphore;
	}

	void CommandsSemaphore::submit( ashes::Queue const & queue
		, ashes::VkSemaphoreArray & semaphores
		, ashes::VkPipelineStageFlagsArray & stages )const
	{
		queue.submit( *commandBuffer
			, semaphores
			, stages
			, *semaphore );
		semaphores = { *semaphore };
		stages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	}
}
