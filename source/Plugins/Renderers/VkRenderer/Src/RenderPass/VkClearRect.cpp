#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkClearRect convert( renderer::ClearRect const & value )
	{
		return VkClearRect
		{
			{
				convert( value.offset ),
				convert( value.extent )
			},
			value.baseArrayLayer,
			value.layerCount,
		};
	}
}
