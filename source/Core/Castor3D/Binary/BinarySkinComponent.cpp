#include "Castor3D/Binary/BinarySkinComponent.hpp"

#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< SkinComponent >::doWrite( SkinComponent const & obj )
	{
		auto count = uint32_t( obj.getData().m_bones.size() );
		bool result = doWriteChunk( count, ChunkType::eSubmeshBoneCount, m_chunk );

		if ( result )
		{
			result = doWriteChunk( obj.getData().m_bones.data()
				, count
				, ChunkType::eSubmeshBones
				, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< SkinComponent >::Name = cuT( "SkinComponent" );

	bool BinaryParser< SkinComponent >::doParse( SkinComponent & obj )
	{
		bool result = true;
		castor::String name;
		castor::Vector< VertexBoneData > bones;
		uint32_t count{ 0u };
		uint32_t boneCount{ 0u };
		BinaryChunk chunk{ doIsLittleEndian() };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSubmeshBoneCount:
				result = doParseChunk( count, chunk );
				checkError( result, cuT( "Couldn't parse bone count." ) );

				if ( result )
				{
					boneCount = count;
					bones.resize( count );
				}

				break;

			case ChunkType::eSubmeshBones:
				result = doParseChunk( bones, chunk );
				checkError( result, cuT( "Couldn't parse bones data." ) );

				if ( result && boneCount > 0 )
				{
					obj.getData().addDatas( bones );
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
