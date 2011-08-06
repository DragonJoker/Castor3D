#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Image.hpp"
#include "CastorUtils/Path.hpp"
#include "CastorUtils/Rectangle.hpp"

#include <FreeImage.h>

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.hpp"
#endif

using namespace Castor;
using namespace Castor::Utils;
using namespace Castor::Math;
using namespace Castor::Resources;

#pragma warning( push)
#pragma warning( disable:4996)

//*************************************************************************************************

unsigned int DLL_CALLCONV ReadProc( void * p_pBuffer, unsigned int p_uiSize, unsigned int p_uiCount, fi_handle p_fiHandle)
{
	File * l_pFile = (File *)p_fiHandle;
	return l_pFile->ReadArray<unsigned char>( (unsigned char *)p_pBuffer, p_uiSize * p_uiCount);
}

int DLL_CALLCONV SeekProc( fi_handle p_fiHandle, long p_lOffset, int p_iOrigin)
{
	File * l_pFile = (File *)p_fiHandle;
	return l_pFile->Seek( p_lOffset, File::eOFFSET_MODE( p_iOrigin));
}

long DLL_CALLCONV TellProc( fi_handle p_fiHandle)
{
	File * l_pFile = (File *)p_fiHandle;
	return (long)l_pFile->Tell();
}

bool Loader<Image> :: Load( Image & p_image, Path const & p_path)
{
	bool l_bReturn = false;

	if ( ! p_path.empty())
	{
		File l_file( p_path, File::eOPEN_MODE_READ | File::eOPEN_MODE_BINARY);
		FIBITMAP * l_pImage = NULL;
		FREE_IMAGE_FORMAT l_fif = FIF_UNKNOWN;

		l_fif = FreeImage_GetFileType( p_path.char_str(), 0);

		if (l_fif == FIF_UNKNOWN)
		{
			l_fif = FreeImage_GetFIFFromFilename( p_path.char_str());
		}

		if (l_fif != FIF_UNKNOWN && FreeImage_FIFSupportsReading( l_fif))
		{
			FreeImageIO l_fiIo;
			l_fiIo.read_proc = ReadProc;
			l_fiIo.write_proc = NULL;
			l_fiIo.seek_proc = SeekProc;
			l_fiIo.tell_proc = TellProc;

			l_pImage = FreeImage_LoadFromHandle( l_fif, & l_fiIo, (fi_handle)( & l_file), BMP_DEFAULT);
		}

		if (l_pImage)
		{
			FIBITMAP * l_dib32 = FreeImage_ConvertTo32Bits( l_pImage);
			FreeImage_Unload( l_pImage);
			l_pImage = l_dib32;

			if (l_pImage)
			{
				unsigned int l_width = FreeImage_GetWidth( l_pImage);
				unsigned int l_height = FreeImage_GetHeight( l_pImage);
				Size l_size( l_width, l_height);
				unsigned char * l_pixels = FreeImage_GetBits( l_pImage);

				//0=Blue, 1=Green, 2=Red, 3=Alpha
#if FREEIMAGE_COLORORDER != FREEIMAGE_COLORORDER_BGR
				unsigned char * l_pTmp = l_pixels;
				size_t l_uiSize = l_width * l_height;

				for (unsigned int i = 0 ; i < l_uiSize ; i++)
				{
					std::swap( l_pTmp[0], l_pTmp[2]);
					l_pTmp += 4;
				}
#endif
				p_image.Initialise<ePIXEL_FORMAT_A8R8G8B8, ePIXEL_FORMAT_A8R8G8B8>( l_size, l_pixels, p_path);

				FreeImage_Unload( l_pImage);
				l_bReturn = true;
			}
		}
		else
		{
			LOADER_ERROR( cuT( "Can't load image : ") + p_path);
		}
	}

	return l_bReturn;
}

//*********************************************************************************************

Image :: Image( String const & p_strName)
	:	Resource<Image>( p_strName)
	,	m_ePixelFormat	( ePIXEL_FORMAT_A8R8G8B8)
	,	m_pBuffer		( PxBufferBase::create( Point<size_t, 2>( 1, 1), m_ePixelFormat, NULL, ePIXEL_FORMAT_A8R8G8B8))
{
}

Image :: Image( Image const & p_image)
	:	Resource<Image>( p_image)
	,	m_path			( p_image.m_path)
	,	m_ePixelFormat	( p_image.m_ePixelFormat)
	,	m_pBuffer		( p_image.m_pBuffer->clone())
{
	m_ptSize.copy( p_image.m_ptSize);
	CHECK_INVARIANTS();
}

Image & Image :: operator =( Image const & p_image)
{
	Resource<Image>::operator =( p_image);
	m_path = p_image.m_path;
	m_ePixelFormat = p_image.m_ePixelFormat;
	m_pBuffer = p_image.m_pBuffer->clone();
	m_ptSize.copy( p_image.m_ptSize);
	CHECK_INVARIANTS();
	return * this;
}

Image :: ~Image()
{
	delete m_pBuffer;
}

BEGIN_INVARIANT_BLOCK( Image)
	CHECK_INVARIANT( m_ePixelFormat >= 0 && m_ePixelFormat < ePIXEL_FORMAT_COUNT);
	CHECK_INVARIANT( m_pBuffer->count() > 0);
END_INVARIANT_BLOCK()

BEGIN_SERIALISE_MAP( Image, Serialisable)
	ADD_ELEMENT( m_pBuffer)
	ADD_ELEMENT( m_ptSize)
	ADD_ELEMENT( m_ePixelFormat)
END_SERIALISE_MAP()

void Image :: Initialise( Point<unsigned int, 2> const & p_ptSize, ePIXEL_FORMAT p_ePixelFormat, std::vector<unsigned char> const & p_buffer, ePIXEL_FORMAT p_eBufferFormat, Path const & p_path)
{
	Initialise( p_ptSize, p_ePixelFormat, & p_buffer[0], p_eBufferFormat, p_path);
}

void Image :: Initialise( Point<unsigned int, 2> const & p_ptSize, ePIXEL_FORMAT p_ePixelFormat, unsigned char const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat, Path const & p_path)
{
	m_path = p_path;
	delete m_pBuffer;
	m_pBuffer = PxBufferBase::create( p_ptSize, p_ePixelFormat, p_pBuffer, p_eBufferFormat);
	m_ePixelFormat = p_ePixelFormat;
	m_ptSize.copy( p_ptSize);
	CHECK_INVARIANTS();
}

void Image :: Initialise( Point<unsigned int, 2> const & p_ptSize, PxBufferBase const & p_buffer, Path const & p_path)
{
	m_path = p_path;
	delete m_pBuffer;
	m_pBuffer = p_buffer.clone();
	m_ePixelFormat = m_pBuffer->format();
	m_ptSize.copy( p_ptSize);
	CHECK_INVARIANTS();
}
void Image :: Fill( Colour const & p_clrColour)
{
	CHECK_INVARIANTS();
	SetPixel( 0, 0, p_clrColour);
	unsigned int l_uiBpp = GetBytesPerPixel( m_ePixelFormat);

	for (size_t i = l_uiBpp ; i < m_pBuffer->count() ; i += l_uiBpp)
	{
		memcpy( & m_pBuffer[i], & m_pBuffer[0], l_uiBpp);
	}
	CHECK_INVARIANTS();
}

void Image :: SetPixel( size_t x, size_t y, unsigned char const * p_pPixel)
{
	CHECK_INVARIANTS();
	REQUIRE( x < m_pBuffer->width() && y < m_pBuffer->height());
	memcpy( & m_pBuffer[(x + y * m_ptSize[0]) * GetBytesPerPixel( m_ePixelFormat)], p_pPixel, GetBytesPerPixel( m_ePixelFormat));
	CHECK_INVARIANTS();
}

void Image :: SetPixel( size_t x, size_t y, Colour const & p_clrColour)
{
	CHECK_INVARIANTS();
	REQUIRE( x < m_pBuffer->width() && y < m_pBuffer->height());
	Point4ub l_ptComponents;
	p_clrColour.BGRA( l_ptComponents);
	ConvertPixel( ePIXEL_FORMAT_A8R8G8B8, l_ptComponents.const_ptr(), m_ePixelFormat, m_pBuffer->get_at( x, y));
	CHECK_INVARIANTS();
}

void Image :: SetPixel( size_t x, size_t y, UbPixel const & p_pixel)
{
	CHECK_INVARIANTS();
	REQUIRE( x < m_pBuffer->width() && y < m_pBuffer->height());
	ConvertPixel( p_pixel.format(), p_pixel.const_ptr(), m_ePixelFormat, m_pBuffer->get_at( x, y));
	CHECK_INVARIANTS();
}

void Image :: GetPixel( size_t x, size_t y, UbPixel & p_pixel)const
{
	CHECK_INVARIANTS();
	REQUIRE( x < m_pBuffer->width() && y < m_pBuffer->height());
	ConvertPixel( m_ePixelFormat, m_pBuffer->get_at( x, y), p_pixel.format(), p_pixel.ptr());
	CHECK_INVARIANTS();
}

void Image :: GetPixel( size_t x, size_t y, unsigned char * p_pPixel, ePIXEL_FORMAT p_eFormat)const
{
	CHECK_INVARIANTS();
	REQUIRE( x < m_pBuffer->width() && y < m_pBuffer->height());
	ConvertPixel( m_ePixelFormat, m_pBuffer->get_at( x, y), p_eFormat, p_pPixel);
	CHECK_INVARIANTS();
}

Colour Image :: GetPixel( size_t x, size_t y)const
{
	CHECK_INVARIANTS();
	REQUIRE( x < m_pBuffer->width() && y < m_pBuffer->height());
	Point4ub l_ptComponents;
	ConvertPixel( m_ePixelFormat, m_pBuffer->get_at( x, y), ePIXEL_FORMAT_A8R8G8B8, l_ptComponents.ptr());
	return Colour::FromBGRA( l_ptComponents);
	CHECK_INVARIANTS();
}

void Image :: CopyImage( Image const & p_src)
{
	CHECK_INVARIANTS();
	// Si les dimensions ne sont pas les mêmes on effectue un filtrage, sinon on fait une simple copie
	if (m_ptSize == p_src.m_ptSize)
	{
		// Si les formats sont les mêmes on copie simplement les pixels, sinon on effectue une conversion
		if (m_ePixelFormat == p_src.m_ePixelFormat)
		{
			m_pBuffer = p_src.m_pBuffer;
		}
		else
		{
			// Parcours des pixels de l'image source et conversion / copie dans l'image courante
			for (unsigned int i = 0 ; i < m_ptSize[0] ; ++i)
			{
				for (unsigned int j = 0 ; j < m_ptSize[1] ; ++j)
				{
					ConvertPixel( p_src.m_ePixelFormat, p_src.m_pBuffer->get_at( i, j), m_ePixelFormat, m_pBuffer->get_at( i, j));
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
			UbPixel l_pPixel;

			for (unsigned int i = 0 ; i < m_ptSize[0] ; ++i)
			{
				for (unsigned int j = 0 ; j < m_ptSize[1] ; ++j)
				{
					p_src.GetPixel( static_cast<int>( i * l_ptStep[0]), static_cast<int>( j * l_ptStep[1]), l_pPixel);
					SetPixel( i, j, l_pPixel);
				}
			}
		}
		else
		{
			// Parcours des pixels de l'image source et conversion / copie dans l'image courante
			unsigned char * l_pSrcPix	= new unsigned char[GetBytesPerPixel( p_src.GetPixelFormat())];
			unsigned char * l_pDestPix	= new unsigned char[GetBytesPerPixel( m_ePixelFormat)];

			for (unsigned int i = 0 ; i < m_ptSize[0] ; ++i)
			{
				for (unsigned int j = 0; j < m_ptSize[1] ; ++j)
				{
					p_src.GetPixel( static_cast<int>( i * l_ptStep[0]), static_cast<int>( j * l_ptStep[1]), l_pSrcPix, p_src.GetPixelFormat());
					ConvertPixel( p_src.m_ePixelFormat, l_pSrcPix, m_ePixelFormat, l_pDestPix);
					SetPixel( i, j, l_pDestPix);
				}
			}

			delete [] l_pSrcPix;
			delete [] l_pDestPix;
		}
	}
	CHECK_INVARIANTS();
}

Image Image :: SubImage( Rectangle const & l_rcRect)const
{
	CHECK_INVARIANTS();
	REQUIRE( Rectangle( 0, 0, m_ptSize[0], m_ptSize[1]).Intersects( l_rcRect) == eINTERSECTION_IN);
	Point<unsigned int, 2> l_ptSize( l_rcRect.Width(), l_rcRect.Height());

	// Création de la sous-image à remplir
	Image l_img( m_strName + String( "_Sub") << l_rcRect[0] << "x" << l_rcRect[1] << ":" << l_ptSize[0] << "x" << l_ptSize[1]);
	l_img.Initialise( l_ptSize, m_ePixelFormat);

	// Calcul de variables temporaires
	const unsigned char	*	l_pSrc			= m_pBuffer->get_at( l_rcRect.Left(), l_rcRect.Top());
	unsigned char		*	l_pDest			= l_img.m_pBuffer->get_at( 0, 0);
	const unsigned int		l_uiSrcPitch	= m_ptSize[0] * GetBytesPerPixel( m_ePixelFormat);
	const unsigned int		l_uiDestPitch	= l_img.m_ptSize[0] * GetBytesPerPixel( l_img.m_ePixelFormat);

	// Copie des pixels de l'image originale dans la sous-image
	for (int i = l_rcRect.Left(); i < l_rcRect.Right(); ++i)
	{
		std::copy( l_pSrc, l_pSrc + l_uiDestPitch, l_pDest);
		l_pSrc  += l_uiSrcPitch;
		l_pDest += l_uiDestPitch;
	}

	CHECK_INVARIANTS();
	return l_img;
}

void Image :: Flip()
{
	CHECK_INVARIANTS();
	m_pBuffer->flip();
	CHECK_INVARIANTS();
}

void Image :: Mirror()
{
	CHECK_INVARIANTS();
	m_pBuffer->mirror();
	CHECK_INVARIANTS();
}

//*************************************************************************************************

#pragma warning( pop)
