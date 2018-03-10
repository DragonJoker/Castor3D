#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageSubresourceRange convert( renderer::ImageSubresourceRange const & range )
	{
		return VkImageSubresourceRange
		{
			convert( range.aspectMask ),
			range.baseMipLevel,
			range.levelCount,
			range.baseArrayLayer,
			range.layerCount
		};
	}

	renderer::ImageSubresourceRange convert( VkImageSubresourceRange const & range )
	{
		return renderer::ImageSubresourceRange
		{
			convertAspectMask( range.aspectMask ),
			range.baseMipLevel,
			range.levelCount,
			range.baseArrayLayer,
			range.layerCount
		};
	}
}
