#include "FrameBuffer/GlES3ColourRenderBuffer.hpp"

#include "Common/OpenGlES3.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3ColourRenderBuffer::GlES3ColourRenderBuffer( OpenGlES3 & p_gl, PixelFormat p_format )
		: ColourRenderBuffer( p_format )
		, m_glRenderBuffer( p_gl, p_gl.GetRboStorage( p_format ), *this )
	{
	}

	GlES3ColourRenderBuffer::~GlES3ColourRenderBuffer()
	{
	}

	bool GlES3ColourRenderBuffer::Create()
	{
		return m_glRenderBuffer.Create();
	}

	void GlES3ColourRenderBuffer::Destroy()
	{
		m_glRenderBuffer.Destroy();
	}

	bool GlES3ColourRenderBuffer::Initialise( Castor::Size const & p_size )
	{
		return m_glRenderBuffer.Initialise( p_size );
	}

	void GlES3ColourRenderBuffer::Cleanup()
	{
		m_glRenderBuffer.Cleanup();
	}

	void GlES3ColourRenderBuffer::Bind()
	{
		m_glRenderBuffer.Bind();
	}

	void GlES3ColourRenderBuffer::Unbind()
	{
		m_glRenderBuffer.Unbind();
	}

	bool GlES3ColourRenderBuffer::Resize( Castor::Size const & p_size )
	{
		return m_glRenderBuffer.Resize( p_size );
	}
}
