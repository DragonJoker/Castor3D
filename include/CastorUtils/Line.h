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
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Castor_Line___
#define ___Castor_Line___

#include "Module_Utils.h"

namespace Castor
{	namespace Math
{
	//! 2D line equation
	/*!
	You know y = ax + b ?
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T>
	class Line2D : public MemoryTraced< Line2D<T> >
	{
	private:
		typedef Templates::Policy<T> policy;

	public:
		T a;
		T b;

	public:
		Line2D( T xA, T yA, T xB, T yB)
		{
			policy::assign( a, (yA - yB) / (xA - xB));
			policy::assign( b, yB - a * xB);
		}

		bool Intersects( const Line2D & p_line, T & x, T & y)
		{
			bool l_bReturn = false;

			if (policy::equals( a, p_line.a))
			{
				x = (p_line.b - b) / (a - p_line.a);
				y = a * x + b;
				l_bReturn = true;
			}

			return l_bReturn;
		}
	};

	//! 3D line equation
	/*!
	A slope and an origin
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	template <typename T>
	class Line3D : public MemoryTraced< Line3D<T> >
	{
	private:
		typedef Templates::Policy<T> value;

	public:
		Point<T, 3> m_slope;
		Point<T, 3> m_origin;

	public:
		Line3D( const Point<T, 3> & p_A, const Point<T, 3> & p_B)
		{
			m_origin = Point<T, 3>( p_A);
			m_slope = Point<T, 3>( p_B - p_A);
		}

		bool Intersects( const Line3D & p_line, Point<T, 3> & p_point)
		{
			return true;
		}

		bool IsIn( const Point<T, 3> & p_point)
		{
			return policy::is_null( (p_point[0] - m_origin[0]) / m_slope[0])
				&& policy::is_null( (p_point[1] - m_origin[1]) / m_slope[1])
				&& policy::is_null( (p_point[2] - m_origin[2]) / m_slope[2]);
		}
	};
}
}

#endif