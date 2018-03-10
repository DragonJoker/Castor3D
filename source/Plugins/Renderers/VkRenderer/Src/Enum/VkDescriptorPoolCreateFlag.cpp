#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkDescriptorPoolCreateFlags convert( renderer::DescriptorPoolCreateFlags const & flags )
	{
		VkDescriptorPoolCreateFlags result{ 0 };

		if ( checkFlag( flags, renderer::DescriptorPoolCreateFlag::eFreeDescriptorSet ) )
		{
			result |= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		}

		return result;
	}
}
