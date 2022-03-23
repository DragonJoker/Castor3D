#include "Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/StagingData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#include <ashespp/Buffer/StagingBuffer.hpp>

//*************************************************************************************************

namespace castor3d
{
	namespace smshcompline
	{
		struct LineDistance
		{
			uint32_t m_index[2];
			double m_distance;
		};

		CU_DeclareVector( LineDistance, LineDist );

		struct Compare
		{
			bool operator()( LineDistance const & lhs
				, LineDistance const & rhs )
			{
				return lhs.m_distance < rhs.m_distance;
			}
		};
	}

	castor::String const LinesMapping::Name = "lines_mapping";

	LinesMapping::LinesMapping( Submesh & submesh
		, VkBufferUsageFlags bufferUsageFlags )
		: IndexMapping{ submesh, Name, bufferUsageFlags }
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
		for ( auto & line : castor::makeArrayView( begin, end ) )
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
					&& getOwner()->getBufferOffsets().hasVertices()
					&& getOwner()->getBufferOffsets().hasIndices() )
				{
					auto offsets = getOwner()->getBufferOffsets();
					auto & indices = offsets.getIndexBuffer();
					auto & vertices = offsets.getVertexBuffer();
					auto indexSize = uint32_t( offsets.getIndexCount() );

					m_cameraPosition = cameraPosition;

					if ( auto * index = reinterpret_cast< uint32_t * >( indices.lock( offsets.getIndexOffset()
						, offsets.idxChunk.size
						, 0u ) ) )
					{
						smshcompline::LineDistArray arraySorted;
						arraySorted.reserve( indexSize / 2 );

						if ( auto * vertex = reinterpret_cast< InterleavedVertex const * >( vertices.lock( offsets.getVertexOffset()
							, offsets.vtxChunk.size
							, 0u ) ) )
						{
							for ( uint32_t * it = index + 0; it < index + indexSize; it += 2 )
							{
								double dDistance = 0.0;
								auto & vtx1 = vertex[it[0]].pos;
								dDistance += castor::point::lengthSquared( vtx1 - cameraPosition );
								auto & vtx2 = vertex[it[1]].pos;
								dDistance += castor::point::lengthSquared( vtx2 - cameraPosition );
								arraySorted.push_back( smshcompline::LineDistance{ { it[0], it[1] }, dDistance } );
							}

							std::sort( arraySorted.begin(), arraySorted.end(), smshcompline::Compare{} );

							for ( auto & line : arraySorted )
							{
								*index++ = line.m_index[0];
								*index++ = line.m_index[1];
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
			&& getOwner()->getBufferOffsets().getIndexCount() )
		{
			auto offsets = getOwner()->getBufferOffsets();
			std::copy( m_lines.begin()
				, m_lines.end()
				, offsets.getIndexData< Line >().begin() );
			offsets.markIndexDirty();
		}
	}
}
