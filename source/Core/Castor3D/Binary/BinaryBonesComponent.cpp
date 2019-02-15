#include "Castor3D/Binary/BinaryBonesComponent.hpp"

#include "Castor3D/Mesh/SubmeshComponent/BonesComponent.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< BonesComponent >::doWrite( BonesComponent const & obj )
	{
		uint32_t count = uint32_t( obj.m_bones.size() );
		bool result = doWriteChunk( count, ChunkType::eSubmeshBoneCount, m_chunk );

		if ( result )
		{
			result = doWriteChunk( obj.m_bones.data()
				, count
				, ChunkType::eSubmeshBones
				, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< BonesComponent >::doParse( BonesComponent & obj )
	{
		bool result = true;
		String name;
		std::vector< VertexBoneData > bones;
		uint32_t count{ 0u };
		uint32_t boneCount{ 0u };
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSubmeshBoneCount:
				result = doParseChunk( count, chunk );

				if ( result )
				{
					boneCount = count;
					bones.resize( count );
				}

				break;

			case ChunkType::eSubmeshBones:
				result = doParseChunk( bones, chunk );

				if ( result && boneCount > 0 )
				{
					obj.addBoneDatas( bones );
				}

				boneCount = 0u;
				break;

			default:
				result = false;
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************
}
