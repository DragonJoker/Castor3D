#include "FrameBuffer/GlES3DepthStencilRenderBuffer.hpp"

#include "Common/OpenGlES3.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3DepthStencilRenderBuffer::GlES3DepthStencilRenderBuffer( OpenGlES3 & p_gl, PixelFormat p_format )
		: DepthStencilRenderBuffer( p_format )
		, m_glRenderBuffer( p_gl, p_gl.GetRboStorage( p_format ), *this )
	{
	}

	GlES3DepthStencilRenderBuffer::~GlES3DepthStencilRenderBuffer()
	{
	}

	bool GlES3DepthStencilRenderBuffer::Create()
	{
		return m_glRenderBuffer.Create();
	}

	void GlES3DepthStencilRenderBuffer::Destroy()
	{
		m_glRenderBuffer.Destroy();
	}

	bool GlES3DepthStencilRenderBuffer::Initialise( Castor::Size const & p_size )
	{
		bool l_return = m_glRenderBuffer.Initialise( p_size );

		if ( l_return )
		{
			m_size = p_size;
		}

		return l_return;
	}

	void GlES3DepthStencilRenderBuffer::Cleanup()
	{
		m_glRenderBuffer.Cleanup();
	}

	void GlES3DepthStencilRenderBuffer::Bind()
	{
		m_glRenderBuffer.Bind();
	}

	void GlES3DepthStencilRenderBuffer::Unbind()
	{
		m_glRenderBuffer.Unbind();
	}

	bool GlES3DepthStencilRenderBuffer::Resize( Castor::Size const & p_size )
	{
		bool l_return = m_glRenderBuffer.Resize( p_size );

		if ( l_return )
		{
			m_size = p_size;
		}

		return l_return;
	}
}
