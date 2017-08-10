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
#ifndef ___CASTOR_FONT_H___
#define ___CASTOR_FONT_H___

#include "Design/Resource.hpp"
#include "Data/BinaryLoader.hpp"
#include "Data/TextWriter.hpp"
#include "Design/Collection.hpp"
#include "Math/Point.hpp"
#include "Glyph.hpp"

namespace castor
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
		\date		14/02/2010
		\~english
		\brief		Font text loader
		\~french
		\brief		Loader de Font à partir d'un texte
		*/
		class TextWriter
			: public castor::TextWriter< Font >
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
			 *\brief			Writes a Font into a text file
			 *\param[in]		p_object	The Font to write
			 *\param[in,out]	p_file		The file into which Font is written
			 *\~french
			 *\brief			Ecrit une Font dans un fichier texte
			 *\param[in]		p_object	La Font à écrire
			 *\param[in,out]	p_file		Le fichier dans lequel on écrit la Font
			 */
			CU_API bool operator()( Font const & p_object, TextFile & p_file )override;
		};
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
			: public castor::BinaryLoader< Font >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			CU_API BinaryLoader();
			/**
			 *\~english
			 *\brief			Loads a font
			 *\param[in,out]	p_font		The font to load
			 *\param[in]		p_path		The path of the font file
			 *\param[in]		p_height	The font precision
			 *\~french
			 *\brief			Charge une police
			 *\param[in,out]	p_font		La police à charger
			 *\param[in]		p_path		Le chemin du fichier contenant la police
			 *\param[in]		p_height	La précision de la police
			 */
			CU_API bool operator()( Font & p_font, Path const & p_path, uint32_t p_height );

		private:
			CU_API bool operator()( Font & p_font, Path const & p_path );

		private:
			//!\~english Font wanted height	\~french Hauteur voulue pour la police
			uint32_t m_height;
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
			virtual void initialise() = 0;
			/**
			 *\~english
			 *\brief		Cleans the loader up
			 *\~french
			 *\brief		Nettoie le loader
			 */
			virtual void cleanup() = 0;
			/**
			 *\~english
			 *\brief		Loads wanted glyph.
			 *\param[in]	p_char	The character.
			 *\return		The glyph.
			 *\~french
			 *\brief		Charge le glyphe voulu.
			 *\param[in]	p_char	Le caractère.
			 *\return		Le glyphe.
			 */
			virtual Glyph loadGlyph( char32_t p_char ) = 0;
		};

		DECLARE_VECTOR( Glyph, Glyph );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name		The font name
		 *\param[in]	p_height	The font char height
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name		Le nom de la police
		 *\param[in]	p_height	La hauteur des caractères de la police
		 */
		CU_API Font( String const & p_name, uint32_t p_height );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_path		The font file path
		 *\param[in]	p_name		The font name
		 *\param[in]	p_height	The font char height
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_path		Le chemin d'accès au fichier contenant la police
		 *\param[in]	p_name		Le nom de la police
		 *\param[in]	p_height	La hauteur des caractères de la police
		 */
		CU_API Font( String const & p_name, uint32_t p_height, Path const & p_path );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~Font();
		/**
		 *\~english
		 *\brief		Loads wanted glyph.
		 *\param[in]	p_char	The character.
		 *\~french
		 *\brief		Charge le glyphe voulu.
		 *\param[in]	p_char	Le caractère.
		 */
		CU_API void loadGlyph( char32_t p_char );
		/**
		 *\~english
		 *\brief		Tells if the font already has load ed the wanted glyph.
		 *\param[in]	p_char	The character.
		 *\return		false if not.
		 *\~french
		 *\brief		Dit si la police a déjà chargé la glyphe voulue.
		 *\param[in]	p_char	Le caractère.
		 *\return		false si elle n'est pas chargée.
		 */
		inline bool hasGlyphAt( char32_t p_char )const
		{
			return m_loadedGlyphs.end() != std::find_if( m_loadedGlyphs.begin()
				, m_loadedGlyphs.end()
				, [p_char]( Glyph const & p_glyph )
				{
					return p_glyph.getCharacter() == p_char;
				} );
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
		inline Glyph const & getGlyphAt( char32_t p_char )const
		{
			auto it = std::find_if( m_loadedGlyphs.begin()
				, m_loadedGlyphs.end()
				, [p_char]( Glyph const & p_glyph )
				{
					return p_glyph.getCharacter() == p_char;
				} );

			if ( it == m_loadedGlyphs.end() )
			{
				throw std::range_error( "Font subscript out of range" );
			}

			return *it;
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
		inline Glyph & getGlyphAt( char32_t p_char )
		{
			auto it = std::find_if( m_loadedGlyphs.begin()
				, m_loadedGlyphs.end()
				, [p_char]( Glyph const & p_glyph )
				{
					return p_glyph.getCharacter() == p_char;
				} );

			if ( it == m_loadedGlyphs.end() )
			{
				throw std::range_error( "Font subscript out of range" );
			}

			return *it;
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
		inline Glyph const & operator[]( char32_t p_char )const
		{
			ENSURE( hasGlyphAt( p_char ) );

			return *std::find_if( m_loadedGlyphs.begin()
				, m_loadedGlyphs.end()
				, [p_char]( Glyph const & p_glyph )
				{
					return p_glyph.getCharacter() == p_char;
				} );
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
		inline Glyph & operator[]( char32_t p_char )
		{
			ENSURE( hasGlyphAt( p_char ) );

			return *std::find_if( m_loadedGlyphs.begin()
				, m_loadedGlyphs.end()
				, [p_char]( Glyph const & p_glyph )
				{
					return p_glyph.getCharacter() == p_char;
				} );
		}
		/**
		 *\~english
		 *\brief		Retrieves the height of the font
		 *\return		The font height
		 *\~french
		 *\brief		Récupère la hauteur de la police
		 *\return		La hauteur de la police
		 */
		inline uint32_t getHeight()const
		{
			return m_height;
		}
		/**
		 *\~english
		 *\brief		Retrieves the max height of the glyphs
		 *\return		The glyphs max height
		 *\~french
		 *\brief		Récupère la hauteur maximale des glyphes
		 *\return		La hauteur maximale des glyphes
		 */
		inline uint32_t getMaxHeight()const
		{
			return m_maxHeight;
		}
		/**
		 *\~english
		 *\brief		Retrieves the max width of the glyphs
		 *\return		The glyphs max width
		 *\~french
		 *\brief		Récupère la largeur maximale des glyphes
		 *\return		La largeur maximale des glyphes
		 */
		inline uint32_t getMaxWidth()const
		{
			return m_maxWidth;
		}
		/**
		 *\~english
		 *\brief		sets the max height of the glyphs
		 *\param[in]	p_iHeight	The new height
		 *\~french
		 *\brief		Définit la hauteur maximale des glyphes
		 *\param[in]	p_iHeight	La nouvelle hauteur
		 */
		inline void setMaxHeight( uint32_t p_iHeight )
		{
			m_maxHeight = p_iHeight;
		}
		/**
		 *\~english
		 *\brief		sets the max width of the glyphs
		 *\param[in]	p_iWidth	The new width
		 *\~french
		 *\brief		Définit la largeur maximale des glyphes
		 *\param[in]	p_iWidth	La nouvelle largeur
		 */
		inline void setMaxWidth( uint32_t p_iWidth )
		{
			m_maxWidth = p_iWidth;
		}
		/**
		 *\~english
		 *\brief		sets the glyph loader
		 *\param[in]	p_loader	The value
		 *\~french
		 *\brief		Définit le chargeur de glyphes
		 *\param[in]	p_loader	La valeur
		 */
		inline void setGlyphLoader( std::unique_ptr< SFontImpl > && p_loader )
		{
			m_glyphLoader = std::move( p_loader );
		}
		/**
		 *\~english
		 *\brief		Tells if the font has a glyph loader
		 *\return		The status
		 *\~french
		 *\brief		Dit si la police a un loader de glyphes
		 *\return		Le statut
		 */
		inline bool hasGlyphLoader()
		{
			return m_glyphLoader != nullptr;
		}
		/**
		 *\~english
		 *\brief		Retrieves the glyph loader
		 *\return		The loader
		 *\~french
		 *\brief		Définit le loader de glyphes
		 *\return		Le loader
		 */
		inline SFontImpl & getGlyphLoader()
		{
			return *m_glyphLoader;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the first glyph
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la première glyphe
		 *\return		L'itérateur
		 */
		inline auto begin()
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
		inline auto begin()const
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
		inline auto end()
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
		inline auto end()const
		{
			return m_loadedGlyphs.end();
		}
		/**
		 *\~english
		 *\brief		sets The font face name
		 *\param[in]	p_name	The value
		 *\~french
		 *\brief		Définit le nom de la police
		 *\param[in]	p_name	La valeur
		 */
		inline void setFaceName( String const & p_name )
		{
			m_faceName = p_name;
		}
		/**
		 *\~english
		 *\brief		Retrieves The font face name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de la police
		 *\return		La valeur
		 */
		inline String const & getFaceName()const
		{
			return m_faceName;
		}
		/**
		 *\~english
		 *\brief		Retrieves The font file path.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le chemin du fichier de la police.
		 *\return		La valeur.
		 */
		inline Path const & getFilePath()const
		{
			return m_pathFile;
		}

	private:
		/**
		 *\~english
		 *\brief		Loads wanted glyph.
		 *\param[in]	p_char	The character.
		 *\return		The glyph.
		 *\~french
		 *\brief		Charge le glyphe voulu.
		 *\param[in]	p_char	Le caractère.
		 *\return		Le glyphe.
		 */
		Glyph const & doLoadGlyph( char32_t p_char );

	private:
		//!\~english The height of the font	\~french La hauteur de la police
		uint32_t m_height;
		//!\~english The path of the font file	\~french Le chemin du fichier de la police
		Path m_pathFile;
		//!\~english The array of loaded glyphs	\~french Le tableau de glyphes chargées
		GlyphArray m_loadedGlyphs;
		//!\~english The max height of the glyphs	\~french La hauteur maximale des glyphes
		int m_maxHeight;
		//!\~english The max top of the glyphs	\~french La position haute maximale des glyphes
		int m_maxTop;
		//!\~english The max width of the glyphs	\~french La largeur maximale des glyphes
		int m_maxWidth;
		//!\~english The font face name	\~french Le nom de la police
		String m_faceName;
		//!\~english The glyph loader	\~french Le chargeur de glyphes
		std::unique_ptr< SFontImpl > m_glyphLoader;
	};
}

#endif
