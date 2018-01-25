#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkCommandBufferResetFlags convert( renderer::CommandBufferResetFlags const & flags )
	{
		VkCommandBufferResetFlags result{ 0 };

		if ( checkFlag( flags, renderer::CommandBufferResetFlag::eReleaseResources ) )
		{
			result |= VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT;
		}

		return result;
	}
}
