#include "MeshAnimationInstanceSubmesh.hpp"

#include "Animation/Mesh/MeshAnimation.hpp"
#include "MeshAnimationInstance.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/SubmeshComponent/MorphComponent.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Shader/Uniform/Uniform.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		inline void doFind( real time
			, typename SubmeshAnimationBufferArray::const_iterator const & first
			, typename SubmeshAnimationBufferArray::const_iterator const & last
			, typename SubmeshAnimationBufferArray::const_iterator & prv
			, typename SubmeshAnimationBufferArray::const_iterator & cur )
		{
			while ( prv != first && prv->m_timeIndex >= time )
			{
				// Time has gone too fast backward.
				--prv;
				--cur;
			}

			while ( cur != last && cur->m_timeIndex < time )
			{
				// Time has gone too fast forward.
				++prv;
				++cur;
			}

			ENSURE( prv != cur );
		}
	}

	//*************************************************************************************************

	MeshAnimationInstanceSubmesh::MeshAnimationInstanceSubmesh( MeshAnimationInstance & animationInstance
		, MeshAnimationSubmesh & animationObject )
		: OwnedBy< MeshAnimationInstance >{ animationInstance }
		, m_animationObject{ animationObject }
		, m_first{ animationObject.getBuffers().begin() }
		, m_last{ animationObject.getBuffers().end() }
		, m_prev{ animationObject.getBuffers().empty() ? animationObject.getBuffers().end() : animationObject.getBuffers().begin() }
		, m_curr{ animationObject.getBuffers().empty() ? animationObject.getBuffers().end() : animationObject.getBuffers().begin() }
	{
		if ( !animationObject.getBuffers().empty() )
		{
			++m_curr;
			--m_last;
		}
	}

	MeshAnimationInstanceSubmesh::~MeshAnimationInstanceSubmesh()
	{
	}

	void MeshAnimationInstanceSubmesh::update( Milliseconds const & time )
	{
		m_currentFactor = 0.0f;

		if ( m_first != m_last )
		{
			auto curr = m_curr;
			auto timef = real( time.count() );
			doFind( timef, m_first, m_last, m_prev, m_curr );
			m_currentFactor = float( ( timef - m_prev->m_timeIndex ) / ( m_curr->m_timeIndex - m_prev->m_timeIndex ) );

			if ( curr != m_curr )
			{
				auto & vertexBuffer = m_animationObject.getSubmesh().getVertexBuffer();
				auto & animBuffer = m_animationObject.getComponent().getAnimationBuffer();
				std::memcpy( vertexBuffer.getData(), m_prev->m_buffer.data(), vertexBuffer.getSize() );
				std::memcpy( animBuffer.getData(), m_curr->m_buffer.data(), animBuffer.getSize() );
				m_animationObject.getSubmesh().needsUpdate();
				m_animationObject.getComponent().needsUpdate();
			}
		}
	}

	//*************************************************************************************************
}
