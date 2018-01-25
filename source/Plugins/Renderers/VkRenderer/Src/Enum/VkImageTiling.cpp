#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageTiling convert( renderer::ImageTiling const & value )
	{
		switch ( value )
		{
		case renderer::ImageTiling::eOptimal:
			return VK_IMAGE_TILING_OPTIMAL;

		case renderer::ImageTiling::eLinear:
			return VK_IMAGE_TILING_LINEAR;

		default:
			assert( false && "Unsupported image tiling" );
			return VK_IMAGE_TILING_OPTIMAL;
		}
	}
}
