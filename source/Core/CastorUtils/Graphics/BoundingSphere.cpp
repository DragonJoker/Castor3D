#include "CastorUtils/Graphics/BoundingSphere.hpp"
#include "CastorUtils/Graphics/BoundingBox.hpp"

CU_ImplementSmartPtr( castor, BoundingSphere )

namespace castor
{
	BoundingSphere::BoundingSphere( Point3f const & center, float radius )
		: BoundingContainer3D{ center }
		, m_radius( radius )
	{
	}

	BoundingSphere::BoundingSphere( BoundingBox const & box )
		: BoundingContainer3D{ box.getCenter() }
		, m_radius( float( point::length( box.getMax() - getCenter() ) ) )
	{
	}

	void BoundingSphere::load( Point3f const & center, float radius )
	{
		setCenter( center );
		m_radius = radius;
	}

	void BoundingSphere::load( BoundingBox const & box )
	{
		setCenter( box.getCenter() );
		m_radius = float( point::distance( box.getMax(), getCenter() ) );
#if !defined( NDEBUG )
		auto radius = float( point::distance( getCenter(), box.getMin() ) );
		CU_Ensure( std::abs( m_radius - radius ) < 0.01f );
#endif
	}

	bool BoundingSphere::isWithin( Point3f const & point )const
	{
		return point::length( point - getCenter() ) < m_radius;
	}

	bool BoundingSphere::isOnLimits( Point3f const & point )const
	{
		return ( float( point::length( point - getCenter() ) ) - m_radius ) < 0.0001f;
	}
}
