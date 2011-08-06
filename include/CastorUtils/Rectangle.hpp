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
#ifndef ___Rectangle___
#define ___Rectangle___

#include "Point.hpp"

namespace Castor
{	namespace Math
{
	//! Intersection types
	/*!
	Enumerates the different intersection types between to rectangles
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	typedef enum
	{	eINTERSECTION_IN		//!< Completely inside
	,	eINTERSECTION_OUT		//!< Completely outside
	,	eINTERSECTION_INTERSECT	//!< Intersection
	,	eINTERSECTION_COUNT
	}	eINTERSECTION;
	//! Rectangle class
	/*!
	Rectangle class, inherits from Point, holds the intersection functions and specific accessors
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class Rectangle : public Point4i
	{
	public :
		Rectangle( Point2i const & p_ptOrigin = Point2i( 0, 0), Point2i const & p_ptSize = Point2i( 0, 0));
		Rectangle( int p_iLeft, int p_iTop, int p_iWidth, int p_iHeight);

		void Set( int p_iLeft, int p_iTop, int p_iWidth, int p_iHeight);
		eINTERSECTION Intersects( Point2i const & p_ptPoint)const;
		eINTERSECTION Intersects( Rectangle const & p_rcRect)const;
		void Size( Point2i & p_ptResult)const;

		inline int 		Left	()const { return at( 0); }
		inline int 		Right	()const { return at( 2); }
		inline int 		Top		()const { return at( 1); }
		inline int 		Bottom	()const { return at( 3); }
		inline int 		Width	()const { return at( 2) - at( 0); }
		inline int 		Height	()const { return at( 3) - at( 1); }
	};

//	bool operator ==( Rectangle const & rcA, Rectangle const & rcB);
//	bool operator !=( Rectangle const & rcA, Rectangle const & rcB);
}
}

#endif
