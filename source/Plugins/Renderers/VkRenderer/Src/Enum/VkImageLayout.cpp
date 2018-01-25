#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkImageLayout convert( renderer::ImageLayout const & layout )
	{
		switch ( layout )
		{
		case renderer::ImageLayout::eUndefined:
			return VK_IMAGE_LAYOUT_UNDEFINED;

		case renderer::ImageLayout::eGeneral:
			return VK_IMAGE_LAYOUT_GENERAL;

		case renderer::ImageLayout::eColourAttachmentOptimal:
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		case renderer::ImageLayout::eDepthStencilAttachmentOptimal:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		case renderer::ImageLayout::eDepthStencilReadOnlyOptimal:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		case renderer::ImageLayout::eShaderReadOnlyOptimal:
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		case renderer::ImageLayout::eTransferSrcOptimal:
			return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		case renderer::ImageLayout::eTransferDstOptimal:
			return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

		case renderer::ImageLayout::ePreinitialised:
			return VK_IMAGE_LAYOUT_PREINITIALIZED;

		case renderer::ImageLayout::ePresentSrc:
			return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		default:
			assert( false && "Unsupported image layout" );
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}

	renderer::ImageLayout convertImageLayout( VkImageLayout const & layout )
	{
		switch ( layout )
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			return renderer::ImageLayout::eUndefined;

		case VK_IMAGE_LAYOUT_GENERAL:
			return renderer::ImageLayout::eGeneral;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			return renderer::ImageLayout::eColourAttachmentOptimal;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			return renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
			return renderer::ImageLayout::eDepthStencilReadOnlyOptimal;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			return renderer::ImageLayout::eShaderReadOnlyOptimal;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			return renderer::ImageLayout::eTransferSrcOptimal;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			return renderer::ImageLayout::eTransferDstOptimal;

		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			return renderer::ImageLayout::ePreinitialised;

		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
			return renderer::ImageLayout::ePresentSrc;

		default:
			assert( false && "Unsupported image layout" );
			return renderer::ImageLayout::eUndefined;
		}
	}
}
