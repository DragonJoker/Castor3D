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
#ifndef ___CU_Colour___
#define ___CU_Colour___

#include "Point.h"

#undef RGB

namespace Castor
{	namespace Math
{
	//! RGBA colour representation
	/*!
	Predefines numerous colours
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	class Colour : public Point4f, public Utils::Textable
	{
	public:
		static const Colour FullAlphaWhite;
		static const Colour FullAlphaBlack;
		static const Colour FullAlphaRed;
		static const Colour FullAlphaGreen;
		static const Colour FullAlphaBlue;
		static const Colour FullAlphaDarkRed;
		static const Colour FullAlphaDarkGreen;
		static const Colour FullAlphaDarkBlue;
		static const Colour FullAlphaLightRed;
		static const Colour FullAlphaLightGreen;
		static const Colour FullAlphaLightBlue;

		static const Colour HighAlphaWhite;
		static const Colour HighAlphaBlack;
		static const Colour HighAlphaRed;
		static const Colour HighAlphaGreen;
		static const Colour HighAlphaBlue;
		static const Colour HighAlphaDarkRed;
		static const Colour HighAlphaDarkGreen;
		static const Colour HighAlphaDarkBlue;
		static const Colour HighAlphaLightRed;
		static const Colour HighAlphaLightGreen;
		static const Colour HighAlphaLightBlue;

		static const Colour MediumAlphaWhite;
		static const Colour MediumAlphaBlack;
		static const Colour MediumAlphaRed;
		static const Colour MediumAlphaGreen;
		static const Colour MediumAlphaBlue;
		static const Colour MediumAlphaDarkRed;
		static const Colour MediumAlphaDarkGreen;
		static const Colour MediumAlphaDarkBlue;
		static const Colour MediumAlphaLightRed;
		static const Colour MediumAlphaLightGreen;
		static const Colour MediumAlphaLightBlue;

		static const Colour LowAlphaWhite;
		static const Colour LowAlphaBlack;
		static const Colour LowAlphaRed;
		static const Colour LowAlphaGreen;
		static const Colour LowAlphaBlue;
		static const Colour LowAlphaDarkRed;
		static const Colour LowAlphaDarkGreen;
		static const Colour LowAlphaDarkBlue;
		static const Colour LowAlphaLightRed;
		static const Colour LowAlphaLightGreen;
		static const Colour LowAlphaLightBlue;

		static const Colour Transparent;

	public:
		Colour( float p_r=0, float p_g=0, float p_b=0, float p_a=0);
		Colour( unsigned char p_r, unsigned char p_g, unsigned char p_b, unsigned char p_a);

		/**@name Conversions between float and unsigned char */
		//@{
		//@}

		/**@name Inherited methods from Textable */
		//@{
		virtual bool Write( Utils::File & p_file)const;
		virtual bool Read( Utils::File & p_file);
		//@}

		/**@name Accessors */
		//@{
		float & 		Red		()		{ return at( 0); }
		float & 		Green	()		{ return at( 1); }
		float & 		Blue	()		{ return at( 2); }
		float & 		Alpha	()		{ return at( 3); }
		const float & 	Red		()const	{ return at( 0); }
		const float & 	Green	()const	{ return at( 1); }
		const float & 	Blue	()const	{ return at( 2); }
		const float & 	Alpha	()const	{ return at( 3); }
		//@}

		/**@name Convertors */
		//@{
		void 			CharRGB		( Point<unsigned char, 3> & p_ptResult)const;
		void 			CharBGR		( Point<unsigned char, 3> & p_ptResult)const;
		void 			CharRGBA	( Point<unsigned char, 4> & p_ptResult)const;
		void 			CharBGRA	( Point<unsigned char, 4> & p_ptResult)const;
		void 			CharARGB	( Point<unsigned char, 4> & p_ptResult)const;
		void 			CharABGR	( Point<unsigned char, 4> & p_ptResult)const;
		Point3f			RGB			()const { return Point3f( const_ptr()); }
		Point4f			RGBA		()const { return Point4f( const_ptr()); }
		Point4f			ARGB		()const { return Point4f( at( 3), at( 0), at( 1), at( 2)); }
		Point4f			ABGR		()const { return Point4f( at( 3), at( 2), at( 1), at( 0)); }
		Point4f			BGRA		()const { return Point4f( at( 2), at( 1), at( 0), at( 3)); }
		unsigned long	ToLong		()const;
		//@}
	};
}
}

#endif