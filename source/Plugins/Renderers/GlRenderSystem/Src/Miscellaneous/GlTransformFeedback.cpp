#include "GlTransformFeedback.hpp"

#include "Mesh/GlAttribute.hpp"
#include "Render/GlRenderSystem.hpp"

#include "Buffer/GlBuffer.hpp"

using namespace castor3d;

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
		, m_topology{ p_gl.get( p_topology ) }
	{
	}

	GlTransformFeedback::~GlTransformFeedback()
	{
	}

	bool GlTransformFeedback::doInitialise()
	{
		bool result = BindableType::create();

		if ( result )
		{
			BindableType::bind();
			uint32_t index{ 0u };

			for ( auto & buffer : m_buffers )
			{
				auto & tb = static_cast< GlBuffer const & >( buffer.get().getGpuBuffer() );
				getOpenGl().BindBufferBase( GlBufferTarget::eTransformFeedbackBuffer, index++, tb.getGlName() );
			}

			BindableType::unbind();
		}

		return result;
	}

	void GlTransformFeedback::doCleanup()
	{
		BindableType::destroy();
	}

	void GlTransformFeedback::doBind()const
	{
		BindableType::bind();
	}

	void GlTransformFeedback::doUnbind()const
	{
		BindableType::unbind();
	}

	void GlTransformFeedback::doBegin()const
	{
		getOpenGl().BeginTransformFeedback( m_topology );
	}

	void GlTransformFeedback::doEnd()const
	{
		getOpenGl().EndTransformFeedback();
	}
}
