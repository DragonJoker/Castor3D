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
#ifndef ___CASTOR_GLYPH_H___
#define ___CASTOR_GLYPH_H___

#include "Resource.hpp"
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
		 *\param[in]	p_wcChar	The glyph character
		 *\param[in]	p_size		The glyph dimensions
		 *\param[in]	p_position	The glyph position
		 *\param[in]	p_advance	Pixels to advance in order to go next character
		 *\param[in]	p_bitmap	The glyph image
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_wcChar	Le caractère de la glyphe
		 *\param[in]	p_size		Les dimensions de la glyphe
		 *\param[in]	p_position	La position de la glyphe
		 *\param[in]	p_advance	Nombre de pixels pour placer le caractère suivant
		 *\param[in]	p_bitmap	L'image de la glyphe
		 */
		Glyph( wchar_t p_wcChar = 0, Size const & p_size = Size(), Position const & p_position = Position(), Size const & p_advance = Size(), ByteArray const & p_bitmap = ByteArray() );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Glyph();
		/**
		 *\~english
		 *\brief		Retrieves the glyph character
		 *\return		The value
		 *\~french
		 *\brief		Récupère le caractère de la glyphe
		 *\return		La valeur
		 */
		inline wchar_t GetCharacter()const
		{
			return m_wcCharacter;
		}
		/**
		 *\~english
		 *\brief		Sets the glyph dimensions
		 *\param[in]	p_size	The glyph dimensions
		 *\~french
		 *\brief		Définit les dimensions de la glyphe
		 *\param[in]	p_size	Les dimensions de la glyphe
		 */
		inline void SetSize( Size const & p_size )
		{
			m_size = p_size;
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
		 *\brief		Sets the glyph position
		 *\param[in]	p_position	The glyph position
		 *\~french
		 *\brief		Définit la position de la glyphe
		 *\param[in]	p_position	La position de la glyphe
		 */
		inline void SetPosition( Position const & p_position )
		{
			m_position = p_position;
		}
		/**
		 *\~english
		 *\brief		Retrieves the glyph position
		 *\return		The glyph position
		 *\~french
		 *\brief		Récupère la position de la glyphe
		 *\return		La position de la glyphe
		 */
		inline Position const & GetPosition()const
		{
			return m_position;
		}
		/**
		 *\~english
		 *\brief		Sets the number of pixels to go before drawing next glyph
		 *\param[in]	p_advance	The value
		 *\~french
		 *\brief		Définit le nombre de pixels pour place la prchaine glyphe
		 *\param[in]	p_advance	La valeur
		 */
		inline void SetAdvance( Size const & p_advance )
		{
			m_advance = p_advance;
		}
		/**
		 *\~english
		 *\brief		Retrieves the number of pixels to go before drawing next glyph
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nombre de pixels pour place la prchaine glyphe
		 *\return		La valeur
		 */
		inline Size const & GetAdvance()const
		{
			return m_advance;
		}
		/**
		 *\~english
		 *\brief		Sets the glyph image
		 *\param[in]	p_bitmap	The image of the glyph
		 *\~french
		 *\brief		Définit l'image de la glyphe
		 *\param[in]	p_bitmap	L'image de la glyphe
		 */
		inline void SetBitmap( ByteArray const & p_bitmap )
		{
			m_bitmap = p_bitmap;
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
		/**
		 *\~english
		 *\brief		Adjusts the glyph position
		 *\param[in]	x, y	The glyph position decal
		 *\~french
		 *\brief		Ajuste la position de la glyphe
		 *\param[in]	x, y	Le décalage de position de la glyphe
		 */
		inline void AdjustPosition( uint32_t x, uint32_t y )
		{
			m_position.offset( x, y );
		}

	private:
		//!\~english Glyph position, in texture	\~french Poisition de la glyphe, dans la texture
		Position m_position;
		//!\~english Glyph dimensions	\~french Dimensions de la glyphe
		Size m_size;
		//!\~english Pixels to advance in order to go next character	\~french Nombre de pixels pour placer le caractère suivant
		Size m_advance;
		//!\~english Glyph image	\~french Image de la glyphe
		ByteArray m_bitmap;
		//!\~english Glyph character	\~french Caractère de la glyphe
		wchar_t m_wcCharacter;
	};
}

#endif
