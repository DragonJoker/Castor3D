#include "FrameBuffer/GlDepthStencilRenderBuffer.hpp"

#include "Common/OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlDepthStencilRenderBuffer::GlDepthStencilRenderBuffer( OpenGl & p_gl, ePIXEL_FORMAT p_format )
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
		bool l_return = m_glRenderBuffer.Initialise( p_size );

		if ( l_return )
		{
			m_size = p_size;
		}

		return l_return;
	}

	void GlDepthStencilRenderBuffer::Cleanup()
	{
		m_glRenderBuffer.Cleanup();
	}

	bool GlDepthStencilRenderBuffer::Bind()
	{
		return m_glRenderBuffer.Bind();
	}

	void GlDepthStencilRenderBuffer::Unbind()
	{
		m_glRenderBuffer.Unbind();
	}

	bool GlDepthStencilRenderBuffer::Resize( Castor::Size const & p_size )
	{
		bool l_return = m_glRenderBuffer.Resize( p_size );

		if ( l_return )
		{
			m_size = p_size;
		}

		return l_return;
	}
}
