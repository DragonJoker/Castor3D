#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Pixel.hpp"

using namespace Castor;
using namespace Castor::Templates;
using namespace Castor::Resources;
using namespace Castor::Math;

//*************************************************************************************************

unsigned int Castor::Resources::GetBytesPerPixel( ePIXEL_FORMAT p_pfFormat)
{
	switch (p_pfFormat)
	{
	case ePIXEL_FORMAT_L8		: return pixel_definitions<ePIXEL_FORMAT_L8			>::size;
	case ePIXEL_FORMAT_A8L8		: return pixel_definitions<ePIXEL_FORMAT_A8L8		>::size;
	case ePIXEL_FORMAT_A1R5G5B5	: return pixel_definitions<ePIXEL_FORMAT_A1R5G5B5	>::size;
	case ePIXEL_FORMAT_A4R4G4B4	: return pixel_definitions<ePIXEL_FORMAT_A4R4G4B4	>::size;
	case ePIXEL_FORMAT_R8G8B8	: return pixel_definitions<ePIXEL_FORMAT_R8G8B8		>::size;
	case ePIXEL_FORMAT_A8R8G8B8	: return pixel_definitions<ePIXEL_FORMAT_A8R8G8B8	>::size;
	case ePIXEL_FORMAT_DXTC1	: return pixel_definitions<ePIXEL_FORMAT_DXTC1		>::size;
	case ePIXEL_FORMAT_DXTC3	: return pixel_definitions<ePIXEL_FORMAT_DXTC3		>::size;
	case ePIXEL_FORMAT_DXTC5	: return pixel_definitions<ePIXEL_FORMAT_DXTC5		>::size;
	default						: return 0;
	}
}

void Castor::Resources::ConvertPixel( Resources::ePIXEL_FORMAT p_eSrcFmt, unsigned char const * p_pSrc, Resources::ePIXEL_FORMAT p_eDestFmt, unsigned char * p_pDest)
{
	// Définition d'une macro évitant d'avoir un code de 50 000 lignes de long
#	define CONVERSIONS_FOR(Fmt) \
	case Fmt: \
	{ \
		switch (p_eDestFmt) \
		{ \
			case ePIXEL_FORMAT_L8 :       pixel_converter<Fmt, ePIXEL_FORMAT_L8			>::convert(p_pSrc, p_pDest);	break; \
			case ePIXEL_FORMAT_A8L8 :     pixel_converter<Fmt, ePIXEL_FORMAT_A8L8		>::convert(p_pSrc, p_pDest);	break; \
			case ePIXEL_FORMAT_A1R5G5B5 : pixel_converter<Fmt, ePIXEL_FORMAT_A1R5G5B5	>::convert(p_pSrc, p_pDest);	break; \
			case ePIXEL_FORMAT_A4R4G4B4 : pixel_converter<Fmt, ePIXEL_FORMAT_A4R4G4B4	>::convert(p_pSrc, p_pDest);	break; \
			case ePIXEL_FORMAT_R8G8B8 :   pixel_converter<Fmt, ePIXEL_FORMAT_R8G8B8		>::convert(p_pSrc, p_pDest);	break; \
			case ePIXEL_FORMAT_A8R8G8B8 : pixel_converter<Fmt, ePIXEL_FORMAT_A8R8G8B8	>::convert(p_pSrc, p_pDest);	break; \
			case ePIXEL_FORMAT_DXTC1 :    pixel_converter<Fmt, ePIXEL_FORMAT_DXTC1		>::convert(p_pSrc, p_pDest);	break; \
			case ePIXEL_FORMAT_DXTC3 :    pixel_converter<Fmt, ePIXEL_FORMAT_DXTC3		>::convert(p_pSrc, p_pDest);	break; \
			case ePIXEL_FORMAT_DXTC5 :    pixel_converter<Fmt, ePIXEL_FORMAT_DXTC5		>::convert(p_pSrc, p_pDest);	break; \
		} \
		break; \
	}

	// Gestion de la conversion - appelle la version optimisée de la conversion pour les deux formats mis en jeu
	switch (p_eSrcFmt)
	{
		CONVERSIONS_FOR( ePIXEL_FORMAT_L8)
		CONVERSIONS_FOR( ePIXEL_FORMAT_A8L8)
		CONVERSIONS_FOR( ePIXEL_FORMAT_A1R5G5B5)
		CONVERSIONS_FOR( ePIXEL_FORMAT_A4R4G4B4)
		CONVERSIONS_FOR( ePIXEL_FORMAT_R8G8B8)
		CONVERSIONS_FOR( ePIXEL_FORMAT_A8R8G8B8)
		CONVERSIONS_FOR( ePIXEL_FORMAT_DXTC1)
		CONVERSIONS_FOR( ePIXEL_FORMAT_DXTC3)
		CONVERSIONS_FOR( ePIXEL_FORMAT_DXTC5)
	}

	// On tue la macro une fois qu'on n'en a plus besoin
#		undef CONVERSIONS_FOR
}

//*************************************************************************************************

UbPixel Castor::Resources :: Substract( UbPixel const & p_a, UbPixel const & p_b, UbPixel const & p_min)
{
	UbPixel l_result;

	if (p_a[0] >= p_min[0] + p_b[0])
	{
		l_result[0] = p_a[0] - p_b[0];
	}
	else
	{
		l_result[0] = p_min[0];
	}

	if (p_a[1] >= p_min[1] + p_b[1])
	{
		l_result[1] = p_a[1] - p_b[1];
	}
	else
	{
		l_result[1] = p_min[1];
	}

	if (p_a[2] >= p_min[2] + p_b[2])
	{
		l_result[2] = p_a[2] - p_b[2];
	}
	else
	{
		l_result[2] = p_min[2];
	}

	if (p_a[3] >= p_min[3] + p_b[3])
	{
		l_result[3] = p_a[3] - p_b[3];
	}
	else
	{
		l_result[3] = p_min[3];
	}

	return l_result;
}

UbPixel Castor::Resources :: Add( UbPixel const & p_a, UbPixel const & p_b, UbPixel const & p_max)
{
	UbPixel l_result;

	if (p_a[0] + p_b[0] < p_max[0])
	{
		l_result[0] = p_a[0] + p_b[0];
	}
	else
	{
		l_result[0] = p_max[0];
	}

	if (p_a[1] + p_b[1] < p_max[1])
	{
		l_result[1] = p_a[1] + p_b[1];
	}
	else
	{
		l_result[1] = p_max[1];
	}

	if (p_a[2] + p_b[2] < p_max[2])
	{
		l_result[2] = p_a[2] + p_b[2];
	}
	else
	{
		l_result[2] = p_max[2];
	}

	if (p_a[3] + p_b[3] < p_max[3])
	{
		l_result[3] = p_a[3] + p_b[3];
	}
	else
	{
		l_result[3] = p_max[3];
	}

	return l_result;
}

//*************************************************************************************************

PxBufferBase :: PxBufferBase( Size const & p_size, ePIXEL_FORMAT p_ePixelFormat)
	:	m_ePixelFormat	( p_ePixelFormat)
	,	m_size			( p_size)
	,	m_pBuffer		( NULL)
{
	init( NULL, ePIXEL_FORMAT_A8R8G8B8);
}

PxBufferBase :: PxBufferBase( Size const & p_size, ePIXEL_FORMAT p_ePixelFormat, unsigned char const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat)
	:	m_ePixelFormat	( p_ePixelFormat)
	,	m_size			( p_size)
	,	m_pBuffer		( NULL)
{
	init( p_pBuffer, p_eBufferFormat);
}

PxBufferBase :: PxBufferBase( PxBufferBase const & p_pixelBuffer)
	:	m_ePixelFormat	( p_pixelBuffer.m_ePixelFormat)
	,	m_size			( p_pixelBuffer.m_size)
	,	m_pBuffer		( NULL)
{
	init( p_pixelBuffer.m_pBuffer, p_pixelBuffer.m_ePixelFormat);
}

PxBufferBase :: ~PxBufferBase()
{
	clear();
}

PxBufferBase & PxBufferBase :: operator =( PxBufferBase const & p_pixelBuffer)
{
	clear();
	m_size = p_pixelBuffer.m_size;
	init( p_pixelBuffer.m_pBuffer, p_pixelBuffer.m_ePixelFormat);
	return * this;
}

void PxBufferBase :: clear()
{
	delete [] m_pBuffer;
	m_pBuffer = NULL;
}

void PxBufferBase :: init( unsigned char const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat)
{
	size_t l_uiBpp = GetBytesPerPixel( m_ePixelFormat);
	size_t l_uiSize = count() * l_uiBpp;
	m_pBuffer = new unsigned char[l_uiSize];

	if (p_pBuffer == NULL)
	{
		memset( m_pBuffer, 0, l_uiSize);
	}
	else
	{
		if (p_eBufferFormat == m_ePixelFormat)
		{
			memcpy( m_pBuffer, p_pBuffer, l_uiSize);
		}
		else
		{
			size_t l_uiBufBpp = GetBytesPerPixel( p_eBufferFormat);
			unsigned char * l_pDst = m_pBuffer;
			unsigned char * l_pSrc = const_cast<unsigned char *>( p_pBuffer);

			for (size_t i = 0 ; i < count() ; i++)
			{
				ConvertPixel( p_eBufferFormat, l_pSrc, m_ePixelFormat, l_pDst);
				l_pDst += l_uiBpp;
				l_pSrc += l_uiBufBpp;
			}
		}
	}
}

void PxBufferBase :: init( Size const & p_size)
{
	m_size = p_size;
	init( NULL, ePIXEL_FORMAT_A8R8G8B8);
}

void PxBufferBase :: swap( PxBufferBase & p_pixelBuffer)
{
	std::swap( m_size,			p_pixelBuffer.m_size);
	std::swap( m_ePixelFormat,	p_pixelBuffer.m_ePixelFormat);
	std::swap( m_pBuffer,		p_pixelBuffer.m_pBuffer);
}

PxBufferBase * PxBufferBase :: create( Size const & p_size, ePIXEL_FORMAT p_eWantedFormat, unsigned char const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat)
{
	switch (p_eWantedFormat)
	{
	case ePIXEL_FORMAT_L8		: return new PxBuffer<	ePIXEL_FORMAT_L8		>( p_size, p_pBuffer, p_eBufferFormat);
	case ePIXEL_FORMAT_A8L8		: return new PxBuffer<	ePIXEL_FORMAT_A8L8		>( p_size, p_pBuffer, p_eBufferFormat);
	case ePIXEL_FORMAT_A1R5G5B5	: return new PxBuffer<	ePIXEL_FORMAT_A1R5G5B5	>( p_size, p_pBuffer, p_eBufferFormat);
	case ePIXEL_FORMAT_A4R4G4B4	: return new PxBuffer<	ePIXEL_FORMAT_A4R4G4B4	>( p_size, p_pBuffer, p_eBufferFormat);
	case ePIXEL_FORMAT_R8G8B8	: return new PxBuffer<	ePIXEL_FORMAT_R8G8B8	>( p_size, p_pBuffer, p_eBufferFormat);
	case ePIXEL_FORMAT_A8R8G8B8	: return new PxBuffer<	ePIXEL_FORMAT_A8R8G8B8	>( p_size, p_pBuffer, p_eBufferFormat);
	case ePIXEL_FORMAT_DXTC1	: return new PxBuffer<	ePIXEL_FORMAT_DXTC1		>( p_size, p_pBuffer, p_eBufferFormat);
	case ePIXEL_FORMAT_DXTC3	: return new PxBuffer<	ePIXEL_FORMAT_DXTC3		>( p_size, p_pBuffer, p_eBufferFormat);
	case ePIXEL_FORMAT_DXTC5	: return new PxBuffer<	ePIXEL_FORMAT_DXTC5		>( p_size, p_pBuffer, p_eBufferFormat);
	default						: return NULL;
	}
}

//*************************************************************************************************