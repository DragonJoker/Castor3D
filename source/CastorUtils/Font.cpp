#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/Font.h"
#include "CastorUtils/Image.h"

#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor;
using namespace Castor::Resources;

//*************************************************************************************************

inline int next_p2( int p_value)
{
	int l_iReturn = 1;
	while (l_iReturn < p_value) l_iReturn <<= 1;
	return l_iReturn;
}

//*************************************************************************************************

FontPtr FontLoader :: LoadFromFile( FontManager * p_pManager, const String & p_strName, const String & p_strPath, int p_uiHeight, ImageManager * p_pImageManager)
{
	FontPtr l_pReturn( new Font( p_pManager, p_strName, p_strPath, p_uiHeight, p_pImageManager));
	return l_pReturn;
}

//*********************************************************************************************

Font :: Font( FontManager * p_pManager)
	:	Resource( p_pManager, C3DEmptyString)
	,	m_uiHeight( 0)
{
}

Font :: Font( FontManager * p_pManager, const String & p_strName, const String & p_strPath, size_t p_uiHeight, ImageManager * p_pImageManager)
	:	Resource( p_pManager, p_strName)
	,	m_uiHeight( 0)
{
	Initialise( p_strPath, p_uiHeight, p_pImageManager);
}

void Font :: Initialise( const String & p_strPath, size_t p_uiHeight, ImageManager * p_pImageManager)
{
	m_uiHeight = p_uiHeight;
	m_strPath = p_strPath;
	FT_Library l_ftLibrary;

	if (FT_Init_FreeType( & l_ftLibrary))
	{
		throw std::runtime_error( "FT_Init_FreeType failed");
	}

	FT_Face l_ftFace;

	if (FT_New_Face( l_ftLibrary, p_strPath.char_str(), 0, & l_ftFace))
	{
		throw std::runtime_error( "FT_New_Face failed (there is probably a problem with your font file)");
	}

	FT_Set_Char_Size( l_ftFace, m_uiHeight << 6, m_uiHeight << 6, 96, 96);

	Buffer<unsigned char> l_buffer;
	l_buffer.InitialiseBuffer( 2 * m_uiHeight * m_uiHeight * 256, 0);

	for (unsigned char c = 0 ; c < 255 ; c++)
	{
		_loadChar( l_ftFace, l_buffer, (char)c);
	}

	m_pImage = p_pImageManager->CreateImage( CU_T( "Font_") + GetName(), l_buffer, pxfL4A4);

	FT_Done_Face( l_ftFace);
	FT_Done_FreeType( l_ftLibrary);
}

const unsigned char * Font :: operator []( unsigned char p_char)const
{
	const unsigned char * l_pReturn = NULL;

	if (m_pImage != NULL)
	{
		l_pReturn = & m_pImage->GetBuffer()[p_char * m_uiHeight * m_uiHeight];
	}

	return l_pReturn;
}

unsigned char * Font :: operator []( unsigned char p_char)
{
	unsigned char * l_pReturn = NULL;

	if (m_pImage != NULL)
	{
		l_pReturn = & m_pImage->GetBuffer()[p_char * m_uiHeight * m_uiHeight];
	}

	return l_pReturn;
}

void Font :: _loadChar( FT_Face p_ftFace, Buffer<unsigned char> & p_buffer, char p_char)
{
	if (FT_Load_Glyph( p_ftFace, FT_Get_Char_Index( p_ftFace, p_char), FT_LOAD_DEFAULT))
	{
		throw std::runtime_error("FT_Load_Glyph failed");
	}

	FT_Glyph l_ftGlyph;

	if (FT_Get_Glyph( p_ftFace->glyph, & l_ftGlyph))
	{
		throw std::runtime_error("FT_Get_Glyph failed");
	}

	FT_Glyph_To_Bitmap( & l_ftGlyph, ft_render_mode_normal, 0, 1);
	FT_BitmapGlyph l_ftBmpGlyph = (FT_BitmapGlyph)l_ftGlyph;
	FT_Bitmap & l_ftBitmap = l_ftBmpGlyph->bitmap;

	size_t l_uiOffset = p_char * 2 * m_uiHeight * m_uiHeight;

	for (int j = 0 ; j < int( m_uiHeight) ; j++)
	{
		for (int i = 0 ; i < int( m_uiHeight) ; i++)
		{
			p_buffer[l_uiOffset + 2 * (i + j * m_uiHeight)] = p_buffer[l_uiOffset + 2 * (i + j * m_uiHeight) + 1] = (i >= l_ftBitmap.width || j >= l_ftBitmap.rows) ? 0 : l_ftBitmap.buffer[i + l_ftBitmap.width * j];
		}
	}

}

//*********************************************************************************************

FontPtr FontManager :: CreateFont( const String & p_strName, const String & p_strPath, size_t p_uiHeight, ImageManager * p_pImageManager)
{
	FontPtr l_pFont = GetElementByName( p_strName);

	if (l_pFont != NULL)
	{
		l_pFont->Ref();
	}
	else
	{
		FontLoader l_loader;
		l_pFont = l_loader.LoadFromFile( this, p_strName, p_strPath, p_uiHeight, p_pImageManager);

		if (l_pFont != NULL)
		{
			AddElement( l_pFont);
		}
	}

	return l_pFont;
}

//*************************************************************************************************