#include "Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Vertex.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

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
		CU_DeclareVector( FaceDistance, FaceDist );
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
		, castor::Point3f const & minUV
		, castor::Point3f const & maxUV )
	{
		addFace( a, b, c );
		addFace( a, c, d );
		getOwner()->getPoint( a ).tex = Point3f{ minUV[0], minUV[1], 0.0f };
		getOwner()->getPoint( b ).tex = Point3f{ maxUV[0], minUV[1], 0.0f };
		getOwner()->getPoint( c ).tex = Point3f{ maxUV[0], maxUV[1], 0.0f };
		getOwner()->getPoint( d ).tex = Point3f{ minUV[0], maxUV[1], 0.0f };
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
					&& getOwner()->getVertexBuffer().getCount()
					&& getOwner()->getIndexBuffer().getCount() )
				{
					auto & indices = getOwner()->getIndexBuffer();
					auto & vertices = getOwner()->getVertexBuffer();

					m_cameraPosition = cameraPosition;
					auto indexSize = uint32_t( indices.getCount() );

					if ( uint32_t * index = reinterpret_cast< uint32_t * >( indices.getBuffer().lock( 0
						, uint32_t( indexSize * sizeof( uint32_t ) )
						, 0u ) ) )
					{
						FaceDistArray arraySorted;
						arraySorted.reserve( indexSize / 3 );

						if ( InterleavedVertex * vertex = vertices.lock( 0
							, vertices.getCount()
							, 0u ) )
						{
							for ( uint32_t * it = index + 0; it < index + indexSize; it += 3 )
							{
								double dDistance = 0.0;
								auto & vtx1 = vertex[it[0]].pos;
								dDistance += point::lengthSquared( vtx1 - cameraPosition );
								auto & vtx2 = vertex[it[1]].pos;
								dDistance += point::lengthSquared( vtx2 - cameraPosition );
								auto & vtx3 = vertex[it[2]].pos;
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

							indices.getBuffer().flush( 0u, vertices.getCount() );
							indices.getBuffer().unlock();
						}

						indices.getBuffer().flush( 0u, uint32_t( indexSize * sizeof( uint32_t ) ) );
						indices.getBuffer().unlock();
					}
				}
			}
		}
		catch ( Exception const & exc )
		{
			log::error << "Submesh::SortFaces - Error: " << exc.what() << std::endl;
		}
	}

	void TriFaceMapping::doCleanup()
	{
		m_faces.clear();
	}

	void TriFaceMapping::doFill()
	{
	}

	void TriFaceMapping::doUpload()
	{
		auto count = uint32_t( m_faces.size() * 3 );

		if ( count )
		{
			auto & indexBuffer = getOwner()->getIndexBuffer();

			if ( auto * buffer = indexBuffer.lock( 0, count, 0u ) )
			{
				for ( auto const & face : m_faces )
				{
					*buffer = face[0];
					++buffer;
					*buffer = face[1];
					++buffer;
					*buffer = face[2];
					++buffer;
				}

				indexBuffer.flush( 0u, count );
				indexBuffer.unlock();
			}

			//m_faces.clear();
		}
	}
}
