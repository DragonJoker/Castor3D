#include "BoundingBox.hpp"

namespace castor
{
	BoundingBox::BoundingBox()
		: BoundingContainer3D()
		, m_min()
		, m_max()
	{
	}

	BoundingBox::BoundingBox( Point3r const & min, Point3r const & max )
		: BoundingContainer3D( min + ( max - min ) / real( 2.0 ) )
		, m_min( min )
		, m_max( max )
	{
	}

	bool BoundingBox::isWithin( Point3r const & point )const
	{
		return ( point[0] > m_min[0] && point[0] < m_max[0] )
			&& ( point[1] > m_min[1] && point[1] < m_max[1] )
			&& ( point[2] > m_min[2] && point[2] < m_max[2] );
	}

	bool BoundingBox::isOnLimits( Point3r const & point )const
	{
		return !isWithin( point )
			&& ( point[0] == m_min[0]
				|| point[0] == m_max[0]
				|| point[1] == m_min[1]
				|| point[1] == m_max[1]
				|| point[2] == m_min[2]
				|| point[2] == m_max[2] );
	}

	void BoundingBox::load( Point3r const & min, Point3r const & max )
	{
		m_min = min;
		m_max = max;
		m_center = m_min + ( m_max - m_min ) / real( 2.0 );
	}

	BoundingBox BoundingBox::getAxisAligned( Matrix4x4r const & transformations )const
	{
		Point3r corners[]
		{
			m_min,
			m_max,
			Point3r{ m_min[0], m_max[1], m_min[2] },
			Point3r{ m_max[0], m_max[1], m_min[2] },
			Point3r{ m_max[0], m_min[1], m_min[2] },
			Point3r{ m_min[0], m_max[1], m_max[2] },
			Point3r{ m_min[0], m_min[1], m_max[2] },
			Point3r{ m_max[0], m_min[1], m_max[2] }
		};

		// Express object box in axis aligned coordinates.
		for ( auto & corner : corners )
		{
			corner = transformations * corner;
		}

		// Retrieve axis aligned box boundaries.
		Point3r min( corners[0] );
		Point3r max( corners[1] );

		for ( auto & corner : corners )
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
}
