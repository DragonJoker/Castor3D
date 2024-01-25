#include "Castor3D/Binary/BinaryMesh.hpp"

#include "Castor3D/Binary/BinarySkeleton.hpp"
#include "Castor3D/Binary/BinarySubmesh.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Mesh >::doWrite( Mesh const & obj )
	{
		bool result = true;

		for ( auto & submesh : obj )
		{
			if ( result
				&& submesh->getPointsCount() > 0
				&& ( !submesh->getIndexMapping()
					|| submesh->getIndexMapping()->getCount() > 0 ) )
			{
				result = BinaryWriter< Submesh >{}.write( *submesh, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< Mesh >::Name = cuT( "Mesh" );

	bool BinaryParser< Mesh >::doParse( Mesh & obj )
	{
		bool result = true;
		SubmeshUPtr submesh{};
		BinaryChunk chunk{ doIsLittleEndian() };

		while ( result && doGetSubChunk( chunk ) )
		{
			if ( chunk.getChunkType() == ChunkType::eSubmesh )
			{
				submesh = castor::makeUnique< Submesh >( obj, obj.getSubmeshCount() );
				result = createBinaryParser< Submesh >().parse( *submesh, chunk );
				checkError( result, cuT( "Couldn't parse submesh." ) );

				if ( result
					&& submesh->getPointsCount() > 0
					&& ( !submesh->getIndexMapping()
						|| submesh->getIndexMapping()->getCount() > 0 ) )
				{
					obj.m_submeshes.push_back( castor::move( submesh ) );
				}
			}
		}

		return result;
	}

	bool BinaryParser< Mesh >::doParse_v1_2( Mesh & obj )
	{
		bool result = true;
		SkeletonRPtr skeleton;
		BinaryChunk chunk{ doIsLittleEndian() };

		while ( result && doGetSubChunk( chunk ) )
		{
			if ( chunk.getChunkType() == ChunkType::eSkeleton )
			{
				skeleton = obj.getScene()->addNewSkeleton( obj.getName(), *obj.getScene() );
				result = createBinaryParser< Skeleton >().parse( *skeleton, chunk );
				checkError( result, cuT( "Couldn't parse skeleton." ) );

				if ( result )
				{
					obj.setSkeleton( skeleton );
				}
			}
		}

		return result;
	}

	bool BinaryParser< Mesh >::doParse_v1_4( Mesh & obj )
	{
		bool result = true;
		BinaryChunk chunk{ doIsLittleEndian() };

		while ( doGetSubChunk( chunk ) )
		{
			if ( chunk.getChunkType() == ChunkType::eName )
			{
				// Name is ignored, since it's now set by the scene file
			}
		}

		return result;
	}
}
