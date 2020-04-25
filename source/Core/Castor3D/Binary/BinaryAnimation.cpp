#include "Castor3D/Binary/BinaryAnimation.hpp"

#include "Castor3D/Animation/Animation.hpp"
#include "Castor3D/Binary/BinaryMeshAnimation.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimation.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Animation >::doWrite( Animation const & obj )
	{
		bool result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );

		if ( result )
		{
			result = doWriteChunk( obj.getLength().count() / 1000.0f, ChunkType::eAnimLength, m_chunk );
		}

		if ( result )
		{
			switch ( obj.getType() )
			{
			case AnimationType::eSkeleton:
				BinaryWriter< SkeletonAnimation >{}.write( static_cast< SkeletonAnimation const & >( obj ), m_chunk );
				break;

			case AnimationType::eMesh:
				BinaryWriter< MeshAnimation >{}.write( static_cast< MeshAnimation const & >( obj ), m_chunk );
				break;

			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< Animation >::Name = cuT( "Animation" );

	bool BinaryParser< Animation >::doParse( Animation & obj )
	{
		bool result = true;
		String name;
		BinaryChunk chunk;
		float length{ 0.0f };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( name, chunk );
				checkError( result, "Couldn't parse name." );

				if ( result )
				{
					obj.m_name = name;
				}

				break;

			case ChunkType::eAnimLength:
				result = doParseChunk( length, chunk );
				checkError( result, "Couldn't parse length." );
				obj.m_length = Milliseconds( uint64_t( length ) * 1000u );
				break;

			case ChunkType::eSkeletonAnimation:
				log::info << "Loading skeleton animation [" << obj.m_name << "]\n";
				result = createBinaryParser< SkeletonAnimation >().parse( static_cast< SkeletonAnimation & >( obj ), chunk );
				break;

			case ChunkType::eMeshAnimation:
				log::info << "Loading mesh animation [" << obj.m_name << "]\n";
				result = createBinaryParser< MeshAnimation >().parse( static_cast< MeshAnimation & >( obj ), chunk );
				break;

			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************
}
