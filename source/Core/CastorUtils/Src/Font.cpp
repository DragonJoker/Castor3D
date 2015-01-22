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
	namespace
	{
		inline int next_p2( int p_value )
		{
			int l_iReturn = 1;

			while ( l_iReturn < p_value )
			{
				l_iReturn <<= 1;
			}

			return l_iReturn;
		}
	}

	namespace ft
	{
#define CHECK_FT_ERR( func, ... ) CheckErr( func( __VA_ARGS__ ), #func )

		inline bool CheckErr( FT_Error p_iErr, const char * p_szName )
		{
			bool l_bReturn = true;
			static std::map< FT_Error, std::string > MapErrors;

			if ( MapErrors.empty() )
			{
				MapErrors[0x0000] = "Success";						;
				MapErrors[0x0001] = "Invalid face handle"			;
				MapErrors[0x0002] = "Invalid instance handle"		;
				MapErrors[0x0003] = "Invalid glyph handle"			;
				MapErrors[0x0004] = "Invalid charmap handle"		;
				MapErrors[0x0005] = "Invalid result address"		;
				MapErrors[0x0006] = "Invalid glyph index"			;
				MapErrors[0x0007] = "Invalid argument"				;
				MapErrors[0x0008] = "Could not open file"			;
				MapErrors[0x0009] = "File is not collection"		;
				MapErrors[0x000A] = "Table missing"					;
				MapErrors[0x000B] = "Invalid horiz metrics"			;
				MapErrors[0x000C] = "Invalid charmap format"		;
				MapErrors[0x000D] = "Invalid ppem"					;
				MapErrors[0x0010] = "Invalid file format"			;
				MapErrors[0x0020] = "Invalid engine"				;
				MapErrors[0x0021] = "Too many extensions"			;
				MapErrors[0x0022] = "Extensions unsupported"		;
				MapErrors[0x0023] = "Invalid extension id"			;
				MapErrors[0x0080] = "Max profile missing"			;
				MapErrors[0x0081] = "Header table missing"			;
				MapErrors[0x0082] = "Horiz header missing"			;
				MapErrors[0x0083] = "Locations missing"				;
				MapErrors[0x0084] = "Name table missing"			;
				MapErrors[0x0085] = "CMap table missing"			;
				MapErrors[0x0086] = "Hmtx table missing"			;
				MapErrors[0x0087] = "OS2 table missing"				;
				MapErrors[0x0088] = "Post table missing"			;
				MapErrors[0x0100] = "Out of memory"					;
				MapErrors[0x0200] = "Invalid file offset"			;
				MapErrors[0x0201] = "Invalid file read"				;
				MapErrors[0x0202] = "Invalid frame access"			;
				MapErrors[0x0300] = "Too many points"				;
				MapErrors[0x0301] = "Too many contours"				;
				MapErrors[0x0302] = "Invalid composite glyph"		;
				MapErrors[0x0303] = "Too many instructions"			;
				MapErrors[0x0400] = "Invalid opcode"				;
				MapErrors[0x0401] = "Too few arguments"				;
				MapErrors[0x0402] = "Stack overflow"				;
				MapErrors[0x0403] = "Code overflow"					;
				MapErrors[0x0404] = "Bad argument"					;
				MapErrors[0x0405] = "Divide by zero"				;
				MapErrors[0x0406] = "Storage overflow"				;
				MapErrors[0x0407] = "Cvt overflow"					;
				MapErrors[0x0408] = "Invalid reference"				;
				MapErrors[0x0409] = "Invalid distance"				;
				MapErrors[0x040A] = "Interpolate twilight"			;
				MapErrors[0x040B] = "Debug opcode"					;
				MapErrors[0x040C] = "ENDF in exec stream"			;
				MapErrors[0x040D] = "Out of coderanges"				;
				MapErrors[0x040E] = "Nested defs"					;
				MapErrors[0x040F] = "Invalid coderange"				;
				MapErrors[0x0410] = "Invalid displacement"			;
				MapErrors[0x0411] = "Execution too long"			;
				MapErrors[0x0500] = "Nested frame access"			;
				MapErrors[0x0501] = "Invalid cache list"			;
				MapErrors[0x0502] = "Could not find context"		;
				MapErrors[0x0503] = "Unlisted object"				;
				MapErrors[0x0600] = "Raster pool overflow"			;
				MapErrors[0x0601] = "Raster negative height"		;
				MapErrors[0x0602] = "Raster invalid value"			;
				MapErrors[0x0603] = "Raster not initialized"		;
				MapErrors[0x0A00] = "Invalid kerning table format"	;
				MapErrors[0x0A01] = "Invalid kerning table"			;
				MapErrors[0x0B00] = "Invalid post table format"		;
				MapErrors[0x0B01] = "Invalid post table"			;
			};

			if ( p_iErr != 0 )
			{
				std::map< FT_Error, std::string >::const_iterator l_it = MapErrors.find( p_iErr );
				std::string l_strError = "ERROR : " + std::string( p_szName ) + " failed - " + str_utils::to_str( str_utils::to_string( p_iErr ) );

				if ( l_it != MapErrors.end() )
				{
					l_strError += " (" + l_it->second + ")";
				}

				LOADER_ERROR( l_strError );
				l_bReturn = false;
			}

			return l_bReturn;
		}

		struct SFreeTypeFontImpl
				: public Font::SFontImpl
		{
			SFreeTypeFontImpl( Path const & p_pathFile, uint32_t p_height )
				: m_height( p_height )
				, m_path( p_pathFile )
				, m_library( NULL )
				, m_face( NULL )
			{
			}

			virtual ~SFreeTypeFontImpl()
			{
			}

			virtual void Initialise()
			{
				CHECK_FT_ERR( FT_Init_FreeType, &m_library );
				CHECK_FT_ERR( FT_New_Face, m_library, str_utils::to_str( m_path ).c_str(), 0, &m_face );
				CHECK_FT_ERR( FT_Select_Charmap, m_face, FT_ENCODING_UNICODE );
				CHECK_FT_ERR( FT_Set_Pixel_Sizes, m_face, 0, m_height );
			}

			virtual void Cleanup()
			{
				CHECK_FT_ERR( FT_Done_Face, m_face );
				CHECK_FT_ERR( FT_Done_FreeType, m_library );
				m_library = NULL;
				m_face = NULL;
			}

			virtual int LoadGlyph( Glyph & p_glyph )
			{
				FT_Glyph l_ftGlyph;
				CHECK_FT_ERR( FT_Load_Glyph, m_face, FT_Get_Char_Index( m_face, p_glyph.GetCharacter() ), FT_LOAD_DEFAULT );
				CHECK_FT_ERR( FT_Get_Glyph, m_face->glyph, &l_ftGlyph );
				CHECK_FT_ERR( FT_Glyph_To_Bitmap, &l_ftGlyph, FT_RENDER_MODE_NORMAL, 0, 1 );
				FT_BitmapGlyph l_ftBmpGlyph = FT_BitmapGlyph( l_ftGlyph );
				FT_Bitmap & l_ftBitmap = l_ftBmpGlyph->bitmap;
				Size l_advance( uint32_t( std::abs( l_ftGlyph->advance.x ) / 65536.0 ), uint32_t( l_ftGlyph->advance.y  / 65536.0 ) );
				uint32_t l_pitch = std::abs( l_ftBitmap.pitch );
				Size l_size( l_pitch, l_ftBitmap.rows );
				Position l_ptPosition( l_ftBmpGlyph->left, l_ftBmpGlyph->top );
				uint32_t l_uiSize = l_size.width() * l_size.height();
				ByteArray l_bitmap( l_uiSize, 0 );
				uint32_t l_uiIndex = 0;
				CASTOR_ASSERT( l_uiSize == l_size.width() * l_size.height() );

				if ( l_ftBitmap.pitch < 0 )
				{
					uint8_t * l_dst = l_bitmap.data();
					uint8_t const * l_src = l_ftBitmap.buffer;

					for ( uint32_t i = 0; i < uint32_t( l_ftBitmap.rows ); i++ )
					{
						memcpy( l_dst, l_src, l_ftBitmap.width );
						l_src += l_pitch;
						l_dst += l_pitch;
					}
				}
				else
				{
					uint8_t * l_dst = l_bitmap.data() + l_uiSize - l_pitch;
					uint8_t const * l_src = l_ftBitmap.buffer;
					
					for ( uint32_t i = 0; i < uint32_t( l_ftBitmap.rows ); i++ )
					{
						memcpy( l_dst, l_src, l_ftBitmap.width );
						l_src += l_pitch;
						l_dst -= l_pitch;
					}
				}

				p_glyph.SetSize( l_size );
				p_glyph.SetPosition( l_ptPosition );
				p_glyph.SetAdvance( l_advance );
				p_glyph.SetBitmap( l_bitmap );
				return l_ftBmpGlyph->top;
			}

			Path m_path;
			uint32_t m_height;
			FT_Library m_library;
			FT_Face m_face;
		};

		void LoadGlyph( wchar_t p_char, Font & p_font, int & p_iMaxTop )
		{
			p_iMaxTop = std::max( p_iMaxTop, p_font.LoadGlyph( p_char ) );
		}
	}

	Font::BinaryLoader::BinaryLoader()
		:	Loader< Font, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
	{
	}

	bool Font::BinaryLoader::operator()( Font & p_font, Path const & p_pathFile, uint32_t p_uiHeight )
	{
		m_uiHeight = p_uiHeight;
		return operator()( p_font, p_pathFile );
	}

	bool Font::BinaryLoader::operator()( Font & p_font, Path const & p_pathFile )
	{
		bool l_bReturn = false;

		if ( ! p_pathFile.empty() )
		{
			String l_strFontName = p_pathFile.GetFullFileName();

			try
			{
				std::unique_ptr< ft::SFreeTypeFontImpl > l_loader = std::make_unique< ft::SFreeTypeFontImpl >( p_pathFile, m_uiHeight );
				p_font.SetGlyphLoader( std::unique_ptr< SFontImpl >( static_cast< Font::SFontImpl *>( l_loader.release() ) ) );
				p_font.GetGlyphLoader()->Initialise();
				uint16_t l_usMin = std::numeric_limits< uint8_t >::lowest();
				uint16_t l_usMax = std::numeric_limits< uint8_t >::max();
				int l_iMaxHeight = 0;
				int l_iMaxWidth = 0;
				int l_iMaxTop = 0;

				// We first load the glyphs, updating the top position
				for ( wchar_t c = l_usMin; c < l_usMax; c++ )
				{
					ft::LoadGlyph( c, p_font, l_iMaxTop );
					Glyph & l_glyph = p_font[c];
					Size l_size( l_glyph.GetSize() );
					Position l_ptPosition( l_glyph.GetPosition() );
					l_iMaxHeight = std::max< int >( l_iMaxHeight, l_size.height() );
					l_iMaxWidth = std::max< int >( l_iMaxWidth, l_size.width() );
				}

				p_font.GetGlyphLoader()->Cleanup();
				p_font.SetMaxHeight( l_iMaxHeight );
				p_font.SetMaxWidth( l_iMaxWidth );
				l_bReturn = true;
			}
			catch ( std::runtime_error & p_exc )
			{
				LOADER_ERROR( "Font loading failed : " + std::string( p_exc.what() ) );
			}
		}

		return l_bReturn;
	}

	//*********************************************************************************************

	Font::Font( String const & p_strName, uint32_t p_uiHeight )
		:	Resource< Font >( p_strName )
		,	m_uiHeight( p_uiHeight )
		,	m_iMaxHeight( 0 )
		,	m_iMaxTop( 0 )
		,	m_iMaxWidth( 0 )
		,	m_glyphs( std::numeric_limits< uint16_t >::max() )
	{
	}

	Font::Font( Path const & p_path, String const & p_strName, uint32_t p_uiHeight )
		:	Resource< Font >( p_strName )
		,	m_uiHeight( p_uiHeight )
		,	m_iMaxHeight( 0 )
		,	m_iMaxTop( 0 )
		,	m_iMaxWidth( 0 )
		,	m_glyphs( std::numeric_limits< uint16_t >::max() )
	{
		Font::BinaryLoader()( *this, p_path, m_uiHeight );
	}

	Font::~Font()
	{
	}

	void Font::SetGlyphAt( wchar_t p_char, Size const & p_size, Position p_position, Size const & p_advance, ByteArray const & p_bitmap )
	{
		m_glyphs[p_char] = Glyph( p_char, p_size, p_position, p_advance, p_bitmap );
	}

	int Font::LoadGlyph( wchar_t p_char )
	{
		Glyph & l_glyph = m_glyphs[p_char];
		l_glyph = Glyph( p_char );
		int l_top = m_glyphLoader->LoadGlyph( l_glyph );
		m_loadedGlyphs[p_char] = &l_glyph;
		return l_top;
	}

	BEGIN_INVARIANT_BLOCK( Font )
	CHECK_INVARIANT( m_uiHeight != 0 );
	END_INVARIANT_BLOCK()
}
