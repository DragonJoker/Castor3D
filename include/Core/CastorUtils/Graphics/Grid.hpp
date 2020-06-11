/*
See LICENSE file in root folder
*/
#ifndef ___Cu_Grid_HPP___
#define ___Cu_Grid_HPP___

#include "CastorUtils/Math/SquareMatrix.hpp"
#include "CastorUtils/Math/Point.hpp"

namespace castor
{
	class Grid
	{
	public:
		inline Grid()
		{
		}

		CU_API Grid( uint32_t gridSize
			, float cellSize
			, Point3f max
			, Point3f min
			, float scale
			, uint32_t level );
		CU_API Grid( Grid const & old
			, float scale
			, uint32_t level );
		CU_API ~Grid();

		CU_API void transform( Point3f pos, Point3f dir );

		inline Point3f const & getMin()const
		{
			return m_min;
		}

		inline Point3f const & getMax()const
		{
			return m_max;
		}

		inline Point3ui const & getDimensions()const
		{
			return m_dimensions;
		}

		inline float getScale()const
		{
			return m_scale;
		}

		inline float getCellSize()const
		{
			return m_cellSize;
		}

		inline Matrix4x4f const & getModelMatrix()const
		{
			return m_matrix;
		}

		inline Point3f getCenter()const
		{
			return m_center;
		}

	private:
		void setUp();

	private:
		uint32_t m_maxGridSize{};
		float m_maxCellSize{};
		Point3f m_min;
		Point3f m_max;
		Point3f m_center;
		Point3f m_offset;
		Point3f m_centerToMin;
		Point3f m_centerToMax;
		Point3f m_origMin;
		Point3f m_origMax;
		Point3ui m_dimensions;
		float m_scale{};
		float m_cellSize{};
		uint32_t m_level{};
		Matrix4x4f m_matrix;
	};
}

#endif
