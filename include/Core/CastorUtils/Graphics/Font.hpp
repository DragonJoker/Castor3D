/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_FONT_H___
#define ___CASTOR_FONT_H___

#include "CastorUtils/Data/BinaryLoader.hpp"
#include "CastorUtils/Data/TextWriter.hpp"
#include "CastorUtils/Design/Collection.hpp"
#include "CastorUtils/Design/Named.hpp"
#include "CastorUtils/Graphics/Glyph.hpp"
#include "CastorUtils/Math/Point.hpp"

namespace castor
{
	class Font
		: public Named
	{
	public:
		/**
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
			 *\param[in,out]	font	The font to load
			 *\param[in]		path	The path of the font file
			 *\param[in]		height	The font precision
			 *\~french
			 *\brief			Charge une police
			 *\param[in,out]	font	La police à charger
			 *\param[in]		path	Le chemin du fichier contenant la police
			 *\param[in]		height	La précision de la police
			 */
			CU_API bool operator()( Font & font, Path const & path, uint32_t height );

		private:
			using castor::BinaryLoader< Font >::operator();
			bool doLoad( Font & font, Path const & path );

		private:
			//!\~english	Font wanted height.
			//!\~french		Hauteur voulue pour la police
			uint32_t m_height;
		};
		/**
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
			virtual ~SFontImpl() = default;
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
			 *\param[in]	c	The character.
			 *\return		The glyph.
			 *\~french
			 *\brief		Charge le glyphe voulu.
			 *\param[in]	c	Le caractère.
			 *\return		Le glyphe.
			 */
			virtual Glyph loadGlyph( char32_t c ) = 0;
		};

		CU_DeclareVector( Glyph, Glyph );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name	The font name
		 *\param[in]	height	The font char height
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name	Le nom de la police
		 *\param[in]	height	La hauteur des caractères de la police
		 */
		CU_API Font( String const & name, uint32_t height );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	path	The font file path
		 *\param[in]	name	The font name
		 *\param[in]	height	The font char height
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	path	Le chemin d'accès au fichier contenant la police
		 *\param[in]	name	Le nom de la police
		 *\param[in]	height	La hauteur des caractères de la police
		 */
		CU_API Font( String const & name, uint32_t height, Path const & path );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API virtual ~Font() = default;
		/**
		 *\~english
		 *\brief		Loads wanted glyph.
		 *\param[in]	c	The character.
		 *\~french
		 *\brief		Charge le glyphe voulu.
		 *\param[in]	c	Le caractère.
		 */
		CU_API void loadGlyph( char32_t c );
		/**
		 *\~english
		 *\brief		Loads wanted glyph.
		 *\param[in]	c	The character.
		 *\~french
		 *\brief		Charge le glyphe voulu.
		 *\param[in]	c	Le caractère.
		 */
		CU_API TextMetrics getTextMetrics( std::u32string const & v
			, uint32_t maxWidth
			, bool splitLines );
		/**
		 *\~english
		 *\brief		Tells if the font already has load ed the wanted glyph.
		 *\param[in]	c	The character.
		 *\return		false if not.
		 *\~french
		 *\brief		Dit si la police a déjà chargé la glyphe voulue.
		 *\param[in]	c	Le caractère.
		 *\return		false si elle n'est pas chargée.
		 */
		bool hasGlyphAt( char32_t c )const
		{
			return m_loadedGlyphs.end() != std::find_if( m_loadedGlyphs.begin()
				, m_loadedGlyphs.end()
				, [c]( Glyph const & glyph )
				{
					return glyph.getCharacter() == c;
				} );
		}
		/**
		 *\~english
		 *\brief		Retrieves the glyph of wanted chaaracter
		 *\param[in]	c	The wanted character
		 *\return		A constant reference on the glyph
		 *\~french
		 *\brief		Récupère la glyphe du caractère voulu
		 *\param[in]	c	Le caractère voulu
		 *\return		Une référence constante sur la glyphe
		 */
		Glyph const & getGlyphAt( char32_t c )const
		{
			auto it = std::find_if( m_loadedGlyphs.begin()
				, m_loadedGlyphs.end()
				, [c]( Glyph const & glyph )
				{
					return glyph.getCharacter() == c;
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
		 *\param[in]	c	The wanted character
		 *\return		A reference on the glyph
		 *\~french
		 *\brief		Récupère la glyphe du caractère voulu
		 *\param[in]	c	Le caractère voulu
		 *\return		Une référence sur la glyphe
		 */
		Glyph & getGlyphAt( char32_t c )
		{
			auto it = std::find_if( m_loadedGlyphs.begin()
				, m_loadedGlyphs.end()
				, [c]( Glyph const & glyph )
				{
					return glyph.getCharacter() == c;
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
		 *\param[in]	c	The wanted character
		 *\return		A reference on the glyph
		 *\~french
		 *\brief		Récupère la glyphe du caractère voulu
		 *\param[in]	c	Le caractère voulu
		 *\return		Une référence sur la glyphe
		 */
		Glyph const & operator[]( char32_t c )const
		{
			CU_Ensure( hasGlyphAt( c ) );

			return *std::find_if( m_loadedGlyphs.begin()
				, m_loadedGlyphs.end()
				, [c]( Glyph const & glyph )
				{
					return glyph.getCharacter() == c;
				} );
		}
		/**
		 *\~english
		 *\brief		Retrieves the glyph of wanted chaaracter
		 *\param[in]	c	The wanted character
		 *\return		A reference on the glyph
		 *\~french
		 *\brief		Récupère la glyphe du caractère voulu
		 *\param[in]	c	Le caractère voulu
		 *\return		Une référence sur la glyphe
		 */
		Glyph & operator[]( char32_t c )
		{
			CU_Ensure( hasGlyphAt( c ) );

			return *std::find_if( m_loadedGlyphs.begin()
				, m_loadedGlyphs.end()
				, [c]( Glyph const & glyph )
				{
					return glyph.getCharacter() == c;
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
		uint32_t getHeight()const noexcept
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
		uint32_t getMaxHeight()const noexcept
		{
			return uint32_t( m_maxSize->x );
		}
		/**
		 *\~english
		 *\brief		Retrieves the max height of the glyphs
		 *\return		The glyphs max height
		 *\~french
		 *\brief		Récupère la hauteur maximale des glyphes
		 *\return		La hauteur maximale des glyphes
		 */
		castor::Point2i getMaxRange()const noexcept
		{
			return m_maxRange;
		}
		/**
		 *\~english
		 *\brief		Retrieves the max width of the glyphs
		 *\return		The glyphs max width
		 *\~french
		 *\brief		Récupère la largeur maximale des glyphes
		 *\return		La largeur maximale des glyphes
		 */
		uint32_t getMaxWidth()const noexcept
		{
			return uint32_t( m_maxSize->x );
		}
		/**
		 *\~english
		 *\brief		sets the glyph loader
		 *\param[in]	loader	The value
		 *\~french
		 *\brief		Définit le chargeur de glyphes
		 *\param[in]	loader	La valeur
		 */
		void setGlyphLoader( std::unique_ptr< SFontImpl > loader )noexcept
		{
			m_glyphLoader = std::move( loader );
		}
		/**
		 *\~english
		 *\brief		Tells if the font has a glyph loader
		 *\return		The status
		 *\~french
		 *\brief		Dit si la police a un loader de glyphes
		 *\return		Le statut
		 */
		bool hasGlyphLoader()const noexcept
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
		SFontImpl & getGlyphLoader()const noexcept
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
		auto begin()noexcept
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
		auto begin()const noexcept
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
		auto end()noexcept
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
		auto end()const noexcept
		{
			return m_loadedGlyphs.end();
		}
		/**
		 *\~english
		 *\brief		sets The font face name
		 *\param[in]	name	The value
		 *\~french
		 *\brief		Définit le nom de la police
		 *\param[in]	name	La valeur
		 */
		void setFaceName( String name )noexcept
		{
			m_faceName = std::move( name );
		}
		/**
		 *\~english
		 *\brief		Retrieves The font face name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom de la police
		 *\return		La valeur
		 */
		String const & getFaceName()const noexcept
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
		Path const & getFilePath()const noexcept
		{
			return m_pathFile;
		}

		void initialise()
		{
		}

		void cleanup()
		{
		}

		bool isSerialisable()const noexcept
		{
			return m_serialisable;
		}

		void setSerialisable( bool v )noexcept
		{
			m_serialisable = v;
		}

	private:
		/**
		 *\~english
		 *\brief		Loads wanted glyph.
		 *\param[in]	c	The character.
		 *\return		The glyph.
		 *\~french
		 *\brief		Charge le glyphe voulu.
		 *\param[in]	c	Le caractère.
		 *\return		Le glyphe.
		 */
		Glyph const & doLoadGlyph( char32_t c );

	private:
		//!\~english	The height of the font.
		//!\~french		La hauteur de la police.
		uint32_t m_height;
		//!\~english	The path of the font file.
		//!\~french		Le chemin du fichier de la police.
		Path m_pathFile;
		//!\~english	The array of loaded glyphs.
		//!\~french		Le tableau de glyphes chargées.
		GlyphArray m_loadedGlyphs;
		//!\~english	The max size of the glyphs.
		//!\~french		La dimension maximale des glyphes.
		castor::Point2i m_maxSize;
		//!\~english	The max bearing range of the glyphs.
		//!\~french		L'intervalle maximal de bearing des glyphes.
		castor::Point2i m_maxRange;
		//!\~english	The font face name.
		//!\~french		Le nom de la police.
		String m_faceName;
		//!\~english	The glyph loader.
		//!\~french		Le chargeur de glyphes.
		std::unique_ptr< SFontImpl > m_glyphLoader;
		bool m_serialisable{ true };
	};
}

#endif
