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
#ifndef ___CASTOR_FONT_H___
#define ___CASTOR_FONT_H___

#include "Resource.hpp"
#include "Loader.hpp"
#include "Collection.hpp"
#include "Point.hpp"
#include "Glyph.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		17/01/2011
	\~english
	\brief		Font resource
	\remark		Representation of a font : face, precision, and others
				<br />The generated font will be put in an image
	\~french
	\brief		Ressource Font
	\remark		Représentation d'une font : face, précision, et autres
				<br />La font chargée sera placée dans une image
	*/
	class Font
		: public Resource< Font >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		03/01/2011
		\~english
		\brief		Font loader
		\remark		Uses FreeType to load font
		\~french
		\brief		Loader de Font
		\remark		Utilise FreeType pour charger la police
		*/
		class BinaryLoader
			: public Loader< Font, eFILE_TYPE_BINARY, BinaryFile >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			BinaryLoader();
			/**
			 *\~english
			 *\brief			Loads a font
			 *\param[in,out]	p_font		The font to load
			 *\param[in]		p_path		The path of the font file
			 *\param[in]		p_uiHeight	The font precision
			 *\~french
			 *\brief			Charge une police
			 *\param[in,out]	p_font		La police à charger
			 *\param[in]		p_path		Le chemin du fichier contenant la police
			 *\param[in]		p_uiHeight	La précision de la police
			 */
			bool operator()( Font & p_font, Path const & p_path, uint32_t p_uiHeight );

		private:
			virtual bool operator()( Font & p_font, Path const & p_path );

		private:
			//!\~english Font wanted height	\~french Hauteur voulue pour la police
			uint32_t m_uiHeight;
		};

		/*!
		\author		Sylvain DOREMUS
		\version	0.6.1.0
		\date		17/01/2011
		\~english
		\brief		Structure used to load glyphs on demand
		\remark		PImpl to hide FreeType inclusions
		\~french
		\brief		Structure utilisée pour charger des glyphes à la demande
		\remark		PImpl pour cacher les inclusions de FreeType
		*/
		struct SFontImpl
		{
			/**
			 *\~english
			 *\brief		Initialises the loader
			 *\~french
			 *\brief		Initialise le loader
			 */
			virtual void Initialise() = 0;
			/**
			 *\~english
			 *\brief		Cleans the loader up
			 *\~french
			 *\brief		Nettoie le loader
			 */
			virtual void Cleanup() = 0;
			/**
			 *\~english
			 *\brief		Loads wanted glyph
			 *\param[in]	p_glyph	The glyph
			 *\return		The glyph
			 *\~french
			 *\brief		Charge le glyphe voulue
			 *\param[in]	p_glyph	Le glyphe
			 */
			virtual int LoadGlyph( Glyph & p_glyph ) = 0;
		};

		DECLARE_VECTOR( Glyph, Glyph );
		DECLARE_MAP( wchar_t, Glyph *, Glyph );

	protected:
		DECLARE_INVARIANT_BLOCK()

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_strName	The font name
		 *\param[in]	p_uiHeight	The font char height
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_strName	Le nom de la police
		 *\param[in]	p_uiHeight	La hauteur des caractères de la police
		 */
		Font( String const & p_strName, uint32_t p_uiHeight );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_path		The font file path
		 *\param[in]	p_strName	The font name
		 *\param[in]	p_uiHeight	The font char height
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_path		Le chemin d'accès au fichier contenant la police
		 *\param[in]	p_strName	Le nom de la police
		 *\param[in]	p_uiHeight	La hauteur des caractères de la police
		 */
		Font( Path const & p_path, String const & p_strName, uint32_t p_uiHeight );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Font();
		/**
		 *\~english
		 *\brief		Sets the glyph for given character
		 *\param[in]	p_char		The character
		 *\param[in]	p_size		The glyph dimensions
		 *\param[in]	p_position	The glyph position
		 *\param[in]	p_advance	Pixels to advance in order to go next character
		 *\param[in]	p_bitmap	The glyph image
		 *\~french
		 *\brief		Définit la glyphe pour le caractère donné
		 *\param[in]	p_char		Le caractère
		 *\param[in]	p_size		Les dimensions de la glyphe
		 *\param[in]	p_position	La position de la glyphe
		 *\param[in]	p_advance	Nombre de pixels pour placer le caractère suivant
		 *\param[in]	p_bitmap	L'image de la glyphe
		 */
		void SetGlyphAt( wchar_t p_char, Size const & p_size, Position p_position, Size const & p_advance, ByteArray const & p_bitmap );
		/**
		 *\~english
		 *\brief		Loads wanted glyph
		 *\param[in]	p_char	The character
		 *\return		The glyph
		 *\~french
		 *\brief		Charge le glyphe voulue
		 *\param[in]	p_char	Le caractère
		 */
		int LoadGlyph( wchar_t p_char );
		/**
		 *\~english
		 *\brief		Retrieves the glyph of wanted chaaracter
		 *\param[in]	p_char The wanted character
		 *\return		A constant reference on the glyph
		 *\~french
		 *\brief		Récupère la glyphe du caractère voulu
		 *\param[in]	p_char	Le caractère voulu
		 *\return		Une référence constante sur la glyphe
		 */
		inline Glyph const & GetGlyphAt( wchar_t p_char )const
		{
			return m_glyphs[p_char];
		}
		/**
		 *\~english
		 *\brief		Retrieves the glyph of wanted chaaracter
		 *\param[in]	p_char The wanted character
		 *\return		A reference on the glyph
		 *\~french
		 *\brief		Récupère la glyphe du caractère voulu
		 *\param[in]	p_char	Le caractère voulu
		 *\return		Une référence sur la glyphe
		 */
		inline Glyph & GetGlyphAt( wchar_t p_char )
		{
			return m_glyphs[p_char];
		}
		/**
		 *\~english
		 *\brief		Retrieves the glyph of wanted chaaracter
		 *\param[in]	p_char The wanted character
		 *\return		A constant reference on the glyph
		 *\~french
		 *\brief		Récupère la glyphe du caractère voulu
		 *\param[in]	p_char	Le caractère voulu
		 *\return		Une référence constante sur la glyphe
		 */
		inline Glyph const & operator []( wchar_t p_char )const
		{
			return m_glyphs[p_char];
		}
		/**
		 *\~english
		 *\brief		Retrieves the glyph of wanted chaaracter
		 *\param[in]	p_char The wanted character
		 *\return		A reference on the glyph
		 *\~french
		 *\brief		Récupère la glyphe du caractère voulu
		 *\param[in]	p_char	Le caractère voulu
		 *\return		Une référence sur la glyphe
		 */
		inline Glyph & operator []( wchar_t p_char )
		{
			return m_glyphs[p_char];
		}
		/**
		 *\~english
		 *\brief		Retrieves the height of the font
		 *\return		The font height
		 *\~french
		 *\brief		Récupère la hauteur de la police
		 *\return		La hauteur de la police
		 */
		inline uint32_t GetHeight()const
		{
			return m_uiHeight;
		}
		/**
		 *\~english
		 *\brief		Retrieves the max height of the glyphs
		 *\return		The glyphs max height
		 *\~french
		 *\brief		Récupère la hauteur maximale des glyphes
		 *\return		La hauteur maximale des glyphes
		 */
		inline int GetMaxHeight()const
		{
			return m_iMaxHeight;
		}
		/**
		 *\~english
		 *\brief		Retrieves the max width of the glyphs
		 *\return		The glyphs max width
		 *\~french
		 *\brief		Récupère la largeur maximale des glyphes
		 *\return		La largeur maximale des glyphes
		 */
		inline int GetMaxWidth()const
		{
			return m_iMaxWidth;
		}
		/**
		 *\~english
		 *\brief		Sets the max height of the glyphs
		 *\param[in]	p_iHeight	The new height
		 *\~french
		 *\brief		Définit la hauteur maximale des glyphes
		 *\param[in]	p_iHeight	La nouvelle hauteur
		 */
		inline void SetMaxHeight( int p_iHeight )
		{
			m_iMaxHeight = p_iHeight;
		}
		/**
		 *\~english
		 *\brief		Sets the max width of the glyphs
		 *\param[in]	p_iWidth	The new width
		 *\~french
		 *\brief		Définit la largeur maximale des glyphes
		 *\param[in]	p_iWidth	La nouvelle largeur
		 */
		inline void SetMaxWidth( int p_iWidth )
		{
			m_iMaxWidth = p_iWidth;
		}
		/**
		 *\~english
		 *\brief		Sets the glyph loader
		 *\param[in]	p_iWidth	The new width
		 *\~french
		 *\brief		Définit la largeur maximale des glyphes
		 *\param[in]	p_iWidth	La nouvelle largeur
		 */
		inline void SetGlyphLoader( std::unique_ptr< SFontImpl > p_loader )
		{
			m_glyphLoader = std::move( p_loader );
		}
		/**
		 *\~english
		 *\brief		Retrieves the glyph loader
		 *\return		The loader
		 *\~french
		 *\brief		Définit le loader de glyphes
		 *\return		Le loader
		 */
		inline std::unique_ptr< SFontImpl > & GetGlyphLoader()
		{
			return m_glyphLoader;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the first glyph
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la première glyphe
		 *\return		L'itérateur
		 */
		inline GlyphMap::iterator Begin()
		{
			return m_loadedGlyphs.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the first glyph
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la première glyphe
		 *\return		L'itérateur
		 */
		inline GlyphMap::const_iterator Begin()const
		{
			return m_loadedGlyphs.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the glyph array end
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau de glyphes
		 *\return		L'itérateur
		 */
		inline GlyphMap::iterator End()
		{
			return m_loadedGlyphs.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the glyph array end
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau de glyphes
		 *\return		L'itérateur
		 */
		inline GlyphMap::const_iterator End()const
		{
			return m_loadedGlyphs.end();
		}

	protected:
		//!\~english The height of the font	\~french La hauteur de la police
		uint32_t m_uiHeight;
		//!\~english The path of the font file	\~french Le chemin du fichier de la police
		Path m_pathFile;
		//!\~english The array of glyphs	\~french Le tableau de glyphes
		GlyphArray m_glyphs;
		//!\~english The array of loaded glyphs	\~french Le tableau de glyphes chargées
		GlyphMap m_loadedGlyphs;
		//!\~english The max height of the glyphs	\~french La hauteur maximale des glyphes
		int m_iMaxHeight;
		//!\~english The max top of the glyphs	\~french La position haute maximale des glyphes
		int m_iMaxTop;
		//!\~english The max width of the glyphs	\~french La largeur maximale des glyphes
		int m_iMaxWidth;
		//!\~english The glyph loader	\~french Le chargeur de glyphes
		std::unique_ptr< SFontImpl > m_glyphLoader;
	};
}

#endif
