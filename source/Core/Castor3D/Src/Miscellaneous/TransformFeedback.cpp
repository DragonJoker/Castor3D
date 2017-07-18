#include "TransformFeedback.hpp"

#include "GpuQuery.hpp"

#include "Render/RenderSystem.hpp"

namespace Castor3D
{
	TransformFeedback::TransformFeedback( RenderSystem & p_renderSystem, BufferDeclaration const & p_computed, Topology p_topology, ShaderProgram & p_program )
		: Castor::OwnedBy< RenderSystem >( p_renderSystem )
		, m_program{ p_program }
		, m_query{ p_renderSystem.CreateQuery( QueryType::eTransformFeedbackPrimitivesWritten ) }
		, m_computed{ p_computed }
	{
	}

	TransformFeedback::~TransformFeedback()
	{
	}

	bool TransformFeedback::Initialise( VertexBufferArray const & p_buffers )
	{
		m_buffers = p_buffers;
		bool l_result = m_query->Initialise();

		if ( l_result )
		{
			l_result = DoInitialise();
		}

		return l_result;
	}

	void TransformFeedback::Cleanup()
	{
		DoCleanup();
		m_query->Cleanup();
		m_query.reset();
	}

	void TransformFeedback::Bind()const
	{
		DoBind();
		m_query->Begin();
		DoBegin();
	}

	void TransformFeedback::Unbind()const
	{
		DoEnd();
		m_query->End();
		DoUnbind();
		m_query->GetInfos( QueryInfo::eResult, m_writtenPrimitives );
	}
}
