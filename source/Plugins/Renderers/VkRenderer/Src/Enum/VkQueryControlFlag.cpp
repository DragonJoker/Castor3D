#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkQueryControlFlags convert( renderer::QueryControlFlags const & flags )
	{
		VkQueryControlFlags result{ 0 };

		if ( checkFlag( flags, renderer::QueryControlFlag::ePrecise ) )
		{
			result |= VK_QUERY_CONTROL_PRECISE_BIT;
		}

		return result;
	}
}
