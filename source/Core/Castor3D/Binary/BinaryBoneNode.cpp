#include "Castor3D/Binary/BinaryBoneNode.hpp"

#include "Castor3D/Model/Skeleton/BoneNode.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< BoneNode >::doWrite( BoneNode const & obj )
	{
		bool result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );

		if ( result )
		{
			result = doWriteChunk( obj.getInverseTransform(), ChunkType::eBoneOffsetMatrix, m_chunk );
		}

		if ( obj.getParent() )
		{
			result = doWriteChunk( obj.getParent()->getName(), ChunkType::eBoneParentName, m_chunk );
		}

		if ( obj.getParent() )
		{
			result = doWriteChunk( obj.getId(), ChunkType::eBoneId, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< BoneNode >::Name = cuT( "Bone" );

	bool BinaryParser< BoneNode >::doParse( BoneNode & obj )
	{
		bool result = true;
		castor::String name;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( name, chunk );
				checkError( result, "Couldn't parse name." );
				if ( result )
				{
					obj.rename( name );
				}
				break;
			case ChunkType::eBoneParentName:
				result = doParseChunk( parentName, chunk );
				checkError( result, "Couldn't parse parent name." );
				break;
			case ChunkType::eBoneOffsetMatrix:
				result = doParseChunk( obj.m_inverseTransform, chunk );
				checkError( result, "Couldn't parse offset matrix." );
				break;
			case ChunkType::eBoneId:
				result = doParseChunk( obj.m_id, chunk );
				checkError( result, "Couldn't parse id." );
				break;
			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************
}
