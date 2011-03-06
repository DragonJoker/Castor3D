#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/Image.h"
#include "CastorUtils/Path.h"
#include "CastorUtils/PixelFormat.h"
#include "CastorUtils/Rectangle.h"

#include <FreeImage.h>

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
#endif

using namespace Castor;
using namespace Castor::Utils;
using namespace Castor::Math;
using namespace Castor::Resources;

#pragma warning( push)
#pragma warning( disable:4996)

//*************************************************************************************************

ImagePtr ImageLoader :: LoadFromFile( Manager<Image> * p_pManager, const String & p_file)
{
	ImagePtr l_pReturn;
	String l_strPath = p_file;
	l_strPath.replace( "\\", "/");
	FIBITMAP * l_image = NULL;
	FREE_IMAGE_FORMAT l_fif = FIF_UNKNOWN;

	l_fif = FreeImage_GetFileType( l_strPath.char_str(), 0);

	if (l_fif == FIF_UNKNOWN)
	{
		l_fif = FreeImage_GetFIFFromFilename( l_strPath.char_str());
	}

	if (l_fif != FIF_UNKNOWN && FreeImage_FIFSupportsReading( l_fif))
	{
		l_image = FreeImage_Load( l_fif, l_strPath.char_str(), BMP_DEFAULT);
	}

	if (l_image != NULL)
	{
//		FREE_IMAGE_TYPE l_fit = FreeImage_GetImageType( l_image);
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

			//0=Blue, 1=Green, 2=Red, 3=Alpha
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
			for (unsigned int l_pix = 0 ; l_pix < l_width * l_height ; l_pix++)
			{
				l_buffer.Add( l_pixels[l_pix * 4 + 0]);//Blue
				l_buffer.Add( l_pixels[l_pix * 4 + 1]);//Green
				l_buffer.Add( l_pixels[l_pix * 4 + 2]);//Red
				l_buffer.Add( l_pixels[l_pix * 4 + 3]);//Alpha
			}
#else
			for (unsigned int l_pix = 0 ; l_pix < l_width * l_height ; l_pix++)
			{
				l_buffer.Add( l_pixels[l_pix * 4 + 2]);
				l_buffer.Add( l_pixels[l_pix * 4 + 1]);
				l_buffer.Add( l_pixels[l_pix * 4 + 0]);
				l_buffer.Add( l_pixels[l_pix * 4 + 3]);
			}
#endif
			l_pReturn.reset( new Image( p_pManager, Point<size_t, 2>( l_width, l_height), eA8R8G8B8, l_buffer, p_file));

			FreeImage_Unload( l_image);
		}
	}
	else
	{
		LOADER_ERROR( CU_T( "Can't load image : ") + l_strPath);
	}

	return l_pReturn;
}

ImagePtr ImageLoader :: LoadFromBuffer( Manager<Image> * p_pManager, const Math::Point<size_t, 2> & p_ptSize, ePIXEL_FORMAT p_pixelFormat, const Buffer <unsigned char> & p_buffer)
{
	ImagePtr l_pReturn( new Image( p_pManager, p_ptSize, p_pixelFormat, p_buffer));
	return l_pReturn;
}

//*********************************************************************************************

Image :: Image( Manager<Image> * p_pManager, const Point<size_t, 2> & p_ptSize, ePIXEL_FORMAT p_ePixelFormat)
	:	Resource<Image>( p_pManager, C3DEmptyString)
	,	m_ePixelFormat( p_ePixelFormat)
	,	m_ptSize( p_ptSize)
	,	m_buffer( p_ptSize[0] * p_ptSize[1] * GetBytesPerPixel( p_ePixelFormat))
{
}

Image :: Image( Manager<Image> * p_pManager, const Point<size_t, 2> & p_ptSize, ePIXEL_FORMAT p_ePixelFormat, const Buffer <unsigned char> & p_buffer)
	:	Resource<Image>( p_pManager, C3DEmptyString)
	,	m_buffer( p_buffer)
	,	m_ePixelFormat( p_ePixelFormat)
	,	m_ptSize( p_ptSize)
{
}

Image :: Image( Manager<Image> * p_pManager, const Point<size_t, 2> & p_ptSize, ePIXEL_FORMAT p_ePixelFormat, const Buffer <unsigned char> & p_buffer, const Path & p_path)
	:	Resource<Image>( p_pManager, C3DEmptyString)
	,	m_path( p_path)
	,	m_buffer( p_buffer)
	,	m_ePixelFormat( p_ePixelFormat)
	,	m_ptSize( p_ptSize)
{
}

Image :: ~Image()
{
}

void Image :: Fill( const Colour & p_clrColour)
{
	SetPixel( 0, 0, p_clrColour);
	unsigned int l_uiBpp = GetBytesPerPixel( m_ePixelFormat);

	for (size_t i = l_uiBpp ; i < m_buffer.GetSize() ; i += l_uiBpp)
	{
		memcpy( & m_buffer[i], & m_buffer[0], l_uiBpp);
	}
}

void Image :: SetPixel( int x, int y, const unsigned char * p_pPixel)
{
	memcpy( & m_buffer[(x + y * m_ptSize[0]) * GetBytesPerPixel( m_ePixelFormat)], p_pPixel, GetBytesPerPixel( m_ePixelFormat));
}

void Image :: SetPixel( int x, int y, const Colour & p_clrColour)
{
	unsigned char * l_pPixel = & m_buffer[(x + y * m_ptSize[0]) * GetBytesPerPixel( m_ePixelFormat)];
	Point<unsigned char, 4> l_ptComponents;
	p_clrColour.CharBGRA( l_ptComponents);
	ConvertPixel( eA8R8G8B8, l_ptComponents.const_ptr(), m_ePixelFormat, l_pPixel);
}

void Image :: GetPixel( int x, int y, unsigned char * p_pPixel)const
{
	memcpy( p_pPixel, & m_buffer[(x + y * m_ptSize[0]) * GetBytesPerPixel( m_ePixelFormat)], GetBytesPerPixel( m_ePixelFormat));
}

Colour Image :: GetPixel( int x, int y)const
{
	Point<unsigned char, 4> l_ptComponents;
	const unsigned char * l_pPixel = & m_buffer[(x + y * m_ptSize[0]) * GetBytesPerPixel( m_ePixelFormat)];
	ConvertPixel( m_ePixelFormat, l_pPixel, eA8R8G8B8, l_ptComponents.ptr());
	return Colour( l_ptComponents[1], l_ptComponents[2], l_ptComponents[3], l_ptComponents[0]);
}

void Image :: CopyImage( const Image & p_src)
{
	// Si les dimensions ne sont pas les mêmes on effectue un filtrage, sinon on fait une simple copie
	if (m_ptSize == p_src.m_ptSize)
	{
		// Si les formats sont les mêmes on copie simplement les pixels, sinon on effectue une conversion
		if (m_ePixelFormat == p_src.m_ePixelFormat)
		{
			m_buffer = p_src.m_buffer;
		}
		else
		{
			// Récupération des paramètres pour la copie (pointeurs sur les pixels et Bpps)
			const unsigned char	*	l_pSrcPix  = & p_src.m_buffer[0];
			unsigned char		*	l_pDestPix = & m_buffer[0];
			unsigned int			l_uiSrcBpp  = GetBytesPerPixel( p_src.m_ePixelFormat);
			unsigned int			l_uiDestBpp = GetBytesPerPixel( m_ePixelFormat);

			// Parcours des pixels de l'image source et conversion / copie dans l'image courante
			for (size_t i = 0 ; i < m_ptSize[0] ; ++i)
			{
				for (size_t j = 0 ; j < m_ptSize[1] ; ++j)
				{
					ConvertPixel( p_src.m_ePixelFormat, l_pSrcPix, m_ePixelFormat, l_pDestPix);
					l_pSrcPix  += l_uiSrcBpp;
					l_pDestPix += l_uiDestBpp;
				}
			}
		}
	}
	else
	{
		// Calcul du pas d'avancement dans l'image source
		Point2f l_ptStep( static_cast<float>( p_src.m_ptSize[0]) / m_ptSize[0], static_cast<float>( p_src.m_ptSize[1]) / m_ptSize[1]);

		// Si les formats sont les mêmes on copie simplement les pixels, sinon on effectue une conversion
		if (m_ePixelFormat == p_src.m_ePixelFormat)
		{
			// Copie des pixels
			unsigned char l_pPixel[16];

			for (size_t i = 0 ; i < m_ptSize[0] ; ++i)
			{
				for (size_t j = 0 ; j < m_ptSize[1] ; ++j)
				{
					p_src.GetPixel( static_cast<int>( i * l_ptStep[0]), static_cast<int>( j * l_ptStep[1]), l_pPixel);
					SetPixel( i, j, l_pPixel);
				}
			}
		}
		else
		{
			// Parcours des pixels de l'image source et conversion / copie dans l'image courante
			unsigned char l_pSrcPix[16], l_pDestPix[16];

			for (size_t i = 0 ; i < m_ptSize[0] ; ++i)
			{
				for (size_t j = 0; j < m_ptSize[1] ; ++j)
				{
					p_src.GetPixel( static_cast<int>( i * l_ptStep[0]), static_cast<int>( j * l_ptStep[1]), l_pSrcPix);
					ConvertPixel( p_src.m_ePixelFormat, l_pSrcPix, m_ePixelFormat, l_pDestPix);
					SetPixel( i, j, l_pDestPix);
				}
			}
		}
	}
}

Image Image :: SubImage( const Rectangle & l_rcRect)const
{
	CASTOR_ASSERT( Rectangle( 0, 0, m_ptSize[0], m_ptSize[1]).Intersects( l_rcRect) == eIntersectionIn);

	// Création de la sous-image à remplir
	Image l_img( m_pManager, Point3i( l_rcRect.Width(), l_rcRect.Height()), m_ePixelFormat);

	// Calcul de variables temporaires
	const unsigned char	*	l_pSrc			= & m_buffer[(l_rcRect.Left() + l_rcRect.Top() * m_ptSize[0]) * GetBytesPerPixel( m_ePixelFormat)];
	unsigned char		*	l_pDest			= & l_img.m_buffer[0];
	const unsigned int		l_uiSrcPitch	= m_ptSize[0] * GetBytesPerPixel( m_ePixelFormat);
	const unsigned int		l_uiDestPitch	= l_img.m_ptSize[0] * GetBytesPerPixel( l_img.m_ePixelFormat);

	// Copie des pixels de l'image originale dans la sous-image
	for (int i = l_rcRect.Left(); i < l_rcRect.Right(); ++i)
	{
		std::copy( l_pSrc, l_pSrc + l_uiDestPitch, l_pDest);
		l_pSrc  += l_uiSrcPitch;
		l_pDest += l_uiDestPitch;
	}

	return l_img;
}

void Image :: Flip()
{
	size_t l_uiBpp = GetBytesPerPixel( m_ePixelFormat);

	for (size_t j = 0 ; j < m_ptSize[1] / 2 ; ++j)
	{
		std::swap_ranges(	& m_buffer[j * m_ptSize[0] * l_uiBpp],
							& m_buffer[(j + 1) * m_ptSize[0] * l_uiBpp - 1],
							& m_buffer[(m_ptSize[1] - j - 1) * m_ptSize[0] * l_uiBpp]);
	}
}

void Image :: Mirror()
{
	size_t l_uiBpp = GetBytesPerPixel( m_ePixelFormat);

	for (size_t i = 0 ; i < m_ptSize[0] / 2 ; ++i)
	{
		for (size_t j = 0 ; j < m_ptSize[1] ; ++j)
		{
			std::swap_ranges(	& m_buffer[(i + j * m_ptSize[0]) * l_uiBpp],
								& m_buffer[(i + j * m_ptSize[0] + 1) * l_uiBpp],
								& m_buffer[(m_ptSize[0] - i - 1 + j * m_ptSize[0]) * l_uiBpp]);
		}
	}
}

//*********************************************************************************************

ImagePtr ImageManager :: CreateImage( const String & p_name, const Path & p_path)
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

		if (l_image != NULL)
		{
			l_image->SetName( p_name);
			AddElement( l_image);
		}
	}

	return l_image;
}

ImagePtr ImageManager :: CreateImage( const String & p_name, const Point<size_t, 2> & p_ptSize, ePIXEL_FORMAT p_pixelFormat, const Buffer<unsigned char> & p_buffer)
{
	ImagePtr l_image = GetElementByName( p_name);

	if (l_image != NULL)
	{
		l_image->Ref();
	}
	else
	{
		ImageLoader l_loader;
		l_image = l_loader.LoadFromBuffer( this, p_ptSize, p_pixelFormat, p_buffer);

		if (l_image != NULL)
		{
			l_image->SetName( p_name);
			AddElement( l_image);
		}
	}

	return l_image;
}

//*********************************************************************************************

#pragma warning( pop)