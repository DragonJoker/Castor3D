﻿/*
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

#include "CubeBox.hpp"

namespace Castor
{
	CubeBox::CubeBox()
		: ContainerBox3D()
		, m_min()
		, m_max()
	{
	}

	CubeBox::CubeBox( CubeBox const & p_cube )
		: ContainerBox3D( p_cube )
		, m_min( p_cube.m_min )
		, m_max( p_cube.m_max )
	{
	}

	CubeBox::CubeBox( CubeBox && p_cube )
		: ContainerBox3D( std::move( p_cube ) )
		, m_min( std::move( p_cube.m_min ) )
		, m_max( std::move( p_cube.m_max ) )
	{
		p_cube.m_min = Point3r( 0, 0, 0 );
		p_cube.m_max = Point3r( 0, 0, 0 );
	}

	CubeBox::CubeBox( Point3r const & p_min, Point3r const & p_max )
		: ContainerBox3D( p_min + ( p_max - p_min ) / real( 2.0 ) )
		, m_min( p_min )
		, m_max( p_max )
	{
	}

	CubeBox & CubeBox::operator =( CubeBox const & p_container )
	{
		ContainerBox3D::operator =( p_container );
		m_min = p_container.m_min;
		m_max = p_container.m_max;
		return *this;
	}

	CubeBox & CubeBox::operator =( CubeBox && p_container )
	{
		ContainerBox3D::operator =( std::move( p_container ) );

		if ( this != &p_container )
		{
			m_min = std::move( p_container.m_min );
			m_max = std::move( p_container.m_max );
		}

		return *this;
	}

	bool CubeBox::IsWithin( Point3r const & p_v )
	{
		return ( p_v[0] > m_min[0] && p_v[0] < m_max[0] )
			   && ( p_v[1] > m_min[1] && p_v[1] < m_max[1] )
			   && ( p_v[2] > m_min[2] && p_v[2] < m_max[2] );
	}

	bool CubeBox::IsOnLimits( Point3r const & p_v )
	{
		return ! IsWithin( p_v )
			   && ( policy::equals( p_v[0], m_min[0] )
					|| policy::equals( p_v[0], m_max[0] )
					|| policy::equals( p_v[1], m_min[1] )
					|| policy::equals( p_v[1], m_max[1] )
					|| policy::equals( p_v[2], m_min[2] )
					|| policy::equals( p_v[2], m_max[2] ) );
	}

	void CubeBox::Load( Point3r const & p_ptMin, Point3r const & p_ptMax )
	{
		m_min = p_ptMin;
		m_max = p_ptMax;
		m_ptCenter = ( m_min + m_max ) / real( 2.0 );
	}
}
