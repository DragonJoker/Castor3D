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

	void LinesMapping::doCleanup( RenderDevice const & device )
	{
		m_lines.clear();
	}

	void LinesMapping::doUpload()
	{
		auto count = uint32_t( m_lines.size() * 2 );
		auto & offsets = getOwner()->getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshFlag::eIndex );

		if ( count && buffer.hasData() )
		{
			std::copy( m_lines.begin()
				, m_lines.end()
				, buffer.getData< Line >().begin() );
			buffer.markDirty( VK_ACCESS_INDEX_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
		}
	}
}
