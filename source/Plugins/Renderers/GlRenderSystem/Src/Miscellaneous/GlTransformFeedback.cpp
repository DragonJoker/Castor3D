#include "GlTransformFeedback.hpp"

#include "Mesh/GlAttribute.hpp"
#include "Render/GlRenderSystem.hpp"

#include "Buffer/GlBuffer.hpp"

using namespace Castor3D;

namespace GlRender
{
	GlTransformFeedback::GlTransformFeedback( OpenGl & p_gl, GlRenderSystem & renderSystem, BufferDeclaration const & p_computed, Topology p_topology, ShaderProgram & p_program )
		: TransformFeedback{ renderSystem, p_computed, p_topology, p_program }
		, BindableType{ p_gl,
						"GlTransformFeedback",
						std::bind( &OpenGl::GenTransformFeedbacks, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::DeleteTransformFeedbacks, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
						std::bind( &OpenGl::IsTransformFeedback, std::ref( p_gl ), std::placeholders::_1 ),
						[&p_gl]( uint32_t p_glName )
						{
							return p_gl.BindTransformFeedback( GlBufferTarget::eTransformFeedback, p_glName );
						} }
		, m_topology{ p_gl.Get( p_topology ) }
	{
	}

	GlTransformFeedback::~GlTransformFeedback()
	{
	}

	bool GlTransformFeedback::DoInitialise()
	{
		bool result = BindableType::Create();

		if ( result )
		{
			BindableType::Bind();
			uint32_t index{ 0u };

			for ( auto & buffer : m_buffers )
			{
				auto & tb = static_cast< GlBuffer const & >( buffer.get().GetGpuBuffer() );
				GetOpenGl().BindBufferBase( GlBufferTarget::eTransformFeedbackBuffer, index++, tb.GetGlName() );
			}

			BindableType::Unbind();
		}

		return result;
	}

	void GlTransformFeedback::DoCleanup()
	{
		BindableType::Destroy();
	}

	void GlTransformFeedback::DoBind()const
	{
		BindableType::Bind();
	}

	void GlTransformFeedback::DoUnbind()const
	{
		BindableType::Unbind();
	}

	void GlTransformFeedback::DoBegin()const
	{
		GetOpenGl().BeginTransformFeedback( m_topology );
	}

	void GlTransformFeedback::DoEnd()const
	{
		GetOpenGl().EndTransformFeedback();
	}
}
