#include "PixelBuffer.hpp"

namespace Castor
{
	PxBufferBase::PxBufferBase( Size const & p_size, ePIXEL_FORMAT p_ePixelFormat, uint8_t const * CU_PARAM_UNUSED( p_pBuffer ), ePIXEL_FORMAT CU_PARAM_UNUSED( p_eBufferFormat ) )
		:	m_ePixelFormat( p_ePixelFormat	)
		,	m_size( p_size	)
		,	m_pBuffer( NULL	)
	{
	}

	PxBufferBase::PxBufferBase( PxBufferBase const & p_pixelBuffer )
		:	m_ePixelFormat( p_pixelBuffer.m_ePixelFormat	)
		,	m_size( p_pixelBuffer.m_size	)
		,	m_pBuffer( NULL	)
	{
	}

	PxBufferBase::PxBufferBase( PxBufferBase && p_pixelBuffer )
		:	m_ePixelFormat( std::move( p_pixelBuffer.m_ePixelFormat )	)
		,	m_size( std::move( p_pixelBuffer.m_size )	)
		,	m_pBuffer( std::move( p_pixelBuffer.m_pBuffer )	)
	{
		p_pixelBuffer.m_ePixelFormat	= ePIXEL_FORMAT_A8R8G8B8;
		p_pixelBuffer.m_size			= Size( 0, 0 );
		p_pixelBuffer.m_pBuffer			= NULL;
	}

	PxBufferBase::~PxBufferBase()
	{
	}

	PxBufferBase & PxBufferBase::operator =( PxBufferBase const & p_pixelBuffer )
	{
		clear();
		m_size = p_pixelBuffer.m_size;
		m_ePixelFormat = p_pixelBuffer.m_ePixelFormat;
		init( p_pixelBuffer.m_pBuffer, p_pixelBuffer.m_ePixelFormat );
		return * this;
	}

	PxBufferBase & PxBufferBase::operator =( PxBufferBase && p_pixelBuffer )
	{
		if ( this != & p_pixelBuffer )
		{
			clear();
			m_ePixelFormat	= std::move( p_pixelBuffer.m_ePixelFormat	);
			m_size			= std::move( p_pixelBuffer.m_size	);
			m_pBuffer		= std::move( p_pixelBuffer.m_pBuffer	);
			p_pixelBuffer.m_ePixelFormat	= ePIXEL_FORMAT_A8R8G8B8;
			p_pixelBuffer.m_size			= Size( 0, 0 );
			p_pixelBuffer.m_pBuffer			= NULL;
		}

		return * this;
	}

	void PxBufferBase::clear()
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	void PxBufferBase::init( uint8_t const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat )
	{
		CASTOR_ASSERT( m_pBuffer == NULL );
		uint8_t l_uiBpp = PF::GetBytesPerPixel( m_ePixelFormat );
		uint32_t l_uiSize = count() * l_uiBpp;
		m_pBuffer = new uint8_t[l_uiSize];

		if ( p_pBuffer == NULL )
		{
			memset( m_pBuffer, 0, l_uiSize );
		}
		else
		{
			if ( p_eBufferFormat == m_ePixelFormat )
			{
				memcpy( m_pBuffer, p_pBuffer, l_uiSize );
			}
			else
			{
				PF::ConvertBuffer( p_eBufferFormat, p_pBuffer, count() * PF::GetBytesPerPixel( p_eBufferFormat ), format(), m_pBuffer, size() );
			}
		}
	}

	void PxBufferBase::init( Size const & p_size )
	{
		m_size = p_size;
		init( NULL, ePIXEL_FORMAT_A8R8G8B8 );
	}

	void PxBufferBase::swap( PxBufferBase & p_pixelBuffer )
	{
		std::swap( m_size,			p_pixelBuffer.m_size );
		std::swap( m_ePixelFormat,	p_pixelBuffer.m_ePixelFormat );
		std::swap( m_pBuffer,		p_pixelBuffer.m_pBuffer );
	}

	PxBufferBaseSPtr PxBufferBase::create( Size const & p_size, ePIXEL_FORMAT p_eWantedFormat, uint8_t const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat )
	{
		PxBufferBaseSPtr l_pReturn;

		switch ( p_eWantedFormat )
		{
		case ePIXEL_FORMAT_L8			:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_L8			> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_L16F32F		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_L16F32F		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_L32F			:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_L32F			> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_A8L8			:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_A8L8			> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_AL16F32F		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_AL16F32F		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_AL32F		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_AL32F			> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_A1R5G5B5		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_A1R5G5B5		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_A4R4G4B4		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_A4R4G4B4		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_R5G6B5		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_R5G6B5		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_R8G8B8		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_R8G8B8		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_A8R8G8B8		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_A8R8G8B8		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_RGB16F32F	:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_RGB16F32F		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_ARGB16F32F	:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_ARGB16F32F	> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_RGB32F		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_RGB32F		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_ARGB32F		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_ARGB32F		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DXTC1		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_DXTC1			> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DXTC3		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_DXTC3			> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DXTC5		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_DXTC5			> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_YUY2			:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_YUY2			> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DEPTH16		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_DEPTH16		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DEPTH24		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_DEPTH24		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DEPTH24S8	:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_DEPTH24S8		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DEPTH32		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_DEPTH32		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_STENCIL1		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_STENCIL1		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_STENCIL8		:
			l_pReturn = std::make_shared< PxBuffer<	ePIXEL_FORMAT_STENCIL8		> >( p_size, p_pBuffer, p_eBufferFormat );
			break;
		}

		return l_pReturn;
	}
}
