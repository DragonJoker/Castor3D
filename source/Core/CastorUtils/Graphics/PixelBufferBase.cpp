#include "CastorUtils/Graphics/PixelBuffer.hpp"

namespace castor
{
	PxBufferBase::PxBufferBase( Size const & size, PixelFormat p_format )
		: m_pixelFormat( p_format )
		, m_size( size )
		, m_buffer( 0 )
	{
	}

	PxBufferBase::PxBufferBase( PxBufferBase const & pixelBuffer )
		: m_pixelFormat( pixelBuffer.m_pixelFormat )
		, m_size( pixelBuffer.m_size )
		, m_buffer( 0 )
	{
	}

	PxBufferBase::~PxBufferBase()
	{
	}

	PxBufferBase & PxBufferBase::operator=( PxBufferBase const & pixelBuffer )
	{
		clear();
		m_size = pixelBuffer.m_size;
		m_pixelFormat = pixelBuffer.m_pixelFormat;
		initialise( pixelBuffer.m_buffer.data(), pixelBuffer.m_pixelFormat );
		return * this;
	}

	void PxBufferBase::clear()
	{
		m_buffer.clear();
	}

	void PxBufferBase::initialise( uint8_t const * buffer
		, PixelFormat bufferFormat )
	{
		if ( PF::isCompressed( getFormat() ) )
		{
			uint8_t bpp = PF::getBytesPerPixel( getFormat() );
			uint32_t newSize = getCount();
			CU_Require( newSize % 16u == 0 );
			newSize *= bpp;
			newSize /= 16u;
			m_buffer.resize( newSize );
		}
		else
		{
			uint8_t bpp = PF::getBytesPerPixel( getFormat() );
			uint32_t newSize = getCount() * bpp;
			m_buffer.resize( newSize );
		}

		auto newSize = m_buffer.size();

		if ( !buffer )
		{
			memset( m_buffer.data(), 0, newSize );
		}
		else
		{
			if ( bufferFormat == m_pixelFormat )
			{
				memcpy( m_buffer.data(), buffer, newSize );
			}
			else
			{
				PF::convertBuffer( bufferFormat
					, buffer
					, getCount() * PF::getBytesPerPixel( bufferFormat )
					, getFormat()
					, m_buffer.data()
					, getSize() );
			}
		}
	}

	void PxBufferBase::initialise( Size const & size )
	{
		m_size = size;
		initialise( nullptr, PixelFormat::eR8G8B8A8_UNORM );
	}

	void PxBufferBase::swap( PxBufferBase & pixelBuffer )
	{
		std::swap( m_size, pixelBuffer.m_size );
		std::swap( m_pixelFormat, pixelBuffer.m_pixelFormat );
		std::swap( m_buffer, pixelBuffer.m_buffer );
	}

	void PxBufferBase::flip()
	{
		if ( !PF::isCompressed( m_pixelFormat ) )
		{
			uint32_t fullHeight = getHeight();
			uint32_t linePitch = getSize() / fullHeight;
			uint32_t halfHeight = fullHeight / 2;
			uint8_t * bufferTop = &m_buffer[0];
			uint8_t * bufferBot = &m_buffer[( fullHeight - 1 ) * linePitch];
			std::vector< uint8_t > buffer( linePitch );

			for ( uint32_t i = 0; i < halfHeight; i++ )
			{
				std::memcpy( buffer.data(), bufferTop, linePitch );
				std::memcpy( bufferTop, bufferBot, linePitch );
				std::memcpy( bufferBot, buffer.data(), linePitch );
				bufferTop += linePitch;
				bufferBot -= linePitch;
			}
		}
		else
		{
			m_flipped = !m_flipped;
		}
	}

	PxBufferBaseSPtr PxBufferBase::create( Size const & size
		, PixelFormat wantedFormat
		, uint8_t const * buffer
		, PixelFormat bufferFormat )
	{
		PxBufferBaseSPtr result;

		switch ( wantedFormat )
		{
		case PixelFormat::eR8_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eR8_UNORM > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eR32_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eR32_SFLOAT > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eR8A8_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eR8A8_UNORM > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eR32A32_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eR32A32_SFLOAT > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eR5G5B5A1_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eR5G5B5A1_UNORM > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eR5G6B5_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eR5G6B5_UNORM > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eR8G8B8_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eR8G8B8_UNORM > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eB8G8R8_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eB8G8R8_UNORM > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eR8G8B8_SRGB:
			result = std::make_shared< PxBuffer< PixelFormat::eR8G8B8_SRGB > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eB8G8R8_SRGB:
			result = std::make_shared< PxBuffer< PixelFormat::eB8G8R8_SRGB > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eR8G8B8A8_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eR8G8B8A8_UNORM > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eA8B8G8R8_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eA8B8G8R8_UNORM > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eR8G8B8A8_SRGB:
			result = std::make_shared< PxBuffer< PixelFormat::eR8G8B8A8_SRGB > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eA8B8G8R8_SRGB:
			result = std::make_shared< PxBuffer< PixelFormat::eA8B8G8R8_SRGB > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eR16G16B16_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eR16G16B16_SFLOAT > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eR16G16B16A16_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eR16G16B16A16_SFLOAT > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eR32G32B32_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eR32G32B32_SFLOAT > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eR32G32B32A32_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eR32G32B32A32_SFLOAT > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eBC1_RGB_UNORM_BLOCK:
			result = std::make_shared< PxBuffer< PixelFormat::eBC1_RGB_UNORM_BLOCK > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eBC3_UNORM_BLOCK:
			result = std::make_shared< PxBuffer< PixelFormat::eBC3_UNORM_BLOCK > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eBC5_UNORM_BLOCK:
			result = std::make_shared< PxBuffer< PixelFormat::eBC5_UNORM_BLOCK > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eD16_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eD16_UNORM > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eD24_UNORM_S8_UINT:
			result = std::make_shared< PxBuffer< PixelFormat::eD24_UNORM_S8_UINT > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eD32_UNORM:
			result = std::make_shared< PxBuffer< PixelFormat::eD32_UNORM > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eD32_SFLOAT:
			result = std::make_shared< PxBuffer< PixelFormat::eD32_SFLOAT > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eD32_SFLOAT_S8_UINT:
			result = std::make_shared< PxBuffer< PixelFormat::eD32_SFLOAT_S8_UINT > >( size, buffer, bufferFormat );
			break;

		case PixelFormat::eS8_UINT:
			result = std::make_shared< PxBuffer< PixelFormat::eS8_UINT > >( size, buffer, bufferFormat );
			break;

		default:
			CU_Failure( "Unsupported PixelFormat" );
			break;
		}

		return result;
	}
}
