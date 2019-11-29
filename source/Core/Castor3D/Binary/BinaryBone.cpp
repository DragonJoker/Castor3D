#include "Castor3D/Binary/BinaryBone.hpp"

#include "Castor3D/Mesh/Skeleton/Bone.hpp"
#include "Castor3D/Mesh/Skeleton/Skeleton.hpp"

using namespace castor;

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

	bool BinaryParser< Bone >::doParse( Bone & obj )
	{
		bool result = true;
		BoneSPtr bone;
		String name;
		BinaryChunk chunk;
		auto & skeleton = obj.m_skeleton;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( name, chunk );

				if ( result )
				{
					obj.setName( name );
				}

				break;

			case ChunkType::eBoneOffsetMatrix:
				result = doParseChunk( obj.m_offset, chunk );
				break;

			case ChunkType::eBoneParentName:
				result = doParseChunk( name, chunk );

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
