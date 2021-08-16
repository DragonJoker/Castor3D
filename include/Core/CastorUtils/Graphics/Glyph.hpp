/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_GLYPH_H___
#define ___CASTOR_GLYPH_H___

#include "CastorUtils/Graphics/Position.hpp"
#include "CastorUtils/Graphics/Size.hpp"

namespace castor
{
	class Glyph
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	c		The glyph character
		 *\param[in]	size	The glyph dimensions
		 *\param[in]	bearing	The glyph position relative to cursor
		 *\param[in]	advance	Pixels to advance in order to go next character
		 *\param[in]	bitmap	The glyph image
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	c		Le caractère de la glyphe
		 *\param[in]	size	Les dimensions de la glyphe
		 *\param[in]	bearing	La position de la glyphe par rapport au curseur
		 *\param[in]	advance	Nombre de pixels pour placer le caractère suivant
		 *\param[in]	bitmap	L'image de la glyphe
		 */
		inline Glyph( char32_t c
			, Size const & size
			, Position const & bearing
			, uint32_t advance
			, ByteArray const & bitmap )
			: m_size{ size }
			, m_bearing{ bearing }
			, m_bitmap{ bitmap }
			, m_advance{ advance }
			, m_character{ c }
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
		inline char32_t getCharacter()const
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
		inline Size const & getSize()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The glyph position relative to cursor.
		 *\~french
		 *\return		La position de la glyphe par rapport au curseur.
		 */
		inline Position const & getBearing()const
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
		inline uint32_t getAdvance()const
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
		inline ByteArray const & getBitmap()const
		{
			return m_bitmap;
		}

	private:
		//!\~english	Glyph position relative to cursor.
		//!\~french		Position de la glyphe par rapport au curseur.
		Position const m_bearing;
		//!\~english	Glyph dimensions.
		//!\~french		Dimensions de la glyphe.
		Size const m_size;
		//!\~english	Pixels to advance in order to go next character.
		//!\~french		Nombre de pixels pour placer le caractère suivant.
		uint32_t m_advance;
		//!\~english	Glyph image.
		//!\~french		Image de la glyphe.
		ByteArray const m_bitmap;
		//!\~english	Glyph character.
		//!\~french		Caractère de la glyphe.
		char32_t const m_character;
	};
	/**
	 *\~english
	 *\brief		Equality comparison operator.
	 *\param[in]	lhs, rhs	The values to compare.
	 *\~french
	 *\brief		Opérateur de comparaison d'égalité.
	 *\param[in]	lhs, rhs	Les valeurs à comparer.
	 */
	inline bool operator==( Glyph const & lhs, Glyph const & rhs )
	{
		return lhs.getCharacter() == rhs.getCharacter();
	}
	/**
	 *\~english
	 *\brief		Difference comparison operator.
	 *\param[in]	lhs, rhs	The values to compare.
	 *\~french
	 *\brief		Opérateur de comparaison de différence.
	 *\param[in]	lhs, rhs	Les valeurs à comparer.
	 */
	inline bool operator!=( Glyph const & lhs, Glyph const & rhs )
	{
		return lhs.getCharacter() != rhs.getCharacter();
	}
	/**
	 *\~english
	 *\brief		Less than comparison operator.
	 *\param[in]	lhs, rhs	The values to compare.
	 *\~french
	 *\brief		Opérateur de comparaison inférieur.
	 *\param[in]	lhs, rhs	Les valeurs à comparer.
	 */
	inline bool operator<( Glyph const & lhs, Glyph const & rhs )
	{
		return lhs.getCharacter() < rhs.getCharacter();
	}
	/**
	 *\~english
	 *\brief		Greater than comparison operator.
	 *\param[in]	lhs, rhs	The values to compare.
	 *\~french
	 *\brief		Opérateur de comparaison supérieur.
	 *\param[in]	lhs, rhs	Les valeurs à comparer.
	 */
	inline bool operator>( Glyph const & lhs, Glyph const & rhs )
	{
		return lhs.getCharacter() > rhs.getCharacter();
	}
	/**
	 *\~english
	 *\brief		Less than or equal to comparison operator.
	 *\param[in]	lhs, rhs	The values to compare.
	 *\~french
	 *\brief		Opérateur de comparaison inférieur ou égal.
	 *\param[in]	lhs, rhs	Les valeurs à comparer.
	 */
	inline bool operator<=( Glyph const & lhs, Glyph const & rhs )
	{
		return lhs.getCharacter() <= rhs.getCharacter();
	}
	/**
	 *\~english
	 *\brief		Greater than or equal to comparison operator.
	 *\param[in]	lhs, rhs	The values to compare.
	 *\~french
	 *\brief		Opérateur de comparaison supérieur ou égal.
	 *\param[in]	lhs, rhs	Les valeurs à comparer.
	 */
	inline bool operator>=( Glyph const & lhs, Glyph const & rhs )
	{
		return lhs.getCharacter() >= rhs.getCharacter();
	}
}

#endif
