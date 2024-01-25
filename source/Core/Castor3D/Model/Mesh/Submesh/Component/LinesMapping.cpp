#include "Castor3D/Model/Mesh/Submesh/Component/LinesMapping.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

CU_ImplementSmartPtr( castor3d, LinesMapping )

namespace castor3d
{
	//*********************************************************************************************

	namespace smshcompline
	{
		struct LineDistance
		{
			castor::Array< uint32_t, 2u > m_index;
			double m_distance;
		};

		CU_DeclareVector( LineDistance, LineDist );

		struct Compare
		{
			bool operator()( LineDistance const & lhs
				, LineDistance const & rhs )const
			{
				return lhs.m_distance < rhs.m_distance;
			}
		};
	}

	//*********************************************************************************************

	void LinesMapping::ComponentData::copy( SubmeshComponentDataRPtr data )const
	{
		auto result = static_cast< ComponentData * >( data );
		result->m_lines = m_lines;
		result->m_cameraPosition = m_cameraPosition;
	}

	Line LinesMapping::ComponentData::addLine( uint32_t a, uint32_t b )
	{
		Line result{ a, b };

		if ( auto size = m_submesh.getPointsCount();
			a < size && b < size )
		{
			m_lines.push_back( result );
		}
		else
		{
			throw std::range_error( "addLine - One or more index out of bound" );
		}

		return result;
	}

	void LinesMapping::ComponentData::addLineGroup( LineIndices const * const begin
		, LineIndices const * const end )
	{
		for ( auto & line : castor::makeArrayView( begin, end ) )
		{
			addLine( line.m_index[0], line.m_index[1] );
		}
	}

	void LinesMapping::ComponentData::clearLines()
	{
		m_lines.clear();
	}

	void LinesMapping::ComponentData::doCleanup( RenderDevice const & device )
	{
		m_lines.clear();
	}

	void LinesMapping::ComponentData::doUpload( UploadData & uploader )
	{
		auto count = uint32_t( m_lines.size() * 2 );
		auto & offsets = m_submesh.getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshData::eIndex );

		if ( count && buffer.hasData() )
		{
			uploader.pushUpload( m_lines.data()
				, m_lines.size() * sizeof( Line )
				, buffer.getBuffer()
				, buffer.getOffset()
				, VK_ACCESS_INDEX_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
		}
	}

	//*********************************************************************************************

	castor::String const LinesMapping::TypeName = C3D_MakeSubmeshIndexComponentName( "lines" );

	LinesMapping::LinesMapping( Submesh & submesh
		, VkBufferUsageFlags bufferUsageFlags )
		: IndexMapping{ submesh, TypeName
			, castor::make_unique< ComponentData >( submesh, bufferUsageFlags ) }
	{
	}

	void LinesMapping::computeNormals( bool reverted )
	{
	}

	void LinesMapping::computeTangents()
	{
	}

	SubmeshComponentUPtr LinesMapping::clone( Submesh & submesh )const
	{
		auto result = castor::makeUnique< LinesMapping >( submesh );
		result->getData().copy( &getData() );
		return castor::ptrRefCast< SubmeshComponent >( result );
	}

	uint32_t LinesMapping::getCount()const
	{
		return getDataT< ComponentData >()->getCount();
	}

	uint32_t LinesMapping::getComponentsCount()const
	{
		return 2u;
	}

	//*********************************************************************************************
}
