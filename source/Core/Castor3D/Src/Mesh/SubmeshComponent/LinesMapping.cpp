#include "LinesMapping.hpp"

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
		struct LineDistance
		{
			uint32_t m_index[2];
			double m_distance;
		};
		bool operator<( LineDistance const & lhs, LineDistance const & rhs )
		{
			return lhs.m_distance < rhs.m_distance;
		}
		DECLARE_VECTOR( LineDistance, LineDist );
	}

	String const LinesMapping::Name = "lines_mapping";

	LinesMapping::LinesMapping( Submesh & submesh )
		: IndexMapping{ submesh, Name }
	{
	}

	LinesMapping::~LinesMapping()
	{
		cleanup();
	}

	Line LinesMapping::addLine( uint32_t a, uint32_t b )
	{
		Line result{ a, b };
		auto size = getOwner()->getPointsCount();

		if ( a < size && b < size )
		{
			m_lines.push_back( result );
		}
		else
		{
			throw std::range_error( "addLine - One or more index out of bound" );
		}

		return result;
	}

	void LinesMapping::addLineGroup( LineIndices const * const begin
		, LineIndices const * const end )
	{
		for ( auto & line : makeArrayView( begin, end ) )
		{
			addLine( line.m_index[0], line.m_index[1] );
		}
	}

	void LinesMapping::clearLines()
	{
		m_lines.clear();
	}

	void LinesMapping::computeNormals( bool reverted )
	{
	}

	uint32_t LinesMapping::getCount()const
	{
		return uint32_t( m_lines.size() );
	}

	uint32_t TriFaceMapping::getComponentsCount()const
	{
		return 2u;
	}

	void LinesMapping::sortByDistance( Point3r const & cameraPosition )
	{
		REQUIRE( getOwner()->isInitialised() );

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
					uint32_t indexSize = indices.getCount();

					if ( uint32_t * index = reinterpret_cast< uint32_t * >( indices.getBuffer().lock( 0
						, uint32_t( indexSize * sizeof( uint32_t ) )
						, renderer::MemoryMapFlag::eRead | renderer::MemoryMapFlag::eWrite ) ) )
					{
						LineDistArray arraySorted;
						arraySorted.reserve( indexSize / 2 );

						if ( InterleavedVertex * vertex = vertices.lock( 0
							, vertices.getCount()
							, renderer::MemoryMapFlag::eRead ) )
						{
							for ( uint32_t * it = index + 0; it < index + indexSize; it += 2 )
							{
								double dDistance = 0.0;
								auto & vtx1 = vertex[it[0]].pos;
								dDistance += point::lengthSquared( vtx1 - cameraPosition );
								auto & vtx2 = vertex[it[1]].pos;
								dDistance += point::lengthSquared( vtx2 - cameraPosition );
								arraySorted.push_back( LineDistance{ { it[0], it[1] }, dDistance } );
							}

							std::sort( arraySorted.begin(), arraySorted.end() );

							for ( auto & line : arraySorted )
							{
								*index++ = line.m_index[0];
								*index++ = line.m_index[1];
								*index++ = line.m_index[2];
							}

							vertices.flush( 0u, vertices.getCount() );
							vertices.unlock();
						}

						indices.getBuffer().flush( 0u, uint32_t( indexSize * sizeof( uint32_t ) ) );
						indices.getBuffer().unlock();
					}
				}
			}
		}
		catch ( Exception const & exc )
		{
			Logger::logError( std::stringstream() << "Submesh::SortLines - Error: " << exc.what() );
		}
	}

	void LinesMapping::doCleanup()
	{
		m_lines.clear();
	}

	void LinesMapping::doFill()
	{
	}

	void LinesMapping::doUpload()
	{
		auto count = uint32_t( m_lines.size() * 2 );

		if ( count )
		{
			auto & indexBuffer = getOwner()->getIndexBuffer();

			if ( uint32_t * buffer = reinterpret_cast< uint32_t * >( indexBuffer.getBuffer().lock( 0
				, uint32_t( count * sizeof( uint32_t ) )
				, renderer::MemoryMapFlag::eRead | renderer::MemoryMapFlag::eWrite ) ) )
			{
				for ( auto const & line : m_lines )
				{
					*buffer = line[0];
					++buffer;
					*buffer = line[1];
					++buffer;
				}

				indexBuffer.getBuffer().flush( 0u, uint32_t( count * sizeof( uint32_t ) ) );
				indexBuffer.getBuffer().unlock();
			}

			m_lines.clear();
		}
	}
}
