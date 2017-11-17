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
		return m_lineCount;
	}

	void LinesMapping::sortByDistance( Point3r const & cameraPosition )
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
						LineDistArray arraySorted;
						arraySorted.reserve( indexSize / 2 );

						if ( vertex )
						{
							for ( uint32_t * it = index + 0; it < index + indexSize; it += 2 )
							{
								double dDistance = 0.0;
								Coords3r vtx1( reinterpret_cast< real * >( &vertex[it[0] * stride] ) );
								dDistance += point::lengthSquared( vtx1 - cameraPosition );
								Coords3r vtx2( reinterpret_cast< real * >( &vertex[it[1] * stride] ) );
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
			Logger::logError( std::stringstream() << "Submesh::SortLines - Error: " << p_exc.what() );
		}
	}

	void LinesMapping::doCleanup()
	{
		m_lines.clear();
	}

	void LinesMapping::doFill()
	{
		auto count = uint32_t( m_lines.size() * 2 );
		IndexBuffer & indexBuffer = getOwner()->getIndexBuffer();

		if ( count )
		{
			m_lineCount = uint32_t( m_lines.size() );

			if ( indexBuffer.getSize() != count )
			{
				indexBuffer.resize( count );
			}

			uint32_t index = 0;

			for ( auto const & line : m_lines )
			{
				indexBuffer[index++] = line[0];
				indexBuffer[index++] = line[1];
			}

			m_lines.clear();
		}
		else
		{
			REQUIRE( m_lineCount * 2 == indexBuffer.getSize() );
		}
	}
}
