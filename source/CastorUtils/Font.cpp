#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Font.hpp"
#include "CastorUtils/Image.hpp"

#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include <freetype/fttypes.h>

using namespace Castor;

//*************************************************************************************************

inline int next_p2( int p_value )
{
	int l_iReturn = 1;

	while( l_iReturn < p_value )
	{
		l_iReturn <<= 1;
	}

	return l_iReturn;
}

namespace ft
{
	inline bool CheckErr( FT_Error p_iErr, const char * p_szName )
	{
		bool l_bReturn = true;

		static std::map< FT_Error, std::string > MapErrors;

		if( MapErrors.empty() )
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

		if( p_iErr != 0 )
		{
			std::map< FT_Error, std::string >::const_iterator l_it = MapErrors.find( p_iErr );
			std::string l_strError = "ERROR : " + std::string( p_szName ) + " failed - " + std::to_string( p_iErr );

			if( l_it != MapErrors.end() )
			{
				l_strError += " (" + l_it->second + ")";
			}

			LOADER_ERROR( l_strError );
			l_bReturn = false;
		}

		return l_bReturn;
	}
}

//*************************************************************************************************

Font::BinaryLoader :: BinaryLoader()
	:	Loader< Font, eFILE_TYPE_BINARY, BinaryFile >( File::eOPEN_MODE_DUMMY )
{
}

bool Font::BinaryLoader :: operator ()( Font & p_font, Path const & p_pathFile, uint32_t p_uiHeight )
{
	m_uiHeight = p_uiHeight;
	return operator()( p_font, p_pathFile );
}

bool Font::BinaryLoader :: operator ()( Font & p_font, Path const & p_pathFile )
{
	bool l_bReturn = false;

	if( ! p_pathFile.empty() )
	{
		String l_strFontName = p_pathFile.GetFullFileName();

		try
		{
			FT_Library l_ftLibrary;
			FT_Face l_ftFace;

			ft::CheckErr( FT_Init_FreeType( &l_ftLibrary ), "FT_Init_FreeType" );
			ft::CheckErr( FT_New_Face( l_ftLibrary, str_utils::to_str( p_pathFile ).c_str(), 0, &l_ftFace ), "FT_New_Face" );
			ft::CheckErr( FT_Set_Pixel_Sizes( l_ftFace, 0, m_uiHeight ), "FT_Set_Pixel_Sizes" );

			uint16_t l_usMin = std::numeric_limits< uint16_t >::lowest();
			uint16_t l_usMax = std::numeric_limits< uint16_t >::max();
			int l_iMaxHeight = 0;
			int l_iMaxWidth = 0;
			int l_iMaxTop = 0;

			// We first load the glyphs, updating the top position
			for( wchar_t c = l_usMin; c < l_usMax; c++ )
			{
				DoLoadGlyph( l_ftFace, c, p_font, l_iMaxTop );
				Glyph & l_glyph = p_font.GetGlyphAt( c );
				Size l_size( l_glyph.GetSize() );
				Position l_ptPosition( l_glyph.GetPosition() );
				//l_iMaxHeight = std::max< int >( l_iMaxHeight, l_ptPosition.y() + l_size.height() );
				//l_iMaxWidth = std::max< int >( l_iMaxWidth, l_ptPosition.x() + l_size.width() );
				l_iMaxHeight = std::max< int >( l_iMaxHeight, l_size.height() );
				l_iMaxWidth = std::max< int >( l_iMaxWidth, l_size.width() );
			}

			ft::CheckErr( FT_Done_Face( l_ftFace ), "FT_Done_Face" );
			ft::CheckErr( FT_Done_FreeType( l_ftLibrary ), "FT_Done_FreeType" );
			p_font.SetMaxHeight( l_iMaxHeight );
			p_font.SetMaxWidth( l_iMaxWidth );
			l_bReturn = true;
		}
		catch( std::runtime_error & p_exc )
		{
			LOADER_ERROR( "Font loading failed : " + std::string( p_exc.what() ) );
		}
	}

	return l_bReturn;
}

void Font::BinaryLoader :: DoLoadGlyph( FT_Face p_ftFace, wchar_t p_char, Font & p_font, int & p_iMaxTop )
{
	FT_Glyph l_ftGlyph;

	ft::CheckErr( FT_Load_Glyph( p_ftFace, FT_Get_Char_Index( p_ftFace, p_char ), FT_LOAD_DEFAULT ), "FT_Load_Glyph" );
	ft::CheckErr( FT_Get_Glyph( p_ftFace->glyph, &l_ftGlyph ), "FT_Get_Glyph" );
	ft::CheckErr( FT_Glyph_To_Bitmap( &l_ftGlyph, FT_RENDER_MODE_NORMAL, 0, 1 ), "FT_Glyph_To_Bitmap" );

	FT_BitmapGlyph	l_ftBmpGlyph	= FT_BitmapGlyph( l_ftGlyph );
	FT_Bitmap &		l_ftBitmap		= l_ftBmpGlyph->bitmap;
	Size			l_size			( std::abs( l_ftBitmap.width ), l_ftBitmap.rows );
	Size			l_advance		( uint32_t( std::abs( l_ftGlyph->advance.x ) / 65536.0 ), uint32_t( l_ftGlyph->advance.y  / 65536.0 ) );
	Position		l_ptPosition	( l_ftBmpGlyph->left, l_ftBmpGlyph->top );
	uint32_t		l_uiSize		= std::abs( l_ftBitmap.pitch ) * l_ftBitmap.rows;
	uint32_t		l_uiIndex		= 0;
	ByteArray		l_bitmap		( l_uiSize, 0 );

	CASTOR_ASSERT( l_uiSize == l_size.width() * l_size.height() );

	p_iMaxTop = std::max< int >( p_iMaxTop, l_ftBmpGlyph->top );

	for( int i = 0; i < l_ftBitmap.rows; i++ )
	{
		for( int j = 0; j < std::abs( l_ftBitmap.pitch ); j++ )
		{
			l_bitmap[l_uiIndex++] = l_ftBitmap.buffer[l_ftBitmap.pitch * i + j];
		}
	}

	p_font.SetGlyphAt( p_char, l_size, l_ptPosition, l_advance, l_bitmap );
}

//*********************************************************************************************

Glyph :: Glyph( wchar_t p_wcChar,  Size const & p_size, Position const & p_position, Size const & p_advance, ByteArray const & p_bitmap )
	:	m_size			( p_size		)
	,	m_position		( p_position	)
	,	m_bitmap		( p_bitmap		)
	,	m_advance		( p_advance		)
	,	m_wcCharacter	( p_wcChar		)
{
}

Glyph :: Glyph( Glyph const & p_glyph )
	:	m_position		( p_glyph.m_position		)
	,	m_size			( p_glyph.m_size			)
	,	m_bitmap		( p_glyph.m_bitmap			)
	,	m_advance		( p_glyph.m_advance			)
	,	m_wcCharacter	( p_glyph.m_wcCharacter		)
{
}

Glyph :: Glyph( Glyph && p_glyph )
	:	m_position		( std::move( p_glyph.m_position		)	)
	,	m_size			( std::move( p_glyph.m_size			)	)
	,	m_bitmap		( std::move( p_glyph.m_bitmap		)	)
	,	m_advance		( std::move( p_glyph.m_advance		)	)
	,	m_wcCharacter	( std::move( p_glyph.m_wcCharacter	)	)
{

	p_glyph.m_position			= Position( 0, 0 );
	p_glyph.m_size				= Size( 0, 0 );
	p_glyph.m_advance			= Size( 0, 0 );
	p_glyph.m_wcCharacter		= 0;
	p_glyph.m_bitmap			.clear();
}

Glyph & Glyph :: operator =( Glyph const & p_glyph )
{
	m_position		= p_glyph.m_position;
	m_size			= p_glyph.m_size;
	m_bitmap		= p_glyph.m_bitmap;
	m_advance		= p_glyph.m_advance;
	m_wcCharacter	= p_glyph.m_wcCharacter;

	return * this;
}

Glyph & Glyph :: operator =( Glyph && p_glyph )
{
	if( this != &p_glyph )
	{
		m_position		= std::move( p_glyph.m_position		);
		m_size			= std::move( p_glyph.m_size			);
		m_bitmap		= std::move( p_glyph.m_bitmap		);
		m_advance		= std::move( p_glyph.m_advance		);
		m_wcCharacter	= std::move( p_glyph.m_wcCharacter	);

		p_glyph.m_position		= Position( 0, 0 );
		p_glyph.m_size			= Size( 0, 0 );
		p_glyph.m_advance		= Size( 0, 0 );
		p_glyph.m_wcCharacter	= 0;
		p_glyph.m_bitmap		.clear();
	}

	return * this;
}

Glyph :: ~Glyph()
{
	m_bitmap.clear();
}

//*********************************************************************************************

Font :: Font( String const & p_strName, uint32_t p_uiHeight )
	:	Resource< Font >	( p_strName									)
	,	m_uiHeight			( p_uiHeight								)
	,	m_iMaxHeight		( 0											)
	,	m_iMaxTop			( 0											)
	,	m_iMaxWidth			( 0											)
	,	m_arrayGlyphs		( std::numeric_limits< uint16_t >::max()	)
{
}

Font :: Font( Font const & p_font )
	:	Resource< Font >	( p_font				)
	,	m_uiHeight			( p_font.m_uiHeight		)
	,	m_pathFile			( p_font.m_pathFile		)
	,	m_iMaxHeight		( p_font.m_iMaxHeight	)
	,	m_iMaxTop			( p_font.m_iMaxTop		)
	,	m_iMaxWidth			( p_font.m_iMaxWidth	)
	,	m_arrayGlyphs		( p_font.m_arrayGlyphs	)
{
}

Font :: Font( Font && p_font )
	:	Resource< Font >	( std::move( p_font					)	)
	,	m_uiHeight			( std::move( p_font.m_uiHeight		)	)
	,	m_pathFile			( std::move( p_font.m_pathFile		)	)
	,	m_iMaxHeight		( std::move( p_font.m_iMaxHeight	)	)
	,	m_iMaxTop			( std::move( p_font.m_iMaxTop		)	)
	,	m_iMaxWidth			( std::move( p_font.m_iMaxWidth		)	)
	,	m_arrayGlyphs		( std::move( p_font.m_arrayGlyphs	)	)
{

	p_font.m_uiHeight	= 0;
	p_font.m_pathFile.clear();
	p_font.m_iMaxHeight	= 0;
	p_font.m_iMaxTop	= 0;
	p_font.m_iMaxWidth	= 0;
}

Font & Font :: operator =( Font const & p_font)
{
	Resource< Font >::operator =( p_font);
	m_uiHeight		= p_font.m_uiHeight;
	m_pathFile		= p_font.m_pathFile;
	m_iMaxHeight	= p_font.m_iMaxHeight;
	m_iMaxTop		= p_font.m_iMaxTop;
	m_arrayGlyphs	= p_font.m_arrayGlyphs;
	m_iMaxWidth		= p_font.m_iMaxWidth;
	return * this;
}

Font & Font :: operator =( Font && p_font)
{
	Resource< Font >::operator =( std::move( p_font ) );

	if (this != & p_font)
	{
		m_uiHeight		= std::move( p_font.m_uiHeight		);
		m_pathFile		= std::move( p_font.m_pathFile		);
		m_iMaxHeight	= std::move( p_font.m_iMaxHeight	);
		m_iMaxTop		= std::move( p_font.m_iMaxTop		);
		m_iMaxWidth		= std::move( p_font.m_iMaxWidth		);
		m_arrayGlyphs	= std::move( p_font.m_arrayGlyphs	);

		p_font.m_uiHeight	= 0;
		p_font.m_pathFile.clear();
		p_font.m_iMaxHeight	= 0;
		p_font.m_iMaxTop	= 0;
		p_font.m_iMaxWidth	= 0;
	}

	return * this;
}

Font :: ~Font()
{
}

void Font :: SetGlyphAt( wchar_t p_char, Size const & p_size, Position p_position, Size const & p_advance, ByteArray const & p_bitmap )
{
	m_arrayGlyphs[p_char] = Glyph( p_char, p_size, p_position, p_advance, p_bitmap );
}

BEGIN_INVARIANT_BLOCK( Font )
	CHECK_INVARIANT( m_uiHeight != 0 );
END_INVARIANT_BLOCK()

//*************************************************************************************************
