#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageCopy convert( renderer::ImageCopy const & value )
	{
		return VkImageCopy
		{
			convert( value.srcSubresource ),
			convert( value.srcOffset ),
			convert( value.dstSubresource ),
			convert( value.dstOffset ),
			convert( value.extent ),
		};
	}
}
