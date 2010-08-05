/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_Plane___
#define ___C3D_Plane___

#include "Line.h"

namespace General
{	namespace Math
{
	/*!
	Plane representation : ax + by + cz + d = 0
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class Plane
	{
	public:
		Vector3f m_origin;
		Vector3f m_normal;	// a, b, c coefficients
		float m_d;			// d coefficient

	public:
		Plane( const Vector3f & p_p1, const Vector3f & p_p2, const Vector3f & p_p3)
		{
			Vector3f l_v( p_p1, p_p2);
			Vector3f l_w( p_p1, p_p3);
			m_normal = l_v ^ l_w;

			m_normal.Normalise();
			m_origin = Vector3f( p_p1);
			m_d = 0.0f - m_normal.dotProduct( m_origin);
		}

		bool IsParallel( const Plane & p_plane)const
		{
			float l_ratioA = m_normal.x / p_plane.m_normal.x;
			float l_ratioB = m_normal.y / p_plane.m_normal.y;
			float l_ratioC = m_normal.z / p_plane.m_normal.z;

			return (abs( l_ratioA - l_ratioB) < 0.000001f) && (abs( l_ratioA - l_ratioC) < 0.000001f);
		}

		bool operator ==( const Plane & p_plane)const
		{
			if (! IsParallel( p_plane))
			{
				return false;
			}

			float l_ratioA = m_normal.x / p_plane.m_normal.x;
			float l_ratioD = m_d / p_plane.m_d;
			return (abs( l_ratioA - l_ratioD) < 0.000001f);
		}

		bool operator !=( const Plane & p_plane)const
		{
			return ( ! (*this == p_plane));
		}

		bool Intersects( const Plane & p_plane, Line & p_line)const
		{
			if (IsParallel( p_plane))
			{
				return false;
			}

			Vector3f l_normal = m_normal ^ p_plane.m_normal;

			//TODO : trouver le point d'origine

			return true;
		}

		bool Intersects( const Plane & p_plane1, const Plane & p_plane2, Vector3f & p_intersection)const
		{
			if (IsParallel( p_plane1))
			{
				return false;
			}
			if (IsParallel( p_plane2))
			{
				return false;
			}
			if (p_plane1.IsParallel( p_plane2))
			{
				return false;
			}

			float a1=m_normal.x, a2=p_plane1.m_normal.x, a3=p_plane2.m_normal.x;
			float b1=m_normal.y, b2=p_plane1.m_normal.y, b3=p_plane2.m_normal.y;
			float c1=m_normal.z, c2=p_plane1.m_normal.z, c3=p_plane2.m_normal.z;
			float alpha, beta;

			alpha = (a3 - (a2 * (b3 - (a3 / a1)) / (b2 - (a2 / a1)))) / a1;
			beta = (b3 - (a3 / a1)) / (b2 - (a2 / a1));

			float l_c3 = (c1 * alpha) + (c2 * beta);

			if (abs(c3 - l_c3) < 0.000001)
			{
				return false;
			}

			alpha = ((a2 * c1) / (a1 * (b2 - (a2 * b1) / a1))) - (c2 / (b2 - (a2 * b1) / a1));
			beta = ((a2 * m_d) / (a1 * (b2 - (a2 * b1) / a1))) - (p_plane1.m_d / (b2 - (a2 * b1) / a1));

			float x, y, z;

			z = ((a3 * ((m_d + (beta * b1)) / a1)) - p_plane2.m_d) / ((b3 * alpha) + c3 - (a3 * ((alpha * b1) + c1) / a1));
			y = (alpha * z) + beta;
			x = (z * (0.0f - ((alpha * b1) + c1)) / a1) - ((m_d + (b1 * beta)) / a1);

			p_intersection.x = x;
			p_intersection.y = y;
			p_intersection.z = z;

			return true;
		}
	};
}
}

#endif