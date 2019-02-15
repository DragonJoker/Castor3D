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
		initialise( nullptr, PixelFormat::eR8G8B8A8_UNORM );
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
		case PixelFormat::eR8_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eR8_UNORM > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR32_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eR32_SFLOAT > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR8A8_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eR8A8_UNORM > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR32A32_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eR32A32_SFLOAT > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR5G5B5A1_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eR5G5B5A1_UNORM > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR5G6B5_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eR5G6B5_UNORM > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR8G8B8_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eR8G8B8_UNORM > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eB8G8R8_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eB8G8R8_UNORM > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR8G8B8_SRGB:
			result = std::make_shared< PxBuffer< PixelFormat::eR8G8B8_SRGB > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eB8G8R8_SRGB:
			result = std::make_shared< PxBuffer< PixelFormat::eB8G8R8_SRGB > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR8G8B8A8_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eR8G8B8A8_UNORM > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eA8B8G8R8_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eA8B8G8R8_UNORM > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR8G8B8A8_SRGB:
			result = std::make_shared< PxBuffer< PixelFormat::eR8G8B8A8_SRGB > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eA8B8G8R8_SRGB:
			result = std::make_shared< PxBuffer< PixelFormat::eA8B8G8R8_SRGB > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR16G16B16_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eR16G16B16_SFLOAT > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR16G16B16A16_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eR16G16B16A16_SFLOAT > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR32G32B32_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eR32G32B32_SFLOAT > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR32G32B32A32_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eR32G32B32A32_SFLOAT > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eBC1_RGB_UNORM_BLOCK:
			result = std::make_shared< PxBuffer< PixelFormat::eBC1_RGB_UNORM_BLOCK > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eBC3_UNORM_BLOCK:
			result = std::make_shared< PxBuffer< PixelFormat::eBC3_UNORM_BLOCK > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eBC5_UNORM_BLOCK:
			result = std::make_shared< PxBuffer< PixelFormat::eBC5_UNORM_BLOCK > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD16_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eD16_UNORM > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD24_UNORM_S8_UINT:
			result = std::make_shared< PxBuffer< PixelFormat::eD24_UNORM_S8_UINT > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD32_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eD32_UNORM > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD32_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eD32_SFLOAT > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD32_SFLOAT_S8_UINT:
			result = std::make_shared< PxBuffer< PixelFormat::eD32_SFLOAT_S8_UINT > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eS8_UINT:
			result = std::make_shared< PxBuffer< PixelFormat::eS8_UINT > >( p_size, p_buffer, p_eBufferFormat );
			break;
		}

		return result;
	}
}
