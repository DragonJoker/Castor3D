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
#ifndef ___Rectangle___
#define ___Rectangle___

#include "Point.h"

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
	{
		eIntersectionIn,        //< Completely inside
		eIntersectionOut,       //< Completely outside
		eIntersectionIntersect, //< Intersection
	}
	eINTERSECTION;
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
		Rectangle(const Point2i & p_ptOrigin = Point2i( 0, 0), const Point2i & p_ptSize = Point2i( 0, 0));
		Rectangle(int p_iLeft, int p_iTop, int p_iWidth, int p_iHeight);

		void Set( int p_iLeft, int p_iTop, int p_iWidth, int p_iHeight);
		eINTERSECTION Intersects( const Point2i & p_ptPoint)const;
		eINTERSECTION Intersects( const Rectangle & p_rcRect)const;

		inline int 		Left	()const { return at( 0); }
		inline int 		Right	()const { return at( 2); }
		inline int 		Top		()const { return at( 1); }
		inline int 		Bottom	()const { return at( 3); }
		inline int 		Width	()const { return at( 2) - at( 0); }
		inline int 		Height	()const { return at( 3) - at( 1); }
		inline Point2i	Size	()const { return at( 0); }

		bool operator ==( const Rectangle & rcRect) const;
		bool operator !=( const Rectangle & rcRect) const;
	};

}
}

#endif
