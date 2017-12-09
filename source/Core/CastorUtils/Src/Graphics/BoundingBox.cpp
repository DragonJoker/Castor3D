#include "BoundingBox.hpp"

#include <Design/ArrayView.hpp>

namespace castor
{
	BoundingBox::BoundingBox()
		: BoundingContainer3D()
	{
	}

	BoundingBox::BoundingBox( Point3r const & min, Point3r const & max )
		: BoundingContainer3D( min + ( max - min ) / real( 2.0 ) )
		, m_dimensions( max - min )
	{
	}

	BoundingBox BoundingBox::getUnion( BoundingBox const & bb )const
	{
		auto min = getMin();
		auto max = getMax();
		auto bbmin = bb.getMin();
		auto bbmax = bb.getMax();

		for ( uint32_t i = 0u; i < 3u; ++i )
		{
			min[i] = std::min( min[i], bbmin[i] );
			max[i] = std::max( max[i], bbmax[i] );
		}

		return BoundingBox{ min, max };
	}

	bool BoundingBox::isWithin( Point3r const & point )const
	{
		auto min = getMin();
		auto max = getMax();
		return ( point[0] > min[0] && point[0] < max[0] )
			&& ( point[1] > min[1] && point[1] < max[1] )
			&& ( point[2] > min[2] && point[2] < max[2] );
	}

	bool BoundingBox::isOnLimits( Point3r const & point )const
	{
		auto min = getMin();
		auto max = getMax();
		return !isWithin( point )
			&& ( point[0] == min[0]
				|| point[0] == max[0]
				|| point[1] == min[1]
				|| point[1] == max[1]
				|| point[2] == min[2]
				|| point[2] == max[2] );
	}

	void BoundingBox::load( Point3r const & min, Point3r const & max )
	{
		m_center = min + ( max - min ) / real( 2.0 );
		m_dimensions = max - min;
	}

	BoundingBox BoundingBox::getAxisAligned( Matrix4x4r const & transformations )const
	{
		auto min = getMin();
		auto max = getMax();
		Point3r corners[8]
		{
			min,
			max,
			Point3r{ min[0], max[1], min[2] },
			Point3r{ max[0], max[1], min[2] },
			Point3r{ max[0], min[1], min[2] },
			Point3r{ min[0], max[1], max[2] },
			Point3r{ min[0], min[1], max[2] },
			Point3r{ max[0], min[1], max[2] }
		};

		// Express object box in axis aligned coordinates.
		for ( auto & corner : corners )
		{
			corner = transformations * corner;
		}

		// Retrieve axis aligned box boundaries.
		min = corners[0];
		max = corners[0];
		for ( auto & corner : makeArrayView( corners + 1, corners + 8 ) )
		{
			min[0] = std::min( corner[0], min[0] );
			min[1] = std::min( corner[1], min[1] );
			min[2] = std::min( corner[2], min[2] );

			max[0] = std::max( corner[0], max[0] );
			max[1] = std::max( corner[1], max[1] );
			max[2] = std::max( corner[2], max[2] );
		}

		return BoundingBox{ min, max };
	}

	Point3r BoundingBox::getPositiveVertex( Point3r const & normal )const
	{
		Point3r result{ getCenter() };

		if ( normal[0] > 0.0_r )
		{
			result[0] += m_dimensions[0];
		}

		if ( normal[1] > 0.0_r )
		{
			result[1] += m_dimensions[1];
		}

		if ( normal[2] > 0.0_r )
		{
			result[2] += m_dimensions[2];
		}

		return result;
	}

	Point3r BoundingBox::getNegativeVertex( Point3r const & normal )const
	{
		Point3r result{ getCenter() };

		if ( normal[0] < 0.0_r )
		{
			result[0] += m_dimensions[0];
		}

		if ( normal[1] < 0.0_r )
		{
			result[1] += m_dimensions[1];
		}

		if ( normal[2] < 0.0_r )
		{
			result[2] += m_dimensions[2];
		}

		return result;
	}
}
