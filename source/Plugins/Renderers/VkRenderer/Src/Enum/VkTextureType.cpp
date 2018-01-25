#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageViewType convert( renderer::TextureType const & value )noexcept
	{
		switch ( value )
		{
		case renderer::TextureType::e1D:
			return VK_IMAGE_VIEW_TYPE_1D;

		case renderer::TextureType::e2D:
			return VK_IMAGE_VIEW_TYPE_2D;

		case renderer::TextureType::e3D:
			return VK_IMAGE_VIEW_TYPE_3D;

		case renderer::TextureType::eCube:
			return VK_IMAGE_VIEW_TYPE_CUBE;

		case renderer::TextureType::e1DArray:
			return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

		case renderer::TextureType::e2DArray:
			return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

		case renderer::TextureType::eCubeArray:
			return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

		default:
			assert( false && "Unsupported texture type" );
			return VK_IMAGE_VIEW_TYPE_2D;
		}
	}
}
