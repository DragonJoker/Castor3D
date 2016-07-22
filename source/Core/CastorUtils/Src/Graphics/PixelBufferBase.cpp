#include "PixelBuffer.hpp"

namespace Castor
{
	PxBufferBase::PxBufferBase( Size const & p_size, ePIXEL_FORMAT p_format )
		: m_pixelFormat( p_format )
		, m_size( p_size )
		, m_buffer( 0 )
	{
	}

	PxBufferBase::PxBufferBase( PxBufferBase const & p_pixelBuffer )
		: m_pixelFormat( p_pixelBuffer.m_pixelFormat )
		, m_size( p_pixelBuffer.m_size )
		, m_buffer( 0 )
	{
	}

	PxBufferBase::~PxBufferBase()
	{
	}

	PxBufferBase & PxBufferBase::operator=( PxBufferBase const & p_pixelBuffer )
	{
		clear();
		m_size = p_pixelBuffer.m_size;
		m_pixelFormat = p_pixelBuffer.m_pixelFormat;
		init( p_pixelBuffer.m_buffer.data(), p_pixelBuffer.m_pixelFormat );
		return * this;
	}

	void PxBufferBase::clear()
	{
		m_buffer.clear();
	}

	void PxBufferBase::init( uint8_t const * p_buffer, ePIXEL_FORMAT p_bufferFormat )
	{
		uint8_t l_bpp = PF::GetBytesPerPixel( format() );
		uint32_t l_size = count() * l_bpp;
		m_buffer.resize( l_size );

		if ( p_buffer == nullptr )
		{
			memset( m_buffer.data(), 0, l_size );
		}
		else
		{
			if ( p_bufferFormat == m_pixelFormat )
			{
				memcpy( m_buffer.data(), p_buffer, l_size );
			}
			else
			{
				PF::ConvertBuffer( p_bufferFormat, p_buffer, count() * PF::GetBytesPerPixel( p_bufferFormat ), format(), m_buffer.data(), size() );
			}
		}
	}

	void PxBufferBase::init( Size const & p_size )
	{
		m_size = p_size;
		init( nullptr, ePIXEL_FORMAT_A8R8G8B8 );
	}

	void PxBufferBase::swap( PxBufferBase & p_pixelBuffer )
	{
		std::swap( m_size, p_pixelBuffer.m_size );
		std::swap( m_pixelFormat, p_pixelBuffer.m_pixelFormat );
		std::swap( m_buffer, p_pixelBuffer.m_buffer );
	}

	void PxBufferBase::flip()
	{
		uint32_t l_width = width() * PF::GetBytesPerPixel( m_pixelFormat );
		uint32_t l_height = height();
		uint32_t l_hheight = l_height / 2;
		uint8_t * l_bufferTop = &m_buffer[0];
		uint8_t * l_bufferBot = &m_buffer[( l_height - 1 ) * l_width - 1];
		std::vector< uint8_t > l_buffer( l_width );

		for ( uint32_t i = 0; i < l_hheight; i++ )
		{
			auto l_topLine = l_bufferTop;
			auto l_botLine = l_bufferBot;
			std::memcpy( l_buffer.data(), l_bufferTop, l_width );
			std::memcpy( l_bufferTop, l_bufferBot, l_width );
			std::memcpy( l_bufferBot, l_buffer.data(), l_width );
			l_bufferTop += l_width;
			l_bufferBot -= l_width;
		}
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

		case ePIXEL_FORMAT_RGB16F:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_RGB16F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case ePIXEL_FORMAT_ARGB16F:
			l_return = std::make_shared< PxBuffer< ePIXEL_FORMAT_ARGB16F > >( p_size, p_buffer, p_eBufferFormat );
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
