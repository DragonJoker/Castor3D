#include "CastorUtils/Graphics/BoundingSphere.hpp"
#include "CastorUtils/Graphics/BoundingBox.hpp"

namespace castor
{
	BoundingSphere::BoundingSphere()
		: BoundingContainer3D()
		, m_radius( 0 )
	{
	}

	BoundingSphere::BoundingSphere( Point3r const & center, real radius )
		: BoundingContainer3D( center )
		, m_radius( radius )
	{
	}

	BoundingSphere::BoundingSphere( BoundingBox const & box )
		: BoundingContainer3D( box.getCenter() )
		, m_radius( real( point::length( box.getMax() - m_center ) ) )
	{
	}

	void BoundingSphere::load( Point3r const & center, real radius )
	{
		m_center = center;
		m_radius = radius;
	}

	void BoundingSphere::load( BoundingBox const & box )
	{
		m_center = box.getCenter();
		m_radius = real( point::distance( box.getMax(), m_center ) );
#if !defined( NDEBUG )
		auto radius = real( point::distance( m_center, box.getMin() ) );
		CU_Ensure( std::abs( m_radius - radius ) < 0.01 );
#endif
	}

	bool BoundingSphere::isWithin( Point3r const & point )const
	{
		return point::length( point - m_center ) < m_radius;
	}

	bool BoundingSphere::isOnLimits( Point3r const & point )const
	{
		return ( real( point::length( point - m_center ) ) - m_radius ) < 0.0001_r;
	}
}
