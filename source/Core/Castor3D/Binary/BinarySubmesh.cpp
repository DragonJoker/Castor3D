#include "Castor3D/Binary/BinarySubmesh.hpp"

#include "Castor3D/Binary/BinaryBonesComponent.hpp"
#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Mesh/SubmeshComponent/BonesComponent.hpp"
#include "Castor3D/Mesh/SubmeshComponent/TriFaceMapping.hpp"
#include "Castor3D/Mesh/SubmeshComponent/LinesMapping.hpp"

using namespace castor;

//*************************************************************************************************

namespace castor3d
{
	namespace
	{
		template< typename T, typename U >
		inline void doCopyVertices( uint32_t count
			, OldInterleavedVertexT< T > const * src
			, InterleavedVertexT< U > * dst )
		{
			for ( uint32_t i{ 0u }; i < count; ++i )
			{
				dst->pos[0] = U( src->pos[0] );
				dst->pos[1] = U( src->pos[1] );
				dst->pos[2] = U( src->pos[2] );
				dst->nml[0] = U( src->nml[0] );
				dst->nml[1] = U( src->nml[1] );
				dst->nml[2] = U( src->nml[2] );
				dst->tan[0] = U( src->tan[0] );
				dst->tan[1] = U( src->tan[1] );
				dst->tan[2] = U( src->tan[2] );
				dst->tex[0] = U( src->tex[0] );
				dst->tex[1] = U( src->tex[1] );
				dst->tex[2] = U( src->tex[2] );
				dst++;
				src++;
			}
		}

		template< typename T, typename U >
		inline void doCopyVertices( uint32_t count
			, InterleavedVertexT< T > const * src
			, InterleavedVertexT< U > * dst )
		{
			for ( uint32_t i{ 0u }; i < count; ++i )
			{
				dst->pos[0] = U( src->pos[0] );
				dst->pos[1] = U( src->pos[1] );
				dst->pos[2] = U( src->pos[2] );
				dst->nml[0] = U( src->nml[0] );
				dst->nml[1] = U( src->nml[1] );
				dst->nml[2] = U( src->nml[2] );
				dst->tan[0] = U( src->tan[0] );
				dst->tan[1] = U( src->tan[1] );
				dst->tan[2] = U( src->tan[2] );
				dst->tex[0] = U( src->tex[0] );
				dst->tex[1] = U( src->tex[1] );
				dst->tex[2] = U( src->tex[2] );
				dst++;
				src++;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< Submesh >::doWrite( Submesh const & obj )
	{
		auto count = obj.getPointsCount();
		bool result = doWriteChunk( count, ChunkType::eSubmeshVertexCount, m_chunk );

		if ( result )
		{
			result = doWriteChunk( obj.getPoints(), ChunkType::eSubmeshVertex, m_chunk );
		}

		if ( result )
		{
			count = obj.getFaceCount();

			if ( obj.hasComponent( TriFaceMapping::Name ) )
			{
				result = doWriteChunk( 3u, ChunkType::eSubmeshIndexComponentCount, m_chunk );

				if ( result )
				{
					result = doWriteChunk( count, ChunkType::eSubmeshIndexCount, m_chunk );
				}

				if ( result )
				{
					auto const * data = reinterpret_cast< FaceIndices const * >( obj.getComponent< TriFaceMapping >()->getFaces().data() );
					result = doWriteChunk( data, count, ChunkType::eSubmeshIndices, m_chunk );
				}
			}
			else if ( obj.hasComponent( LinesMapping::Name ) )
			{
				result = doWriteChunk( 2u, ChunkType::eSubmeshIndexComponentCount, m_chunk );
				count = obj.getFaceCount();

				if ( result )
				{
					result = doWriteChunk( count, ChunkType::eSubmeshIndexCount, m_chunk );
				}

				if ( result )
				{
					auto const * data = reinterpret_cast< LineIndices const * >( obj.getComponent< LinesMapping >()->getFaces().data() );
					result = doWriteChunk( data, count, ChunkType::eSubmeshIndices, m_chunk );
				}
			}
		}

		if ( result )
		{
			auto it = obj.m_components.find( BonesComponent::Name );

			if ( it != obj.m_components.end() )
			{
				BinaryWriter< BonesComponent >{}.write( *std::static_pointer_cast< BonesComponent >( it->second ), m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Submesh >::doParse( Submesh & obj )
	{
		bool result = true;
		String name;
		std::vector< FaceIndices > faces;
		std::vector< LineIndices > lines;
		std::vector< VertexBoneData > bones;
		std::vector< InterleavedVertex > vertices;
		uint32_t count{ 0u };
		uint32_t components{ 0u };
		uint32_t faceCount{ 0u };
		uint32_t boneCount{ 0u };
		BinaryChunk chunk;
		std::shared_ptr< BonesComponent > bonesComponent;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSubmeshVertexCount:
				if ( m_fileVersion > Version{ 1, 3, 0 } )
				{
					result = doParseChunk( count, chunk );

					if ( result )
					{
						vertices.resize( count );
					}
				}
				break;

			case ChunkType::eSubmeshVertex:
				if ( m_fileVersion > Version{ 1, 3, 0 } )
				{
					result = doParseChunk( vertices, chunk );

					if ( result && !vertices.empty() )
					{
						obj.addPoints( vertices );
					}
				}
				break;

			case ChunkType::eSubmeshBoneCount:
				if ( !bonesComponent )
				{
					bonesComponent = std::make_shared< BonesComponent >( obj );
					obj.addComponent( bonesComponent );
				}

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
					bonesComponent->addBoneDatas( bones );
				}

				boneCount = 0u;
				break;

			case ChunkType::eBonesComponent:
				bonesComponent = std::make_shared< BonesComponent >( obj );
				result = createBinaryParser< BonesComponent >().parse( *bonesComponent, chunk );

				if ( result )
				{
					obj.addComponent( bonesComponent );
				}

				break;

			case ChunkType::eSubmeshIndexComponentCount:
				result = doParseChunk( components, chunk );
				break;

			case ChunkType::eSubmeshIndexCount:
				result = doParseChunk( count, chunk );

				if ( result )
				{
					faceCount = count;

					if ( components == 3u )
					{
						faces.resize( count );
					}
					else if ( components == 2u )
					{
						lines.resize( count );
					}
				}

				break;

			case ChunkType::eSubmeshIndices:

				if ( faceCount > 0 )
				{
					if ( components == 3u )
					{
						result = doParseChunk( faces, chunk );

						if ( result )
						{
							auto indexMapping = std::make_shared< TriFaceMapping >( obj );
							indexMapping->addFaceGroup( faces );
							obj.setIndexMapping( indexMapping );
						}
					}
					else if ( components == 2u )
					{
						result = doParseChunk( lines, chunk );

						if ( result )
						{
							auto indexMapping = std::make_shared< LinesMapping >( obj );
							indexMapping->addLineGroup( lines );
							obj.setIndexMapping( indexMapping );
						}
					}
				}

				faceCount = 0u;
				break;

			case ChunkType::eSubmeshFaceCount:
				break;

			case ChunkType::eSubmeshFaces:
				break;

			default:
				result = false;
				break;
			}
		}

		return result;
	}

	bool BinaryParser< Submesh >::doParse_v1_3( Submesh & obj )
	{
		bool result = true;

		if ( m_fileVersion <= Version{ 1, 3, 0 } )
		{
			std::vector< FaceIndices > faces;
			std::vector< OldInterleavedVertexT< double > > srcbuf;
			uint32_t count{ 0u };
			uint32_t faceCount{ 0u };
			BinaryChunk chunk;

			while ( result && doGetSubChunk( chunk ) )
			{
				switch ( chunk.getChunkType() )
				{
				case ChunkType::eSubmeshVertexCount:
					result = doParseChunk( count, chunk );

					if ( result )
					{
						srcbuf.resize( count );
					}

					break;

				case ChunkType::eSubmeshVertex:
					result = doParseChunk( srcbuf, chunk );

					if ( result && !srcbuf.empty() )
					{
						std::vector< InterleavedVertex > dstbuf( srcbuf.size() );
						doCopyVertices( uint32_t( srcbuf.size() ), srcbuf.data(), dstbuf.data() );
						obj.addPoints( dstbuf );
					}

					break;

				case ChunkType::eSubmeshFaceCount:
					result = doParseChunk( count, chunk );

					if ( result )
					{
						faceCount = count;
						faces.resize( count );
					}

					break;

				case ChunkType::eSubmeshFaces:
					result = doParseChunk( faces, chunk );

					if ( result && faceCount > 0 )
					{
						auto indexMapping = std::make_shared< TriFaceMapping >( obj );

						for ( auto & face : faces )
						{
							std::swap( face.m_index[1], face.m_index[2] );
						}

						indexMapping->addFaceGroup( faces );
						obj.setIndexMapping( indexMapping );
					}

					faceCount = 0u;
					break;
				}
			}
		}

		return result;
	}
}
