#include "Bone.hpp"

#include "Skeleton.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Bone >::doWrite( Bone const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( p_obj.getName(), ChunkType::eName, m_chunk );
		}

		if ( result )
		{
			result = doWriteChunk( p_obj.getOffsetMatrix(), ChunkType::eBoneOffsetMatrix, m_chunk );
		}

		if ( p_obj.getParent() )
		{
			result = doWriteChunk( p_obj.getParent()->getName(), ChunkType::eBoneParentName, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Bone >::doParse( Bone & p_obj )
	{
		bool result = true;
		BoneSPtr bone;
		String name;
		BinaryChunk chunk;
		auto & skeleton = p_obj.m_skeleton;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( name, chunk );

				if ( result )
				{
					p_obj.setName( name );
				}

				break;

			case ChunkType::eBoneOffsetMatrix:
				result = doParseChunk( p_obj.m_offset, chunk );
				break;

			case ChunkType::eBoneParentName:
				result = doParseChunk( name, chunk );

				if ( result )
				{
					auto parent = skeleton.findBone( name );

					if ( parent )
					{
						parent->addChild( p_obj.shared_from_this() );
						p_obj.setParent( parent );
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
		: Named( cuEmptyString )
		, m_skeleton( p_skeleton )
	{
	}

	Bone::~Bone()
	{
	}

	void Bone::addChild( BoneSPtr p_bone )
	{
		if ( m_children.end() == m_children.find( p_bone->getName() ) )
		{
			m_children.insert( { p_bone->getName(), p_bone } );
		}
	}
}
