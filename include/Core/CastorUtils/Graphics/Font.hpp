/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_FONT_H___
#define ___CASTOR_FONT_H___

#include "CastorUtils/Data/BinaryLoader.hpp"
#include "CastorUtils/Data/TextWriter.hpp"
#include "CastorUtils/Design/Named.hpp"
#include "CastorUtils/Graphics/Glyph.hpp"
#include "CastorUtils/Math/Point.hpp"

namespace castor
{
	class Font
		: public Named
	{
	public:
		CU_DeclareVector( Glyph, Glyph );
		using GlyphKerning = Map< char32_t, float >;
		using GlyphKerningMap = Map< char32_t, GlyphKerning >;
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
			CU_API BinaryLoader() = default;
			/**
			 *\~english
			 *\brief			Loads a non SDF font
			 *\param[in,out]	font	The font to load
			 *\param[in]		path	The path of the font file
			 *\param[in]		height	The font precision
			 *\~french
			 *\brief			Charge une police non SDF
			 *\param[in,out]	font	La police à charger
			 *\param[in]		path	Le chemin du fichier contenant la police
			 *\param[in]		height	La précision de la police
			 */
			CU_API bool operator()( Font & font, Path const & path, uint32_t height );
			/**
			 *\~english
			 *\brief			Loads an SDF font.
			 *\param[in,out]	font	The font to load.
			 *\param[in]		path	The path of the font file.
			 *\~french
			 *\brief			Charge une police SDF.
			 *\param[in,out]	font	La police à charger.
			 *\param[in]		path	Le chemin du fichier contenant la police.
			 */
			CU_API bool operator()( Font & font, Path const & path );

		private:
			using castor::BinaryLoader< Font >::operator();
			bool doLoad( Font & font, Path const & path );

		private:
			//!\~english	Font wanted height.
			//!\~french		Hauteur voulue pour la police
			uint32_t m_height{};
		};
		friend class BinaryLoader;

		struct SdfInfo
		{
			uint32_t emSize{};
			float pixelRange{};
			float verticalAdvance{};
		};
		/**
		\~english
		\brief		Structure used to load glyphs on demand
		\remark		PImpl to hide FreeType inclusions
		\~french
		\brief		Structure utilisée pour charger des glyphes à la demande
		\remark		PImpl pour cacher les inclusions de FreeType
		*/
		struct GlyphLoader
			: public NonMovable
		{
			virtual ~GlyphLoader()noexcept = default;
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
			/**
			 *\~english
			 *\brief			Completes the given kerning table for given glyph.
			 *\param[in]		c		The glyph character.
			 *\param[in]		glyphs	The currently loaded glyphs.
			 *\param[in,out]	table	The table to fill.
			 *\~french
			 *\brief			Complète la table de kerning pour la glyphe donnée.
			 *\param[in]		c		Le caractère de la glyphe.
			 *\param[in]		glyphs	Les glyphes actuellement chargées.
			 *\param[in,out]	table	La table à remplir.
			 */
			virtual void fillKerningTable( char32_t c
				, GlyphArray const & glyphs
				, GlyphKerningMap & table ) = 0;
		};

	public:
		/**
		 *\~english
		 *\brief		Non SDF font constructor.
		 *\param[in]	name	The font name.
		 *\param[in]	height	The font char height.
		 *\~french
		 *\brief		Constructeur de police non SDF.
		 *\param[in]	name	Le nom de la police.
		 *\param[in]	height	La hauteur des caractères de la police.
		 */
		CU_API Font( String const & name, uint32_t height );
		/**
		 *\~english
		 *\brief		Non SDF font constructor.
		 *\param[in]	path	The font file path.
		 *\param[in]	name	The font name.
		 *\param[in]	height	The font char height.
		 *\~french
		 *\brief		Constructeur de police non SDF.
		 *\param[in]	path	Le chemin d'accès au fichier contenant la police.
		 *\param[in]	name	Le nom de la police.
		 *\param[in]	height	La hauteur des caractères de la police.
		 */
		CU_API Font( String const & name, uint32_t height, Path const & path );
		/**
		 *\~english
		 *\brief		SDF font constructor.
		 *\param[in]	name	The font name.
		 *\~french
		 *\brief		Constructeur de police SDF.
		 *\param[in]	name	Le nom de la police.
		 */
		CU_API explicit Font( String const & name );
		/**
		 *\~english
		 *\brief		SDF font constructor.
		 *\param[in]	path	The font file path.
		 *\param[in]	name	The font name.
		 *\~french
		 *\brief		Constructeur de police SDF.
		 *\param[in]	path	Le chemin d'accès au fichier contenant la police.
		 *\param[in]	name	Le nom de la police.
		 */
		CU_API Font( String const & name, Path const & path );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\remarks		Virtual to enable use as ResourceT.
		 *\~french
		 *\brief		Destructeur.
		 *\remarks		Virtual pour permettre l'utilisation en tant que ResourceT.
		 */
		CU_API virtual ~Font()noexcept = default;
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
		 *\brief		Retrieves the metrics of given text.
		 *\param[in]	v			The text.
		 *\param[in]	maxWidth	The maximum width if \p splitLines is \p true.
		 *\param[in]	splitLines	\p true to split lines according to \p maxWidth.
		 *\param[in]	height		The font height considered for computing.
		 *\~french
		 *\brief		Récupère les métriques du texte donné.
		 *\param[in]	v			Le texte.
		 *\param[in]	maxWidth	La largeur maximale, si \p splitLines vaut \p true.
		 *\param[in]	splitLines	\p true pour découper les lignes en fonction de \p maxWidth.
		 *\param[in]	height		La hauteur de police utilisée lors du calcul.
		 */
		CU_API TextMetrics getTextMetrics( std::u32string const & v
			, uint32_t maxWidth
			, bool splitLines
			, uint32_t height )const;
		/**
		 *\~english
		 *\brief		Retrieves the metrics of given text.
		 *\remarks		Uses the font height defined at construction.
		 *\param[in]	v			The text.
		 *\param[in]	maxWidth	The maximum width if \p splitLines is \p true.
		 *\param[in]	splitLines	\p true to split lines according to \p maxWidth.
		 *\~french
		 *\brief		Récupère les métriques du texte donné.
		 *\remarks		Utilise la hauteur définie lors de la construction de la police.
		 *\param[in]	v			Le texte.
		 *\param[in]	maxWidth	La largeur maximale, si \p splitLines vaut \p true.
		 *\param[in]	splitLines	\p true pour découper les lignes en fonction de \p maxWidth.
		 */
		CU_API TextMetrics getTextMetrics( std::u32string const & v
			, uint32_t maxWidth
			, bool splitLines )const;
		/**
		 *\~english
		 *\brief		Retrieves the kerning between two characters.
		 *\param[in]	lhs, rhs	The characters.
		 *\param[in]	height		The font height considered for computing.
		 *\~french
		 *\brief		Récupère le kerning entre deux caractères.
		 *\param[in]	lhs, rhs	Les caractères.
		 *\param[in]	height		La hauteur de police utilisée lors du calcul.
		 */
		CU_API float getKerning( char32_t lhs, char32_t rhs, uint32_t height )const;
		/**
		 *\~english
		 *\brief		Retrieves the kerning between two characters.
		 *\remarks		Uses the font height defined at construction.
		 *\param[in]	lhs, rhs	The characters.
		 *\param[in]	height		The font height considered for computing.
		 *\~french
		 *\brief		Récupère le kerning entre deux caractères.
		 *\remarks		Utilise la hauteur définie lors de la construction de la police.
		 *\param[in]	lhs, rhs	Les caractères.
		 *\param[in]	height		La hauteur de police utilisée lors du calcul.
		 */
		CU_API float getKerning( char32_t lhs, char32_t rhs )const;
		/**
		 *\~english
		 *\param[in]	c	The character.
		 *\return		\p true if the font already has loaded the wanted glyph.
		 *\~french
		 *\param[in]	c	Le caractère.
		 *\return		\p true si la police a déjà chargé la glyphe voulue.
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
		 *\param[in]	c	The wanted character.
		 *\return		The glyph of wanted character.
		 *\~french
		 *\param[in]	c	Le caractère voulu.
		 *\return		La glyphe du caractère voulu.
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
		 *\param[in]	c	The wanted character.
		 *\return		The glyph of wanted character.
		 *\~french
		 *\param[in]	c	Le caractère voulu.
		 *\return		La glyphe du caractère voulu.
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
		 *\name		Array access.
		 */
		/*@{*/
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
		/*@}*/
		/**
		 *\name		Iteration.
		 */
		/*@{*/
		auto begin()noexcept
		{
			return m_loadedGlyphs.begin();
		}

		auto begin()const noexcept
		{
			return m_loadedGlyphs.begin();
		}

		auto end()noexcept
		{
			return m_loadedGlyphs.end();
		}

		auto end()const noexcept
		{
			return m_loadedGlyphs.end();
		}
		/*@}*/
		/**
		 *\name		Getters.
		 */
		/*@{*/
		uint32_t getHeight()const noexcept
		{
			CU_Require( !isSDF() );
			return m_height;
		}

		uint32_t getMaxGlyphHeight()const noexcept
		{
			return uint32_t( m_maxSize->x );
		}

		uint32_t getMaxGlyphWidth()const noexcept
		{
			return uint32_t( m_maxSize->x );
		}

		Point2f getMaxBearing()const noexcept
		{
			return m_maxBearing;
		}

		uint32_t getMaxImageWidth()const noexcept
		{
			return uint32_t( m_maxImageSize->x );
		}

		uint32_t getMaxImageHeight()const noexcept
		{
			return uint32_t( m_maxImageSize->y );
		}

		bool hasGlyphLoader()const noexcept
		{
			return m_glyphLoader != nullptr;
		}

		GlyphLoader & getGlyphLoader()const noexcept
		{
			CU_Require( hasGlyphLoader() );
			return *m_glyphLoader;
		}

		String const & getFaceName()const noexcept
		{
			return m_faceName;
		}

		Path const & getFilePath()const noexcept
		{
			return m_pathFile;
		}

		bool isSerialisable()const noexcept
		{
			return m_serialisable;
		}

		bool isSDF()const noexcept
		{
			return m_sdf;
		}

		float getPixelRange()const noexcept
		{
			return m_sdfInfo.pixelRange;
		}

		float getVerticalAdvance()const noexcept
		{
			return isSDF()
				? m_sdfInfo.verticalAdvance
				: float( m_height );
		}
		/*@}*/
		/**
		 *\name		Mutators.
		 */
		/*@{*/
		void setGlyphLoader( castor::RawUniquePtr< GlyphLoader > loader )noexcept
		{
			m_glyphLoader = castor::move( loader );
		}

		void setFaceName( String name )noexcept
		{
			m_faceName = castor::move( name );
		}

		void setSerialisable( bool v )noexcept
		{
			m_serialisable = v;
		}
		/*@}*/

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
		uint32_t m_height{};
		SdfInfo m_sdfInfo{};
		Path m_pathFile{};
		GlyphArray m_loadedGlyphs{};
		Point2f m_maxSize{};
		Point2i m_maxImageSize{};
		Point2f m_maxBearing{ 100, 0 };
		String m_faceName{};
		castor::RawUniquePtr< GlyphLoader > m_glyphLoader{};
		bool m_serialisable{ true };
		bool m_sdf{ false };
		GlyphKerningMap m_kerningTable;
	};
}

#endif
