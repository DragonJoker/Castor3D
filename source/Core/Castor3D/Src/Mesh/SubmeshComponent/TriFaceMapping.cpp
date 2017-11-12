#include "TriFaceMapping.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/SubmeshUtils.hpp"
#include "Mesh/Vertex.hpp"

#include <Design/ArrayView.hpp>

using namespace castor;

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
		DECLARE_VECTOR( FaceDistance, FaceDist );
	}

	String const TriFaceMapping::Name = "triface_mapping";

	TriFaceMapping::TriFaceMapping( Submesh & submesh )
		: IndexMapping{ submesh, Name }
	{
	}

	TriFaceMapping::~TriFaceMapping()
	{
		cleanup();
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
		for ( auto & face : makeArrayView( begin, end ) )
		{
			addFace( face.m_index[0], face.m_index[1], face.m_index[2] );
		}
	}

	void TriFaceMapping::addQuadFace( uint32_t a
		, uint32_t b
		, uint32_t c
		, uint32_t d
		, Point3r const & minUV
		, Point3r const & maxUV )
	{
		addFace( a, b, c );
		addFace( a, c, d );
		Vertex::setTexCoord( *getOwner()->getPoint( a ), minUV[0], minUV[1] );
		Vertex::setTexCoord( *getOwner()->getPoint( b ), maxUV[0], minUV[1] );
		Vertex::setTexCoord( *getOwner()->getPoint( c ), maxUV[0], maxUV[1] );
		Vertex::setTexCoord( *getOwner()->getPoint( d ), minUV[0], maxUV[1] );
	}

	void TriFaceMapping::clearFaces()
	{
		m_faces.clear();
	}

	void TriFaceMapping::computeFacesFromPolygonVertex()
	{
		SubmeshUtils::computeFacesFromPolygonVertex( *getOwner()
			, *this );
	}

	void TriFaceMapping::computeNormals( bool reverted )
	{
		if ( !m_hasNormals )
		{
			SubmeshUtils::computeNormals( *getOwner()
				, *this
				, reverted );
			m_hasNormals = true;
		}
	}

	void TriFaceMapping::computeNormals( Face const & face )
	{
		SubmeshUtils::computeNormals( *getOwner(), face );
	}

	void TriFaceMapping::computeTangents( Face const & face )
	{
		SubmeshUtils::computeTangents( *getOwner(), face );
	}

	void TriFaceMapping::computeTangentsFromNormals()
	{
		SubmeshUtils::computeTangentsFromNormals( *getOwner()
			, *this );
	}

	void TriFaceMapping::computeTangentsFromBitangents()
	{
		SubmeshUtils::computeTangentsFromBitangents( *getOwner() );
	}

	void TriFaceMapping::computeBitangents()
	{
		SubmeshUtils::computeBitangents( *getOwner() );
	}

	void TriFaceMapping::sortByDistance( Point3r const & cameraPosition )
	{
		REQUIRE( getOwner()->isInitialised() );

		try
		{
			if ( m_cameraPosition != cameraPosition )
			{
				if ( getOwner()->isInitialised()
					&& !getOwner()->getVertexBuffer().isEmpty()
					&& !getOwner()->getIndexBuffer().isEmpty() )
				{
					IndexBuffer & indices = getOwner()->getIndexBuffer();
					VertexBuffer & vertices = getOwner()->getVertexBuffer();

					vertices.bind();
					indices.bind();
					m_cameraPosition = cameraPosition;
					uint32_t indexSize = indices.getSize();
					uint32_t * index = indices.lock( 0, indexSize, AccessType::eRead | AccessType::eWrite );

					if ( index )
					{
						uint32_t stride = vertices.getDeclaration().stride();
						uint8_t * vertex = vertices.getData();
						FaceDistArray arraySorted;
						arraySorted.reserve( indexSize / 3 );

						if ( vertex )
						{
							for ( uint32_t * it = index + 0; it < index + indexSize; it += 3 )
							{
								double dDistance = 0.0;
								Coords3r vtx1( reinterpret_cast< real * >( &vertex[it[0] * stride] ) );
								dDistance += point::lengthSquared( vtx1 - cameraPosition );
								Coords3r vtx2( reinterpret_cast< real * >( &vertex[it[1] * stride] ) );
								dDistance += point::lengthSquared( vtx2 - cameraPosition );
								Coords3r vtx3( reinterpret_cast< real * >( &vertex[it[2] * stride] ) );
								dDistance += point::lengthSquared( vtx3 - cameraPosition );
								arraySorted.push_back( FaceDistance{ { it[0], it[1], it[2] }, dDistance } );
							}

							std::sort( arraySorted.begin(), arraySorted.end() );

							for ( auto & face : arraySorted )
							{
								*index++ = face.m_index[0];
								*index++ = face.m_index[1];
								*index++ = face.m_index[2];
							}
						}

						indices.unlock();
					}

					indices.unbind();
					vertices.unbind();
				}
			}
		}
		catch ( Exception const & p_exc )
		{
			Logger::logError( std::stringstream() << "Submesh::SortFaces - Error: " << p_exc.what() );
		}
	}

	void TriFaceMapping::doCleanup()
	{
		m_faces.clear();
	}

	void TriFaceMapping::doFill()
	{
		auto count = uint32_t( m_faces.size() * 3 );
		IndexBuffer & indexBuffer = getOwner()->getIndexBuffer();

		if ( count )
		{
			m_faceCount = uint32_t( m_faces.size() );

			if ( indexBuffer.getSize() != count )
			{
				indexBuffer.resize( count );
			}

			uint32_t index = 0;

			for ( auto const & face : m_faces )
			{
				indexBuffer[index++] = face[0];
				indexBuffer[index++] = face[1];
				indexBuffer[index++] = face[2];
			}

			m_faces.clear();
		}
		else
		{
			REQUIRE( m_faceCount * 3 == indexBuffer.getSize() );
		}
	}
}
