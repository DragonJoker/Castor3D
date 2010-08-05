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
#ifndef ___CU_Colour___
#define ___CU_Colour___

#include "Point.h"

namespace Castor3D
{
	/*!
	RGBA colour representation
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	class Colour : public General::Math::Point4D<float>
	{
	public:
		static Colour FullAlphaWhite;
		static Colour FullAlphaBlack;
		static Colour FullAlphaRed;
		static Colour FullAlphaGreen;
		static Colour FullAlphaBlue;
		static Colour FullAlphaDarkRed;
		static Colour FullAlphaDarkGreen;
		static Colour FullAlphaDarkBlue;
		static Colour FullAlphaLightRed;
		static Colour FullAlphaLightGreen;
		static Colour FullAlphaLightBlue;

		static Colour HighAlphaWhite;
		static Colour HighAlphaBlack;
		static Colour HighAlphaRed;
		static Colour HighAlphaGreen;
		static Colour HighAlphaBlue;
		static Colour HighAlphaDarkRed;
		static Colour HighAlphaDarkGreen;
		static Colour HighAlphaDarkBlue;
		static Colour HighAlphaLightRed;
		static Colour HighAlphaLightGreen;
		static Colour HighAlphaLightBlue;

		static Colour MediumAlphaWhite;
		static Colour MediumAlphaBlack;
		static Colour MediumAlphaRed;
		static Colour MediumAlphaGreen;
		static Colour MediumAlphaBlue;
		static Colour MediumAlphaDarkRed;
		static Colour MediumAlphaDarkGreen;
		static Colour MediumAlphaDarkBlue;
		static Colour MediumAlphaLightRed;
		static Colour MediumAlphaLightGreen;
		static Colour MediumAlphaLightBlue;

		static Colour LowAlphaWhite;
		static Colour LowAlphaBlack;
		static Colour LowAlphaRed;
		static Colour LowAlphaGreen;
		static Colour LowAlphaBlue;
		static Colour LowAlphaDarkRed;
		static Colour LowAlphaDarkGreen;
		static Colour LowAlphaDarkBlue;
		static Colour LowAlphaLightRed;
		static Colour LowAlphaLightGreen;
		static Colour LowAlphaLightBlue;

		static Colour Transparent;

	public:
		Colour( float p_r=0, float p_g=0, float p_b=0, float p_a=0)
			:	Point4D<float>( p_r, p_g, p_b, p_a)
		{}
	};
}

#endif