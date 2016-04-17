#include "Font.hpp"
#include "Image.hpp"

#include <ft2build.h>

FT_BEGIN_HEADER
#	include FT_FREETYPE_H
#	include FT_GLYPH_H
#	include FT_OUTLINE_H
#	include FT_TRIGONOMETRY_H
#	include FT_TYPES_H
FT_END_HEADER

namespace Castor
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

			bool l_return = true;

			if ( p_error != 0 )
			{
				std::map< FT_Error, std::string >::const_iterator l_it = MapErrors.find( p_error );
				std::string l_error = "ERROR : " + std::string( p_name ) + " failed - " + string::string_cast< char >( string::to_string( p_error ) );

				if ( l_it != MapErrors.end() )
				{
					l_error += " (" + l_it->second + ")";
				}

				LOADER_ERROR( l_error );
				l_return = false;
			}

			return l_return;
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

			virtual void Initialise()
			{
				CHECK_FT_ERR( FT_Init_FreeType, &m_library );
				CHECK_FT_ERR( FT_New_Face, m_library, string::string_cast< char >( m_path ).c_str(), 0, &m_face );
				CHECK_FT_ERR( FT_Select_Charmap, m_face, FT_ENCODING_UNICODE );
				CHECK_FT_ERR( FT_Set_Pixel_Sizes, m_face, 0, m_height );
			}

			virtual void Cleanup()
			{
				CHECK_FT_ERR( FT_Done_Face, m_face );
				CHECK_FT_ERR( FT_Done_FreeType, m_library );
				m_library = nullptr;
				m_face = nullptr;
			}

			virtual Glyph LoadGlyph( char32_t p_char )
			{
				FT_Glyph l_glyph{};
				FT_ULong const l_char( p_char );
				FT_UInt const l_index{ FT_Get_Char_Index( m_face, l_char ) };
				CHECK_FT_ERR( FT_Load_Glyph, m_face, l_index, FT_LOAD_DEFAULT );
				CHECK_FT_ERR( FT_Get_Glyph, m_face->glyph, &l_glyph );
				CHECK_FT_ERR( FT_Glyph_To_Bitmap, &l_glyph, FT_RENDER_MODE_NORMAL, 0, 1 );
				FT_BitmapGlyph const l_bmpGlyph = FT_BitmapGlyph( l_glyph );
				FT_Bitmap const & l_bitmap = l_bmpGlyph->bitmap;
				uint32_t const l_pitch( std::abs( l_bitmap.pitch ) );
				Size const l_size{ l_pitch, uint32_t( l_bitmap.rows ) };
				Position const l_bearing{ l_bmpGlyph->left, l_bmpGlyph->top };
				ByteArray l_buffer( l_size.width() * l_size.height() );
				uint32_t l_advance{ uint32_t( std::abs( l_glyph->advance.x ) / 65536.0 ) };

				if ( l_advance < l_size[0] )
				{
					l_advance = l_size[0] + l_bearing[0];
				}

				if ( l_bitmap.pitch < 0 )
				{
					uint8_t * l_dst = l_buffer.data();
					uint8_t const * l_src = l_bitmap.buffer;

					for ( uint32_t i = 0; i < uint32_t( l_bitmap.rows ); i++ )
					{
						memcpy( l_dst, l_src, l_bitmap.width );
						l_src += l_pitch;
						l_dst += l_pitch;
					}
				}
				else
				{
					uint8_t * l_dst = l_buffer.data() + l_buffer.size() - l_pitch;
					uint8_t const * l_src = l_bitmap.buffer;

					for ( uint32_t i = 0; i < uint32_t( l_bitmap.rows ); i++ )
					{
						memcpy( l_dst, l_src, l_bitmap.width );
						l_src += l_pitch;
						l_dst -= l_pitch;
					}
				}

				return Glyph{ p_char, l_size, l_bearing, l_advance, l_buffer };
			}

		private:
			Path const m_path;
			uint32_t const m_height;
			FT_Library m_library{};
			FT_Face m_face{};
		};
	}

	Font::BinaryLoader::BinaryLoader()
		: Loader< Font, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
	{
	}

	bool Font::BinaryLoader::operator()( Font & p_font, Path const & p_pathFile, uint32_t p_height )
	{
		m_height = p_height;
		return operator()( p_font, p_pathFile );
	}

	bool Font::BinaryLoader::operator()( Font & p_font, Path const & p_pathFile )
	{
		bool l_return = false;

		if ( ! p_pathFile.empty() )
		{
			String l_strFontName = p_pathFile.GetFullFileName();

			try
			{
				if ( !p_font.HasGlyphLoader() )
				{
					p_font.SetGlyphLoader( std::make_unique< ft::SFreeTypeFontImpl >( p_pathFile, m_height ) );
				}

				p_font.SetFaceName( p_pathFile.GetFileName() );
				p_font.GetGlyphLoader().Initialise();
				uint8_t const l_min = std::numeric_limits< uint8_t >::lowest();
				uint8_t const l_max = std::numeric_limits< uint8_t >::max();
				uint32_t l_maxHeight = 0;
				uint32_t l_maxWidth = 0;

				// We load the glyphs
				for ( uint8_t c = l_min; c < l_max; c++ )
				{
					char l_tmp[] { char( c ), 0, 0, 0 };
					Glyph const & l_glyph = p_font.DoLoadGlyph( string::utf8::to_utf8( l_tmp ) );
					l_maxHeight = std::max< int >( l_maxHeight, l_glyph.GetSize().height() );
					l_maxWidth = std::max< int >( l_maxWidth, l_glyph.GetAdvance() );
				}

				p_font.GetGlyphLoader().Cleanup();
				p_font.SetMaxHeight( l_maxHeight );
				p_font.SetMaxWidth( l_maxWidth );
				l_return = true;
			}
			catch ( std::runtime_error & p_exc )
			{
				LOADER_ERROR( "Font loading failed : " + std::string( p_exc.what() ) );
			}
		}

		return l_return;
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

	Font::Font( Path const & p_path, String const & p_name, uint32_t p_height )
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

	void Font::LoadGlyph( char32_t p_char )
	{
		m_glyphLoader->Initialise();
		DoLoadGlyph( p_char );
		m_glyphLoader->Cleanup();
	}

	Glyph const & Font::DoLoadGlyph( char32_t p_char )
	{
		auto && l_it = std::find_if( m_loadedGlyphs.begin(), m_loadedGlyphs.end(), [p_char]( Glyph const & p_glyph )
		{
			return p_glyph.GetCharacter() == p_char;
		} );

		if ( l_it == m_loadedGlyphs.end() )
		{
			m_loadedGlyphs.push_back( m_glyphLoader->LoadGlyph( p_char ) );

			l_it = std::find_if( m_loadedGlyphs.begin(), m_loadedGlyphs.end(), [p_char]( Glyph const & p_glyph )
			{
				return p_glyph.GetCharacter() == p_char;
			} );
		}

		return *l_it;
	}
}
