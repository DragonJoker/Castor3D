#include "CastorUtilsPch.hpp"

#include "SphereBox.hpp"
#include "CubeBox.hpp"

namespace Castor
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
		: ContainerBox3D( p_box.GetCenter() )
		, m_radius( real( point::distance( p_box.GetMax() - m_ptCenter ) ) )
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

	void SphereBox::Load( Point3r const & p_center, real p_radius )
	{
		m_ptCenter = p_center;
		m_radius = p_radius;
	}

	void SphereBox::Load( CubeBox const & p_box )
	{
		m_ptCenter = p_box.GetCenter();
		m_radius = real( point::distance( p_box.GetMax() - m_ptCenter ) );
	}

	bool SphereBox::IsWithin( Point3r const & p_v )
	{
		return point::distance( p_v - m_ptCenter ) < m_radius;
	}

	bool SphereBox::IsOnLimits( Point3r const & p_v )
	{
		return policy::equals( real( point::distance( p_v - m_ptCenter ) ), m_radius );
	}
}
