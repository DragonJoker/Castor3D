#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkSubresourceLayout convert( renderer::SubresourceLayout const & range )
	{
		return VkSubresourceLayout
		{
			range.offset,
			range.size,
			range.rowPitch,
			range.arrayPitch,
			range.depthPitch
		};
	}

	renderer::SubresourceLayout convert( VkSubresourceLayout const & range )
	{
		return renderer::SubresourceLayout
		{
			range.offset,
			range.size,
			range.rowPitch,
			range.arrayPitch,
			range.depthPitch
		};
	}
}
