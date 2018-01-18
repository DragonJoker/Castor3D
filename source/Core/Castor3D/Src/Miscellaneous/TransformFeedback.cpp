#include "TransformFeedback.hpp"

#include "GpuQuery.hpp"

#include "Render/RenderSystem.hpp"

namespace castor3d
{
	TransformFeedback::TransformFeedback( RenderSystem & renderSystem, BufferDeclaration const & p_computed, Topology p_topology, ShaderProgram & p_program )
		: castor::OwnedBy< RenderSystem >( renderSystem )
		, m_program{ p_program }
		, m_query{ renderSystem.createQuery( QueryType::eTransformFeedbackPrimitivesWritten ) }
		, m_computed{ p_computed }
	{
	}

	TransformFeedback::~TransformFeedback()
	{
	}

	bool TransformFeedback::initialise( renderer::VertexBufferCRefArray const & p_buffers )
	{
		m_buffers = p_buffers;
		bool result = m_query->initialise();

		if ( result )
		{
			result = doInitialise();
		}

		return result;
	}

	void TransformFeedback::cleanup()
	{
		doCleanup();
		m_query->cleanup();
		m_query.reset();
	}

	void TransformFeedback::bind()const
	{
		doBind();
		m_query->begin();
		doBegin();
	}

	void TransformFeedback::unbind()const
	{
		doEnd();
		m_query->end();
		doUnbind();
		m_query->getInfos( QueryInfo::eResult, m_writtenPrimitives );
	}
}
