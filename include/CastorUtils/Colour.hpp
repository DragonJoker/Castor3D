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
#ifndef ___CU_Colour___
#define ___CU_Colour___

#include "Point.hpp"
#include "Loader.hpp"

#undef RGB

namespace Castor
{	namespace Math
{
	class Colour;
}

namespace Resources
{
	//! Colour loader
	/*!
	Loads and saves colours from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class Loader<Math::Colour>
	{
	public:
		/**
		 * Loads a colour from a text file
		 *@param p_file : [in/out] the file to load the colour in
		 *@param p_colour : [in/out] the colour to load
		 */
		static bool Load( Math::Colour & p_colour, Utils::File & p_file);
		/**
		 * Saves a colour into a text file
		 *@param p_file : [in/out] the file to save the colour in
		 *@param p_colour : [in] the colour to save
		 */
		static bool Save( Math::Colour const & p_colour, Utils::File & p_file);
		/**
		 * Reads a colour from a text file
		 *@param p_file : [in/out] the file to read the colour in
		 *@param p_colour : [in/out] the colour to read
		 */
		static bool Read( Math::Colour & p_colour, Utils::File & p_file);
		/**
		 * Writes a colour into a text file
		 *@param p_file : [in/out] the file to save the colour in
		 *@param p_colour : [in] the colour to save
		 */
		static bool Write( Math::Colour const & p_colour, Utils::File & p_file);
	};
}

namespace Math
{
	//! RGBA colour representation
	/*!
	Predefines numerous colours
	Allows conversion to different colour formats (RGB, ARGB, BGR, ...) and different data types (long, float, char, ...)
	\author Sylvain DOREMUS
	\date 14/08/2010
	*/
	class Colour : public Point4f
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

	private:
		DECLARE_INVARIANT_BLOCK()

	public:
		Colour();
		Colour( Colour const & p_colour);
		Colour & operator =( Colour const & p_colour);

		template <typename T>
		static Colour	FromComponents( T p_r, T p_g, T p_b, T p_a)
		{
			Colour l_clrReturn;
			l_clrReturn[0] = float( p_r);
			l_clrReturn[1] = float( p_r);
			l_clrReturn[2] = float( p_r);
			l_clrReturn[3] = float( p_r);
			return l_clrReturn;
		}
		static Colour	FromRGB		( Point3ub const & p_ptColour);
		static Colour	FromBGR		( Point3ub const & p_ptColour);
		static Colour	FromRGBA	( Point4ub const & p_ptColour);
		static Colour	FromBGRA	( Point4ub const & p_ptColour);
		static Colour	FromARGB	( Point4ub const & p_ptColour);
		static Colour	FromABGR	( Point4ub const & p_ptColour);
		static Colour	FromRGB		( Point3f const & p_ptColour);
		static Colour	FromBGR		( Point3f const & p_ptColour);
		static Colour	FromRGBA	( Point4f const & p_ptColour);
		static Colour	FromARGB	( Point4f const & p_ptColour);
		static Colour	FromABGR	( Point4f const & p_ptColour);
		static Colour	FromBGRA	( Point4f const & p_ptColour);
		static Colour 	FromRGB		( unsigned long p_colour);
		static Colour 	FromBGR		( unsigned long p_colour);
		static Colour 	FromARGB	( unsigned long p_colour);
		static Colour 	FromRGBA	( unsigned long p_colour);
		static Colour 	FromABGR	( unsigned long p_colour);
		static Colour 	FromBGRA	( unsigned long p_colour);

		/**@name Accessors */
		//@{
		float & 		Red		()		{ return at( 0); }
		float & 		Green	()		{ return at( 1); }
		float & 		Blue	()		{ return at( 2); }
		float & 		Alpha	()		{ return at( 3); }
		float const & 	Red		()const	{ return at( 0); }
		float const & 	Green	()const	{ return at( 1); }
		float const & 	Blue	()const	{ return at( 2); }
		float const & 	Alpha	()const	{ return at( 3); }
		//@}

		/**@name Convertors */
		//@{
		void 			RGB		( Point3ub & p_ptResult)const;
		void 			BGR		( Point3ub & p_ptResult)const;
		void 			RGBA	( Point4ub & p_ptResult)const;
		void 			BGRA	( Point4ub & p_ptResult)const;
		void 			ARGB	( Point4ub & p_ptResult)const;
		void 			ABGR	( Point4ub & p_ptResult)const;
		void			RGB		( Point3f & p_ptResult)const;
		void			BGR		( Point3f & p_ptResult)const;
		void			RGBA	( Point4f & p_ptResult)const;
		void			ARGB	( Point4f & p_ptResult)const;
		void			ABGR	( Point4f & p_ptResult)const;
		void			BGRA	( Point4f & p_ptResult)const;
		unsigned long	RGB		()const;
		unsigned long	BGR		()const;
		unsigned long	ARGB	()const;
		unsigned long	RGBA	()const;
		unsigned long	ABGR	()const;
		unsigned long	BGRA	()const;
		//@}
	};
}
}

#endif