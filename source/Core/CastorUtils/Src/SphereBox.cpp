/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
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
