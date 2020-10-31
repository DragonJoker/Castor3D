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
		, castor::String const & name )
		: CommandsSemaphore{ device.graphicsCommandPool->createCommandBuffer( name )
			, device->createSemaphore( name ) }
	{
	}

	void CommandsSemaphore::submit( ashes::Queue const & queue )const
	{
		queue.submit( { *commandBuffer }
			, {}
			, { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }
			, {}
			, nullptr );
	}

	ashes::Semaphore const & CommandsSemaphore::submit( ashes::Queue const & queue
		, ashes::Semaphore const & toWait )const
	{
		auto * result = &toWait;

		queue.submit( *commandBuffer
			, toWait
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *semaphore
			, nullptr );

		return *semaphore;
	}
}
