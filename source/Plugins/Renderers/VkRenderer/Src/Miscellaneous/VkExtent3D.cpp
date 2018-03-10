#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkExtent3D convert( renderer::Extent3D const & value )
	{
		return VkExtent3D
		{
			value.width,
			value.height,
			value.depth
		};
	}
}
