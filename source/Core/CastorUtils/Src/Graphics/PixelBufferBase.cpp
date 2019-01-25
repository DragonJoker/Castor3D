#include "PixelBuffer.hpp"

namespace castor
{
	PxBufferBase::PxBufferBase( Size const & p_size, PixelFormat p_format )
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
		initialise( p_pixelBuffer.m_buffer.data(), p_pixelBuffer.m_pixelFormat );
		return * this;
	}

	void PxBufferBase::clear()
	{
		m_buffer.clear();
	}

	void PxBufferBase::initialise( uint8_t const * p_buffer, PixelFormat p_bufferFormat )
	{
		uint8_t bpp = PF::getBytesPerPixel( format() );
		uint32_t newSize = count() * bpp;
		m_buffer.resize( newSize );

		if ( p_buffer == nullptr )
		{
			memset( m_buffer.data(), 0, newSize );
		}
		else
		{
			if ( p_bufferFormat == m_pixelFormat )
			{
				memcpy( m_buffer.data(), p_buffer, newSize );
			}
			else
			{
				PF::convertBuffer( p_bufferFormat
					, p_buffer
					, count() * PF::getBytesPerPixel( p_bufferFormat )
					, format()
					, m_buffer.data()
					, size() );
			}
		}
	}

	void PxBufferBase::initialise( Size const & p_size )
	{
		m_size = p_size;
		initialise( nullptr, PixelFormat::eA8R8G8B8 );
	}

	void PxBufferBase::swap( PxBufferBase & p_pixelBuffer )
	{
		std::swap( m_size, p_pixelBuffer.m_size );
		std::swap( m_pixelFormat, p_pixelBuffer.m_pixelFormat );
		std::swap( m_buffer, p_pixelBuffer.m_buffer );
	}

	void PxBufferBase::flip()
	{
		uint32_t fheight = getHeight();
		uint32_t fwidth = size() / fheight;
		uint32_t hheight = fheight / 2;
		uint8_t * bufferTop = &m_buffer[0];
		uint8_t * bufferBot = &m_buffer[( fheight - 1 ) * fwidth];
		std::vector< uint8_t > buffer( fwidth );

		for ( uint32_t i = 0; i < hheight; i++ )
		{
			std::memcpy( buffer.data(), bufferTop, fwidth );
			std::memcpy( bufferTop, bufferBot, fwidth );
			std::memcpy( bufferBot, buffer.data(), fwidth );
			bufferTop += fwidth;
			bufferBot -= fwidth;
		}
	}

	PxBufferBaseSPtr PxBufferBase::create( Size const & p_size, PixelFormat p_eWantedFormat, uint8_t const * p_buffer, PixelFormat p_eBufferFormat )
	{
		PxBufferBaseSPtr result;

		switch ( p_eWantedFormat )
		{
		case PixelFormat::eL8:
			result = std::make_shared< PxBuffer< PixelFormat::eL8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eL32F:
			result = std::make_shared< PxBuffer< PixelFormat::eL32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eA8L8:
			result = std::make_shared< PxBuffer< PixelFormat::eA8L8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eAL32F:
			result = std::make_shared< PxBuffer< PixelFormat::eAL32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eA1R5G5B5:
			result = std::make_shared< PxBuffer< PixelFormat::eA1R5G5B5 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR5G6B5:
			result = std::make_shared< PxBuffer< PixelFormat::eR5G6B5 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR8G8B8:
			result = std::make_shared< PxBuffer< PixelFormat::eR8G8B8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eB8G8R8:
			result = std::make_shared< PxBuffer< PixelFormat::eB8G8R8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR8G8B8_SRGB:
			result = std::make_shared< PxBuffer< PixelFormat::eR8G8B8_SRGB > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eB8G8R8_SRGB:
			result = std::make_shared< PxBuffer< PixelFormat::eB8G8R8_SRGB > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eA8R8G8B8:
			result = std::make_shared< PxBuffer< PixelFormat::eA8R8G8B8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eA8B8G8R8:
			result = std::make_shared< PxBuffer< PixelFormat::eA8B8G8R8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eA8R8G8B8_SRGB:
			result = std::make_shared< PxBuffer< PixelFormat::eA8R8G8B8_SRGB > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eA8B8G8R8_SRGB:
			result = std::make_shared< PxBuffer< PixelFormat::eA8B8G8R8_SRGB > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eRGB16F:
			result = std::make_shared< PxBuffer< PixelFormat::eRGB16F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eRGBA16F:
			result = std::make_shared< PxBuffer< PixelFormat::eRGBA16F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eRGB32F:
			result = std::make_shared< PxBuffer< PixelFormat::eRGB32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eRGBA32F:
			result = std::make_shared< PxBuffer< PixelFormat::eRGBA32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eDXTC1:
			result = std::make_shared< PxBuffer< PixelFormat::eDXTC1 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eDXTC3:
			result = std::make_shared< PxBuffer< PixelFormat::eDXTC3 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eDXTC5:
			result = std::make_shared< PxBuffer< PixelFormat::eDXTC5 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD16:
			result = std::make_shared< PxBuffer< PixelFormat::eD16 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD24S8:
			result = std::make_shared< PxBuffer< PixelFormat::eD24S8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD32:
			result = std::make_shared< PxBuffer< PixelFormat::eD32 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD32F:
			result = std::make_shared< PxBuffer< PixelFormat::eD32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD32FS8:
			result = std::make_shared< PxBuffer< PixelFormat::eD32FS8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eS8:
			result = std::make_shared< PxBuffer< PixelFormat::eS8 > >( p_size, p_buffer, p_eBufferFormat );
			break;
		}

		return result;
	}
}
