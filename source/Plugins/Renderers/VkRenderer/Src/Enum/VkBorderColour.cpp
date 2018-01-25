#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkBorderColor convert( renderer::BorderColour const & value )noexcept
	{
		switch ( value )
		{
		case renderer::BorderColour::eFloatTransparentBlack:
			return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

		case renderer::BorderColour::eIntTransparentBlack:
			return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;

		case renderer::BorderColour::eFloatOpaqueBlack:
			return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

		case renderer::BorderColour::eIntOpaqueBlack:
			return VK_BORDER_COLOR_INT_OPAQUE_BLACK;

		case renderer::BorderColour::eFloatOpaqueWhite:
			return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

		case renderer::BorderColour::eIntOpaqueWhite:
			return VK_BORDER_COLOR_INT_OPAQUE_WHITE;

		default:
			assert( false && "Unsupported border colour" );
			return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		}
	}
}
