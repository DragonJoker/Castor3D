#include "CastorUtils/Graphics/Font.hpp"

#include <ft2build.h>

FT_BEGIN_HEADER
#	include FT_FREETYPE_H
#	include FT_GLYPH_H
FT_END_HEADER

CU_ImplementCUSmartPtr( castor, Font )

namespace castor
{
	namespace ft
	{
#define CHECK_FT_ERR( func, ... ) CheckErr( func( __VA_ARGS__ ), #func )

		inline bool CheckErr( FT_Error error, const char * name )
		{
			static std::map< FT_Error, std::string > MapErrors =
			{
				{ 0x0000, "Success" },
				{ 0x0001, "Invalid face handle" },
				{ 0x0002, "Invalid instance handle" },
				{ 0x0003, "Invalid glyph handle" },
				{ 0x0004, "Invalid charmap handle" },
				{ 0x0005, "Invalid result address" },
				{ 0x0006, "Invalid glyph index" },
				{ 0x0007, "Invalid argument" },
				{ 0x0008, "Could not open file" },
				{ 0x0009, "File is not collection" },
				{ 0x000A, "Table missing" },
				{ 0x000B, "Invalid horiz metrics" },
				{ 0x000C, "Invalid charmap format" },
				{ 0x000D, "Invalid ppem" },
				{ 0x0010, "Invalid file format" },
				{ 0x0020, "Invalid engine" },
				{ 0x0021, "Too many extensions" },
				{ 0x0022, "Extensions unsupported" },
				{ 0x0023, "Invalid extension id" },
				{ 0x0080, "Max profile missing" },
				{ 0x0081, "Header table missing" },
				{ 0x0082, "Horiz header missing" },
				{ 0x0083, "Locations missing" },
				{ 0x0084, "Name table missing" },
				{ 0x0085, "CMap table missing" },
				{ 0x0086, "Hmtx table missing" },
				{ 0x0087, "OS2 table missing" },
				{ 0x0088, "Post table missing" },
				{ 0x0100, "Out of memory" },
				{ 0x0200, "Invalid file offset" },
				{ 0x0201, "Invalid file read" },
				{ 0x0202, "Invalid frame access" },
				{ 0x0300, "Too many points" },
				{ 0x0301, "Too many contours" },
				{ 0x0302, "Invalid composite glyph" },
				{ 0x0303, "Too many instructions" },
				{ 0x0400, "Invalid opcode" },
				{ 0x0401, "Too few arguments" },
				{ 0x0402, "Stack overflow" },
				{ 0x0403, "Code overflow" },
				{ 0x0404, "Bad argument" },
				{ 0x0405, "Divide by zero" },
				{ 0x0406, "Storage overflow" },
				{ 0x0407, "Cvt overflow" },
				{ 0x0408, "Invalid reference" },
				{ 0x0409, "Invalid distance" },
				{ 0x040A, "Interpolate twilight" },
				{ 0x040B, "Debug opcode" },
				{ 0x040C, "ENDF in exec stream" },
				{ 0x040D, "Out of coderanges" },
				{ 0x040E, "Nested defs" },
				{ 0x040F, "Invalid coderange" },
				{ 0x0410, "Invalid displacement" },
				{ 0x0411, "Execution too long" },
				{ 0x0500, "Nested frame access" },
				{ 0x0501, "Invalid cache list" },
				{ 0x0502, "Could not find context" },
				{ 0x0503, "Unlisted object" },
				{ 0x0600, "Raster pool overflow" },
				{ 0x0601, "Raster negative height" },
				{ 0x0602, "Raster invalid value" },
				{ 0x0603, "Raster not initialized" },
				{ 0x0A00, "Invalid kerning table format" },
				{ 0x0A01, "Invalid kerning table" },
				{ 0x0B00, "Invalid post table format" },
				{ 0x0B01, "Invalid post table" },
			};

			bool result = error == 0;

			if ( !result )
			{
				std::map< FT_Error, std::string >::const_iterator it = MapErrors.find( error );
				std::string err = "ERROR : " + std::string( name ) + " failed - " + string::stringCast< char >( string::toString( error ) );

				if ( it != MapErrors.end() )
				{
					err += " (" + it->second + ")";
				}

				CU_LoaderError( err );
			}

			return result;
		}

		struct SFreeTypeFontImpl
			: public Font::SFontImpl
		{
			SFreeTypeFontImpl( Path const & pathFile, uint32_t height )
				: m_path{ pathFile }
				, m_height{ height }
			{
			}

			~SFreeTypeFontImpl()override
			{
			}

			void initialise()override
			{
				CHECK_FT_ERR( FT_Init_FreeType, &m_library );
				CHECK_FT_ERR( FT_New_Face, m_library, string::stringCast< char >( m_path ).c_str(), 0, &m_face );
				CHECK_FT_ERR( FT_Select_Charmap, m_face, FT_ENCODING_UNICODE );
				CHECK_FT_ERR( FT_Set_Pixel_Sizes, m_face, 0, m_height );
			}

			void cleanup()override
			{
				CHECK_FT_ERR( FT_Done_Face, m_face );
				CHECK_FT_ERR( FT_Done_FreeType, m_library );
				m_library = nullptr;
				m_face = nullptr;
			}

			Glyph loadGlyph( char32_t c32 )override
			{
				FT_Glyph glyph{};
				FT_ULong const cl( c32 );
				FT_UInt const index{ FT_Get_Char_Index( m_face, cl ) };
				CHECK_FT_ERR( FT_Load_Glyph, m_face, index, FT_LOAD_DEFAULT );
				auto glyphSlot = m_face->glyph;
				CHECK_FT_ERR( FT_Render_Glyph, glyphSlot, FT_RENDER_MODE_NORMAL );
				Position const bearing{ glyphSlot->bitmap_left
					, glyphSlot->bitmap_top };
				Size const size{ uint32_t( glyphSlot->metrics.width / 64 )
					, uint32_t( glyphSlot->metrics.height / 64 ) };
				auto advance = int32_t( glyphSlot->advance.x / 64 );

				FT_Bitmap const & bitmap = glyphSlot->bitmap;
				auto const pitch( uint32_t( std::abs( bitmap.pitch ) ) );
				ByteArray buffer( size_t( bitmap.width ) * size_t( bitmap.rows ) );

				if ( bitmap.pitch < 0 )
				{
					uint8_t * dst = buffer.data();
					uint8_t const * src = bitmap.buffer;

					for ( uint32_t i = 0; i < uint32_t( bitmap.rows ); i++ )
					{
						memcpy( dst, src, bitmap.width );
						src += pitch;
						dst += pitch;
					}
				}
				else
				{
					uint8_t * dst = buffer.data() + buffer.size() - pitch;
					uint8_t const * src = bitmap.buffer;

					for ( uint32_t i = 0; i < uint32_t( bitmap.rows ); i++ )
					{
						memcpy( dst, src, bitmap.width );
						src += pitch;
						dst -= pitch;
					}
				}

				FT_Done_Glyph( glyph );
				return Glyph{ c32, size, bearing, advance, buffer };
			}

		private:
			Path const m_path;
			uint32_t const m_height;
			FT_Library m_library{};
			FT_Face m_face{};
		};
	}

	//************************************************************************************************

	Font::BinaryLoader::BinaryLoader()
	{
	}

	bool Font::BinaryLoader::operator()( Font & font
		, Path const & pathFile
		, uint32_t height )
	{
		m_height = height;
		return doLoad( font, pathFile );
	}

	bool Font::BinaryLoader::doLoad( Font & font
		, Path const & pathFile )
	{
		bool result = false;

		if ( ! pathFile.empty() )
		{
			String strFontName = pathFile.getFullFileName();

			try
			{
				if ( !font.hasGlyphLoader() )
				{
					font.setGlyphLoader( std::make_unique< ft::SFreeTypeFontImpl >( pathFile, m_height ) );
				}

				font.setFaceName( pathFile.getFileName() );
				font.getGlyphLoader().initialise();
				uint8_t const min = std::numeric_limits< uint8_t >::lowest();
				uint8_t const max = std::numeric_limits< uint8_t >::max();

				// We load the glyphs
				for ( uint8_t c = min; c < max; c++ )
				{
					char tmp[] { char( c ), 0, 0, 0 };
					font.doLoadGlyph( string::utf8::toUtf8( tmp ) );
				}

				font.getGlyphLoader().cleanup();
				result = true;
			}
			catch ( std::runtime_error & exc )
			{
				CU_LoaderError( "Font loading failed : " + std::string( exc.what() ) );
			}
		}

		return result;
	}

	//*********************************************************************************************

	Font::Font( String const & name, uint32_t height )
		: Named{ name }
		, m_height{ height }
		, m_maxSize{}
		, m_maxRange{ 100, 0 }
	{
	}

	Font::Font( String const & name, uint32_t height, Path const & path )
		: Named{ name }
		, m_height{ height }
		, m_pathFile{ path }
		, m_maxSize{}
		, m_maxRange{ 100, 0 }
		, m_glyphLoader{ std::make_unique< ft::SFreeTypeFontImpl >( path, height ) }
	{
		BinaryLoader{}( *this, path, height );
	}

	void Font::loadGlyph( char32_t c32 )
	{
		m_glyphLoader->initialise();
		doLoadGlyph( c32 );
		m_glyphLoader->cleanup();
	}

	TextMetrics Font::getTextMetrics( std::u32string const & v
		, uint32_t maxWidth
		, bool splitLines )
	{
		TextMetrics result;
		uint32_t charIndex{};
		int32_t charLeft{};
		int32_t wordLeft{};
		int32_t totalLeft{};
		TextLineMetrics word;

		auto nextLine = [&]()
		{
			if ( !word.chars.empty() )
			{
				for ( auto & c : word.chars )
				{
					c -= wordLeft;
				}
			}

			auto & line = result.lines.emplace_back();
			line.firstCharIndex = charIndex;
			line.top = result.height;
			charLeft = totalLeft - wordLeft;
			totalLeft = charLeft;
			line.width = uint32_t( totalLeft );
			wordLeft = 0;
			return &line;
		};

		auto line = nextLine();

		auto finishWord = [&]()
		{
			if ( !word.chars.empty() )
			{
				line->yMin = std::min( line->yMin, word.yMin );
				line->yMax = std::max( line->yMax, word.yMax );

				line->chars.insert( line->chars.end()
					, word.chars.begin()
					, word.chars.end() );
			}

			word = {};
			wordLeft = totalLeft;
		};

		auto finishLine = [&]()
		{
			if ( !line->chars.empty() )
			{
				result.yMin = std::min( result.yMin, line->yMin );
				result.yMax = std::max( result.yMax, line->yMax );

				result.height += line->yMax - line->yMin;
				result.width = std::max( result.width, line->width );
			}
			else
			{
				result.height += getHeight();
			}
		};

		auto addChar = [&]( castor::Size const & charSize
			, castor::Point2i const & bearing
			, int32_t advance )
		{
			auto xMin = bearing->x;
			auto xMax = xMin + int32_t( charSize->x );
			auto yMin = -bearing->y;
			auto yMax = yMin + int32_t( charSize->y );

			if ( splitLines
				&& wordLeft > 0
				&& ( wordLeft > int32_t( maxWidth )
					|| totalLeft + xMax > int32_t( maxWidth ) ) )
			{
				finishLine();
				line = nextLine();
			}

			word.yMin = std::min( word.yMin, yMin );
			word.yMax = std::max( word.yMax, yMax );
			totalLeft += advance;
			charLeft += advance;
			word.width += advance;
			line->width += advance;
		};

		for ( auto c : v )
		{
			if ( c == U'\n' )
			{
				finishWord();
				finishLine();
				line = nextLine();
				charLeft = 0;
				word.chars.push_back( 0u );
			}
			else
			{
				auto & glyph = getGlyphAt( c );

				if ( c == U' ' || c == U'\t' )
				{
					totalLeft += glyph.getAdvance();
					finishWord();
					charLeft = 0;
					line->width += glyph.getAdvance();
				}
				else
				{
					addChar( { glyph.getSize()->x, glyph.getSize()->y }
						, { glyph.getBearing().x(), glyph.getBearing().y() }
						, glyph.getAdvance() );
				}

				word.chars.push_back( uint32_t( totalLeft ) );
			}

			++charIndex;
		}

		finishWord();
		finishLine();

		if ( !result.lines.empty() )
		{
			// Adjust lines heights to maxHeight
			auto lineMin = result.yMin;
			auto lineMax = result.yMax;
			auto lineHeight = int32_t( lineMax - lineMin );
			result.height = 0;

			for ( auto & ln : result.lines )
			{
				ln.top = uint32_t( result.height );
				ln.yMin = lineMin;
				ln.yMax = lineMax;
				result.height += lineHeight;
			}
		}

		return result;
	}

	Glyph const & Font::doLoadGlyph( char32_t c )
	{
		auto it = std::find_if( m_loadedGlyphs.begin()
			, m_loadedGlyphs.end()
			, [c]( Glyph const & lookup )
		{
			return lookup.getCharacter() == c;
		} );

		if ( it == m_loadedGlyphs.end() )
		{
			m_loadedGlyphs.push_back( m_glyphLoader->loadGlyph( c ) );
			it = std::next( m_loadedGlyphs.begin()
				, ptrdiff_t( m_loadedGlyphs.size() - 1u ) );
			m_maxSize->x = std::max( m_maxSize->x, int32_t( it->getSize().getWidth() ) );
			m_maxSize->y = std::max( m_maxSize->y, int32_t( it->getSize().getHeight() ) );
			m_maxRange->x = std::min( m_maxRange->x, it->getBearing().y() );
			m_maxRange->y = std::max( m_maxRange->y, it->getBearing().y() );
		}

		return *it;
	}
}
