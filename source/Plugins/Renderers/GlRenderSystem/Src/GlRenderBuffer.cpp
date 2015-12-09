#include "GlRenderBuffer.hpp"
#include "OpenGl.hpp"

#include <RenderBuffer.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlRenderBuffer::GlRenderBuffer( OpenGl & p_gl, eGL_RENDERBUFFER_STORAGE p_internal, RenderBuffer & p_renderBuffer )
		: BindableType( p_gl,
						"GlRenderBuffer",
						std::bind( &OpenGl::GenRenderbuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::DeleteRenderbuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::IsRenderbuffer, std::ref( p_gl ), std::placeholders::_1 ),
						[&p_gl]( uint32_t p_glName )
						{
							return p_gl.BindRenderbuffer( eGL_RENDERBUFFER_MODE_DEFAULT, p_glName );
						} )
		, m_internal( p_internal )
		, m_size( 0, 0 )
		, m_renderBuffer( p_renderBuffer )
	{
		REQUIRE( p_internal != 0 );
	}

	GlRenderBuffer::~GlRenderBuffer()
	{
	}

	bool GlRenderBuffer::Initialise( Size const & p_size )
	{
		bool l_return = m_size == p_size;

		if ( !l_return && Bind() )
		{
			m_size = p_size;

			if ( m_renderBuffer.GetSamplesCount() > 1 )
			{
				l_return = GetOpenGl().RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE_DEFAULT, m_renderBuffer.GetSamplesCount(), m_internal, p_size );
			}
			else
			{
				l_return = GetOpenGl().RenderbufferStorage( eGL_RENDERBUFFER_MODE_DEFAULT, m_internal, p_size );
			}

			Unbind();
		}

		return l_return;
	}

	void GlRenderBuffer::Cleanup()
	{
	}

	bool GlRenderBuffer::Resize( Size const & p_size )
	{
		bool l_return = m_size == p_size;

		if ( !l_return && Bind() )
		{
			m_size = p_size;

			if ( m_renderBuffer.GetSamplesCount() > 1 )
			{
				l_return = GetOpenGl().RenderbufferStorageMultisample( eGL_RENDERBUFFER_MODE_DEFAULT, m_renderBuffer.GetSamplesCount(), m_internal, p_size );
			}
			else
			{
				l_return = GetOpenGl().RenderbufferStorage( eGL_RENDERBUFFER_MODE_DEFAULT, m_internal, p_size );
			}

			Unbind();
		}

		return l_return;
	}
}
