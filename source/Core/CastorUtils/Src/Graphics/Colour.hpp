/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CASTOR_COLOUR_H___
#define ___CASTOR_COLOUR_H___

#include "Math/Point.hpp"
#include "Data/TextLoader.hpp"
#include "Data/TextWriter.hpp"
#include "ColourComponent.hpp"

#undef RGB

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/08/2010
	\~english
	\brief		RGBA colour representation
	\remark		Predefines numerous colours
				<br />Allows conversion to different colour formats (RGB, ARGB, BGR, ...) and different data types (long, float, char, ...)
	\~french
	\brief		Représentation d'une couleur RGBA
	\remark		Prédéfinit differentes couleurs
				<br />Permet la conversion entre différents types de couleurs (RGB, ARGB, BGR, ...) et différents types de données (long, float, char, ...)
	*/
	class Colour
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Colour text loader
		\~french
		\brief		Loader de Colour à partir d'un texte
		*/
		class TextLoader
			: public Castor::TextLoader< Colour >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			CU_API TextLoader();
			/**
			 *\~english
			 *\brief			Reads a colour from a text file
			 *\param[in,out]	p_file		The file from where we read the colour
			 *\param[in,out]	p_colour	The colour to read
			 *\~french
			 *\brief			Lit une couleur à partir d'un fichier texte
			 *\param[in,out]	p_file		Le fichier dans lequel on lit la couleur
			 *\param[in,out]	p_colour	La couleur à lire
			 */
			CU_API virtual bool operator()( Colour & p_colour, TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Colour text Writer
		\~french
		\brief		Writer de Colour à partir d'un texte
		*/
		class TextWriter
			: public Castor::TextWriter< Colour >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			CU_API TextWriter( String const & p_tabs );
			/**
			 *\~english
			 *\brief			Writes a colour into a text file
			 *\param[in,out]	p_file		The file into which colour is written
			 *\param[in]		p_colour	The colour to write
			 *\~french
			 *\brief			Ecrit une couleur dans un fichier texte
			 *\param[in,out]	p_file		Le fichier dans lequel on écrit la couleur
			 *\param[in]		p_colour	La couleur à écrire
			 */
			CU_API virtual bool operator()( Colour const & p_colour, TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Predefined colours enumeration
		\~french
		\brief		Enumération de couleurs prédéfinies
		*/
		enum class Predefined
			: uint32_t
		{
			eTransparentBlack = 0x00000000,
			eLowAlphaBlack = 0x00000040,
			eMedAlphaBlack = 0x0000007F,
			eHighAlphaBlack = 0x000000BF,
			eOpaqueBlack = 0x000000FF,
			eTransparentDarkBlue = 0x00007F00,
			eLowAlphaDarkBlue = 0x00007F40,
			eMedAlphaDarkBlue = 0x00007F7F,
			eHighAlphaDarkBlue = 0x00007FBF,
			eOpaqueDarkBlue = 0x00007FFF,
			eTransparentBlue = 0x0000FF00,
			eLowAlphaBlue = 0x0000FF40,
			eMedAlphaBlue = 0x0000FF7F,
			eHighAlphaBlue = 0x0000FFBF,
			eOpaqueBlue = 0x0000FFFF,
			eTransparentDarkGreen = 0x007F0000,
			eLowAlphaDarkGreen = 0x007F0040,
			eMedAlphaDarkGreen = 0x007F007F,
			eHighAlphaDarkGreen = 0x007F00BF,
			eOpaqueDarkGreen = 0x007F00FF,
			eTransparentGreen = 0x00FF0000,
			eLowAlphaGreen = 0x00FF0040,
			eMedAlphaGreen = 0x00FF007F,
			eHighAlphaGreen = 0x00FF00BF,
			eOpaqueGreen = 0x00FF00FF,
			eTransparentDarkRed = 0x7F000000,
			eLowAlphaDarkRed = 0x7F000040,
			eMedAlphaDarkRed = 0x7F00007F,
			eHighAlphaDarkRed = 0x7F0000BF,
			eOpaqueDarkRed = 0x7F0000FF,
			eTransparentRed = 0xFF000000,
			eLowAlphaRed = 0xFF000040,
			eMedAlphaRed = 0xFF00007F,
			eHighAlphaRed = 0xFF0000BF,
			eOpaqueRed = 0xFF0000FF,
			eTransparentLightBlue = 0x7F7FFF00,
			eLowAlphaLightBlue = 0x7F7FFF40,
			eMedAlphaLightBlue = 0x7F7FFF7F,
			eHighAlphaLightBlue = 0x7F7FFFBF,
			eOpaqueLightBlue = 0x7F7FFFFF,
			eTransparentLightGreen = 0x7FFF7F00,
			eLowAlphaLightGreen = 0x7FFF7F40,
			eMedAlphaLightGreen = 0x7FFF7F7F,
			eHighAlphaLightGreen = 0x7FFF7FBF,
			eOpaqueLightGreen = 0x7FFF7FFF,
			eTransparentLightRed = 0xFF7F7F00,
			eLowAlphaLightRed = 0xFF7F7F40,
			eMedAlphaLightRed = 0xFF7F7F7F,
			eHighAlphaLightRed = 0xFF7F7FBF,
			eOpaqueLightRed = 0xFF7F7FFF,
			eTransparentWhite = 0xFFFFFF00,
			eLowAlphaWhite = 0xFFFFFF40,
			eMedAlphaWhite = 0xFFFFFF7F,
			eHighAlphaWhite = 0xFFFFFFBF,
			eOpaqueWhite = 0xFFFFFFFF,
		};
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		19/10/2011
		\~english
		\brief		Colour components enumeration
		\~french
		\brief		Enumération des composantes d'une couleur
		*/
		enum class Component
			: uint8_t
		{
			//!\~english Red component	\~french Composante rouge
			eRed,
			//!\~english Green component	\~french Composante verte
			eGreen,
			//!\~english Blue component	\~french Composante bleue
			eBlue,
			//!\~english Alpha component	\~french Composante alpha
			eAlpha,
			CASTOR_SCOPED_ENUM_BOUNDS( eRed )
		};

	private:
		friend CU_API void swap( Colour & p_c1, Colour & p_c2 );
		DECLARE_ARRAY( float, Component::eCount, Float4 );
		DECLARE_ARRAY( ColourComponent, Component::eCount, ColourComponent );

	public:
		/**
		 *\~english
		 *\brief		Default Constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		CU_API Colour();
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_rhs	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_rhs	Couleur à copier
		 */
		CU_API Colour( Colour const & p_rhs );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_rhs	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_rhs	Couleur à déplacer
		 */
		CU_API Colour( Colour && p_rhs );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_rhs	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_rhs	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		CU_API Colour & operator=( Colour const & p_rhs );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_rhs	The object to copy
		 *\return		Reference to this colour
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_rhs	Couleur à déplacer
		 *\return		Référence sur cette couleur
		 */
		CU_API Colour & operator=( Colour && p_rhs );
		/**
		 *\~english
		 *\brief		Constructor from components
		 *\param[in]	p_r, p_g, p_b, p_a	The colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir des composantes
		 *\param[in]	p_r, p_g, p_b, p_a	Les composantes de la couleur
		 *\return		La Colour construite
		 */
		template< typename T1, typename T2, typename T3, typename T4 >
		static Colour from_components( T1 const & p_r, T2 const & p_g, T3 const & p_b, T4 const & p_a )
		{
			Colour l_clrReturn;
			l_clrReturn.m_arrayComponents[size_t( Component::eRed )] = p_r;
			l_clrReturn.m_arrayComponents[size_t( Component::eGreen )] = p_g;
			l_clrReturn.m_arrayComponents[size_t( Component::eBlue )] = p_b;
			l_clrReturn.m_arrayComponents[size_t( Component::eAlpha )] = p_a;
			return l_clrReturn;
		}
		/**
		 *\~english
		 *\brief		Constructor from a predefined colour
		 *\param[in]	p_ePredef
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'une couleur prédéfinie
		 *\param[in]	p_ePredef
		 *\return		La Colour construite
		 */
		static Colour from_predef( Predefined p_ePredef )
		{
			return from_rgba( ( uint32_t )p_ePredef );
		}
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_rgb( Point3ub const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_bgr( Point3ub const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_rgba( Point4ub const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_bgra( Point4ub const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_argb( Point4ub const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_abgr( Point4ub const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_rgb( Point3f const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_bgr( Point3f const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_rgba( Point4f const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_argb( Point4f const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_abgr( Point4f const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_bgra( Point4f const & p_ptColour );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_rgb( uint8_t const( & p_ptColour )[3] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_bgr( uint8_t const( & p_ptColour )[3] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_rgba( uint8_t const( & p_ptColour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_bgra( uint8_t const( & p_ptColour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_argb( uint8_t const( & p_ptColour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_abgr( uint8_t const( & p_ptColour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGB components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGB
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_rgb( float const( & p_ptColour )[3] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGR components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGR
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_bgr( float const( & p_ptColour )[3] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing RGBA components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes RGBA
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_rgba( float const( & p_ptColour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ARGB components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ARGB
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_argb( float const( & p_ptColour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing ABGR components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes ABGR
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_abgr( float const( & p_ptColour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from a point representing BGRA components
		 *\param[in]	p_ptColour	The point containing the colour components
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un point représentant des composantes BGRA
		 *\param[in]	p_ptColour	Le point contenant les composantes
		 *\return		La Colour construite
		 */
		CU_API static Colour from_bgra( float const( & p_ptColour )[4] );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing RGB components
		 *\param[in]	p_colour	The uint32_t containing the colour (0x00RRGGBB)
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes RGB
		 *\param[in]	p_colour	L'uint32_t contenant la couleur (0x00RRGGBB)
		 *\return		La Colour construite
		 */
		CU_API static Colour from_rgb( uint32_t p_colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing BGR components
		 *\param[in]	p_colour	The uint32_t containing the colour (0x00BBGGRR)
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes BGR
		 *\param[in]	p_colour	L'uint32_t contenant la couleur (0x00BBGGRR)
		 *\return		La Colour construite
		 */
		CU_API static Colour from_bgr( uint32_t p_colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing ARGB components
		 *\param[in]	p_colour	The uint32_t containing the colour (0xAARRGGBB)
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes ARGB
		 *\param[in]	p_colour	L'uint32_t contenant la couleur (0xAARRGGBB)
		 *\return		La Colour construite
		 */
		CU_API static Colour from_argb( uint32_t p_colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing RGBA components
		 *\param[in]	p_colour	The uint32_t containing the colour (0xRRGGBBAA)
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes RGBA
		 *\param[in]	p_colour	L'uint32_t contenant la couleur (0xRRGGBBAA)
		 *\return		La Colour construite
		 */
		CU_API static Colour from_rgba( uint32_t p_colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing ABGR components
		 *\param[in]	p_colour	The uint32_t containing the colour (0xAABBGGRR)
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes ABGR
		 *\param[in]	p_colour	L'uint32_t contenant la couleur (0xAABBGGRR)
		 *\return		La Colour construite
		 */
		CU_API static Colour from_abgr( uint32_t p_colour );
		/**
		 *\~english
		 *\brief		Constructor from an uint32_t representing BGRA components
		 *\param[in]	p_colour	The uint32_t containing the colour (0xBBGGRRAA)
		 *\return		The built Colour
		 *\~french
		 *\brief		Constructeur à partir d'un uint32_t représentant des composantes BGRA
		 *\param[in]	p_colour	L'uint32_t contenant la couleur (0xBBGGRRAA)
		 *\return		La Colour construite
		 */
		CU_API static Colour from_bgra( uint32_t p_colour );
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first ColourComponent
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le premier ColourComponent
		 *\return		L'itérateur
		 */
		inline ColourComponentArrayIt begin()
		{
			return m_arrayComponents.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to the first ColourComponent
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur le premier ColourComponent
		 *\return		L'itérateur
		 */
		inline ColourComponentArrayConstIt begin()const
		{
			return m_arrayComponents.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to beyond the last ColourComponent
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur après le dernier ColourComponent
		 *\return		L'itérateur
		 */
		inline ColourComponentArrayIt end()
		{
			return m_arrayComponents.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator to beyond the last ColourComponent
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur après le dernier ColourComponent
		 *\return		L'itérateur
		 */
		inline ColourComponentArrayConstIt end()const
		{
			return m_arrayComponents.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant pointer to the colour values
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur constant sur les valeurs de la couleur
		 *\return		Le pointeur
		 */
		inline float const * const_ptr()const
		{
			return &m_arrayValues[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves a pointer to the colour values
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur sur les valeurs de la couleur
		 *\return		Le pointeur
		 */
		inline float * ptr()
		{
			return &m_arrayValues[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	p_eComponent	The asked component
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	p_eComponent	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ColourComponent & operator[]( Component p_eComponent )
		{
			return m_arrayComponents[size_t( p_eComponent )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	p_eComponent	The asked component
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	p_eComponent	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ColourComponent const & operator[]( Component p_eComponent )const
		{
			return m_arrayComponents[size_t( p_eComponent )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	p_eComponent	The asked component
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	p_eComponent	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ColourComponent & get( Component p_eComponent )
		{
			return m_arrayComponents[size_t( p_eComponent )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the asked component value
		 *\param[in]	p_eComponent	The asked component
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante demandée
		 *\param[in]	p_eComponent	La composante demandée
		 *\return		La valeur de la composante
		 */
		inline ColourComponent const & get( Component p_eComponent )const
		{
			return m_arrayComponents[size_t( p_eComponent )];
		}
		/**
		 *\~english
		 *\brief		Retrieves the red component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante rouge
		 *\return		La valeur de la composante
		 */
		inline ColourComponent & red()
		{
			return get( Component::eRed );
		}
		/**
		 *\~english
		 *\brief		Retrieves the red component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante rouge
		 *\return		La valeur de la composante
		 */
		inline ColourComponent const & red()const
		{
			return get( Component::eRed );
		}
		/**
		 *\~english
		 *\brief		Retrieves the green component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante verte
		 *\return		La valeur de la composante
		 */
		inline ColourComponent & green()
		{
			return get( Component::eGreen );
		}
		/**
		 *\~english
		 *\brief		Retrieves the green component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante verte
		 *\return		La valeur de la composante
		 */
		inline ColourComponent const & green()const
		{
			return get( Component::eGreen );
		}
		/**
		 *\~english
		 *\brief		Retrieves the blue component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante bleue
		 *\return		La valeur de la composante
		 */
		inline ColourComponent & blue()
		{
			return get( Component::eBlue );
		}
		/**
		 *\~english
		 *\brief		Retrieves the blue component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante bleue
		 *\return		La valeur de la composante
		 */
		inline ColourComponent const & blue()const
		{
			return get( Component::eBlue );
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante alpha
		 *\return		La valeur de la composante
		 */
		inline ColourComponent & alpha()
		{
			return get( Component::eAlpha );
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha component value
		 *\return		The component value
		 *\~french
		 *\brief		Récupère la valeur de la composante alpha
		 *\return		La valeur de la composante
		 */
		inline ColourComponent const & alpha()const
		{
			return get( Component::eAlpha );
		}
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_clrB	The colours to add
		 *\return		Result of this + p_clrB
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_clrB	Les couleurs à ajouter
		 *\return		Resultat de this + p_clrB
		 */
		CU_API Colour & operator+=( Colour const & p_clrB );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_clrB	The colours to substract
		 *\return		Result of this - p_clrB
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_clrB	Les couleurs à soustraire
		 *\return		Resultat de this - p_clrB
		 */
		CU_API Colour & operator-=( Colour const & p_clrB );
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_scalar	The value to add
		 *\return		Result of this + p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_scalar	La valeur à ajouter
		 *\return		Resultat de this + p_scalar
		 */
		template< typename T >
		Colour & operator+=( T p_scalar )
		{
			for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
			{
				m_arrayComponents[i] += p_scalar;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_scalar	The value to substract
		 *\return		Result of this - p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_scalar	La valeur à soustraire
		 *\return		Resultat de this - p_scalar
		 */
		template< typename T >
		Colour & operator-=( T p_scalar )
		{
			for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
			{
				m_arrayComponents[i] -= p_scalar;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_scalar	The value to multiply
		 *\return		Result of this * p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_scalar	La valeur à multiplier
		 *\return		Resultat de this * p_scalar
		 */
		template< typename T >
		Colour & operator*=( T p_scalar )
		{
			for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
			{
				m_arrayComponents[i] *= p_scalar;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_scalar	The value to divide
		 *\return		Result of this / p_scalar
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_scalar	La valeur à diviser
		 *\return		Resultat de this / p_scalar
		 */
		template< typename T >
		Colour & operator/=( T p_scalar )
		{
			for ( uint8_t i = 0; i < uint8_t( Component::eCount ); i++ )
			{
				m_arrayComponents[i] /= p_scalar;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Addition assignment operator
		 *\param[in]	p_component	The value to add
		 *\return		Result of this + p_component
		 *\~french
		 *\brief		Opérateur d'affectation par addition
		 *\param[in]	p_component	La valeur à ajouter
		 *\return		Resultat de this + p_component
		 */
		CU_API Colour & operator+=( ColourComponent const & p_component );
		/**
		 *\~english
		 *\brief		Substraction assignment operator
		 *\param[in]	p_component	The value to substract
		 *\return		Result of this - p_component
		 *\~french
		 *\brief		Opérateur d'affectation par soustraction
		 *\param[in]	p_component	La valeur à soustraire
		 *\return		Resultat de this - p_component
		 */
		CU_API Colour & operator-=( ColourComponent const & p_component );
		/**
		 *\~english
		 *\brief		Multiplication assignment operator
		 *\param[in]	p_component	The value to multiply
		 *\return		Result of this * p_component
		 *\~french
		 *\brief		Opérateur d'affectation par multiplication
		 *\param[in]	p_component	La valeur à multiplier
		 *\return		Resultat de this * p_component
		 */
		CU_API Colour & operator*=( ColourComponent const & p_component );
		/**
		 *\~english
		 *\brief		Division assignment operator
		 *\param[in]	p_component	The value to divide
		 *\return		Result of this / p_component
		 *\~french
		 *\brief		Opérateur d'affectation par division
		 *\param[in]	p_component	La valeur à diviser
		 *\return		Resultat de this / p_component
		 */
		CU_API Colour & operator/=( ColourComponent const & p_component );
		/**
		 *\~english
		 *\brief		Retrieves predefined colour name
		 *\param[in]	p_predefined	The predefined colour
		 *\return		The colour name
		 *\~french
		 *\brief		Récupère le nom d'une couleur prédéfinie
		 *\param[in]	p_predefined	La couleur prédéfinie
		 *\return		Le nom de la couleur
		 */
		CU_API static String get_predefined_name( Predefined p_predefined );
		/**
		 *\~english
		 *\brief		Retrieves predefined colour from a name
		 *\param[in]	p_name	The predefined colour name
		 *\return		The predefined colour
		 *\~french
		 *\brief		Récupère une couleur prédéfinie à partir de son nom
		 *\param[in]	p_name	Le nom de la couleur prédéfinie
		 *\return		La couleur prédéfinie
		 */
		CU_API static Predefined get_predefined( String const & p_name );

	private:
		ColourComponentArray m_arrayComponents;
		Float4Array m_arrayValues;
		DECLARE_INVARIANT_BLOCK()
	};
	/**
	 *\~english
	 *\brief		Equality operator
	 *\param[in]	p_lhs, p_rhs	The colours to compare
	 *\~french
	 *\brief		Opérateur d'égalité
	 *\param[in]	p_lhs, p_rhs	Les couleurs à comparer
	 */
	CU_API bool operator==( Colour const & p_lhs, Colour const & p_rhs );
	/**
	 *\~english
	 *\brief		Inequality operator
	 *\param[in]	p_lhs, p_rhs	The colours to compare
	 *\~french
	 *\brief		Opérateur de différence
	 *\param[in]	p_lhs, p_rhs	Les couleurs à comparer
	 */
	CU_API bool operator!=( Colour const & p_lhs, Colour const & p_rhs );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_lhs, p_rhs	The colours to add
	 *\return		Result of p_lhs + p_rhs
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_lhs, p_rhs	Les couleurs à ajouter
	 *\return		Resultat de p_lhs + p_rhs
	 */
	CU_API Colour operator+( Colour const & p_lhs, Colour const & p_rhs );
	/**
	 *\~english
	 *\brief		Substraction operator
	 *\param[in]	p_lhs, p_rhs	The colours to substract
	 *\return		Result of p_lhs - p_rhs
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_lhs, p_rhs	Les couleurs à soustraire
	 *\return		Resultat de p_lhs - p_rhs
	 */
	CU_API Colour operator-( Colour const & p_lhs, Colour const & p_rhs );
	/**
	 *\~english
	 *\brief		Addition operator
	 *\param[in]	p_lhs, p_rhs	The values to add
	 *\return		Result of p_lhs + p_rhs
	 *\~french
	 *\brief		Opérateur d'addition
	 *\param[in]	p_lhs, p_rhs	Les valeurs à ajouter
	 *\return		Resultat de p_lhs + p_rhs
	 */
	template< typename T >
	Colour operator+( Colour const & p_lhs, T p_rhs )
	{
		Colour l_clrResult( p_lhs );
		l_clrResult += p_rhs;
		return l_clrResult;
	}
	/**
	 *\~english
	 *\brief		Subtraction operator
	 *\param[in]	p_lhs, p_rhs	The values to subtract
	 *\return		Result of p_lhs - p_rhs
	 *\~french
	 *\brief		Opérateur de soustraction
	 *\param[in]	p_lhs, p_rhs	Les valeurs à soustraire
	 *\return		Resultat de p_lhs - p_rhs
	 */
	template< typename T >
	Colour operator-( Colour const & p_lhs, T p_rhs )
	{
		Colour l_clrResult( p_lhs );
		l_clrResult -= p_rhs;
		return l_clrResult;
	}

	/**
	 *\~english
	 *\brief		Multiplication operator
	 *\param[in]	p_lhs, p_rhs	The values to multiply
	 *\return		Result of p_lhs * p_rhs
	 *\~french
	 *\brief		Opérateur de multiplication
	 *\param[in]	p_lhs, p_rhs	Les valeurs à multiplier
	 *\return		Resultat de p_lhs * p_rhs
	 */
	template< typename T >
	Colour operator*( Colour const & p_lhs, T p_rhs )
	{
		Colour l_clrResult( p_lhs );
		l_clrResult *= p_rhs;
		return l_clrResult;
	}

	/**
	 *\~english
	 *\brief		Division operator
	 *\param[in]	p_lhs, p_rhs	The values to divide
	 *\return		Result of p_lhs / p_rhs
	 *\~french
	 *\brief		Opérateur de division
	 *\param[in]	p_lhs, p_rhs	Les valeurs à diviser
	 *\return		Resultat de p_lhs / p_rhs
	 */
	template< typename T >
	Colour operator/( Colour const & p_lhs, T p_rhs )
	{
		Colour l_clrResult( p_lhs );
		l_clrResult /= p_rhs;
		return l_clrResult;
	}
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in RGB format.
	 *\param[in]	p_colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format RGB.
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	CU_API Point3ub rgb_byte( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in BGR format.
	 *\param[in]	p_colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format BGR.
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	CU_API Point3ub bgr_byte( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in RGBA format.
	 *\param[in]	p_colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format RGBA.
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	CU_API Point4ub rgba_byte( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in BGRA format.
	 *\param[in]	p_colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format BGRA.
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	CU_API Point4ub bgra_byte( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in ARGB format.
	 *\param[in]	p_colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format ARGB.
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	CU_API Point4ub argb_byte( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in ABGR format.
	 *\param[in]	p_colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format ABGR.
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	CU_API Point4ub abgr_byte( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in RGB format.
	 *\param[in]	p_colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format RGB.
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	CU_API Point3f rgb_float( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in BGR format.
	 *\param[in]	p_colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format BGR.
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	CU_API Point3f bgr_float( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in RGBA format.
	 *\param[in]	p_colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format RGBA.
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	CU_API Point4f rgba_float( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in ARGB format.
	 *\param[in]	p_colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format ARGB.
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	CU_API Point4f argb_float( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in ABGR format.
	 *\param[in]	p_colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format ABGR.
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	CU_API Point4f abgr_float( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Stores a colour's components into a point in BGRA format.
	 *\param[in]	p_colour	The colour.
	 *\return		The colour's components.
	 *\~french
	 *\brief		Stoque les composantes de cette couleur dans un point, au format BGRA.
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes de la couleur.
	 */
	CU_API Point4f bgra_float( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Packs a colour's components into an uint32_t in the RGB format (0x00RRGGBB).
	 *\param[in]	p_colour	The colour.
	 *\return		The packed colour.
	 *\~french
	 *\brief		Compile les composantes de la couleur dans un uint32_t, au format RGB (0x00RRGGBB).
	 *\param[in]	p_colour	La couleur.
	 */
	CU_API uint32_t rgb_packed( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Packs a colour's components into an uint32_t in the BGR format (0x00BBGGRR).
	 *\param[in]	p_colour	The colour.
	 *\return		The packed colour.
	 *\~french
	 *\brief		Compile les composantes de la couleur dans un uint32_t, au format BGR (0x00BBGGRR).
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes compilées de la couleur.
	 */
	CU_API uint32_t bgr_packed( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Packs a colour's components into an uint32_t in the ARGB format (0xAARRGGBB).
	 *\param[in]	p_colour	The colour.
	 *\return		The packed colour.
	 *\~french
	 *\brief		Compile les composantes de la couleur dans un uint32_t, au format ARGB (0xAARRGGBB).
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes compilées de la couleur.
	 */
	CU_API uint32_t argb_packed( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Packs a colour's components into an uint32_t in the RGBA format (0xRRGGBBAA).
	 *\param[in]	p_colour	The colour.
	 *\return		The packed colour.
	 *\~french
	 *\brief		Compile les composantes de la couleur dans un uint32_t, au format RGBA (0xRRGGBBAA).
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes compilées de la couleur.
	 */
	CU_API uint32_t rgba_packed( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Packs a colour's components into an uint32_t in the ABGR format (0xAABBGGRR).
	 *\param[in]	p_colour	The colour.
	 *\return		The packed colour.
	 *\~french
	 *\brief		Compile les composantes de la couleur dans un uint32_t, au format ABGR (0xAABBGGRR).
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes compilées de la couleur.
	 */
	CU_API uint32_t abgr_packed( Colour const & p_colour );
	/**
	 *\~english
	 *\brief		Packs a colour's components into an uint32_t in the BGRA format (0xBBGGRRAA).
	 *\param[in]	p_colour	The colour.
	 *\return		The packed colour.
	 *\~french
	 *\brief		Compile les composantes de la couleur dans un uint32_t, au format BGRA (0xBBGGRRAA).
	 *\param[in]	p_colour	La couleur.
	 *\return		Les composantes compilées de la couleur.
	 */
	CU_API uint32_t bgra_packed( Colour const & p_colour );
}

#endif
