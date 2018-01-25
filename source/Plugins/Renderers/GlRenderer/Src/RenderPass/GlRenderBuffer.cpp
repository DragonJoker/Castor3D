#include "RenderPass/GlRenderBuffer.hpp"

#include "Core/GlDevice.hpp"
#include "Sync/GlImageMemoryBarrier.hpp"

#include <Image/ImageSubresourceRange.hpp>

namespace gl_renderer
{
	namespace
	{
		renderer::ImageUsageFlag getAttachmentFlag( renderer::PixelFormat format )
		{
			return ( renderer::isDepthFormat( format ) 
					|| renderer::isDepthStencilFormat( format )
					|| renderer::isStencilFormat( format ) )
				? renderer::ImageUsageFlag::eDepthStencilAttachment
				: renderer::ImageUsageFlag::eColourAttachment;
		}
	}

	RenderBuffer::RenderBuffer( Device const & device
		, renderer::PixelFormat format
		, renderer::UIVec2 const & size )
		: renderer::RenderBuffer{ device, format, size }
		, m_texture{ device }
	{
		m_texture.setImage( format
			, size
			, getAttachmentFlag( format )
			, renderer::ImageTiling::eOptimal );
		makeDepthStencilAttachment();
	}

	renderer::ImageMemoryBarrier RenderBuffer::makeColourAttachment()const
	{
		return m_texture.makeColourAttachment( { renderer::getAspectMask( m_texture.getFormat() )
			, 0u
			, 1u
			, 0u
			, 1u } );
	}

	renderer::ImageMemoryBarrier RenderBuffer::makeDepthStencilAttachment()const
	{
		return m_texture.makeDepthStencilAttachment( { renderer::getAspectMask( m_texture.getFormat() )
			, 0u
			, 1u
			, 0u
			, 1u } );
	}
}
