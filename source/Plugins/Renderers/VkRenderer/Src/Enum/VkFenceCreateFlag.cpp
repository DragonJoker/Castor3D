#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkFenceCreateFlags convert( renderer::FenceCreateFlags const & flags )
	{
		VkFenceCreateFlags result{ 0 };

		if ( checkFlag( flags, renderer::FenceCreateFlag::eSignaled ) )
		{
			result |= VK_FENCE_CREATE_SIGNALED_BIT;
		}

		return result;
	}
}
