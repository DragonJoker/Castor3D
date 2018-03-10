#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkComponentMapping convert( renderer::ComponentMapping const & value )
	{
		return VkComponentMapping
		{
			convert( value.r ),
			convert( value.g ),
			convert( value.b ),
			convert( value.a )
		};
	}
}
