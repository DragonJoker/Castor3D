#include "MeshAnimation.hpp"

#include "Engine.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Mesh/Submesh.hpp"
#include "Animation/Mesh/MeshAnimationSubmesh.hpp"

#include "Scene/Geometry.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< MeshAnimation >::DoWrite( MeshAnimation const & p_obj )
	{
		bool l_return = true;

		for ( auto const & l_submesh : p_obj.m_submeshes )
		{
			l_return &= DoWriteChunk( l_submesh.GetSubmesh().GetId(), ChunkType::eMeshAnimationSubmeshID, m_chunk );
			l_return &= BinaryWriter< MeshAnimationSubmesh >{}.Write( l_submesh, m_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< MeshAnimation >::DoParse( MeshAnimation & p_obj )
	{
		bool l_return = true;
		MeshAnimationSubmeshUPtr l_submeshAnim;
		SubmeshSPtr l_submesh;
		BinaryChunk l_chunk;
		uint32_t l_id{ 0u };

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case ChunkType::eMeshAnimationSubmeshID:
				l_return = DoParseChunk( l_id, l_chunk );

				if ( l_return )
				{
					l_submesh = static_cast< Mesh & >( *p_obj.GetOwner() ).GetSubmesh( l_id );
				}

				break;

			case ChunkType::eMeshAnimationSubmesh:
				if ( l_submesh )
				{
					MeshAnimationSubmesh l_submeshAnim{ p_obj, *l_submesh };
					l_return = BinaryParser< MeshAnimationSubmesh >{}.Parse( l_submeshAnim, l_chunk );

					if ( l_return )
					{
						p_obj.AddChild( std::move( l_submeshAnim ) );
					}
				}

				break;
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	MeshAnimation::MeshAnimation( Animable & p_animable, String const & p_name )
		: Animation{ AnimationType::eMesh, p_animable, p_name }
	{
	}

	MeshAnimation::~MeshAnimation()
	{
	}

	void MeshAnimation::AddChild( MeshAnimationSubmesh && p_object )
	{
		m_submeshes.push_back( std::move( p_object ) );
	}

	void MeshAnimation::DoUpdateLength()
	{
		for ( auto const & l_submesh : m_submeshes )
		{
			m_length = std::max( m_length, l_submesh.GetLength() );
		}
	}

	//*************************************************************************************************
}
