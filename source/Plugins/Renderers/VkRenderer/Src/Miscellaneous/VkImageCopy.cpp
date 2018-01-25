#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageCopy convert( renderer::ImageCopy const & value )
	{
		return VkImageCopy
		{
			convert( value.srcSubresource ),
			{ value.srcOffset[0], value.srcOffset[1], value.srcOffset[2] },
			convert( value.dstSubresource ),
			{ value.dstOffset[0], value.dstOffset[1], value.dstOffset[2] },
			{ value.extent[0], value.extent[1], value.extent[2] },
		};
	}
}
