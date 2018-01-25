#include "Buffer/GlBufferView.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Core/GlDevice.hpp"

namespace gl_renderer
{
	BufferView::BufferView( renderer::Device const & device
		, Buffer const & buffer
		, renderer::PixelFormat format
		, uint32_t offset
		, uint32_t range )
		: renderer::BufferView{ device, buffer, format, offset, range }
	{
		glLogCall( gl::GenTextures, 1, &m_name );
		glLogCall( gl::ActiveTexture, GL_TEXTURE0 );
		glLogCall( gl::BindTexture, GL_BUFFER_TARGET_TEXTURE, m_name );
		glLogCall( gl::TexBufferRange, GL_BUFFER_TARGET_TEXTURE, getInternal( format ), buffer.getBuffer(), offset, range );
		glLogCall( gl::BindTexture, GL_BUFFER_TARGET_TEXTURE, 0u );
	}

	BufferView::~BufferView()
	{
		glLogCall( gl::DeleteTextures, 1, &m_name );
	}
}
