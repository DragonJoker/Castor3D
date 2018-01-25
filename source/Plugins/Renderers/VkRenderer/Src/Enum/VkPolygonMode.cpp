#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPolygonMode convert( renderer::PolygonMode const & value )
	{
		switch ( value )
		{
		case renderer::PolygonMode::eFill:
			return VK_POLYGON_MODE_FILL;

		case renderer::PolygonMode::eLine:
			return VK_POLYGON_MODE_LINE;

		case renderer::PolygonMode::ePoint:
			return VK_POLYGON_MODE_POINT;

		default:
			assert( false && "Unsupported polygon mode" );
			return VK_POLYGON_MODE_FILL;
		}
	}
}
