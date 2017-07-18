#include "FrameBuffer/GlDepthStencilRenderBuffer.hpp"

#include "Common/OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlDepthStencilRenderBuffer::GlDepthStencilRenderBuffer( OpenGl & p_gl, PixelFormat p_format )
		: DepthStencilRenderBuffer( p_format )
		, m_glRenderBuffer( p_gl, p_gl.GetRboStorage( p_format ), *this )
	{
	}

	GlDepthStencilRenderBuffer::~GlDepthStencilRenderBuffer()
	{
	}

	bool GlDepthStencilRenderBuffer::Create()
	{
		return m_glRenderBuffer.Create();
	}

	void GlDepthStencilRenderBuffer::Destroy()
	{
		m_glRenderBuffer.Destroy();
	}

	bool GlDepthStencilRenderBuffer::Initialise( Castor::Size const & p_size )
	{
		bool l_result = m_glRenderBuffer.Initialise( p_size );

		if ( l_result )
		{
			m_size = p_size;
		}

		return l_result;
	}

	void GlDepthStencilRenderBuffer::Cleanup()
	{
		m_glRenderBuffer.Cleanup();
	}

	void GlDepthStencilRenderBuffer::Bind()
	{
		m_glRenderBuffer.Bind();
	}

	void GlDepthStencilRenderBuffer::Unbind()
	{
		m_glRenderBuffer.Unbind();
	}

	bool GlDepthStencilRenderBuffer::Resize( Castor::Size const & p_size )
	{
		bool l_result = m_glRenderBuffer.Resize( p_size );

		if ( l_result )
		{
			m_size = p_size;
		}

		return l_result;
	}
}
