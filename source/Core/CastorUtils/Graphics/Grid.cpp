#include "CastorUtils/Graphics/Grid.hpp"

namespace castor
{
	Grid::Grid( uint32_t gridSize
		, float cellSize
		, Point3f max
		, Point3f min
		, float scale )
		: m_maxGridSize{ gridSize }
		, m_maxCellSize{ cellSize }
		, m_min{ min }
		, m_max{ max }
		, m_scale{ scale }
	{
		setUp();
	}

	Grid::Grid( Grid const & old
		, float scale )
		: m_maxGridSize{ old.m_maxGridSize }
		, m_maxCellSize{ old.m_maxCellSize }
		, m_min{ old.m_min }
		, m_max{ old.m_max }
		, m_scale{ scale }
	{
		setUp();
	}

	void Grid::transform( Point3f pos, Point3f dir )
	{
		Point4f newCenter{ pos->x, pos->y, pos->z, 1.0 };

		auto centerToNewCenter = Point3f( newCenter ) - m_center;
		auto snapX = int( centerToNewCenter->x / m_cellSize + 0.5 );
		auto snapY = int( centerToNewCenter->y / m_cellSize + 0.5 );
		auto snapZ = int( centerToNewCenter->z / m_cellSize + 0.5 );

		auto xOffset = int( m_center->x + float( snapX ) * m_cellSize );
		auto yOffset = int( m_center->y + float( snapY ) * m_cellSize );
		auto zOffset = int( m_center->z + float( snapZ ) * m_cellSize );
		auto offset = Point3f( xOffset, yOffset, zOffset );
		Point3f newMin{ offset + m_centerToMin };
		Point3f newMax{ offset + m_centerToMax };

		auto halfDisplacement = 0.8f * float( m_maxGridSize ) * 0.5f * m_cellSize;
		auto displacement = dir * Point3f{ halfDisplacement, halfDisplacement, halfDisplacement };
		auto snapXDisp = int( displacement->x / m_cellSize + 0.5 );
		auto snapYDisp = int( displacement->y / m_cellSize + 0.5 );
		auto snapZDisp = int( displacement->z / m_cellSize + 0.5 );

		Point3f offsetDisp{ float( snapXDisp ) * m_cellSize
			, float( snapYDisp ) * m_cellSize
			, float( snapZDisp ) * m_cellSize };
		Point3f displacedMin = newMin + offsetDisp;
		Point3f displacedMax = newMax + offsetDisp;

		m_min = displacedMin;
		m_max = displacedMax;
	}

	void Grid::setUp()
	{
		m_center = Point3f( 0 );
		m_cellSize = m_maxCellSize * m_scale;
		auto size = float( m_maxGridSize ) * 0.5f * m_cellSize;
		m_min = m_center - Point3f{ size, size, size };
		m_max = m_center + Point3f{ size, size, size };
		m_centerToMin = m_min - m_center;
		m_centerToMax = m_max - m_center;

		m_dimensions->x = m_maxGridSize;
		m_dimensions->y = m_maxGridSize;
		m_dimensions->z = m_maxGridSize;
	}
}
