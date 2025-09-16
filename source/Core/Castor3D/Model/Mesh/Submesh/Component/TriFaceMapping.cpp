#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Binary/ChunkWriter.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( c3d, TriFaceMapping )

namespace c3d
{
	//*********************************************************************************************

	template<>
	class TextWriter< TriFaceMapping >
		: public TextWriterT< TriFaceMapping >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< TriFaceMapping >{ tabs }
		{
		}

		bool operator()( TriFaceMapping const & object
			, StringStream & file )override
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, cuT( "faces" ) ) } )
			{
				result = true;
				for ( auto & value : object.getData().getFaces() )
				{
					StringStream stream;
					stream << value[0] << " " << value[1] << " " << value[2];
					result = result && write( file, cuT( "value " ), stream.str() );
				}
			}

			return result;
		}
	};

	//*********************************************************************************************

	namespace smshcomptri
	{
		static constexpr SectionId sectionId = makeSectionName( 'S', 'M', 'S', 'H', 'T', 'R', 'M', 'P' );

		struct FaceDistance
		{
			Array< uint32_t, 3u > m_index;
			double m_distance;
		};

		CU_DeclareVector( FaceDistance, FaceDist );

		struct Compare
		{
			bool operator()( FaceDistance const & lhs, FaceDistance const & rhs )const
			{
				return lhs.m_distance < rhs.m_distance;
			}
		};

		struct FaceMappingContext
		{
			SubmeshContext * submesh;
			Vector< FaceIndices > values;
		};

		static CU_ImplementAttributeParserNewBlock( parserSection, SubmeshContext, FaceMappingContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
			{
				newBlockContext->submesh = blockContext;
			}
		}
		CU_EndAttributePushNewBlock( sectionId )

			static CU_ImplementAttributeParserBlock( parserValue, FaceMappingContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
			{
				Point3ui indices;
				params[0]->get( indices );
				auto & face = blockContext->values.emplace_back();
				face[0] = indices[0];
				face[1] = indices[1];
				face[2] = indices[2];
			}
		}
		CU_EndAttribute()

			static CU_ImplementAttributeParserBlock( parserEnd, FaceMappingContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else if ( !blockContext->values.empty() )
			{
				if ( auto component = blockContext->submesh->submesh->createComponent< TriFaceMapping >() )
					component->getData().addFaceGroup( blockContext->values );
			}
		}
		CU_EndAttributePop()
	}

	//*********************************************************************************************

	void TriFaceMapping::ComponentData::copy( SubmeshComponentDataRPtr data )const
	{
		auto result = static_cast< ComponentData * >( data );
		result->m_faces = m_faces;
		result->m_hasNormals = m_hasNormals;
		result->m_cameraPosition = m_cameraPosition;
	}

	void TriFaceMapping::ComponentData::clearFaces()
	{
		m_faces.clear();
	}

	Face TriFaceMapping::ComponentData::addFace( uint32_t a, uint32_t b, uint32_t c )
	{
		doCheckFace( m_submesh.getPointsCount(), a, b, c );
		auto result = doAddFace( a, b, c );
		m_hasNormals = false;
		needsUpdate();
		return result;
	}

	void TriFaceMapping::ComponentData::addFaceGroup( FaceIndices const * const begin
		, FaceIndices const * const end )
	{
		for ( auto size = m_submesh.getPointsCount();
			auto & face : makeArrayView( begin, end ) )
		{
			doCheckFace( size, face.m_index[0], face.m_index[1], face.m_index[2] );
		}

		for ( auto & face : makeArrayView( begin, end ) )
		{
			doAddFace( face.m_index[0], face.m_index[1], face.m_index[2] );
		}

		m_hasNormals = false;
		needsUpdate();
	}

	void TriFaceMapping::ComponentData::addQuadFace( uint32_t a
		, uint32_t b
		, uint32_t c
		, uint32_t d
		, Point3f const & minUV
		, Point3f const & maxUV )
	{
		auto size = m_submesh.getPointsCount();
		doCheckFace( size, a, b, c );
		doCheckFace( size, a, c, d );
		doAddFace( a, b, c );
		doAddFace( a, c, d );
		m_hasNormals = false;
		needsUpdate();

		if ( auto texComp = m_submesh.getComponent< Texcoords0Component >() )
		{
			if ( auto texData = texComp->getDataT< Texcoords0Component::ComponentData >() )
			{
				texData->getData()[a] = Point3f{ minUV[0], minUV[1], 0.0f };
				texData->getData()[b] = Point3f{ maxUV[0], minUV[1], 0.0f };
				texData->getData()[c] = Point3f{ maxUV[0], maxUV[1], 0.0f };
				texData->getData()[d] = Point3f{ minUV[0], maxUV[1], 0.0f };
			}
		}
	}

	void TriFaceMapping::ComponentData::doCheckFace( uint32_t size, uint32_t a, uint32_t b, uint32_t c )const
	{
		if ( a >= size || b >= size || c >= size )
		{
			throw std::range_error( "checkFaceFace - One or more index out of bound" );
		}
	}

	Face TriFaceMapping::ComponentData::doAddFace( uint32_t a, uint32_t b, uint32_t c )
	{
		return m_faces.emplace_back( a, b, c );
	}

	void TriFaceMapping::ComponentData::doCleanup( RenderDevice const & device )
	{
		m_faces.clear();
	}

	void TriFaceMapping::ComponentData::doUpload( UploadData & uploader )
	{
		auto count = uint32_t( m_faces.size() * 3 );
		auto & offsets = m_submesh.getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshData::eIndex );

		if ( count && buffer.hasData() )
		{
			uploader.pushUpload( m_faces.data()
				, m_faces.size() * sizeof( Face )
				, buffer.getBuffer(), buffer.getOffset()
				, VertexIndexInputState );
		}
	}

	//*********************************************************************************************

	String const TriFaceMapping::TypeName = C3D_MakeSubmeshIndexComponentName( "triangles" );

	TriFaceMapping::TriFaceMapping( Submesh & submesh
		, BufferUsageFlags bufferUsageFlags )
		: IndexMapping{ submesh, TypeName
			, makeRawUnique< ComponentData >( submesh, bufferUsageFlags ) }
	{
	}

	void TriFaceMapping::computeFacesFromPolygonVertex()
	{
		SubmeshUtils::computeFacesFromPolygonVertex( getOwner()->getTexcoords0()
			, *this );
	}

	void TriFaceMapping::computeNormals( Point3fArray const & positions
		, Point3fArray & normals
		, bool reverted )const
	{
		SubmeshUtils::computeNormals( positions
			, normals
			, getDataT< ComponentData >()->getFaces()
			, reverted );
	}

	void TriFaceMapping::computeTangentsFromNormals( Point3fArray const & positions
		, Point3fArray const & texcoords
		, Point3fArray const & normals
		, Point4fArray & tangents )const
	{
		SubmeshUtils::computeTangentsFromNormals( positions
			, texcoords
			, normals
			, tangents
			, getDataT< ComponentData >()->getFaces() );
	}

	uint32_t TriFaceMapping::getCount()const
	{
		return getDataT< ComponentData >()->getCount();
	}

	void TriFaceMapping::setCount( uint32_t value )
	{
		getDataT< ComponentData >()->setCount( value );
	}

	uint32_t TriFaceMapping::getComponentsCount()const
	{
		return 3u;
	}

	void TriFaceMapping::computeNormals( bool reverted )
	{
		if ( !getDataT< ComponentData >()->hasNormals() )
		{
			getOwner()->getNormals().resize( getOwner()->getPositions().size() );
			computeNormals( getOwner()->getPositions()
				, getOwner()->getNormals()
				, reverted );
			getDataT< ComponentData >()->setHasNormals();
		}
	}

	void TriFaceMapping::computeTangents()
	{
		static Point4fArray tan;
		static Point3fArray tex;
		Point4fArray * tangents = &tan;
		Point3fArray const * texcoords = &tex;

		if ( auto tanComp = getOwner()->getComponent< TangentsComponent >() )
		{
			tangents = &tanComp->getDataT< TangentsComponent::ComponentData >()->getData();
		}

		if ( auto texComp = getOwner()->getComponent< Texcoords0Component >() )
		{
			texcoords = &texComp->getDataT< Texcoords0Component::ComponentData >()->getData();
		}

		computeTangentsFromNormals( getOwner()->getPositions()
			, *texcoords
			, getOwner()->getNormals()
			, *tangents );
	}

	SubmeshComponentUPtr TriFaceMapping::clone( Submesh & submesh )const
	{
		auto result = makeUnique< TriFaceMapping >( submesh );
		getData().copy( &result->getData() );
		return ptrRefCast< SubmeshComponent >( result );
	}

	bool TriFaceMapping::doWriteText( String const & tabs
		, StringStream & file )const
	{
		return TextWriter< TriFaceMapping >{ tabs }( *this, file );
	}

	bool TriFaceMapping::doWriteBinary( BinaryChunk & chunk )const
	{
		auto count = getData().getCount();
		auto result = ChunkWriter< u32 >::write( 3u, ChunkType::eSubmeshIndexComponentCount, chunk )
			&& ChunkWriter< u32 >::write( count, ChunkType::eSubmeshIndexCount, chunk );

		if ( result )
		{
			auto & faces = getData().getFaces();
			auto const * data = std::bit_cast< FaceIndices const * >( faces.data() );
			result = ChunkWriter< FaceIndices >::write( data, data + count, ChunkType::eSubmeshIndices, chunk );
		}

		return result;
	}

	//*********************************************************************************************

	void TriFaceMapping::Plugin::createParsers( AttributeParsers & result )const
	{
		BlockParserContextT< SubmeshContext > submeshContext{ result, CSCNSection::eSubmesh, CSCNSection::eMesh };
		BlockParserContextT< smshcomptri::FaceMappingContext > sectionContext{ result, smshcomptri::sectionId, CSCNSection::eSubmesh };

		submeshContext.addPushParser( "faces", smshcomptri::sectionId, smshcomptri::parserSection );

		sectionContext.addParser( cuT( "value" ), smshcomptri::parserValue, { makeParameter< ParameterType::ePoint3U >() } );
		sectionContext.addPopParser( cuT( "}" ), smshcomptri::parserEnd );
	}

	void TriFaceMapping::Plugin::createSections( StrSectionIdMap & sections )const
	{
		sections.try_emplace( SectionId( smshcomptri::sectionId ), "faces" );
	}

	//*********************************************************************************************
}
