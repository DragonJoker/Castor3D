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
		, castor::String const & name )
		: CommandsSemaphore{ queueData.commandPool->createCommandBuffer( name )
			, device->createSemaphore( name ) }
	{
	}

	void CommandsSemaphore::submit( ashes::Queue const & queue )const
	{
		queue.submit( ashes::VkCommandBufferArray{ *commandBuffer }
			, ashes::VkSemaphoreArray{}
			, { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }
			, ashes::VkSemaphoreArray{} );
	}

	ashes::Semaphore const & CommandsSemaphore::submit( ashes::Queue const & queue
		, ashes::Semaphore const & toWait )const
	{
		return submit( queue, { toWait, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT } );
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
}
