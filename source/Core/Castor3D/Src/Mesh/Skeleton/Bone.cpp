#include "Bone.hpp"

#include "Skeleton.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< Bone >::DoWrite( Bone const & p_obj )
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetName(), ChunkType::eName, m_chunk );
		}

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetOffsetMatrix(), ChunkType::eBoneOffsetMatrix, m_chunk );
		}

		if ( p_obj.GetParent() )
		{
			l_return = DoWriteChunk( p_obj.GetParent()->GetName(), ChunkType::eBoneParentName, m_chunk );
		}

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< Bone >::DoParse( Bone & p_obj )
	{
		bool l_return = true;
		BoneSPtr l_bone;
		String l_name;
		BinaryChunk l_chunk;
		auto & l_skeleton = p_obj.m_skeleton;

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case ChunkType::eName:
				l_return = DoParseChunk( l_name, l_chunk );

				if ( l_return )
				{
					p_obj.SetName( l_name );
				}

				break;

			case ChunkType::eBoneOffsetMatrix:
				l_return = DoParseChunk( p_obj.m_offset, l_chunk );
				break;

			case ChunkType::eBoneParentName:
				l_return = DoParseChunk( l_name, l_chunk );

				if ( l_return )
				{
					auto l_parent = l_skeleton.FindBone( l_name );

					if ( l_parent )
					{
						l_parent->AddChild( p_obj.shared_from_this() );
						p_obj.SetParent( l_parent );
					}
					else
					{
						l_return = false;
					}
				}

				break;
			}
		}

		return l_return;
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
