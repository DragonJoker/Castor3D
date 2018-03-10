#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkOffset2D convert( renderer::Offset2D const & value )
	{
		return VkOffset2D
		{
			value.x,
			value.y,
		};
	}
}
