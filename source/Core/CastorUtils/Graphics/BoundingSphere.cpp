#include "CastorUtils/Graphics/BoundingSphere.hpp"
#include "CastorUtils/Graphics/BoundingBox.hpp"

CU_ImplementCUSmartPtr( castor, BoundingSphere )

namespace castor
{
	BoundingSphere::BoundingSphere()
		: BoundingContainer3D()
		, m_radius( 0 )
	{
	}

	BoundingSphere::BoundingSphere( Point3f const & center, float radius )
		: BoundingContainer3D( center )
		, m_radius( radius )
	{
	}

	BoundingSphere::BoundingSphere( BoundingBox const & box )
		: BoundingContainer3D( box.getCenter() )
		, m_radius( float( point::length( box.getMax() - m_center ) ) )
	{
	}

	void BoundingSphere::load( Point3f const & center, float radius )
	{
		m_center = center;
		m_radius = radius;
	}

	void BoundingSphere::load( BoundingBox const & box )
	{
		m_center = box.getCenter();
		m_radius = float( point::distance( box.getMax(), m_center ) );
#if !defined( NDEBUG )
		auto radius = float( point::distance( m_center, box.getMin() ) );
		CU_Ensure( std::abs( m_radius - radius ) < 0.01f );
#endif
	}

	bool BoundingSphere::isWithin( Point3f const & point )const
	{
		return point::length( point - m_center ) < m_radius;
	}

	bool BoundingSphere::isOnLimits( Point3f const & point )const
	{
		return ( float( point::length( point - m_center ) ) - m_radius ) < 0.0001f;
	}
}
