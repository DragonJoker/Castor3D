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
#ifndef ___C3D_Line___
#define ___C3D_Line___

namespace General
{	namespace Math
{
	/*!
	2d line equation
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	class Line2D
	{
	public:
		float a;
		float b;

	public:
		Line2D( float xA, float yA, float xB, float yB)
		{
			a = (yA - yB) / (xA - xB);
			b = yB - a * xB;
		}

		bool Intersects( const Line2D & p_line, float & x, float & y)
		{
			if (abs( a - p_line.a) < 0.00000001)
			{
				return false;
			}
			x = (p_line.b - b) / (a - p_line.a);
			y = a * x + b;
			return true;
		}
	};

	/*!
	3d line equation : a slope and an origin
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	class Line3D
	{
	public:
		Vector3f m_slope;
		Vector3f m_origin;

	public:
		Line3D( const Vector3f & p_A, const Vector3f & p_B)
		{
			m_origin = Vector3f( p_A);
			m_slope = Vector3f( p_A, p_B);
		}

		bool Intersects( const Line3D & p_line, Vector3f & p_point)
		{
			return true;
		}

		bool IsIn( const Vector3f & p_point)
		{
			return (abs( p_point.x - m_origin.x) / m_slope.x < 0.000001f) 
				&& (abs( p_point.y - m_origin.y) / m_slope.y < 0.000001f)
				&& (abs( p_point.z - m_origin.z) / m_slope.z < 0.000001f);
		}
	};
}
}

#endif