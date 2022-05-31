#include "Castor3D/Binary/BinarySubmesh.hpp"

#include "Castor3D/Binary/BinaryMorphComponent.hpp"
#include "Castor3D/Binary/BinarySkinComponent.hpp"
#include "Castor3D/Buffer/GeometryBuffers.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp"

namespace castor3d
{
	//*************************************************************************************************

	namespace v1_3
	{
		template< typename T >
		inline void copyVertices( uint32_t count
			, OldInterleavedVertexT< T > const * src
			, InterleavedVertex * dst )
		{
			for ( uint32_t i{ 0u }; i < count; ++i )
			{
				dst->pos[0] = float( src->pos[0] );
				dst->pos[1] = float( src->pos[1] );
				dst->pos[2] = float( src->pos[2] );
				dst->nml[0] = float( src->nml[0] );
				dst->nml[1] = float( src->nml[1] );
				dst->nml[2] = float( src->nml[2] );
				dst->tan[0] = float( src->tan[0] );
				dst->tan[1] = float( src->tan[1] );
				dst->tan[2] = float( src->tan[2] );
				dst->tex[0] = float( src->tex[0] );
				dst->tex[1] = float( src->tex[1] );
				dst->tex[2] = float( src->tex[2] );
				dst++;
				src++;
			}
		}
	}

	//*************************************************************************************************

	namespace v1_5
	{
		inline void dispatchVertices( InterleavedVertexArray const & src
			, castor::Point3fArray & pos
			, castor::Point3fArray & nml
			, castor::Point3fArray & tan
			, castor::Point3fArray & tex )
		{
			pos.reserve( src.size() );
			nml.reserve( src.size() );
			tan.reserve( src.size() );
			tex.reserve( src.size() );

			for ( auto & vertex : src )
			{
				pos.push_back( vertex.pos );
				nml.push_back( vertex.nml );
				tan.push_back( vertex.tan );
				tex.push_back( vertex.tex );
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< Submesh >::doWrite( Submesh const & obj )
	{
		auto count = obj.getPointsCount();
		bool result = doWriteChunk( count, ChunkType::eSubmeshVertexCount, m_chunk );

		if ( result
			&& obj.hasComponent( PositionsComponent::Name ) )
		{
			auto & values = obj.getComponent< PositionsComponent >()->getData();
			result = doWriteChunk( values, ChunkType::eSubmeshPositions, m_chunk );
		}

		if ( result
			&& obj.hasComponent( NormalsComponent::Name ) )
		{
			auto & values = obj.getComponent< NormalsComponent >()->getData();
			result = doWriteChunk( values, ChunkType::eSubmeshNormals, m_chunk );
		}

		if ( result
			&& obj.hasComponent( TangentsComponent::Name ) )
		{
			auto & values = obj.getComponent< TangentsComponent >()->getData();
			result = doWriteChunk( values, ChunkType::eSubmeshTangents, m_chunk );
		}

		if ( result
			&& obj.hasComponent( Texcoords0Component::Name ) )
		{
			auto & values = obj.getComponent< Texcoords0Component >()->getData();
			result = doWriteChunk( values, ChunkType::eSubmeshTexcoords0, m_chunk );
		}

		if ( result
			&& obj.hasComponent( Texcoords1Component::Name ) )
		{
			auto & values = obj.getComponent< Texcoords1Component >()->getData();
			result = doWriteChunk( values, ChunkType::eSubmeshTexcoords1, m_chunk );
		}

		if ( result
			&& obj.hasComponent( Texcoords2Component::Name ) )
		{
			auto & values = obj.getComponent< Texcoords2Component >()->getData();
			result = doWriteChunk( values, ChunkType::eSubmeshTexcoords2, m_chunk );
		}

		if ( result
			&& obj.hasComponent( Texcoords3Component::Name ) )
		{
			auto & values = obj.getComponent< Texcoords3Component >()->getData();
			result = doWriteChunk( values, ChunkType::eSubmeshTexcoords3, m_chunk );
		}

		if ( result
			&& obj.hasComponent( ColoursComponent::Name ) )
		{
			auto & values = obj.getComponent< ColoursComponent >()->getData();
			result = doWriteChunk( values, ChunkType::eSubmeshColours, m_chunk );
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
			if ( auto component = obj.getComponent< SkinComponent >() )
			{
				BinaryWriter< SkinComponent >{}.write( *component, m_chunk );
			}
		}

		if ( result )
		{
			if ( auto component = obj.getComponent< MorphComponent >() )
			{
				BinaryWriter< MorphComponent >{}.write( *component, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< Submesh >::Name = cuT( "Submesh" );

	bool BinaryParser< Submesh >::doParse( Submesh & obj )
	{
		bool result = true;
		castor::String name;
		std::vector< FaceIndices > faces;
		std::vector< LineIndices > lines;
		castor::Point3fArray values;
		uint32_t count{ 0u };
		uint32_t components{ 0u };
		uint32_t faceCount{ 0u };
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSubmeshVertexCount:
				if ( m_fileVersion > Version{ 1, 5, 0 } )
				{
					result = doParseChunk( count, chunk );
					checkError( result, "Couldn't parse vertex count." );

					if ( result )
					{
						values.resize( count );
					}
				}
				break;
			case ChunkType::eSubmeshPositions:
				if ( auto component = std::make_shared< PositionsComponent >( obj ) )
				{
					result = doParseChunk( values, chunk );
					checkError( result, "Couldn't parse vertex positions." );

					if ( result )
					{
						component->setData( values );
						obj.addComponent( component );
					}
				}
				break;
			case ChunkType::eSubmeshNormals:
				if ( auto component = std::make_shared< NormalsComponent >( obj ) )
				{
					result = doParseChunk( values, chunk );
					checkError( result, "Couldn't parse vertex normals." );

					if ( result )
					{
						component->setData( values );
						obj.addComponent( component );
					}
				}
				break;
			case ChunkType::eSubmeshTangents:
				if ( auto component = std::make_shared< TangentsComponent >( obj ) )
				{
					result = doParseChunk( values, chunk );
					checkError( result, "Couldn't parse vertex tangents." );

					if ( result )
					{
						component->setData( values );
						obj.addComponent( component );
					}
				}
				break;
			case ChunkType::eSubmeshTexcoords0:
				if ( auto component = std::make_shared< Texcoords0Component >( obj ) )
				{
					result = doParseChunk( values, chunk );
					checkError( result, "Couldn't parse vertex texcoords." );

					if ( result )
					{
						component->setData( values );
						obj.addComponent( component );
					}
				}
				break;
			case ChunkType::eSubmeshTexcoords1:
				if ( auto component = std::make_shared< Texcoords1Component >( obj ) )
				{
					result = doParseChunk( values, chunk );
					checkError( result, "Couldn't parse vertex texcoords." );

					if ( result )
					{
						component->setData( values );
						obj.addComponent( component );
					}
				}
				break;
			case ChunkType::eSubmeshTexcoords2:
				if ( auto component = std::make_shared< Texcoords2Component >( obj ) )
				{
					result = doParseChunk( values, chunk );
					checkError( result, "Couldn't parse vertex texcoords." );

					if ( result )
					{
						component->setData( values );
						obj.addComponent( component );
					}
				}
				break;
			case ChunkType::eSubmeshTexcoords3:
				if ( auto component = std::make_shared< Texcoords3Component >( obj ) )
				{
					result = doParseChunk( values, chunk );
					checkError( result, "Couldn't parse vertex texcoords." );

					if ( result )
					{
						component->setData( values );
						obj.addComponent( component );
					}
				}
				break;
			case ChunkType::eSubmeshColours:
				if ( auto component = std::make_shared< ColoursComponent >( obj ) )
				{
					result = doParseChunk( values, chunk );
					checkError( result, "Couldn't parse vertex colours." );

					if ( result )
					{
						component->setData( values );
						obj.addComponent( component );
					}
				}
				break;
			case ChunkType::eBonesComponent:
				if ( auto component = std::make_shared< SkinComponent >( obj ) )
				{
					result = createBinaryParser< SkinComponent >().parse( *component, chunk );
					checkError( result, "Couldn't parse bones component." );

					if ( result )
					{
						obj.addComponent( component );
					}
				}
				break;
			case ChunkType::eMorphComponent:
				if ( auto component = std::make_shared< MorphComponent >( obj, MorphFlags{} ) )
				{
					result = createBinaryParser< MorphComponent >().parse( *component, chunk );
					checkError( result, "Couldn't parse morph component." );

					if ( result )
					{
						obj.addComponent( component );
					}
				}
				break;
			case ChunkType::eSubmeshIndexComponentCount:
				result = doParseChunk( components, chunk );
				checkError( result, "Couldn't parse index component size." );
				break;
			case ChunkType::eSubmeshIndexCount:
				result = doParseChunk( count, chunk );
				checkError( result, "Couldn't parse index count." );
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
						checkError( result, "Couldn't parse index data." );

						if ( result )
						{
							auto indexMapping = obj.createComponent< TriFaceMapping >();
							indexMapping->addFaceGroup( faces );
						}
					}
					else if ( components == 2u )
					{
						result = doParseChunk( lines, chunk );
						checkError( result, "Couldn't parse index data." );

						if ( result )
						{
							auto indexMapping = obj.createComponent< LinesMapping >();
							indexMapping->addLineGroup( lines );
						}
					}
				}
				faceCount = 0u;
				break;
			default:
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
			uint32_t boneCount{ 0u };
			std::vector< VertexBoneData > bones;
			std::shared_ptr< SkinComponent > bonesComponent;

			while ( result && doGetSubChunk( chunk ) )
			{
				switch ( chunk.getChunkType() )
				{
				case ChunkType::eSubmeshVertexCount:
					result = doParseChunk( count, chunk );
					checkError( result, "Couldn't parse vertex count." );
					if ( result )
					{
						srcbuf.resize( count );
					}
					break;

#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				case ChunkType::eSubmeshVertex:
#pragma GCC diagnostic pop
#pragma warning( pop )
					result = doParseChunk( srcbuf, chunk );
					checkError( result, "Couldn't parse vertex data." );
					if ( result && !srcbuf.empty() )
					{
						std::vector< InterleavedVertex > dstbuf( srcbuf.size() );
						v1_3::copyVertices( uint32_t( srcbuf.size() ), srcbuf.data(), dstbuf.data() );
						auto positions = obj.createComponent< PositionsComponent >();
						auto normals = obj.createComponent< NormalsComponent >();
						auto tangents = obj.createComponent< TangentsComponent >();
						auto texcoords = obj.createComponent< Texcoords0Component >();
						v1_5::dispatchVertices( dstbuf
							, positions->getData()
							, normals->getData()
							, tangents->getData()
							, texcoords->getData() );
					}
					break;
#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				case ChunkType::eSubmeshFaceCount:
#pragma GCC diagnostic pop
#pragma warning( pop )
					result = doParseChunk( count, chunk );
					checkError( result, "Couldn't parse faces count." );
					if ( result )
					{
						faceCount = count;
						faces.resize( count );
					}
					break;
#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				case ChunkType::eSubmeshFaces:
#pragma GCC diagnostic pop
#pragma warning( pop )
					result = doParseChunk( faces, chunk );
					checkError( result, "Couldn't parse faces data." );
					if ( result && faceCount > 0 )
					{
						auto indexMapping = obj.createComponent< TriFaceMapping >();

						for ( auto & face : faces )
						{
							std::swap( face.m_index[1], face.m_index[2] );
						}

						indexMapping->addFaceGroup( faces );
					}
					faceCount = 0u;
					break;
				case ChunkType::eSubmeshBoneCount:
					if ( !bonesComponent )
					{
						bonesComponent = std::make_shared< SkinComponent >( obj );
						obj.addComponent( bonesComponent );
					}
					result = doParseChunk( count, chunk );
					checkError( result, "Couldn't parse bones count." );
					if ( result )
					{
						boneCount = count;
						bones.resize( count );
					}
					break;
				case ChunkType::eSubmeshBones:
					result = doParseChunk( bones, chunk );
					checkError( result, "Couldn't parse bones data." );
					if ( result && boneCount > 0 )
					{
						bonesComponent->addDatas( bones );
					}
					boneCount = 0u;
					break;
				default:
					break;
				}
			}
		}

		return result;
	}

	bool BinaryParser< Submesh >::doParse_v1_5( Submesh & obj )
	{
		bool result = true;

		if ( m_fileVersion <= Version{ 1, 5, 0 } )
		{
			BinaryChunk chunk;
			uint32_t count{};
			std::vector< InterleavedVertex > vertices;

			while ( result && doGetSubChunk( chunk ) )
			{
				switch ( chunk.getChunkType() )
				{
				case ChunkType::eSubmeshVertexCount:
					if ( m_fileVersion > Version{ 1, 3, 0 } )
					{
						result = doParseChunk( count, chunk );
						checkError( result, "Couldn't parse vertex count." );
						if ( result )
						{
							vertices.resize( count );
						}
					}
					break;
#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				case ChunkType::eSubmeshVertex:
#pragma GCC diagnostic pop
#pragma warning( pop )
					if ( m_fileVersion > Version{ 1, 3, 0 } )
					{
						result = doParseChunk( vertices, chunk );
						checkError( result, "Couldn't parse vertex data." );

						if ( result && !vertices.empty() )
						{
							auto positions = obj.createComponent< PositionsComponent >();
							auto normals = obj.createComponent< NormalsComponent >();
							auto tangents = obj.createComponent< TangentsComponent >();
							auto texcoords = obj.createComponent< Texcoords0Component >();
							v1_5::dispatchVertices( vertices
								, positions->getData()
								, normals->getData()
								, tangents->getData()
								, texcoords->getData() );
						}
					}
					break;
				default:
					break;
				}
			}
		}

		return result;
	}

	//*************************************************************************************************
}
