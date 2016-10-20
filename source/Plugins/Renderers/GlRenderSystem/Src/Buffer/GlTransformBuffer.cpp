#include "GlTransformBuffer.hpp"

#include "Render/GlRenderSystem.hpp"

namespace GlRender
{
	GlTransformBuffer::GlTransformBuffer( GlRenderSystem & p_renderSystem, OpenGl & p_gl, size_t p_elementSize )
		: Castor3D::GpuTransformBuffer( p_renderSystem, p_elementSize )
		, BindableType{ p_gl,
						"GlTransformBuffer",
						std::bind( &OpenGl::GenTransformFeedbacks, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::DeleteTransformFeedbacks, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::IsTransformFeedback, std::ref( p_gl ), std::placeholders::_1 ),
						[&p_gl]( uint32_t p_glName )
						{
							return p_gl.BindTransformFeedback( eGL_BUFFER_TARGET_TRANSFORM_FEEDBACK, p_glName );
						} }
		, m_glBuffer{ p_gl, eGL_BUFFER_TARGET_ARRAY }
	{
	}

	GlTransformBuffer::~GlTransformBuffer()
	{
	}

	bool GlTransformBuffer::Create()
	{
		bool l_return = BindableType::Create();

		if ( l_return )
		{
			l_return = m_glBuffer.Create();
		}

		return l_return;
	}

	void GlTransformBuffer::Destroy()
	{
		m_glBuffer.Destroy();
		BindableType::Destroy();
	}

	bool GlTransformBuffer::Initialise()
	{
		bool l_return = BindableType::Bind();

		if ( l_return )
		{
			l_return = m_glBuffer.Bind();

			if ( l_return )
			{
				l_return = GetOpenGl().BindBufferBase( eGL_BUFFER_TARGET_TRANSFORM_FEEDBACK_BUFFER, 0, m_glBuffer.GetGlName() );
				m_glBuffer.Unbind();
			}

			BindableType::Unbind();
		}

		return l_return;
	}

	void GlTransformBuffer::Cleanup()
	{
		m_glBuffer.Cleanup();
	}

	bool GlTransformBuffer::Bind()
	{
		return BindableType::Bind();
	}

	void GlTransformBuffer::Unbind()
	{
		BindableType::Unbind();
	}

	bool GlTransformBuffer::Fill( uint8_t const * p_buffer, ptrdiff_t p_size, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )
	{
		return m_glBuffer.Fill( p_buffer, p_size, p_type, p_nature );
	}

	uint8_t * GlTransformBuffer::Lock( uint32_t p_offset, uint32_t p_count, Castor3D::AccessType p_flags )
	{
		return m_glBuffer.Lock( p_offset, p_count, p_flags );
	}

	void GlTransformBuffer::Unlock()
	{
		m_glBuffer.Unlock();
	}
}
