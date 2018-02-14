/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlBlitImageCommand.hpp"

#include "Image/GlTexture.hpp"
#include "Image/GlTextureView.hpp"
#include "RenderPass/GlFrameBuffer.hpp"

#include <Image/ImageSubresourceRange.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>

namespace gl_renderer
{
	namespace
	{
		GlImageAspectFlags getMask( renderer::PixelFormat format )
		{
			GlImageAspectFlags result = 0u;

			if ( renderer::isDepthFormat( format ) )
			{
				result |= GL_DEPTH_BUFFER_BIT;
			}

			if ( renderer::isStencilFormat( format ) )
			{
				result |= GL_STENCIL_BUFFER_BIT;
			}

			if ( !renderer::isDepthFormat( format )
				&& !renderer::isStencilFormat( format ) )
			{
				result |= GL_COLOR_BUFFER_BIT;
			}

			return result;
		}
	}

	BlitImageCommand::BlitImageCommand( renderer::ImageBlit const & blitInfo
		, renderer::FrameBufferAttachment const & src
		, renderer::FrameBufferAttachment const & dst
		, renderer::Filter filter )
		: m_blitInfo{ blitInfo }
		, m_srcTexture{ static_cast< Texture const & >( src.getTexture() ) }
		, m_dstTexture{ static_cast< Texture const & >( dst.getTexture() ) }
		, m_srcFrameBuffer{ static_cast< FrameBuffer const & >( src.getFrameBuffer() ) }
		, m_dstFrameBuffer{ static_cast< FrameBuffer const & >( dst.getFrameBuffer() ) }
		, m_filter{ convert( filter ) }
		, m_mask{ getMask( m_srcTexture.getFormat() ) }
	{
	}

	BlitImageCommand::~BlitImageCommand()
	{
	}

	void BlitImageCommand::apply()const
	{
		glLogCall( gl::BindFramebuffer, GL_READ_FRAMEBUFFER, m_srcFrameBuffer.getFrameBuffer() );
		glLogCall( gl::BindFramebuffer, GL_DRAW_FRAMEBUFFER, m_dstFrameBuffer.getFrameBuffer() );

		glLogCall( gl::BlitFramebuffer
			, m_blitInfo.srcOffset[0]
			, m_blitInfo.srcOffset[1]
			, m_srcTexture.getDimensions()[0]
			, m_srcTexture.getDimensions()[1]
			, m_blitInfo.dstOffset[0]
			, m_blitInfo.dstOffset[1]
			, m_dstTexture.getDimensions()[0]
			, m_dstTexture.getDimensions()[1]
			, m_mask
			, m_filter );

		glLogCall( gl::BindFramebuffer, GL_DRAW_FRAMEBUFFER, 0u );
		glLogCall( gl::BindFramebuffer, GL_READ_FRAMEBUFFER, 0u );
	}

	CommandPtr BlitImageCommand::clone()const
	{
		return std::make_unique< BlitImageCommand >( *this );
	}
}
