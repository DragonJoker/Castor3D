#include "Castor3D/Binary/BinaryBone.hpp"

#include "Castor3D/Model/Skeleton/Bone.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Bone >::doWrite( Bone const & obj )
	{
		bool result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );

		if ( result )
		{
			result = doWriteChunk( obj.getOffsetMatrix(), ChunkType::eBoneOffsetMatrix, m_chunk );
		}

		if ( obj.getParent() )
		{
			result = doWriteChunk( obj.getParent()->getName(), ChunkType::eBoneParentName, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< Bone >::Name = cuT( "Bone" );

	bool BinaryParser< Bone >::doParse( Bone & obj )
	{
		bool result = true;
		BoneSPtr bone;
		castor::String name;
		BinaryChunk chunk;
		auto & skeleton = obj.m_skeleton;

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

			case ChunkType::eBoneOffsetMatrix:
				result = doParseChunk( obj.m_offset, chunk );
				checkError( result, "Couldn't parse offset matrix." );
				break;

			case ChunkType::eBoneParentName:
				result = doParseChunk( name, chunk );
				checkError( result, "Couldn't parse parent name." );

				if ( result )
				{
					auto parent = skeleton.findBone( name );

					if ( parent )
					{
						parent->addChild( obj.shared_from_this() );
						obj.setParent( parent );
					}
					else
					{
						result = false;
						checkError( result, "Couldn't find parent bone " + name );
					}
				}

				break;

			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************
}
