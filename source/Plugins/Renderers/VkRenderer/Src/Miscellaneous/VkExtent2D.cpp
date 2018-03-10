#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkExtent2D convert( renderer::Extent2D const & value )
	{
		return VkExtent2D
		{
			value.width,
			value.height,
		};
	}
}
