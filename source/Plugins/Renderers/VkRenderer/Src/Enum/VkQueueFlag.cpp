#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkQueueFlags convert( renderer::QueueFlags const & flags )
	{
		VkQueueFlags result{ 0 };

		if ( checkFlag( flags, renderer::QueueFlag::eGraphics ) )
		{
			result |= VK_QUEUE_GRAPHICS_BIT;
		}

		if ( checkFlag( flags, renderer::QueueFlag::eCompute ) )
		{
			result |= VK_QUEUE_COMPUTE_BIT;
		}

		if ( checkFlag( flags, renderer::QueueFlag::eTransfer ) )
		{
			result |= VK_QUEUE_TRANSFER_BIT;
		}

		if ( checkFlag( flags, renderer::QueueFlag::eSparseBinding ) )
		{
			result |= VK_QUEUE_SPARSE_BINDING_BIT;
		}

		return result;
	}

	renderer::QueueFlags convertQueueFlags( VkQueueFlags const & flags )
	{
		renderer::QueueFlags result;

		if ( renderer::checkFlag( flags, VK_QUEUE_GRAPHICS_BIT ) )
		{
			result |= renderer::QueueFlag::eGraphics;
		}

		if ( renderer::checkFlag( flags, VK_QUEUE_COMPUTE_BIT ) )
		{
			result |= renderer::QueueFlag::eCompute;
		}

		if ( renderer::checkFlag( flags, VK_QUEUE_TRANSFER_BIT ) )
		{
			result |= renderer::QueueFlag::eTransfer;
		}

		if ( renderer::checkFlag( flags, VK_QUEUE_SPARSE_BINDING_BIT ) )
		{
			result |= renderer::QueueFlag::eSparseBinding;
		}

		return result;
	}
}
