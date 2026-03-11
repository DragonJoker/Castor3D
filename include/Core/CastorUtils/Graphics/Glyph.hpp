/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_GLYPH_H___
#define ___CASTOR_GLYPH_H___

#include "CastorUtils/Graphics/Position.hpp"
#include "CastorUtils/Graphics/Size.hpp"

namespace c3d
{
	class Glyph
	{
	private:
		Glyph( Glyph const & ) = delete;
		Glyph & operator=( Glyph const & ) = delete;

	public:
		CU_INL_API Glyph( Glyph && )noexcept = default;
		CU_INL_API Glyph & operator=( Glyph && )noexcept = default;
		CU_INL_API ~Glyph()noexcept = default;
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	c			The glyph character.
		 *\param[in]	size		The glyph dimensions.
		 *\param[in]	bearing		The glyph position relative to cursor.
		 *\param[in]	advance		Pixels to advance in order to go next character.
		 *\param[in]	bitmapSize	The bitmap dimensions.
		 *\param[in]	bitmap		The glyph image.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	c			Le caractère de la glyphe.
		 *\param[in]	size		Les dimensions de la glyphe.
		 *\param[in]	bearing		La position de la glyphe par rapport au curseur.
		 *\param[in]	advance		Nombre de pixels pour placer le caractère suivant.
		 *\param[in]	bitmapSize	Dimensions du bitmap.
		 *\param[in]	bitmap		L'image de la glyphe.
		 */
		Glyph( char32_t c
			, Point2f size
			, Point2f bearing
			, float advance
			, Size bitmapSize
			, ByteArray bitmap )
			: m_bearing{ c3d::move( bearing ) }
			, m_size{ c3d::move( size ) }
			, m_advance{ advance }
			, m_bitmapSize{ c3d::move( bitmapSize ) }
			, m_bitmap{ c3d::move( bitmap ) }
			, m_character{ c }
		{
		}
		/**
		 *\~english
		 *\return		The glyph character.
		 *\~french
		 *\return		Le caractère de la glyphe.
		 */
		char32_t getCharacter()const noexcept
		{
			return m_character;
		}
		/**
		 *\~english
		 *\return		The glyph dimensions.
		 *\~french
		 *\return		Les dimensions de la glyphe.
		 */
		Point2f const & getSize()const noexcept
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The glyph position relative to cursor.
		 *\~french
		 *\return		La position de la glyphe par rapport au curseur.
		 */
		Point2f const & getBearing()const noexcept
		{
			return m_bearing;
		}
		/**
		 *\~english
		 *\return		The number of pixels to go before drawing next glyph.
		 *\~french
		 *\return		Le nombre de pixels pour place la prchaine glyphe.
		 */
		float getAdvance()const noexcept
		{
			return m_advance;
		}
		/**
		 *\~english
		 *\return		The image of the glyph.
		 *\~french
		 *\return		L'image de la glyphe.
		 */
		ByteArray const & getBitmap()const noexcept
		{
			return m_bitmap;
		}
		/**
		 *\~english
		 *\return		The glyph image dimensions.
		 *\~french
		 *\return		Les dimensions de l'image de la glyphe.
		 */
		Size const & getBitmapSize()const noexcept
		{
			return m_bitmapSize;
		}

	private:
		Point2f m_bearing;
		Point2f m_size;
		float m_advance;
		Size m_bitmapSize;
		ByteArray m_bitmap;
		char32_t m_character;

		friend bool operator==( Glyph const & lhs, Glyph const & rhs )noexcept
		{
			return lhs.getCharacter() == rhs.getCharacter();
		}

		friend bool operator<( Glyph const & lhs, Glyph const & rhs )noexcept
		{
			return lhs.getCharacter() < rhs.getCharacter();
		}

		friend bool operator>( Glyph const & lhs, Glyph const & rhs )noexcept
		{
			return lhs.getCharacter() > rhs.getCharacter();
		}

		friend bool operator<=( Glyph const & lhs, Glyph const & rhs )noexcept
		{
			return lhs.getCharacter() <= rhs.getCharacter();
		}

		friend bool operator>=( Glyph const & lhs, Glyph const & rhs )noexcept
		{
			return lhs.getCharacter() >= rhs.getCharacter();
		}
	};
}

#endif
