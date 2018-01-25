#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GlImageLayout convert( renderer::ImageLayout const & layout )
	{
		switch ( layout )
		{
		case renderer::ImageLayout::eUndefined:
		case renderer::ImageLayout::ePreinitialised:
		case renderer::ImageLayout::ePresentSrc:
			return GL_LAYOUT_UNDEFINED_EXT;

		case renderer::ImageLayout::eGeneral:
			return GL_LAYOUT_GENERAL_EXT;

		case renderer::ImageLayout::eColourAttachmentOptimal:
			return GL_LAYOUT_COLOR_ATTACHMENT_EXT;

		case renderer::ImageLayout::eDepthStencilAttachmentOptimal:
			return GL_LAYOUT_DEPTH_STENCIL_ATTACHMENT_EXT;

		case renderer::ImageLayout::eDepthStencilReadOnlyOptimal:
			return GL_LAYOUT_DEPTH_STENCIL_READ_ONLY_EXT;

		case renderer::ImageLayout::eShaderReadOnlyOptimal:
			return GL_LAYOUT_SHADER_READ_ONLY_EXT;

		case renderer::ImageLayout::eTransferSrcOptimal:
			return GL_LAYOUT_TRANSFER_SRC_EXT;

		case renderer::ImageLayout::eTransferDstOptimal:
			return GL_LAYOUT_TRANSFER_DST_EXT;

		default:
			assert( false && "Unsupported image layout" );
			return GL_LAYOUT_UNDEFINED_EXT;
		}
	}

	renderer::ImageLayout convertImageLayout( GlImageLayout layout )
	{
		switch ( layout )
		{
		case GL_LAYOUT_UNDEFINED_EXT:
			return renderer::ImageLayout::eUndefined;

		case GL_LAYOUT_GENERAL_EXT:
			return renderer::ImageLayout::eGeneral;

		case GL_LAYOUT_COLOR_ATTACHMENT_EXT:
			return renderer::ImageLayout::eColourAttachmentOptimal;

		case GL_LAYOUT_DEPTH_STENCIL_ATTACHMENT_EXT:
			return renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		case GL_LAYOUT_DEPTH_STENCIL_READ_ONLY_EXT:
			return renderer::ImageLayout::eDepthStencilReadOnlyOptimal;

		case GL_LAYOUT_SHADER_READ_ONLY_EXT:
			return renderer::ImageLayout::eShaderReadOnlyOptimal;

		case GL_LAYOUT_TRANSFER_SRC_EXT:
			return renderer::ImageLayout::eTransferSrcOptimal;

		case GL_LAYOUT_TRANSFER_DST_EXT:
			return renderer::ImageLayout::eTransferDstOptimal;

		default:
			assert( false && "Unsupported image layout" );
			return renderer::ImageLayout::eUndefined;
		}
	}
}

