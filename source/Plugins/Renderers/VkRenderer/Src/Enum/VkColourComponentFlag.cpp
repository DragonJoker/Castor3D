#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkColorComponentFlags convert( renderer::ColourComponentFlags const & flags )
	{
		VkColorComponentFlags result{ 0 };

		if ( checkFlag( flags, renderer::ColourComponentFlag::eR ) )
		{
			result |= VK_COLOR_COMPONENT_R_BIT;
		}

		if ( checkFlag( flags, renderer::ColourComponentFlag::eG ) )
		{
			result |= VK_COLOR_COMPONENT_G_BIT;
		}

		if ( checkFlag( flags, renderer::ColourComponentFlag::eB ) )
		{
			result |= VK_COLOR_COMPONENT_B_BIT;
		}

		if ( checkFlag( flags, renderer::ColourComponentFlag::eA ) )
		{
			result |= VK_COLOR_COMPONENT_A_BIT;
		}

		return result;
	}
}
