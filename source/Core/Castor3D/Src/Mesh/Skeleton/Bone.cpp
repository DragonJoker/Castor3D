#include "Bone.hpp"

#include "Skeleton.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< Bone >::DoWrite( Bone const & p_obj )
	{
		bool l_result = true;

		if ( l_result )
		{
			l_result = DoWriteChunk( p_obj.GetName(), ChunkType::eName, m_chunk );
		}

		if ( l_result )
		{
			l_result = DoWriteChunk( p_obj.GetOffsetMatrix(), ChunkType::eBoneOffsetMatrix, m_chunk );
		}

		if ( p_obj.GetParent() )
		{
			l_result = DoWriteChunk( p_obj.GetParent()->GetName(), ChunkType::eBoneParentName, m_chunk );
		}

		return l_result;
	}

	//*************************************************************************************************

	bool BinaryParser< Bone >::DoParse( Bone & p_obj )
	{
		bool l_result = true;
		BoneSPtr l_bone;
		String l_name;
		BinaryChunk l_chunk;
		auto & l_skeleton = p_obj.m_skeleton;

		while ( l_result && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case ChunkType::eName:
				l_result = DoParseChunk( l_name, l_chunk );

				if ( l_result )
				{
					p_obj.SetName( l_name );
				}

				break;

			case ChunkType::eBoneOffsetMatrix:
				l_result = DoParseChunk( p_obj.m_offset, l_chunk );
				break;

			case ChunkType::eBoneParentName:
				l_result = DoParseChunk( l_name, l_chunk );

				if ( l_result )
				{
					auto l_parent = l_skeleton.FindBone( l_name );

					if ( l_parent )
					{
						l_parent->AddChild( p_obj.shared_from_this() );
						p_obj.SetParent( l_parent );
					}
					else
					{
						l_result = false;
					}
				}

				break;
			}
		}

		return l_result;
	}

	//*************************************************************************************************

	Bone::Bone( Skeleton & p_skeleton )
		: m_skeleton( p_skeleton )
	{
	}

	Bone::~Bone()
	{
	}

	void Bone::AddChild( BoneSPtr p_bone )
	{
		if ( m_children.end() == m_children.find( p_bone->GetName() ) )
		{
			m_children.insert( { p_bone->GetName(), p_bone } );
		}
	}
}
