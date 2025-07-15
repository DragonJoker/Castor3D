#include "Castor3D/Binary/BinarySkeletonNode.hpp"

#include "Castor3D/Model/Skeleton/SkeletonNode.hpp"

namespace c3d
{
	//*************************************************************************************************

	bool BinaryWriter< SkeletonNode >::doWrite( SkeletonNode const & obj )
	{
		bool result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );

		if ( obj.getParent() )
		{
			result = doWriteChunk( obj.getParent()->getName(), ChunkType::eBoneParentName, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	String BinaryParserBase< SkeletonNode >::Name = cuT( "Bone" );

	bool BinaryParser< SkeletonNode >::doParse( SkeletonNode & obj )
	{
		bool result = true;
		String name;
		BinaryChunk chunk{ doIsLittleEndian() };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( name, chunk );
				checkError( result, cuT( "Couldn't parse name." ) );
				if ( result )
				{
					obj.rename( name );
				}
				break;
			case ChunkType::eBoneParentName:
				result = doParseChunk( parentName, chunk );
				checkError( result, cuT( "Couldn't parse parent name." ) );
				break;
			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************
}
