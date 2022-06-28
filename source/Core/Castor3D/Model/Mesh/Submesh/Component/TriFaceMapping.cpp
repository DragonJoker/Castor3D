#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/StagingData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Buffer/StagingBuffer.hpp>

//*************************************************************************************************

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

	castor::String const TriFaceMapping::Name = "triface_mapping";

	TriFaceMapping::TriFaceMapping( Submesh & submesh
		, VkBufferUsageFlags bufferUsageFlags )
		: IndexMapping{ submesh, Name, bufferUsageFlags }
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
		, castor::Point3fArray const & texcoords
		, castor::Point3fArray & normals
		, castor::Point3fArray & tangents
		, bool reverted )const
	{
		SubmeshUtils::computeNormals( positions
			, texcoords
			, normals
			, tangents
			, getFaces()
			, reverted );
	}

	void TriFaceMapping::computeNormals( Face const & face )
	{
		static castor::Point3fArray tan;
		static castor::Point3fArray tex;
		castor::Point3fArray * tangents = &tan;
		castor::Point3fArray const * texcoords = &tex;

		if ( auto tanComp = getOwner()->getComponent< TangentsComponent >() )
		{
			tangents = &tanComp->getData();
		}

		if ( auto texComp = getOwner()->getComponent< Texcoords0Component >() )
		{
			texcoords = &texComp->getData();
		}

		SubmeshUtils::computeNormals( getOwner()->getPositions()
			, *texcoords
			, getOwner()->getNormals()
			, *tangents
			, face );
	}

	void TriFaceMapping::computeNormals( castor::Point3fArray const & positions
		, castor::Point3fArray const & texcoords
		, castor::Point3fArray & normals
		, castor::Point3fArray & tangents
		, Face const & face )const
	{
		SubmeshUtils::computeNormals( positions
			, texcoords
			, normals
			, tangents
			, face );
	}

	void TriFaceMapping::computeTangents( Face const & face )
	{
		static castor::Point3fArray tan;
		static castor::Point3fArray tex;
		castor::Point3fArray * tangents = &tan;
		castor::Point3fArray const * texcoords = &tex;

		if ( auto tanComp = getOwner()->getComponent< TangentsComponent >() )
		{
			tangents = &tanComp->getData();
		}

		if ( auto texComp = getOwner()->getComponent< Texcoords0Component >() )
		{
			texcoords = &texComp->getData();
		}

		SubmeshUtils::computeTangents( getOwner()->getPositions()
			, *texcoords
			, *tangents
			, face );
	}

	void TriFaceMapping::computeTangents( castor::Point3fArray const & positions
		, castor::Point3fArray const & texcoords
		, castor::Point3fArray & tangents
		, Face const & face )const
	{
		SubmeshUtils::computeTangents( positions
			, texcoords
			, tangents
			, face );
	}

	void TriFaceMapping::computeTangentsFromNormals()
	{
		static castor::Point3fArray tan;
		static castor::Point3fArray tex;
		castor::Point3fArray * tangents = &tan;
		castor::Point3fArray const * texcoords = &tex;

		if ( auto tanComp = getOwner()->getComponent< TangentsComponent >() )
		{
			tangents = &tanComp->getData();
		}

		if ( auto texComp = getOwner()->getComponent< Texcoords0Component >() )
		{
			texcoords = &texComp->getData();
		}

		SubmeshUtils::computeTangentsFromNormals( getOwner()->getPositions()
			, *texcoords
			, getOwner()->getNormals()
			, *tangents
			, getFaces() );
	}

	void TriFaceMapping::computeTangentsFromNormals( castor::Point3fArray const & positions
		, castor::Point3fArray const & texcoords
		, castor::Point3fArray const & normals
		, castor::Point3fArray & tangents )const
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
			static castor::Point3fArray tan;
			static castor::Point3fArray tex;
			castor::Point3fArray * tangents = &tan;
			castor::Point3fArray const * texcoords = &tex;

			if ( auto tanComp = getOwner()->getComponent< TangentsComponent >() )
			{
				tangents = &tanComp->getData();
			}

			if ( auto texComp = getOwner()->getComponent< Texcoords0Component >() )
			{
				texcoords = &texComp->getData();
			}

			getOwner()->getNormals().resize( getOwner()->getPositions().size() );
			SubmeshUtils::computeNormals( getOwner()->getPositions()
				, *texcoords
				, getOwner()->getNormals()
				, *tangents
				, getFaces()
				, reverted );
			m_hasNormals = true;
		}
	}

	SubmeshComponentSPtr TriFaceMapping::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< TriFaceMapping >( submesh );
		result->m_faces = m_faces;
		result->m_hasNormals = m_hasNormals;
		result->m_cameraPosition = m_cameraPosition;
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	void TriFaceMapping::doCleanup( RenderDevice const & device )
	{
		m_faces.clear();
	}

	void TriFaceMapping::doUpload()
	{
		auto count = uint32_t( m_faces.size() * 3 );
		auto & offsets = getOwner()->getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshFlag::eIndex );

		if ( count && buffer.hasData() )
		{
			std::copy( m_faces.begin()
				, m_faces.end()
				, buffer.getData< Face >().begin() );
			buffer.markDirty( VK_ACCESS_INDEX_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
		}
	}
}
