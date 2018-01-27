#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageBlit convert( renderer::ImageBlit const & value )
	{
		return VkImageBlit
		{
			convert( value.srcSubresource ),
			{ value.srcOffset[0], value.srcOffset[1], value.srcOffset[2] },
			convert( value.dstSubresource ),
			{ value.dstOffset[0], value.dstOffset[1], value.dstOffset[2] },
		};
	}
}
