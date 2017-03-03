#include "GlES3TransformFeedback.hpp"

#include "Mesh/GlES3Attribute.hpp"
#include "Render/GlES3RenderSystem.hpp"

#include "Buffer/GlES3Buffer.hpp"

using namespace Castor3D;

namespace GlES3Render
{
	GlES3TransformFeedback::GlES3TransformFeedback( OpenGlES3 & p_gl, GlES3RenderSystem & p_renderSystem, BufferDeclaration const & p_computed, Topology p_topology, ShaderProgram & p_program )
		: TransformFeedback{ p_renderSystem, p_computed, p_topology, p_program }
		, BindableType{ p_gl,
						"GlES3TransformFeedback",
						std::bind( &OpenGlES3::GenTransformFeedbacks, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGlES3::DeleteTransformFeedbacks, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGlES3::IsTransformFeedback, std::ref( p_gl ), std::placeholders::_1 ),
						[&p_gl]( uint32_t p_glName )
						{
							return p_gl.BindTransformFeedback( GlES3BufferTarget::eTransformFeedback, p_glName );
						} }
		, m_topology{ p_gl.Get( p_topology ) }
	{
	}

	GlES3TransformFeedback::~GlES3TransformFeedback()
	{
	}

	bool GlES3TransformFeedback::DoInitialise()
	{
		bool l_return = BindableType::Create();

		if ( l_return )
		{
			BindableType::Bind();
			uint32_t l_index{ 0u };

			for ( auto & l_buffer : m_buffers )
			{
				auto & l_tb = static_cast< GlES3Buffer< uint8_t > const & >( l_buffer.get().GetGpuBuffer() );
				GetOpenGlES3().BindBufferBase( GlES3BufferTarget::eTransformFeedbackBuffer, l_index++, l_tb.GetGlES3Name() );
			}

			BindableType::Unbind();
		}

		return l_return;
	}

	void GlES3TransformFeedback::DoCleanup()
	{
		BindableType::Destroy();
	}

	void GlES3TransformFeedback::DoBind()const
	{
		BindableType::Bind();
	}

	void GlES3TransformFeedback::DoUnbind()const
	{
		BindableType::Unbind();
	}

	void GlES3TransformFeedback::DoBegin()const
	{
		GetOpenGlES3().BeginTransformFeedback( m_topology );
	}

	void GlES3TransformFeedback::DoEnd()const
	{
		GetOpenGlES3().EndTransformFeedback();
	}
}
