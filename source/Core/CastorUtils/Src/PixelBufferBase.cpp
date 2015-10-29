#include "PixelBuffer.hpp"

namespace Castor
{
	PxBufferBase::PxBufferBase( Size const & p_size, ePIXEL_FORMAT p_ePixelFormat )
		: m_ePixelFormat( p_ePixelFormat )
		, m_size( p_size )
		, m_pBuffer( 0 )
	{
	}

	PxBufferBase::PxBufferBase( PxBufferBase const & p_pixelBuffer )
		: m_ePixelFormat( p_pixelBuffer.m_ePixelFormat )
		, m_size( p_pixelBuffer.m_size )
		, m_pBuffer( 0 )
	{
	}

	PxBufferBase::~PxBufferBase()
	{
	}

	PxBufferBase & PxBufferBase::operator=( PxBufferBase const & p_pixelBuffer )
	{
		clear();
		m_size = p_pixelBuffer.m_size;
		m_ePixelFormat = p_pixelBuffer.m_ePixelFormat;
		init( p_pixelBuffer.m_pBuffer.data(), p_pixelBuffer.m_ePixelFormat );
		return * this;
	}

	void PxBufferBase::clear()
	{
		m_pBuffer.clear();
	}

	void PxBufferBase::init( uint8_t const * p_buffer, ePIXEL_FORMAT p_eBufferFormat )
	{
		uint8_t l_uiBpp = PF::GetBytesPerPixel( format() );
		uint32_t l_uiSize = count() * l_uiBpp;
		m_pBuffer.resize( l_uiSize );

		if ( p_buffer == NULL )
		{
			memset( m_pBuffer.data(), 0, l_uiSize );
		}
		else
		{
			if ( p_eBufferFormat == m_ePixelFormat )
			{
				memcpy( m_pBuffer.data(), p_buffer, l_uiSize );
			}
			else
			{
				PF::ConvertBuffer( p_eBufferFormat, p_buffer, count() * PF::GetBytesPerPixel( p_eBufferFormat ), format(), m_pBuffer.data(), size() );
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
		std::swap( m_size, p_pixelBuffer.m_size );
		std::swap( m_ePixelFormat, p_pixelBuffer.m_ePixelFormat );
		std::swap( m_pBuffer, p_pixelBuffer.m_pBuffer );
	}

	PxBufferBaseSPtr PxBufferBase::create( Size const & p_size, ePIXEL_FORMAT p_eWantedFormat, uint8_t const * p_buffer, ePIXEL_FORMAT p_eBufferFormat )
	{
		PxBufferBaseSPtr l_return;

		switch ( p_eWantedFormat )
		{
		case ePIXEL_FORMAT_L8:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_L8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_L16F32F:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_L16F32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_L32F:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_L32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_A8L8:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_A8L8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_AL16F32F:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_AL16F32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_AL32F:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_AL32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_A1R5G5B5:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_A1R5G5B5 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_A4R4G4B4:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_A4R4G4B4 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_R5G6B5:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_R5G6B5 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_R8G8B8:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_R8G8B8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_B8G8R8:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_B8G8R8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_A8R8G8B8:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_A8R8G8B8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_A8B8G8R8:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_A8B8G8R8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_RGB16F32F:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_RGB16F32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_ARGB16F32F:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_ARGB16F32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_RGB32F:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_RGB32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_ARGB32F:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_ARGB32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DXTC1:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_DXTC1 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DXTC3:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_DXTC3 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DXTC5:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_DXTC5 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_YUY2:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_YUY2 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DEPTH16:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_DEPTH16 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DEPTH24:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_DEPTH24 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DEPTH24S8:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_DEPTH24S8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_DEPTH32:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_DEPTH32 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_STENCIL1:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_STENCIL1 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_STENCIL8:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_STENCIL8 > >( p_size, p_buffer, p_eBufferFormat );
			break;
		}

		return l_return;
	}
}
