#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageType convert( renderer::TextureType const & value )noexcept
	{
		switch ( value )
		{
		case renderer::TextureType::e1D:
			return VK_IMAGE_TYPE_1D;

		case renderer::TextureType::e2D:
			return VK_IMAGE_TYPE_2D;

		case renderer::TextureType::e3D:
			return VK_IMAGE_TYPE_3D;

		default:
			assert( false && "Unsupported texture type" );
			return VK_IMAGE_TYPE_2D;
		}
	}
}
