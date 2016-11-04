#include "TransformFeedback.hpp"

#include "GpuQuery.hpp"

#include "Render/RenderSystem.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"

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
		bool l_return = m_query->Initialise();

		if ( l_return )
		{
			l_return = DoInitialise();
		}

		return l_return;
	}

	void TransformFeedback::Cleanup()
	{
		DoCleanup();
		m_query->Cleanup();
		m_query.reset();
	}

	bool TransformFeedback::Bind()const
	{
		bool l_return = DoBind();

		if ( l_return )
		{
			l_return = m_query->Begin();
		}

		if ( l_return )
		{
			l_return = DoBegin();
		}

		return l_return;
	}

	void TransformFeedback::Unbind()const
	{
		DoEnd();
		m_query->End();
		DoUnbind();
		m_query->GetInfos( QueryInfo::eResult, m_writtenPrimitives );
	}
}
