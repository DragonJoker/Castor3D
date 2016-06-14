#include "MeshAnimationInstanceSubmesh.hpp"

#include "Animation/Mesh/MeshAnimation.hpp"
#include "Animation/Mesh/MeshAnimationSubmesh.hpp"
#include "MeshAnimationInstance.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		inline void DoFind( real p_time,
					 typename SubmeshAnimationBufferArray::const_iterator const & p_beg,
					 typename SubmeshAnimationBufferArray::const_iterator const & p_end,
					 typename SubmeshAnimationBufferArray::const_iterator & p_prv,
					 typename SubmeshAnimationBufferArray::const_iterator & p_cur )
		{
			while ( p_prv != p_beg && p_prv->m_timeIndex >= p_time )
			{
				// Time has gone too fast backward.
				--p_prv;
				--p_cur;
			}

			while ( p_cur != p_end && p_cur->m_timeIndex < p_time )
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
		, m_begin{ p_animationObject.GetBuffers().begin() }
		, m_end{ p_animationObject.GetBuffers().end() }
		, m_prev{ p_animationObject.GetBuffers().empty() ? p_animationObject.GetBuffers().end() : p_animationObject.GetBuffers().begin() }
		, m_curr{ p_animationObject.GetBuffers().empty() ? p_animationObject.GetBuffers().end() : p_animationObject.GetBuffers().begin() }
	{
		if ( !p_animationObject.GetBuffers().empty() )
		{
			++m_curr;
			--m_end;
		}
	}

	MeshAnimationInstanceSubmesh::~MeshAnimationInstanceSubmesh()
	{
	}

	void MeshAnimationInstanceSubmesh::Update( real p_time )
	{
		if ( m_begin != m_end )
		{
			DoFind( p_time, m_begin, m_end, m_prev, m_curr );
			uint32_t const l_size = uint32_t( m_curr->m_buffer.size() * sizeof( InterleavedVertex ) );
			auto l_vertexBuffer = m_animationObject.GetSubmesh().GetVertexBuffer();

			if ( l_vertexBuffer.Bind() )
			{
				uint8_t * l_locked{ l_vertexBuffer.Lock( 0, l_size, eACCESS_TYPE_WRITE ) };

				if ( l_locked )
				{
					std::memcpy( l_locked, m_curr->m_buffer.data(), l_size );
					l_vertexBuffer.Unlock();
				}

				l_vertexBuffer.Unbind();
			}
		}
	}

	//*************************************************************************************************
}
