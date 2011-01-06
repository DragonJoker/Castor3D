#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/Image.h"

#include <FreeImage.h>

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
	using namespace Castor::Utils;
#endif

using namespace Castor;
using namespace Castor::Resources;

//*************************************************************************************************

ImagePtr ImageLoader :: LoadFromFile( ImageManager * p_pManager, const String & p_file)
{
	ImagePtr l_pReturn;
	String l_strPath = p_file;
	l_strPath.Replace( "\\", "/");
	FIBITMAP * l_image = NULL;
	FREE_IMAGE_FORMAT l_fif = FIF_UNKNOWN;

	l_fif = FreeImage_GetFileType( l_strPath.char_str(), 0);

	if (l_fif == FIF_UNKNOWN)
	{
		l_fif = FreeImage_GetFIFFromFilename( l_strPath.char_str());
	}

	if (l_fif != FIF_UNKNOWN && FreeImage_FIFSupportsReading( l_fif))
	{
		l_image = FreeImage_Load( l_fif, l_strPath.char_str(), 0);
	}

	if (l_image != NULL)
	{
		FIBITMAP * l_dib32 = FreeImage_ConvertTo32Bits( l_image);
		FreeImage_Unload( l_image);
		l_image = l_dib32;

		if (l_image != NULL)
		{
			unsigned int l_width = FreeImage_GetWidth( l_image);
			unsigned int l_height = FreeImage_GetHeight( l_image);

			Buffer <unsigned char> l_buffer;
			l_buffer.InitialiseBuffer( l_width * l_height * 4, 0);
			unsigned char * l_pixels = FreeImage_GetBits( l_image);

#ifdef _WIN32
			for (unsigned int l_pix = 0 ; l_pix < l_width * l_height ; l_pix++)
			{
				l_buffer.Add( l_pixels[l_pix * 4 + 2]);
				l_buffer.Add( l_pixels[l_pix * 4 + 1]);
				l_buffer.Add( l_pixels[l_pix * 4 + 0]);
				l_buffer.Add( l_pixels[l_pix * 4 + 3]);
			}
#endif
			l_pReturn.reset( new Image( p_pManager, p_file, l_buffer, pxfR4G4B4A4));
			l_pReturn->SetWidth( l_width);
			l_pReturn->SetHeight( l_height);

			FreeImage_Unload( l_image);
		}
	}
	else
	{
		LOADER_ERROR( CU_T( "Can't load image : ") + l_strPath);
	}

	return l_pReturn;
}

ImagePtr ImageLoader :: LoadFromBuffer( ImageManager * p_pManager, const Buffer <unsigned char> & p_buffer, PixelFormat p_pixelFormat)
{
	ImagePtr l_pReturn( new Image( p_pManager, p_buffer, p_pixelFormat));
	return l_pReturn;
}

//*********************************************************************************************

Image :: Image( ImageManager * p_pManager, const String & p_path, const Buffer <unsigned char> & p_buffer, PixelFormat p_pixelFormat)
	:	Resource( p_pManager, C3DEmptyString)
	,	m_path( p_path)
	,	m_buffer( p_buffer)
	,	m_pixelFormat( p_pixelFormat)
{
}

Image :: Image( ImageManager * p_pManager, const Buffer <unsigned char> & p_buffer, PixelFormat p_pixelFormat)
	:	Resource( p_pManager, C3DEmptyString)
	,	m_buffer( p_buffer)
	,	m_pixelFormat( p_pixelFormat)
{
}

Image :: ~Image()
{
}

void Image :: Release()
{
	if (*m_references == 1)
	{
		m_pManager->RemoveElement( ImagePtr( this));
	}

	Resource::Release();
}

//*********************************************************************************************

ImagePtr ImageManager :: CreateImage( const String & p_name, const String & p_path)
{
	ImagePtr l_image = GetElementByName( p_name);

	if (l_image != NULL)
	{
		l_image->Ref();
	}
	else
	{
		ImageLoader l_loader;
		l_image = l_loader.LoadFromFile( this, p_path);

		if ( ! l_image == NULL)
		{
			l_image->SetName( p_name);
			AddElement( l_image);
		}
	}

	return l_image;
}

ImagePtr ImageManager :: CreateImage( const String & p_name, const Buffer<unsigned char> & p_buffer, PixelFormat p_pixelFormat)
{
	ImagePtr l_image = GetElementByName( p_name);

	if (l_image != NULL)
	{
		l_image->Ref();
	}
	else
	{
		ImageLoader l_loader;
		l_image = l_loader.LoadFromBuffer( this, p_buffer, p_pixelFormat);

		if ( ! l_image == NULL)
		{
			l_image->SetName( p_name);
			AddElement( l_image);
		}
	}

	return l_image;
}

//*********************************************************************************************