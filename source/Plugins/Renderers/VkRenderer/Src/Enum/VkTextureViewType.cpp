#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageViewType convert( renderer::TextureViewType const & value )noexcept
	{
		switch ( value )
		{
		case renderer::TextureViewType::e1D:
			return VK_IMAGE_VIEW_TYPE_1D;

		case renderer::TextureViewType::e2D:
			return VK_IMAGE_VIEW_TYPE_2D;

		case renderer::TextureViewType::e3D:
			return VK_IMAGE_VIEW_TYPE_3D;

		case renderer::TextureViewType::eCube:
			return VK_IMAGE_VIEW_TYPE_CUBE;

		case renderer::TextureViewType::e1DArray:
			return VK_IMAGE_VIEW_TYPE_1D_ARRAY;

		case renderer::TextureViewType::e2DArray:
			return VK_IMAGE_VIEW_TYPE_2D_ARRAY;

		case renderer::TextureViewType::eCubeArray:
			return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;

		default:
			assert( false && "Unsupported TextureViewType" );
			return VK_IMAGE_VIEW_TYPE_2D;
		}
	}
}
