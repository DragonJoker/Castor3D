#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageAspectFlags convert( renderer::ImageAspectFlags const & flags )
	{
		VkImageAspectFlags result{ 0 };

		if ( checkFlag( flags, renderer::ImageAspectFlag::eColour ) )
		{
			result |= VK_IMAGE_ASPECT_COLOR_BIT;
		}

		if ( checkFlag( flags, renderer::ImageAspectFlag::eDepth ) )
		{
			result |= VK_IMAGE_ASPECT_DEPTH_BIT;
		}

		if ( checkFlag( flags, renderer::ImageAspectFlag::eStencil ) )
		{
			result |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		if ( checkFlag( flags, renderer::ImageAspectFlag::eMetaData ) )
		{
			result |= VK_IMAGE_ASPECT_METADATA_BIT;
		}

		return result;
	}

	renderer::ImageAspectFlags convertAspectMask( VkImageAspectFlags const & flags )
	{
		renderer::ImageAspectFlags result{ 0 };

		if ( renderer::checkFlag( flags, VK_IMAGE_ASPECT_COLOR_BIT ) )
		{
			result |= renderer::ImageAspectFlag::eColour;
		}

		if ( renderer::checkFlag( flags, VK_IMAGE_ASPECT_DEPTH_BIT ) )
		{
			result |= renderer::ImageAspectFlag::eDepth;
		}

		if ( renderer::checkFlag( flags, VK_IMAGE_ASPECT_STENCIL_BIT ) )
		{
			result |= renderer::ImageAspectFlag::eStencil;
		}

		if ( renderer::checkFlag( flags, VK_IMAGE_ASPECT_METADATA_BIT ) )
		{
			result |= renderer::ImageAspectFlag::eMetaData;
		}

		return result;
	}
}
