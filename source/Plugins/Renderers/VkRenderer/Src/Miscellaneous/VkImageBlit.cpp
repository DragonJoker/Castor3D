#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageBlit convert( renderer::ImageBlit const & value )
	{
		return VkImageBlit
		{
			convert( value.srcSubresource ),
			{
				convert( value.srcOffset ),
				{
					int32_t( value.srcExtent.width ),
					int32_t( value.srcExtent.height ),
					int32_t( value.srcExtent.depth )
				},
			},
			convert( value.dstSubresource ),
			{
				convert( value.dstOffset ),
				{
					int32_t( value.dstExtent.width ),
					int32_t( value.dstExtent.height ),
					int32_t( value.dstExtent.depth )
				},
			},
		};
	}
}
