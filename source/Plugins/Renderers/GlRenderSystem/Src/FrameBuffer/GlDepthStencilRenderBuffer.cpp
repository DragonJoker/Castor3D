#include "FrameBuffer/GlDepthStencilRenderBuffer.hpp"

#include "Common/OpenGl.hpp"

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlDepthStencilRenderBuffer::GlDepthStencilRenderBuffer( OpenGl & p_gl, PixelFormat p_format )
		: DepthStencilRenderBuffer( p_format )
		, m_glRenderBuffer( p_gl, p_gl.getRboStorage( p_format ), *this )
	{
	}

	GlDepthStencilRenderBuffer::~GlDepthStencilRenderBuffer()
	{
	}

	bool GlDepthStencilRenderBuffer::create()
	{
		return m_glRenderBuffer.create();
	}

	void GlDepthStencilRenderBuffer::destroy()
	{
		m_glRenderBuffer.destroy();
	}

	bool GlDepthStencilRenderBuffer::initialise( castor::Size const & p_size )
	{
		bool result = m_glRenderBuffer.initialise( p_size );

		if ( result )
		{
			m_size = p_size;
		}

		return result;
	}

	void GlDepthStencilRenderBuffer::cleanup()
	{
		m_glRenderBuffer.cleanup();
	}

	void GlDepthStencilRenderBuffer::bind()
	{
		m_glRenderBuffer.bind();
	}

	void GlDepthStencilRenderBuffer::unbind()
	{
		m_glRenderBuffer.unbind();
	}

	bool GlDepthStencilRenderBuffer::resize( castor::Size const & p_size )
	{
		bool result = m_glRenderBuffer.resize( p_size );

		if ( result )
		{
			m_size = p_size;
		}

		return result;
	}
}
