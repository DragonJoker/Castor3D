#include "MeshAnimation.hpp"

#include "Engine.hpp"

#include "Mesh/Submesh.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< MeshAnimation >::DoWrite( MeshAnimation const & p_obj )
	{
		bool result = true;

		for ( auto const & submesh : p_obj.m_submeshes )
		{
			result &= DoWriteChunk( submesh.GetSubmesh().GetId(), ChunkType::eMeshAnimationSubmeshID, m_chunk );
			result &= BinaryWriter< MeshAnimationSubmesh >{}.Write( submesh, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< MeshAnimation >::DoParse( MeshAnimation & p_obj )
	{
		bool result = true;
		MeshAnimationSubmeshUPtr submeshAnim;
		SubmeshSPtr submesh;
		BinaryChunk chunk;
		uint32_t id{ 0u };

		while ( result && DoGetSubChunk( chunk ) )
		{
			switch ( chunk.GetChunkType() )
			{
			case ChunkType::eMeshAnimationSubmeshID:
				result = DoParseChunk( id, chunk );

				if ( result )
				{
					submesh = static_cast< Mesh & >( *p_obj.GetOwner() ).GetSubmesh( id );
				}

				break;

			case ChunkType::eMeshAnimationSubmesh:
				if ( submesh )
				{
					MeshAnimationSubmesh submeshAnim{ p_obj, *submesh };
					result = BinaryParser< MeshAnimationSubmesh >{}.Parse( submeshAnim, chunk );

					if ( result )
					{
						p_obj.AddChild( std::move( submeshAnim ) );
					}
				}

				break;
			}
		}

		return result;
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
		for ( auto const & submesh : m_submeshes )
		{
			m_length = std::max( m_length, submesh.GetLength() );
		}
	}

	//*************************************************************************************************
}
