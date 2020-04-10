#include "Castor3D/Binary/BinarySkeletonAnimationNode.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimationObject.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationNode >::doWrite( SkeletonAnimationNode const & obj )
	{
		bool result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );

		if ( result )
		{
			result = BinaryWriter< SkeletonAnimationObject >{}.write( obj, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	castor::String BinaryParser< SkeletonAnimationNode >::Name = cuT( "SkeletonAnimationNode" );

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
				checkError( result, "Couldn't parse name." );
				break;

			case ChunkType::eAnimationObject:
				result = createBinaryParser< SkeletonAnimationObject >().parse( obj, chunk );
				checkError( result, "Couldn't parse animation object." );
				break;

			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************
}
