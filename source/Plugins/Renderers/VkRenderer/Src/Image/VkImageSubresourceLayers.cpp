#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageSubresourceLayers convert( renderer::ImageSubresourceLayers const & value )
	{
		return VkImageSubresourceLayers
		{
			convert( value.aspectMask ),
			value.mipLevel,
			value.baseArrayLayer,
			value.layerCount
		};
	}
}
