#include "Castor3D/Binary/BinaryMesh.hpp"

#include "Castor3D/Binary/BinarySkeleton.hpp"
#include "Castor3D/Binary/BinarySubmesh.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Mesh >::doWrite( Mesh const & obj )
	{
		bool result = true;

		for ( auto submesh : obj )
		{
			result = result && BinaryWriter< Submesh >{}.write( *submesh, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< Mesh >::Name = cuT( "Mesh" );

	bool BinaryParser< Mesh >::doParse( Mesh & obj )
	{
		bool result = true;
		SubmeshSPtr submesh;
		SkeletonSPtr skeleton;
		castor::String name;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSubmesh:
				submesh = std::make_shared< Submesh >( obj, obj.getSubmeshCount() );
				result = createBinaryParser< Submesh >().parse( *submesh, chunk );
				checkError( result, "Couldn't parse submesh." );

				if ( result )
				{
					obj.m_submeshes.push_back( submesh );
				}

				break;

			default:
				break;
			}
		}

		if ( result )
		{
			obj.computeContainers();
		}

		return result;
	}

	bool BinaryParser< Mesh >::doParse_v1_2( Mesh & obj )
	{
		bool result = true;
		SubmeshSPtr submesh;
		SkeletonSPtr skeleton;
		castor::String name;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeleton:
				skeleton = std::make_shared< Skeleton >( *obj.getScene() );
				result = createBinaryParser< Skeleton >().parse( *skeleton, chunk );
				checkError( result, "Couldn't parse skeleton." );

				if ( result )
				{
					obj.setSkeleton( skeleton );
				}

				break;

			default:
				break;
			}
		}

		return result;
	}

	bool BinaryParser< Mesh >::doParse_v1_4( Mesh & obj )
	{
		bool result = true;
		castor::String name;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				// Name is parsed then ignored, since it's now set by the scene file
				result = doParseChunk( name, chunk );
				checkError( result, "Couldn't parse name." );
				break;

			default:
				break;
			}
		}

		return result;
	}
}
