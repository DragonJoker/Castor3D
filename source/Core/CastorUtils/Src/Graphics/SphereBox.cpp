#include "SphereBox.hpp"
#include "CubeBox.hpp"

namespace castor
{
	SphereBox::SphereBox()
		: ContainerBox3D()
		, m_radius( 0 )
	{
	}

	SphereBox::SphereBox( SphereBox const & p_sphere )
		: ContainerBox3D( p_sphere )
		, m_radius( p_sphere.m_radius )
	{
	}

	SphereBox::SphereBox( SphereBox && p_sphere )
		: ContainerBox3D( std::move( p_sphere ) )
		, m_radius( std::move( p_sphere.m_radius ) )
	{
		p_sphere.m_radius = 0;
	}

	SphereBox::SphereBox( Point3r const & p_center, real p_radius )
		: ContainerBox3D( p_center )
		, m_radius( p_radius )
	{
	}

	SphereBox::SphereBox( CubeBox const & p_box )
		: ContainerBox3D( p_box.getCenter() )
		, m_radius( real( point::length( p_box.getMax() - m_ptCenter ) ) )
	{
	}

	SphereBox & SphereBox::operator =( SphereBox const & p_container )
	{
		ContainerBox3D::operator =( p_container );
		m_radius = p_container.m_radius;
		return *this;
	}

	SphereBox & SphereBox::operator =( SphereBox && p_container )
	{
		ContainerBox3D::operator =( std::move( p_container ) );

		if ( this != &p_container )
		{
			m_radius = std::move( p_container.m_radius );
		}

		return *this;
	}

	void SphereBox::load( Point3r const & p_center, real p_radius )
	{
		m_ptCenter = p_center;
		m_radius = p_radius;
	}

	void SphereBox::load( CubeBox const & p_box )
	{
		m_ptCenter = p_box.getCenter();
		m_radius = real( point::distance( p_box.getMax(), m_ptCenter ) );
#if !defined( NDEBUG )
		auto radius = real( point::distance( m_ptCenter, p_box.getMin() ) );
		ENSURE( std::abs( m_radius - radius ) < 0.01 );
#endif
	}

	bool SphereBox::isWithin( Point3r const & p_v )
	{
		return point::length( p_v - m_ptCenter ) < m_radius;
	}

	bool SphereBox::isOnLimits( Point3r const & p_v )
	{
		return real( point::length( p_v - m_ptCenter ) ) == m_radius;
	}
}
