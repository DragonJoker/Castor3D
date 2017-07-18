#include "FrameBuffer/GlRenderBuffer.hpp"

#include "Common/OpenGl.hpp"

#include <FrameBuffer/RenderBuffer.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlRenderBuffer::GlRenderBuffer( OpenGl & p_gl, GlInternal p_internal, RenderBuffer & p_renderBuffer )
		: BindableType( p_gl,
						"GlRenderBuffer",
						std::bind( &OpenGl::GenRenderbuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::DeleteRenderbuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::IsRenderbuffer, std::ref( p_gl ), std::placeholders::_1 ),
						[&p_gl]( uint32_t p_glName )
						{
							return p_gl.BindRenderbuffer( GlRenderBufferMode::eDefault, p_glName );
						} )
		, m_internal( p_internal )
		, m_size( 0, 0 )
		, m_renderBuffer( p_renderBuffer )
	{
		REQUIRE( p_internal != GlInternal( 0 ) );
	}

	GlRenderBuffer::~GlRenderBuffer()
	{
	}

	bool GlRenderBuffer::Initialise( Size const & p_size )
	{
		bool result = m_size == p_size;

		if ( !result )
		{
			Bind();
			m_size = p_size;

			if ( m_renderBuffer.GetSamplesCount() > 1 )
			{
				GetOpenGl().RenderbufferStorageMultisample( GlRenderBufferMode::eDefault, m_renderBuffer.GetSamplesCount(), m_internal, p_size );
			}
			else
			{
				GetOpenGl().RenderbufferStorage( GlRenderBufferMode::eDefault, m_internal, p_size );
			}

			Unbind();
			result = true;
		}

		return result;
	}

	void GlRenderBuffer::Cleanup()
	{
	}

	bool GlRenderBuffer::Resize( Size const & p_size )
	{
		bool result = m_size == p_size;

		if ( !result )
		{
			Bind();
			m_size = p_size;

			if ( m_renderBuffer.GetSamplesCount() > 1 )
			{
				GetOpenGl().RenderbufferStorageMultisample( GlRenderBufferMode::eDefault, m_renderBuffer.GetSamplesCount(), m_internal, p_size );
			}
			else
			{
				GetOpenGl().RenderbufferStorage( GlRenderBufferMode::eDefault, m_internal, p_size );
			}

			Unbind();
		}

		return result;
	}
}
