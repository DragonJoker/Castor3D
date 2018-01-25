#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkAccessFlags convert( renderer::AccessFlags const & flags )
	{
		VkAccessFlags result{ 0 };

		if ( checkFlag( flags, renderer::AccessFlag::eIndirectCommandRead ) )
		{
			result |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eIndexRead ) )
		{
			result |= VK_ACCESS_INDEX_READ_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eVertexAttributeRead ) )
		{
			result |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eUniformRead ) )
		{
			result |= VK_ACCESS_UNIFORM_READ_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eInputAttachmentRead ) )
		{
			result |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eShaderRead ) )
		{
			result |= VK_ACCESS_SHADER_READ_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eShaderWrite ) )
		{
			result |= VK_ACCESS_SHADER_WRITE_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eColourAttachmentRead ) )
		{
			result |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eColourAttachmentWrite ) )
		{
			result |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eDepthStencilAttachmentRead ) )
		{
			result |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eDepthStencilAttachmentWrite ) )
		{
			result |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eTransferRead ) )
		{
			result |= VK_ACCESS_TRANSFER_READ_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eTransferWrite ) )
		{
			result |= VK_ACCESS_TRANSFER_WRITE_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eHostRead ) )
		{
			result |= VK_ACCESS_HOST_READ_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eHostWrite ) )
		{
			result |= VK_ACCESS_HOST_WRITE_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eMemoryRead ) )
		{
			result |= VK_ACCESS_MEMORY_READ_BIT;
		}

		if ( checkFlag( flags, renderer::AccessFlag::eMemoryWrite ) )
		{
			result |= VK_ACCESS_MEMORY_WRITE_BIT;
		}

		return result;
	}

	renderer::AccessFlags convertAccessFlags( VkAccessFlags const & flags )
	{
		renderer::AccessFlags result;

		if ( renderer::checkFlag( flags, VK_ACCESS_INDIRECT_COMMAND_READ_BIT ) )
		{
			result |= renderer::AccessFlag::eIndirectCommandRead;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_INDEX_READ_BIT ) )
		{
			result |= renderer::AccessFlag::eIndexRead;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT ) )
		{
			result |= renderer::AccessFlag::eVertexAttributeRead;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_UNIFORM_READ_BIT ) )
		{
			result |= renderer::AccessFlag::eUniformRead;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT ) )
		{
			result |= renderer::AccessFlag::eInputAttachmentRead;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_SHADER_READ_BIT ) )
		{
			result |= renderer::AccessFlag::eShaderRead;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_SHADER_WRITE_BIT ) )
		{
			result |= renderer::AccessFlag::eShaderWrite;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT ) )
		{
			result |= renderer::AccessFlag::eColourAttachmentRead;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT ) )
		{
			result |= renderer::AccessFlag::eColourAttachmentWrite;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT ) )
		{
			result |= renderer::AccessFlag::eDepthStencilAttachmentRead;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT ) )
		{
			result |= renderer::AccessFlag::eDepthStencilAttachmentWrite;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_TRANSFER_READ_BIT ) )
		{
			result |= renderer::AccessFlag::eTransferRead;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_TRANSFER_WRITE_BIT ) )
		{
			result |= renderer::AccessFlag::eTransferWrite;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_HOST_READ_BIT ) )
		{
			result |= renderer::AccessFlag::eHostRead;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_HOST_WRITE_BIT ) )
		{
			result |= renderer::AccessFlag::eHostWrite;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_MEMORY_READ_BIT ) )
		{
			result |= renderer::AccessFlag::eMemoryRead;
		}

		if ( renderer::checkFlag( flags, VK_ACCESS_MEMORY_WRITE_BIT ) )
		{
			result |= renderer::AccessFlag::eMemoryWrite;
		}

		return result;
	}
}
