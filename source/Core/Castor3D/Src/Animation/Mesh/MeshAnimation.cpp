#include "MeshAnimation.hpp"

#include "Engine.hpp"

#include "Mesh/Submesh.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< MeshAnimation >::DoWrite( MeshAnimation const & p_obj )
	{
		bool l_result = true;

		for ( auto const & l_submesh : p_obj.m_submeshes )
		{
			l_result &= DoWriteChunk( l_submesh.GetSubmesh().GetId(), ChunkType::eMeshAnimationSubmeshID, m_chunk );
			l_result &= BinaryWriter< MeshAnimationSubmesh >{}.Write( l_submesh, m_chunk );
		}

		return l_result;
	}

	//*************************************************************************************************

	bool BinaryParser< MeshAnimation >::DoParse( MeshAnimation & p_obj )
	{
		bool l_result = true;
		MeshAnimationSubmeshUPtr l_submeshAnim;
		SubmeshSPtr l_submesh;
		BinaryChunk l_chunk;
		uint32_t l_id{ 0u };

		while ( l_result && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case ChunkType::eMeshAnimationSubmeshID:
				l_result = DoParseChunk( l_id, l_chunk );

				if ( l_result )
				{
					l_submesh = static_cast< Mesh & >( *p_obj.GetOwner() ).GetSubmesh( l_id );
				}

				break;

			case ChunkType::eMeshAnimationSubmesh:
				if ( l_submesh )
				{
					MeshAnimationSubmesh l_submeshAnim{ p_obj, *l_submesh };
					l_result = BinaryParser< MeshAnimationSubmesh >{}.Parse( l_submeshAnim, l_chunk );

					if ( l_result )
					{
						p_obj.AddChild( std::move( l_submeshAnim ) );
					}
				}

				break;
			}
		}

		return l_result;
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
