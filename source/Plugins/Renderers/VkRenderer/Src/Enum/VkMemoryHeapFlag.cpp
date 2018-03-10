#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkMemoryHeapFlags convert( renderer::MemoryHeapFlags const & flags )
	{
		VkMemoryHeapFlags result{ 0 };

		if ( checkFlag( flags, renderer::MemoryHeapFlag::eDeviceLocal ) )
		{
			result |= VK_MEMORY_HEAP_DEVICE_LOCAL_BIT;
		}

		return result;
	}

	renderer::MemoryHeapFlags convertMemoryHeapFlags( VkMemoryHeapFlags const & flags )
	{
		renderer::MemoryHeapFlags result;

		if ( renderer::checkFlag( flags, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT ) )
		{
			result |= renderer::MemoryHeapFlag::eDeviceLocal;
		}

		return result;
	}
}
