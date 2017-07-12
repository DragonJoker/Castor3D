#include "MeshAnimationInstanceSubmesh.hpp"

#include "Animation/Mesh/MeshAnimation.hpp"
#include "MeshAnimationInstance.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Shader/Uniform.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		inline void DoFind( real p_time,
							typename SubmeshAnimationBufferArray::const_iterator const & p_first,
							typename SubmeshAnimationBufferArray::const_iterator const & p_last,
							typename SubmeshAnimationBufferArray::const_iterator & p_prv,
							typename SubmeshAnimationBufferArray::const_iterator & p_cur )
		{
			while ( p_prv != p_first && p_prv->m_timeIndex >= p_time )
			{
				// Time has gone too fast backward.
				--p_prv;
				--p_cur;
			}

			while ( p_cur != p_last && p_cur->m_timeIndex < p_time )
			{
				// Time has gone too fast forward.
				++p_prv;
				++p_cur;
			}

			ENSURE( p_prv != p_cur );
		}
	}

	//*************************************************************************************************

	MeshAnimationInstanceSubmesh::MeshAnimationInstanceSubmesh( MeshAnimationInstance & p_animationInstance, MeshAnimationSubmesh & p_animationObject )
		: OwnedBy< MeshAnimationInstance >{ p_animationInstance }
		, m_animationObject{ p_animationObject }
		, m_first{ p_animationObject.GetBuffers().begin() }
		, m_last{ p_animationObject.GetBuffers().end() }
		, m_prev{ p_animationObject.GetBuffers().empty() ? p_animationObject.GetBuffers().end() : p_animationObject.GetBuffers().begin() }
		, m_curr{ p_animationObject.GetBuffers().empty() ? p_animationObject.GetBuffers().end() : p_animationObject.GetBuffers().begin() }
	{
		if ( !p_animationObject.GetBuffers().empty() )
		{
			++m_curr;
			--m_last;
		}
	}

	MeshAnimationInstanceSubmesh::~MeshAnimationInstanceSubmesh()
	{
	}

	void MeshAnimationInstanceSubmesh::Update( std::chrono::milliseconds const & p_time )
	{
		m_currentFactor = 0.0f;

		if ( m_first != m_last
			 && m_animationObject.GetSubmesh().HasAnimationBuffer() )
		{
			auto l_curr = m_curr;
			real l_time = real( p_time.count() );
			DoFind( l_time, m_first, m_last, m_prev, m_curr );
			m_currentFactor = float( ( l_time - m_prev->m_timeIndex ) / ( m_curr->m_timeIndex - m_prev->m_timeIndex ) );

			if ( l_curr != m_curr )
			{
				uint32_t const l_size = uint32_t( m_curr->m_buffer.size() * sizeof( InterleavedVertex ) );
				auto & l_vertexBuffer = m_animationObject.GetSubmesh().GetVertexBuffer();
				auto & l_animBuffer = m_animationObject.GetSubmesh().GetAnimationBuffer();
				std::memcpy( l_vertexBuffer.GetData(), m_prev->m_buffer.data(), l_vertexBuffer.GetSize() );
				std::memcpy( l_animBuffer.GetData(), m_curr->m_buffer.data(), l_animBuffer.GetSize() );
				m_animationObject.GetSubmesh().NeedUpdate();
			}
		}
	}

	//*************************************************************************************************
}
