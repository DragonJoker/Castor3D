#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkCommandPoolCreateFlags convert( renderer::CommandPoolCreateFlags const & flags )
	{
		VkCommandPoolCreateFlags result{ 0 };

		if ( checkFlag( flags, renderer::CommandPoolCreateFlag::eTransient ) )
		{
			result |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		}

		if ( checkFlag( flags, renderer::CommandPoolCreateFlag::eResetCommandBuffer ) )
		{
			result |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		}

		return result;
	}
}
