#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

CU_ImplementSmartPtr( castor3d, TriFaceMapping )

namespace castor3d
{
	//*********************************************************************************************

	namespace smshcomptri
	{
		struct FaceDistance
		{
			castor::Array< uint32_t, 3u > m_index;
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
		Face result{ a, b, c };

		if ( auto size = m_submesh.getPointsCount();
			a < size && b < size && c < size )
		{
			m_faces.push_back( result );
			m_hasNormals = false;
		}
		else
		{
			throw std::range_error( "addFace - One or more index out of bound" );
		}

		return result;
	}

	void TriFaceMapping::ComponentData::addFaceGroup( FaceIndices const * const begin
		, FaceIndices const * const end )
	{
		for ( auto & face : castor::makeArrayView( begin, end ) )
		{
			addFace( face.m_index[0], face.m_index[1], face.m_index[2] );
		}
	}

	void TriFaceMapping::ComponentData::addQuadFace( uint32_t a
		, uint32_t b
		, uint32_t c
		, uint32_t d
		, castor::Point3f const & minUV
		, castor::Point3f const & maxUV )
	{
		addFace( a, b, c );
		addFace( a, c, d );

		if ( auto texComp = m_submesh.getComponent< Texcoords0Component >() )
		{
			if ( auto texData = texComp->getDataT< Texcoords0Component::ComponentData >() )
			{
				texData->getData()[a] = castor::Point3f{ minUV[0], minUV[1], 0.0f };
				texData->getData()[b] = castor::Point3f{ maxUV[0], minUV[1], 0.0f };
				texData->getData()[c] = castor::Point3f{ maxUV[0], maxUV[1], 0.0f };
				texData->getData()[d] = castor::Point3f{ minUV[0], maxUV[1], 0.0f };
			}
		}
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
				, buffer.getBuffer()
				, buffer.getOffset()
				, VK_ACCESS_INDEX_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
		}
	}

	//*********************************************************************************************

	castor::String const TriFaceMapping::TypeName = C3D_MakeSubmeshIndexComponentName( "triangles" );

	TriFaceMapping::TriFaceMapping( Submesh & submesh
		, VkBufferUsageFlags bufferUsageFlags )
		: IndexMapping{ submesh, TypeName
			, castor::make_unique< ComponentData >( submesh, bufferUsageFlags ) }
	{
	}

	void TriFaceMapping::computeFacesFromPolygonVertex()
	{
		SubmeshUtils::computeFacesFromPolygonVertex( getOwner()->getTexcoords0()
			, *this );
	}

	void TriFaceMapping::computeNormals( castor::Point3fArray const & positions
		, castor::Point3fArray & normals
		, bool reverted )const
	{
		SubmeshUtils::computeNormals( positions
			, normals
			, getDataT< ComponentData >()->getFaces()
			, reverted );
	}

	void TriFaceMapping::computeTangentsFromNormals( castor::Point3fArray const & positions
		, castor::Point3fArray const & texcoords
		, castor::Point3fArray const & normals
		, castor::Point4fArray & tangents )const
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
		static castor::Point4fArray tan;
		static castor::Point3fArray tex;
		castor::Point4fArray * tangents = &tan;
		castor::Point3fArray const * texcoords = &tex;

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
		auto result = castor::makeUnique< TriFaceMapping >( submesh );
		result->getData().copy( &getData() );
		return castor::ptrRefCast< SubmeshComponent >( result );
	}

	//*********************************************************************************************
}
