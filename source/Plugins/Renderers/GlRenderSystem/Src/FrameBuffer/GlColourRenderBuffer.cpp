#include "FrameBuffer/GlColourRenderBuffer.hpp"

#include "Common/OpenGl.hpp"

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlColourRenderBuffer::GlColourRenderBuffer( OpenGl & p_gl, PixelFormat p_format )
		: ColourRenderBuffer( p_format )
		, m_glRenderBuffer( p_gl, p_gl.getRboStorage( p_format ), *this )
	{
	}

	GlColourRenderBuffer::~GlColourRenderBuffer()
	{
	}

	bool GlColourRenderBuffer::create()
	{
		return m_glRenderBuffer.create();
	}

	void GlColourRenderBuffer::destroy()
	{
		m_glRenderBuffer.destroy();
	}

	bool GlColourRenderBuffer::initialise( castor::Size const & p_size )
	{
		return m_glRenderBuffer.initialise( p_size );
	}

	void GlColourRenderBuffer::cleanup()
	{
		m_glRenderBuffer.cleanup();
	}

	void GlColourRenderBuffer::bind()
	{
		m_glRenderBuffer.bind();
	}

	void GlColourRenderBuffer::unbind()
	{
		m_glRenderBuffer.unbind();
	}

	bool GlColourRenderBuffer::resize( castor::Size const & p_size )
	{
		return m_glRenderBuffer.resize( p_size );
	}
}
