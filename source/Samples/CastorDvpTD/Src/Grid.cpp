#include "Grid.hpp"

namespace castortd
{
	Grid::Grid()
	{
		uint32_t x = 0;
		uint32_t y = 0;

		for ( auto & l_cell : m_cells )
		{
			if ( x == m_width )
			{
				++y;
				x = 0;
			}

			l_cell.m_x = x;
			l_cell.m_y = y;
			++x;
		}
	}

	Grid::~Grid()
	{
	}
}
