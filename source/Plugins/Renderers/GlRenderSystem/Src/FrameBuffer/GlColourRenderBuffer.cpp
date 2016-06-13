#include "FrameBuffer/GlColourRenderBuffer.hpp"

#include "Common/OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlColourRenderBuffer::GlColourRenderBuffer( OpenGl & p_gl, ePIXEL_FORMAT p_format )
		: ColourRenderBuffer( p_format )
		, m_glRenderBuffer( p_gl, p_gl.GetRboStorage( p_format ), *this )
	{
	}

	GlColourRenderBuffer::~GlColourRenderBuffer()
	{
	}

	bool GlColourRenderBuffer::Create()
	{
		return m_glRenderBuffer.Create();
	}

	void GlColourRenderBuffer::Destroy()
	{
		m_glRenderBuffer.Destroy();
	}

	bool GlColourRenderBuffer::Initialise( Castor::Size const & p_size )
	{
		return m_glRenderBuffer.Initialise( p_size );
	}

	void GlColourRenderBuffer::Cleanup()
	{
		m_glRenderBuffer.Cleanup();
	}

	bool GlColourRenderBuffer::Bind()
	{
		return m_glRenderBuffer.Bind();
	}

	void GlColourRenderBuffer::Unbind()
	{
		m_glRenderBuffer.Unbind();
	}

	bool GlColourRenderBuffer::Resize( Castor::Size const & p_size )
	{
		return m_glRenderBuffer.Resize( p_size );
	}
}
