#include "CastorUtils/Graphics/Grid.hpp"

namespace castor
{
	Grid::Grid( uint32_t gridSize
		, float cellSize
		, Point3f max
		, Point3f min
		, float scale
		, uint32_t level )
		: m_maxGridSize{ gridSize }
		, m_maxCellSize{ cellSize }
		, m_max{ max }
		, m_min{ min }
		, m_scale{ scale }
		, m_level{ level }
	{
		setUp();
	}

	Grid::Grid( Grid const & old
		, float scale
		, uint32_t level )
		: m_maxGridSize{ old.m_maxGridSize }
		, m_maxCellSize{ old.m_maxCellSize }
		, m_max{ old.m_max }
		, m_min{ old.m_min }
		, m_scale{ scale }
		, m_level{ level }
	{
		setUp();
	}

	Grid::~Grid()
	{
	}

	void Grid::transform( Point3f pos, Point3f dir )
	{
		Point4f newCenter{ pos->x, pos->y, pos->z, 1.0 };

		auto centerToNewCenter = Point3f( newCenter ) - m_center;
		auto snapX = int( centerToNewCenter->x / m_cellSize + 0.5 );
		auto snapY = int( centerToNewCenter->y / m_cellSize + 0.5 );
		auto snapZ = int( centerToNewCenter->z / m_cellSize + 0.5 );

		auto xOffset = int( m_center->x + snapX * m_cellSize );
		auto yOffset = int( m_center->y + snapY * m_cellSize );
		auto zOffset = int( m_center->z + snapZ * m_cellSize );
		auto offset = Point3f( xOffset, yOffset, zOffset );
		Point3f newMin{ offset + m_centerToMin };
		Point3f newMax{ offset + m_centerToMax };

		auto halfDisplacement = 0.8f * m_maxGridSize * 0.5f * m_cellSize;
		auto displacement = dir * Point3f{ halfDisplacement, halfDisplacement, halfDisplacement };
		auto snapXDisp = int( displacement->x / m_cellSize + 0.5 );
		auto snapYDisp = int( displacement->y / m_cellSize + 0.5 );
		auto snapZDisp = int( displacement->z / m_cellSize + 0.5 );

		Point3f offsetDisp{ snapXDisp * m_cellSize, snapYDisp * m_cellSize, snapZDisp * m_cellSize };
		Point3f displacedMin = newMin + offsetDisp;
		Point3f displacedMax = newMax + offsetDisp;

		m_min = displacedMin;
		m_max = displacedMax;
	}

	void Grid::setUp()
	{
		m_center = Point3f( 0 );
		m_cellSize = m_maxCellSize * m_scale;
		auto size = m_maxGridSize * 0.5f * m_cellSize;
		m_min = m_center - Point3f{ size, size, size };
		m_max = m_center + Point3f{ size, size, size };
		m_centerToMin = m_min - m_center;
		m_centerToMax = m_max - m_center;

		m_dimensions->x = m_maxGridSize;
		m_dimensions->y = m_maxGridSize;
		m_dimensions->z = m_maxGridSize;
	}
}