#include "FrameBuffer/GlES3RenderBuffer.hpp"

#include "Common/OpenGlES3.hpp"

#include <FrameBuffer/RenderBuffer.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3RenderBuffer::GlES3RenderBuffer( OpenGlES3 & p_gl, GlES3Internal p_internal, RenderBuffer & p_renderBuffer )
		: BindableType( p_gl,
						"GlES3RenderBuffer",
						std::bind( &OpenGlES3::GenRenderbuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGlES3::DeleteRenderbuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGlES3::IsRenderbuffer, std::ref( p_gl ), std::placeholders::_1 ),
						[&p_gl]( uint32_t p_glName )
						{
							return p_gl.BindRenderbuffer( GlES3RenderBufferMode::eDefault, p_glName );
						} )
		, m_internal( p_internal )
		, m_size( 0, 0 )
		, m_renderBuffer( p_renderBuffer )
	{
		REQUIRE( p_internal != GlES3Internal( 0 ) );
	}

	GlES3RenderBuffer::~GlES3RenderBuffer()
	{
	}

	bool GlES3RenderBuffer::Initialise( Size const & p_size )
	{
		bool l_return = m_size == p_size;

		if ( !l_return )
		{
			Bind();
			m_size = p_size;

			if ( m_renderBuffer.GetSamplesCount() > 1 )
			{
				GetOpenGlES3().RenderbufferStorageMultisample( GlES3RenderBufferMode::eDefault, m_renderBuffer.GetSamplesCount(), m_internal, p_size );
			}
			else
			{
				GetOpenGlES3().RenderbufferStorage( GlES3RenderBufferMode::eDefault, m_internal, p_size );
			}

			Unbind();
			l_return = true;
		}

		return l_return;
	}

	void GlES3RenderBuffer::Cleanup()
	{
	}

	bool GlES3RenderBuffer::Resize( Size const & p_size )
	{
		bool l_return = m_size == p_size;

		if ( !l_return )
		{
			Bind();
			m_size = p_size;

			if ( m_renderBuffer.GetSamplesCount() > 1 )
			{
				GetOpenGlES3().RenderbufferStorageMultisample( GlES3RenderBufferMode::eDefault, m_renderBuffer.GetSamplesCount(), m_internal, p_size );
			}
			else
			{
				GetOpenGlES3().RenderbufferStorage( GlES3RenderBufferMode::eDefault, m_internal, p_size );
			}

			Unbind();
		}

		return l_return;
	}
}
