#include "Castor3D/Binary/BinaryMesh.hpp"

#include "Castor3D/Binary/BinarySkeleton.hpp"
#include "Castor3D/Binary/BinarySubmesh.hpp"
#include "Castor3D/Mesh/Mesh.hpp"
#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Mesh/Skeleton/Skeleton.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Mesh >::doWrite( Mesh const & obj )
	{
		bool result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );

		for ( auto submesh : obj )
		{
			result &= BinaryWriter< Submesh >{}.write( *submesh, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Mesh >::doParse( Mesh & obj )
	{
		bool result = true;
		SubmeshSPtr submesh;
		SkeletonSPtr skeleton;
		String name;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( name, chunk );

				if ( result )
				{
					obj.m_name = name;
				}

				break;

			case ChunkType::eSubmesh:
				submesh = std::make_shared< Submesh >( *obj.getScene(), obj, obj.getSubmeshCount() );
				result = createBinaryParser< Submesh >().parse( *submesh, chunk );

				if ( result )
				{
					obj.m_submeshes.push_back( submesh );
				}

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
		String name;
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeleton:
				skeleton = std::make_shared< Skeleton >( *obj.getScene() );
				result = createBinaryParser< Skeleton >().parse( *skeleton, chunk );

				if ( result )
				{
					obj.setSkeleton( skeleton );
				}

				break;
			}
		}

		return result;
	}
}
