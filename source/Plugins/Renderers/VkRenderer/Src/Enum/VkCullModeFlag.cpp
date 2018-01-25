#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkCullModeFlags convert( renderer::CullModeFlags const & flags )
	{
		VkCullModeFlags result{ 0 };

		if ( checkFlag( flags, renderer::CullModeFlag::eFront ) )
		{
			result |= VK_CULL_MODE_FRONT_BIT;
		}

		if ( checkFlag( flags, renderer::CullModeFlag::eBack ) )
		{
			result |= VK_CULL_MODE_BACK_BIT;
		}

		return result;
	}
}
