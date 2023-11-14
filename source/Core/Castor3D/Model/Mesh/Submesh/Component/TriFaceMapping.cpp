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

//*************************************************************************************************

CU_ImplementSmartPtr( castor3d, TriFaceMapping )

namespace castor3d
{
	namespace smshcomptri
	{
		struct FaceDistance
		{
			uint32_t m_index[3];
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

	castor::String const TriFaceMapping::TypeName = "triface_mapping";

	TriFaceMapping::TriFaceMapping( Submesh & submesh
		, VkBufferUsageFlags bufferUsageFlags )
		: IndexMapping{ submesh, TypeName, bufferUsageFlags }
	{
	}

	void TriFaceMapping::clearFaces()
	{
		m_faces.clear();
	}

	Face TriFaceMapping::addFace( uint32_t a, uint32_t b, uint32_t c )
	{
		Face result{ a, b, c };
		auto size = getOwner()->getPointsCount();

		if ( a < size && b < size && c < size )
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

	void TriFaceMapping::addFaceGroup( FaceIndices const * const begin
		, FaceIndices const * const end )
	{
		for ( auto & face : castor::makeArrayView( begin, end ) )
		{
			addFace( face.m_index[0], face.m_index[1], face.m_index[2] );
		}
	}

	void TriFaceMapping::addQuadFace( uint32_t a
		, uint32_t b
		, uint32_t c
		, uint32_t d
		, castor::Point3f const & minUV
		, castor::Point3f const & maxUV )
	{
		addFace( a, b, c );
		addFace( a, c, d );

		if ( auto texComp = getOwner()->getComponent< Texcoords0Component >() )
		{
			texComp->getData()[a] = castor::Point3f{ minUV[0], minUV[1], 0.0f };
			texComp->getData()[b] = castor::Point3f{ maxUV[0], minUV[1], 0.0f };
			texComp->getData()[c] = castor::Point3f{ maxUV[0], maxUV[1], 0.0f };
			texComp->getData()[d] = castor::Point3f{ minUV[0], maxUV[1], 0.0f };
		}
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
			, getFaces()
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
			, getFaces() );
	}

	uint32_t TriFaceMapping::getCount()const
	{
		return uint32_t( m_faces.size() );
	}

	uint32_t TriFaceMapping::getComponentsCount()const
	{
		return 3u;
	}

	void TriFaceMapping::computeNormals( bool reverted )
	{
		if ( !m_hasNormals )
		{
			getOwner()->getNormals().resize( getOwner()->getPositions().size() );
			computeNormals( getOwner()->getPositions()
				, getOwner()->getNormals()
				, reverted );
			m_hasNormals = true;
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
			tangents = &tanComp->getData();
		}

		if ( auto texComp = getOwner()->getComponent< Texcoords0Component >() )
		{
			texcoords = &texComp->getData();
		}

		computeTangentsFromNormals( getOwner()->getPositions()
			, *texcoords
			, getOwner()->getNormals()
			, *tangents );
	}

	SubmeshComponentUPtr TriFaceMapping::clone( Submesh & submesh )const
	{
		auto result = castor::makeUnique< TriFaceMapping >( submesh );
		result->m_faces = m_faces;
		result->m_hasNormals = m_hasNormals;
		result->m_cameraPosition = m_cameraPosition;
		return castor::ptrRefCast< SubmeshComponent >( result );
	}

	void TriFaceMapping::doCleanup( RenderDevice const & device )
	{
		m_faces.clear();
	}

	void TriFaceMapping::doUpload( UploadData & uploader )
	{
		auto count = uint32_t( m_faces.size() * 3 );
		auto & offsets = getOwner()->getSourceBufferOffsets();
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
}
