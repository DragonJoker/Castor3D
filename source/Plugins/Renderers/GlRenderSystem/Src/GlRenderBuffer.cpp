#include "GlRenderBuffer.hpp"
#include "OpenGl.hpp"

#include <RenderBuffer.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlRenderBuffer::GlRenderBuffer( OpenGl & p_gl, eGL_RENDERBUFFER_STORAGE p_eInternal, RenderBuffer & p_renderBuffer )
		:	m_uiGlName( eGL_INVALID_INDEX )
		,	m_eInternal( p_eInternal )
		,	m_size( 0, 0 )
		,	m_renderBuffer( p_renderBuffer )
		,	m_gl( p_gl )
	{
		CASTOR_ASSERT( m_eInternal != 0 );
	}

	GlRenderBuffer::~GlRenderBuffer()
	{
	}

	bool GlRenderBuffer::Create()
	{
		if ( m_gl.HasFbo() )
		{
			m_gl.GenRenderbuffers( 1, &m_uiGlName );
			glTrack( m_gl, GlRenderBuffer, this );
		}

		return m_uiGlName != eGL_INVALID_INDEX;
	}

	void GlRenderBuffer::Destroy()
	{
		if ( m_uiGlName != eGL_INVALID_INDEX )
		{
			glUntrack( m_gl, this );
			m_gl.DeleteRenderbuffers( 1, &m_uiGlName );
			m_uiGlName = uint32_t( eGL_INVALID_INDEX );
		}
	}

	bool GlRenderBuffer::Initialise( Size const & p_size )
	{
		bool l_return = m_size == p_size;

		if ( !l_return && Bind() )
		{
			m_size = p_size;

			if ( m_renderBuffer.GetSamplesCount() > 1 )
			{
				l_return = m_gl.RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE_DEFAULT, m_renderBuffer.GetSamplesCount(), m_eInternal, p_size );
			}
			else
			{
				l_return = m_gl.RenderbufferStorage( eGL_RENDERBUFFER_MODE_DEFAULT, m_eInternal, p_size );
			}

			Unbind();
		}

		return l_return;
	}

	void GlRenderBuffer::Cleanup()
	{
	}

	bool GlRenderBuffer::Bind()
	{
		return m_uiGlName != eGL_INVALID_INDEX && m_gl.BindRenderbuffer( eGL_RENDERBUFFER_MODE_DEFAULT, m_uiGlName );
	}

	void GlRenderBuffer::Unbind()
	{
		m_uiGlName != eGL_INVALID_INDEX && m_gl.BindRenderbuffer( eGL_RENDERBUFFER_MODE_DEFAULT, 0 );
	}

	bool GlRenderBuffer::Resize( Size const & p_size )
	{
		bool l_return = m_size == p_size;

		if ( !l_return && Bind() )
		{
			m_size = p_size;

			if ( m_renderBuffer.GetSamplesCount() > 1 )
			{
				l_return = m_gl.RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE_DEFAULT, m_renderBuffer.GetSamplesCount(), m_eInternal, p_size );
			}
			else
			{
				l_return = m_gl.RenderbufferStorage( eGL_RENDERBUFFER_MODE_DEFAULT, m_eInternal, p_size );
			}

			Unbind();
		}

		return l_return;
	}
}
