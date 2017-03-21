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
#ifndef ___CASTOR_GLYPH_H___
#define ___CASTOR_GLYPH_H___

#include "Design/Resource.hpp"
#include "Position.hpp"
#include "Size.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		18/08/2011
	\~english
	\brief		Font character (glyph) representation
	\remark		Holds position, size and data of a character
	\~french
	\brief		Représentation d'un caractère d'une Font (glyphe)
	\remark		Contient la position, taille et pixels d'un caractère
	*/
	class Glyph
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_char		The glyph character
		 *\param[in]	p_size		The glyph dimensions
		 *\param[in]	p_bearing	The glyph position relative to cursor
		 *\param[in]	p_advance	Pixels to advance in order to go next character
		 *\param[in]	p_bitmap	The glyph image
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_char		Le caractère de la glyphe
		 *\param[in]	p_size		Les dimensions de la glyphe
		 *\param[in]	p_bearing	La position de la glyphe par rapport au curseur
		 *\param[in]	p_advance	Nombre de pixels pour placer le caractère suivant
		 *\param[in]	p_bitmap	L'image de la glyphe
		 */
		inline Glyph( char32_t p_char
			, Size const & p_size
			, Position const & p_bearing
			, uint32_t p_advance
			, ByteArray const & p_bitmap )
			: m_size{ p_size }
			, m_bearing{ p_bearing }
			, m_bitmap{ p_bitmap }
			, m_advance{ p_advance }
			, m_character{ p_char }
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		inline ~Glyph()
		{
		}
		/**
		 *\~english
		 *\brief		Retrieves the glyph character
		 *\return		The value
		 *\~french
		 *\brief		Récupère le caractère de la glyphe
		 *\return		La valeur
		 */
		inline char32_t GetCharacter()const
		{
			return m_character;
		}
		/**
		 *\~english
		 *\brief		Retrieves the glyph dimensions
		 *\return		The glyph dimensions
		 *\~french
		 *\brief		Récupère les dimensions de la glyphe
		 *\return		Les dimensions de la glyphe
		 */
		inline Size const & GetSize()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The glyph position relative to cursor.
		 *\~french
		 *\return		La position de la glyphe par rapport au curseur.
		 */
		inline Position const & GetBearing()const
		{
			return m_bearing;
		}
		/**
		 *\~english
		 *\brief		Retrieves the number of pixels to go before drawing next glyph
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de pixels pour place la prchaine glyphe
		 *\return		La valeur
		 */
		inline uint32_t GetAdvance()const
		{
			return m_advance;
		}
		/**
		 *\~english
		 *\brief		Retrieves the glyph image
		 *\return		A constant reference to the image of the glyph
		 *\~french
		 *\brief		Récupère l'image de la glyphe
		 *\return		Une référence constante sur l'image de la glyphe
		 */
		inline ByteArray const & GetBitmap()const
		{
			return m_bitmap;
		}

	private:
		//!\~english Glyph position relative to cursor.	\~french Position de la glyphe par rapport au curseur.
		Position const m_bearing;
		//!\~english Glyph dimensions.	\~french Dimensions de la glyphe.
		Size const m_size;
		//!\~english Pixels to advance in order to go next character	\~french Nombre de pixels pour placer le caractère suivant
		uint32_t m_advance;
		//!\~english Glyph image	\~french Image de la glyphe
		ByteArray const m_bitmap;
		//!\~english Glyph character	\~french Caractère de la glyphe
		char32_t const m_character;
	};
	/**
	 *\~english
	 *\brief		Equality comparison operator.
	 *\param[in]	p_lhs, p_rhs	The values to compare.
	 *\~french
	 *\brief		Opérateur de comparaison d'égalité.
	 *\param[in]	p_lhs, p_rhs	Les valeurs à comparer.
	 */
	inline bool operator==( Glyph const & p_lhs, Glyph const & p_rhs )
	{
		return p_lhs.GetCharacter() == p_rhs.GetCharacter();
	}
	/**
	 *\~english
	 *\brief		Difference comparison operator.
	 *\param[in]	p_lhs, p_rhs	The values to compare.
	 *\~french
	 *\brief		Opérateur de comparaison de différence.
	 *\param[in]	p_lhs, p_rhs	Les valeurs à comparer.
	 */
	inline bool operator!=( Glyph const & p_lhs, Glyph const & p_rhs )
	{
		return p_lhs.GetCharacter() != p_rhs.GetCharacter();
	}
	/**
	 *\~english
	 *\brief		Less than comparison operator.
	 *\param[in]	p_lhs, p_rhs	The values to compare.
	 *\~french
	 *\brief		Opérateur de comparaison inférieur.
	 *\param[in]	p_lhs, p_rhs	Les valeurs à comparer.
	 */
	inline bool operator<( Glyph const & p_lhs, Glyph const & p_rhs )
	{
		return p_lhs.GetCharacter() < p_rhs.GetCharacter();
	}
	/**
	 *\~english
	 *\brief		Greater than comparison operator.
	 *\param[in]	p_lhs, p_rhs	The values to compare.
	 *\~french
	 *\brief		Opérateur de comparaison supérieur.
	 *\param[in]	p_lhs, p_rhs	Les valeurs à comparer.
	 */
	inline bool operator>( Glyph const & p_lhs, Glyph const & p_rhs )
	{
		return p_lhs.GetCharacter() > p_rhs.GetCharacter();
	}
	/**
	 *\~english
	 *\brief		Less than or equal to comparison operator.
	 *\param[in]	p_lhs, p_rhs	The values to compare.
	 *\~french
	 *\brief		Opérateur de comparaison inférieur ou égal.
	 *\param[in]	p_lhs, p_rhs	Les valeurs à comparer.
	 */
	inline bool operator<=( Glyph const & p_lhs, Glyph const & p_rhs )
	{
		return p_lhs.GetCharacter() <= p_rhs.GetCharacter();
	}
	/**
	 *\~english
	 *\brief		Greater than or equal to comparison operator.
	 *\param[in]	p_lhs, p_rhs	The values to compare.
	 *\~french
	 *\brief		Opérateur de comparaison supérieur ou égal.
	 *\param[in]	p_lhs, p_rhs	Les valeurs à comparer.
	 */
	inline bool operator>=( Glyph const & p_lhs, Glyph const & p_rhs )
	{
		return p_lhs.GetCharacter() >= p_rhs.GetCharacter();
	}
}

#endif
