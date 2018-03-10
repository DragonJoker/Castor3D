#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkDependencyFlags convert( renderer::DependencyFlags const & flags )
	{
		VkDependencyFlags result{ 0 };

		if ( checkFlag( flags, renderer::DependencyFlag::eByRegion ) )
		{
			result |= VK_DEPENDENCY_BY_REGION_BIT;
		}

		return result;
	}

	renderer::DependencyFlags convertDependencyFlags( VkDependencyFlags const & flags )
	{
		renderer::DependencyFlags result;

		if ( renderer::checkFlag( flags, VK_DEPENDENCY_BY_REGION_BIT ) )
		{
			result |= renderer::DependencyFlag::eByRegion;
		}

		return result;
	}
}
