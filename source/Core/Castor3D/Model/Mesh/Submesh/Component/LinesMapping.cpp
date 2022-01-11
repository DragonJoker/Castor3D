#include "Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

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
		CU_DeclareVector( LineDistance, LineDist );
	}

	String const LinesMapping::Name = "lines_mapping";

	LinesMapping::LinesMapping( Submesh & submesh
		, VkMemoryPropertyFlags bufferMemoryFlags
		, VkBufferUsageFlags bufferUsageFlags )
		: IndexMapping{ submesh, Name, bufferMemoryFlags, bufferUsageFlags }
	{
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

	SubmeshComponentSPtr LinesMapping::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< LinesMapping >( submesh );
		result->m_lines = m_lines;
		result->m_cameraPosition = m_cameraPosition;
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	uint32_t LinesMapping::getCount()const
	{
		return uint32_t( m_lines.size() );
	}

	uint32_t LinesMapping::getComponentsCount()const
	{
		return 2u;
	}

	void LinesMapping::sortByDistance( castor::Point3f const & cameraPosition )
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

					if ( uint32_t * index = reinterpret_cast< uint32_t * >( indices.lock() ) )
					{
						LineDistArray arraySorted;
						arraySorted.reserve( indexSize / 2 );

						if ( InterleavedVertex * vertex = vertices.lock() )
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
							}

							vertices.flush();
							vertices.unlock();
						}

						indices.flush();
						indices.unlock();
					}
				}
			}
		}
		catch ( Exception const & exc )
		{
			log::error << "Submesh::SortLines - Error: " << exc.what() << std::endl;
		}
	}

	void LinesMapping::doCleanup( RenderDevice const & device )
	{
		m_lines.clear();
	}

	void LinesMapping::doUpload()
	{
		auto count = uint32_t( m_lines.size() * 2 );

		if ( count
			&& getOwner()->hasIndexBuffer()
			&& !castor::checkFlag( getMemoryFlags(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) )
		{
			auto & indexBuffer = getOwner()->getIndexBuffer();

			if ( uint32_t * buffer = indexBuffer.lock() )
			{
				for ( auto const & line : m_lines )
				{
					*buffer = line[0];
					++buffer;
					*buffer = line[1];
					++buffer;
				}

				indexBuffer.flush();
				indexBuffer.unlock();
			}

			m_lines.clear();
		}
	}
}
