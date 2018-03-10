#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageSubresource convert( renderer::ImageSubresource const & range )
	{
		return VkImageSubresource
		{
			convert( range.aspectMask ),
			range.mipLevel,
			range.arrayLayer
		};
	}

	renderer::ImageSubresource convert( VkImageSubresource const & range )
	{
		return renderer::ImageSubresource
		{
			convertAspectMask( range.aspectMask ),
			range.mipLevel,
			range.arrayLayer
		};
	}
}
