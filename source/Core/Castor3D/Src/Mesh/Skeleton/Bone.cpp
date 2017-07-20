#include "Bone.hpp"

#include "Skeleton.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< Bone >::DoWrite( Bone const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			result = DoWriteChunk( p_obj.GetName(), ChunkType::eName, m_chunk );
		}

		if ( result )
		{
			result = DoWriteChunk( p_obj.GetOffsetMatrix(), ChunkType::eBoneOffsetMatrix, m_chunk );
		}

		if ( p_obj.GetParent() )
		{
			result = DoWriteChunk( p_obj.GetParent()->GetName(), ChunkType::eBoneParentName, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Bone >::DoParse( Bone & p_obj )
	{
		bool result = true;
		BoneSPtr bone;
		String name;
		BinaryChunk chunk;
		auto & skeleton = p_obj.m_skeleton;

		while ( result && DoGetSubChunk( chunk ) )
		{
			switch ( chunk.GetChunkType() )
			{
			case ChunkType::eName:
				result = DoParseChunk( name, chunk );

				if ( result )
				{
					p_obj.SetName( name );
				}

				break;

			case ChunkType::eBoneOffsetMatrix:
				result = DoParseChunk( p_obj.m_offset, chunk );
				break;

			case ChunkType::eBoneParentName:
				result = DoParseChunk( name, chunk );

				if ( result )
				{
					auto parent = skeleton.FindBone( name );

					if ( parent )
					{
						parent->AddChild( p_obj.shared_from_this() );
						p_obj.SetParent( parent );
					}
					else
					{
						result = false;
					}
				}

				break;
			}
		}

		return result;
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
