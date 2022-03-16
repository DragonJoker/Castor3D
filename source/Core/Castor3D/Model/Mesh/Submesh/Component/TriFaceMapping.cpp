#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/StagingData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Buffer/StagingBuffer.hpp>

//*************************************************************************************************

namespace castor3d
{
	namespace
	{
		struct FaceDistance
		{
			uint32_t m_index[3];
			double m_distance;
		};
		bool operator<( FaceDistance const & lhs, FaceDistance const & rhs )
		{
			return lhs.m_distance < rhs.m_distance;
		}
		CU_DeclareVector( FaceDistance, FaceDist );
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
		getOwner()->getPoint( a ).tex = castor::Point3f{ minUV[0], minUV[1], 0.0f };
		getOwner()->getPoint( b ).tex = castor::Point3f{ maxUV[0], minUV[1], 0.0f };
		getOwner()->getPoint( c ).tex = castor::Point3f{ maxUV[0], maxUV[1], 0.0f };
		getOwner()->getPoint( d ).tex = castor::Point3f{ minUV[0], maxUV[1], 0.0f };
	}

	void TriFaceMapping::computeFacesFromPolygonVertex()
	{
		SubmeshUtils::computeFacesFromPolygonVertex( getOwner()->getPoints()
			, *this );
	}

	void TriFaceMapping::computeNormals( InterleavedVertexArray & points
		, bool reverted )const
	{
		SubmeshUtils::computeNormals( points
			, *this
			, reverted );
	}

	void TriFaceMapping::computeNormals( Face const & face )
	{
		SubmeshUtils::computeNormals( getOwner()->getPoints(), face );
	}

	void TriFaceMapping::computeNormals( InterleavedVertexArray & points
		, Face const & face )const
	{
		SubmeshUtils::computeNormals( points, face );
	}

	void TriFaceMapping::computeTangents( Face const & face )
	{
		SubmeshUtils::computeTangents( getOwner()->getPoints(), face );
	}

	void TriFaceMapping::computeTangents( InterleavedVertexArray & points
		, Face const & face )const
	{
		SubmeshUtils::computeTangents( points, face );
	}

	void TriFaceMapping::computeTangentsFromNormals()
	{
		SubmeshUtils::computeTangentsFromNormals( getOwner()->getPoints()
			, *this );
	}

	void TriFaceMapping::computeTangentsFromNormals( InterleavedVertexArray & points )const
	{
		SubmeshUtils::computeTangentsFromNormals( points
			, *this );
	}

	uint32_t TriFaceMapping::getCount()const
	{
		return uint32_t( m_faces.size() );
	}

	uint32_t TriFaceMapping::getComponentsCount()const
	{
		return 3u;
	}

	void TriFaceMapping::sortByDistance( castor::Point3f const & cameraPosition )
	{
		CU_Require( getOwner()->isInitialised() );

		try
		{
			if ( m_cameraPosition != cameraPosition )
			{
				if ( getOwner()->isInitialised()
					&& getOwner()->getBufferOffsets().hasVertices()
					&& getOwner()->getBufferOffsets().hasIndices() )
				{
					auto offsets = getOwner()->getBufferOffsets();
					auto & indices = offsets.getIndexBuffer();
					auto & vertices = offsets.getVertexBuffer();

					m_cameraPosition = cameraPosition;
					auto indexSize = uint32_t( offsets.getIndexCount() );

					if ( auto * index = reinterpret_cast< uint32_t * >( indices.lock( offsets.getIndexOffset()
						, offsets.idxChunk.size
						, 0u ) ) )
					{
						FaceDistArray arraySorted;
						arraySorted.reserve( indexSize / 3 );

						if ( auto * vertex = reinterpret_cast< InterleavedVertex const * >( vertices.lock( offsets.getVertexOffset()
							, offsets.vtxChunk.size
							, 0u ) ) )
						{
							for ( uint32_t * it = index + 0; it < index + indexSize; it += 3 )
							{
								double dDistance = 0.0;
								auto & vtx1 = vertex[it[0]].pos;
								dDistance += castor::point::lengthSquared( vtx1 - cameraPosition );
								auto & vtx2 = vertex[it[1]].pos;
								dDistance += castor::point::lengthSquared( vtx2 - cameraPosition );
								auto & vtx3 = vertex[it[2]].pos;
								dDistance += castor::point::lengthSquared( vtx3 - cameraPosition );
								arraySorted.push_back( FaceDistance{ { it[0], it[1], it[2] }, dDistance } );
							}

							std::sort( arraySorted.begin(), arraySorted.end() );

							for ( auto & face : arraySorted )
							{
								*index++ = face.m_index[0];
								*index++ = face.m_index[1];
								*index++ = face.m_index[2];
							}

							vertices.unlock();
						}

						indices.flush( offsets.getIndexOffset()
							, offsets.idxChunk.size );
						indices.unlock();
					}
				}
			}
		}
		catch ( castor::Exception const & exc )
		{
			log::error << "Submesh::SortFaces - Error: " << exc.what() << std::endl;
		}
	}

	void TriFaceMapping::computeNormals( bool reverted )
	{
		if ( !m_hasNormals )
		{
			SubmeshUtils::computeNormals( getOwner()->getPoints()
				, *this
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

		if ( count
			&& getOwner()->getBufferOffsets().getIndexCount() )
		{
			auto offsets = getOwner()->getBufferOffsets();
			std::copy( m_faces.begin()
				, m_faces.end()
				, offsets.getIndexData< Face >().begin() );
			offsets.markIndexDirty();
		}
	}
}
