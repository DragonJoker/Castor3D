#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageCreateFlags convert( renderer::ImageCreateFlags const & flags )
	{
		VkImageCreateFlags result{ 0 };

		if ( checkFlag( flags, renderer::ImageCreateFlag::eSparseBinding ) )
		{
			result |= VK_IMAGE_CREATE_SPARSE_BINDING_BIT;
		}

		if ( checkFlag( flags, renderer::ImageCreateFlag::eSparseResidency ) )
		{
			result |= VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT;
		}

		if ( checkFlag( flags, renderer::ImageCreateFlag::eSparseAliased ) )
		{
			result |= VK_IMAGE_CREATE_SPARSE_ALIASED_BIT;
		}

		if ( checkFlag( flags, renderer::ImageCreateFlag::eMutableFormat ) )
		{
			result |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
		}

		if ( checkFlag( flags, renderer::ImageCreateFlag::eCubeCompatible ) )
		{
			result |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		}

		return result;
	}

	renderer::ImageCreateFlags convertImageCreateFlags( VkImageCreateFlags const & flags )
	{
		renderer::ImageCreateFlags result;

		if ( renderer::checkFlag( flags, VK_IMAGE_CREATE_SPARSE_BINDING_BIT ) )
		{
			result |= renderer::ImageCreateFlag::eSparseBinding;
		}

		if ( renderer::checkFlag( flags, VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT ) )
		{
			result |= renderer::ImageCreateFlag::eSparseResidency;
		}

		if ( renderer::checkFlag( flags, VK_IMAGE_CREATE_SPARSE_ALIASED_BIT ) )
		{
			result |= renderer::ImageCreateFlag::eSparseAliased;
		}

		if ( renderer::checkFlag( flags, VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT ) )
		{
			result |= renderer::ImageCreateFlag::eMutableFormat;
		}

		if ( renderer::checkFlag( flags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT ) )
		{
			result |= renderer::ImageCreateFlag::eCubeCompatible;
		}

		return result;
	}
}
