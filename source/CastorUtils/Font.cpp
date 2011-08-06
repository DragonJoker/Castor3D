#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Font.hpp"
#include "CastorUtils/Image.hpp"

#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include <freetype/fttypes.h>

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.hpp"
#endif

using namespace Castor;
using namespace Castor::Math;
using namespace Castor::Utils;
using namespace Castor::Resources;

//*************************************************************************************************

inline int next_p2( int p_value)
{
	int l_iReturn = 1;
	while (l_iReturn < p_value) l_iReturn <<= 1;
	return l_iReturn;
}

//*************************************************************************************************

unsigned int	Loader<Font> :: m_uiHeight;
int				Loader<Font> :: m_iMaxHeight;
int				Loader<Font> :: m_iMaxWidth;
int				Loader<Font> :: m_iMaxTop;

bool Loader<Font> :: Load( Font & p_font, Path const & p_pathFile, unsigned int p_uiHeight)
{
	m_uiHeight = p_uiHeight;
	m_iMaxHeight = 0;
	m_iMaxWidth = 0;
	m_iMaxTop = 0;
	return Load( p_font, p_pathFile);
}

bool Loader<Font> :: Load( Font & p_font, Path const & p_pathFile)
{
	bool l_bReturn = false;

	if ( ! p_pathFile.empty())
	{
		String l_strFontName = p_pathFile.GetFullFileName();

		try
		{
			FT_Library l_ftLibrary;

			if (FT_Init_FreeType( & l_ftLibrary))
			{
				LOADER_ERROR( "FT_Init_FreeType failed");
			}

			FT_Face l_ftFace;

			if (FT_New_Face( l_ftLibrary, p_pathFile.char_str(), 0, & l_ftFace) != 0)
			{
				LOADER_ERROR( "FT_New_Face failed (there is probably a problem with your font file)");
			}

			if (FT_Set_Char_Size( l_ftFace, m_uiHeight << 6, m_uiHeight << 6, 96, 96) != 0)
			{
				LOADER_ERROR( "FT_Set_Char_Size failed");
			}

			wchar_t l_cMin = std::numeric_limits<wchar_t>::min();
			wchar_t l_cMax = std::numeric_limits<wchar_t>::max();
			int l_iTop = 0;
			size_t l_uiSize;
			Point2i l_size;
			Point2i l_ptPosition;
			std::vector<unsigned char> l_bitmap;
			size_t l_uiIndex;

			for (wchar_t c = l_cMin ; c < l_cMax ; c++)
			{
				if (FT_Load_Glyph( l_ftFace, FT_Get_Char_Index( l_ftFace, c), FT_LOAD_DEFAULT) != 0)
				{
					LOADER_ERROR("FT_Load_Glyph failed");
				}

				FT_Glyph l_ftGlyph;

				if (FT_Get_Glyph( l_ftFace->glyph, & l_ftGlyph) != 0)
				{
					LOADER_ERROR("FT_Get_Glyph failed");
				}

				FT_Glyph_To_Bitmap( & l_ftGlyph, ft_render_mode_normal, 0, 1);
				FT_BitmapGlyph l_ftBmpGlyph = (FT_BitmapGlyph)l_ftGlyph;
				FT_Bitmap & l_ftBitmap = l_ftBmpGlyph->bitmap;

				m_iMaxTop = std::max<int>( m_iMaxTop, l_ftBmpGlyph->top);

				l_uiSize = std::abs( l_ftBitmap.pitch) * l_ftBitmap.rows;
				l_size = Point2i( std::abs( l_ftBitmap.pitch), l_ftBitmap.rows);
				l_ptPosition = Point2i( l_ftBmpGlyph->left, l_ftBmpGlyph->top);
				l_bitmap.resize( l_uiSize, 0);
				l_uiIndex = 0;

				if (c == L' ')
				{
					l_size[0] = m_uiHeight / 2;
				}
				else if (c == '\t')
				{
					l_size[0] = 2 * m_uiHeight;
				}

				for (int i = 0 ; i < l_ftBitmap.rows ; i++)
				{
					for (int j = 0 ; j < std::abs( l_ftBitmap.pitch) ; j++)
					{
						l_bitmap[l_uiIndex++] = l_ftBitmap.buffer[l_ftBitmap.pitch * i + j];
					}
				}

				Glyph & l_glyph = p_font.GetGlyphAt( c);
				l_glyph.SetSize( l_size);
				l_glyph.SetPosition( l_ptPosition);
				l_glyph.SetBitmap( l_bitmap);
			}

			for (wchar_t c = l_cMin ; c < l_cMax ; c++)
			{
				Glyph & l_glyph = p_font.GetGlyphAt( c);
				l_ptPosition = l_glyph.GetPosition();
				l_size = l_glyph.GetSize();
				l_ptPosition = Point2i( l_ptPosition[0], m_iMaxTop - l_ptPosition[1]);
				l_glyph.SetPosition( l_ptPosition);
				m_iMaxHeight = std::max<int>( m_iMaxHeight, l_ptPosition[1] + l_size[1]);
				m_iMaxWidth = std::max<int>( m_iMaxWidth, l_ptPosition[0] + l_size[0]);
			}

			FT_Done_Face( l_ftFace);
			FT_Done_FreeType( l_ftLibrary);
			p_font.SetMaxHeight( m_iMaxHeight);
			p_font.SetMaxWidth( m_iMaxWidth);
			l_bReturn = true;
		}
		catch (std::runtime_error p_exc)
		{
		}
	}

	return l_bReturn;
}

//*********************************************************************************************

Glyph :: Glyph()
{
}

Glyph :: Glyph( Glyph const & p_glyph)
	:	m_ptPosition	( p_glyph.m_ptPosition)
	,	m_size			( p_glyph.m_size)
	,	m_bitmap		( p_glyph.m_bitmap)
{
}

Glyph & Glyph :: operator =( Glyph const & p_glyph)
{
	m_ptPosition	= p_glyph.m_ptPosition;
	m_size			= p_glyph.m_size;
	m_bitmap		= p_glyph.m_bitmap;

	return * this;
}

Glyph :: ~Glyph()
{
	m_bitmap.clear();
}

//*********************************************************************************************

Font :: Font( String const & p_strName, unsigned int p_uiHeight)
	:	Resource<Font>( p_strName)
	,	m_uiHeight		( p_uiHeight)
	,	m_iMaxHeight	( 0)
	,	m_iMaxTop		( 0)
{
	m_arrayChars.resize( std::numeric_limits<wchar_t>::max() + 1);
}

Font :: Font( Font const & p_font)
	:	Resource<Font>( p_font)
	,	m_uiHeight		( p_font.m_uiHeight)
	,	m_pathFile		( p_font.m_pathFile)
	,	m_iMaxHeight	( p_font.m_iMaxHeight)
	,	m_iMaxTop		( p_font.m_iMaxTop)
	,	m_arrayChars	( p_font.m_arrayChars)
{
}

Font & Font :: operator =( Font const & p_font)
{
	Resource<Font>::operator =( p_font);
	m_uiHeight		= p_font.m_uiHeight;
	m_pathFile		= p_font.m_pathFile;
	m_iMaxHeight	= p_font.m_iMaxHeight;
	m_iMaxTop		= p_font.m_iMaxTop;
	m_arrayChars	= p_font.m_arrayChars;
	return * this;
}

Font :: ~Font()
{
}

BEGIN_INVARIANT_BLOCK( Font)
	CHECK_INVARIANT( m_uiHeight != 0);
END_INVARIANT_BLOCK()

BEGIN_SERIALISE_MAP( Font, Serialisable)
	ADD_ELEMENT( m_uiHeight)
	ADD_ELEMENT( m_iMaxHeight)
	ADD_ELEMENT( m_iMaxTop)
	ADD_ELEMENT( m_arrayChars)
END_SERIALISE_MAP()

std::vector<unsigned char> const & Font :: operator []( wchar_t p_char)const
{
	CHECK_INVARIANTS();
	std::vector<unsigned char> const & l_arrayReturn = m_arrayChars[p_char].GetBitmap();
	CHECK_INVARIANTS();
	return l_arrayReturn;
}

std::vector<unsigned char> & Font :: operator []( wchar_t p_char)
{
	CHECK_INVARIANTS();
	std::vector<unsigned char> & l_arrayReturn = m_arrayChars[p_char].GetBitmap();
	CHECK_INVARIANTS();
	return l_arrayReturn;
}

//*************************************************************************************************
