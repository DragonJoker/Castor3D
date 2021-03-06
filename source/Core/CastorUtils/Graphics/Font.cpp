#include "CastorUtils/Graphics/Font.hpp"

#include <ft2build.h>

FT_BEGIN_HEADER
#	include FT_FREETYPE_H
#	include FT_GLYPH_H
FT_END_HEADER

namespace castor
{
	namespace ft
	{
#define CHECK_FT_ERR( func, ... ) CheckErr( func( __VA_ARGS__ ), #func )

		inline bool CheckErr( FT_Error p_error, const char * p_name )
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

			bool result = true;

			if ( p_error != 0 )
			{
				std::map< FT_Error, std::string >::const_iterator it = MapErrors.find( p_error );
				std::string error = "ERROR : " + std::string( p_name ) + " failed - " + string::stringCast< char >( string::toString( p_error ) );

				if ( it != MapErrors.end() )
				{
					error += " (" + it->second + ")";
				}

				CU_LoaderError( error );
				result = false;
			}

			return result;
		}

		struct SFreeTypeFontImpl
			: public Font::SFontImpl
		{
			SFreeTypeFontImpl( Path const & p_pathFile, uint32_t p_height )
				: m_height{ p_height }
				, m_path{ p_pathFile }
			{
			}

			virtual ~SFreeTypeFontImpl()
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

			Glyph loadGlyph( char32_t p_c32 )override
			{
				FT_Glyph glyph{};
				FT_ULong const cl( p_c32 );
				FT_UInt const index{ FT_Get_Char_Index( m_face, cl ) };
				CHECK_FT_ERR( FT_Load_Glyph, m_face, index, FT_LOAD_DEFAULT );
				CHECK_FT_ERR( FT_Get_Glyph, m_face->glyph, &glyph );
				CHECK_FT_ERR( FT_Glyph_To_Bitmap, &glyph, FT_RENDER_MODE_NORMAL, 0, 1 );
				FT_BitmapGlyph const bmpGlyph = FT_BitmapGlyph( glyph );
				FT_Bitmap const & bitmap = bmpGlyph->bitmap;
				uint32_t const pitch( std::abs( bitmap.pitch ) );
				Size const size{ pitch, uint32_t( bitmap.rows ) };
				Position const bearing{ bmpGlyph->left, bmpGlyph->top };
				ByteArray buffer( size.getWidth() * size.getHeight() );
				uint32_t advance{ uint32_t( std::abs( glyph->advance.x ) / 65536.0 ) };

				if ( advance < size[0] )
				{
					advance = size[0] + bearing[0];
				}

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
				return Glyph{ p_c32, size, bearing, advance, buffer };
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

	bool Font::BinaryLoader::operator()( Font & p_font, Path const & p_pathFile, uint32_t p_height )
	{
		m_height = p_height;
		return operator()( p_font, p_pathFile );
	}

	bool Font::BinaryLoader::operator()( Font & p_font, Path const & p_pathFile )
	{
		bool result = false;

		if ( ! p_pathFile.empty() )
		{
			String strFontName = p_pathFile.getFullFileName();

			try
			{
				if ( !p_font.hasGlyphLoader() )
				{
					p_font.setGlyphLoader( std::make_unique< ft::SFreeTypeFontImpl >( p_pathFile, m_height ) );
				}

				p_font.setFaceName( p_pathFile.getFileName() );
				p_font.getGlyphLoader().initialise();
				uint8_t const min = std::numeric_limits< uint8_t >::lowest();
				uint8_t const max = std::numeric_limits< uint8_t >::max();
				uint32_t maxHeight = 0;
				uint32_t maxWidth = 0;

				// We load the glyphs
				for ( uint8_t c = min; c < max; c++ )
				{
					char tmp[] { char( c ), 0, 0, 0 };
					Glyph const & glyph = p_font.doLoadGlyph( string::utf8::toUtf8( tmp ) );
					maxHeight = std::max< int >( maxHeight, glyph.getSize().getHeight() );
					maxWidth = std::max< int >( maxWidth, glyph.getAdvance() );
				}

				p_font.getGlyphLoader().cleanup();
				p_font.setMaxHeight( maxHeight );
				p_font.setMaxWidth( maxWidth );
				result = true;
			}
			catch ( std::runtime_error & p_exc )
			{
				CU_LoaderError( "Font loading failed : " + std::string( p_exc.what() ) );
			}
		}

		return result;
	}

	//*********************************************************************************************

	Font::Font( String const & p_name, uint32_t p_height )
		: Resource< Font >( p_name )
		, m_height( p_height )
		, m_maxHeight( 0 )
		, m_maxTop( 0 )
		, m_maxWidth( 0 )
	{
	}

	Font::Font( String const & p_name, uint32_t p_height, Path const & p_path )
		: Resource< Font >( p_name )
		, m_height( p_height )
		, m_maxHeight( 0 )
		, m_maxTop( 0 )
		, m_maxWidth( 0 )
		, m_glyphLoader( std::make_unique< ft::SFreeTypeFontImpl >( p_path, p_height ) )
		, m_pathFile( p_path )
	{
		BinaryLoader()( *this, p_path, p_height );
	}

	Font::~Font()
	{
	}

	void Font::loadGlyph( char32_t p_char )
	{
		m_glyphLoader->initialise();
		doLoadGlyph( p_char );
		m_glyphLoader->cleanup();
	}

	Glyph const & Font::doLoadGlyph( char32_t p_char )
	{
		auto it = std::find_if( m_loadedGlyphs.begin(), m_loadedGlyphs.end(), [p_char]( Glyph const & p_glyph )
		{
			return p_glyph.getCharacter() == p_char;
		} );

		if ( it == m_loadedGlyphs.end() )
		{
			m_loadedGlyphs.push_back( m_glyphLoader->loadGlyph( p_char ) );

			it = std::find_if( m_loadedGlyphs.begin(), m_loadedGlyphs.end(), [p_char]( Glyph const & p_glyph )
			{
				return p_glyph.getCharacter() == p_char;
			} );
		}

		return *it;
	}
}
