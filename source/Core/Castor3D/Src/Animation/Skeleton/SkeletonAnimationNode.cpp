#include "SkeletonAnimationNode.hpp"

#include "SkeletonAnimation.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationNode >::doWrite( SkeletonAnimationNode const & obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );
		}

		if ( result )
		{
			result = BinaryWriter< SkeletonAnimationObject >{}.write( obj, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationNode >::doParse( SkeletonAnimationNode & obj )
	{
		bool result = true;
		String name;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( obj.m_name, chunk );
				break;

			case ChunkType::eAnimationObject:
				result = BinaryParser< SkeletonAnimationObject >{}.parse( obj, chunk );
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	SkeletonAnimationNode::SkeletonAnimationNode( SkeletonAnimation & animation
		, String const & name )
		: SkeletonAnimationObject{ animation, SkeletonAnimationObjectType::eNode }
		, m_name{ name }
	{
	}

	SkeletonAnimationNode :: ~SkeletonAnimationNode()
	{
	}

	//*************************************************************************************************
}
