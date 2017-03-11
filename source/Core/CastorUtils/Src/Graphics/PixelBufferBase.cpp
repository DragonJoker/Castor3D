#include "PixelBuffer.hpp"

namespace Castor
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
		init( p_pixelBuffer.m_buffer.data(), p_pixelBuffer.m_pixelFormat );
		return * this;
	}

	void PxBufferBase::clear()
	{
		m_buffer.clear();
	}

	void PxBufferBase::init( uint8_t const * p_buffer, PixelFormat p_bufferFormat )
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
		init( nullptr, PixelFormat::eA8R8G8B8 );
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

	PxBufferBaseSPtr PxBufferBase::create( Size const & p_size, PixelFormat p_eWantedFormat, uint8_t const * p_buffer, PixelFormat p_eBufferFormat )
	{
		PxBufferBaseSPtr l_return;

		switch ( p_eWantedFormat )
		{
		case PixelFormat::eL8:
			l_return = std::make_shared< PxBuffer< PixelFormat::eL8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eL16F32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::eL16F32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eL32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::eL32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eA8L8:
			l_return = std::make_shared< PxBuffer< PixelFormat::eA8L8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eAL16F32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::eAL16F32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eAL32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::eAL32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eA1R5G5B5:
			l_return = std::make_shared< PxBuffer< PixelFormat::eA1R5G5B5 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eA4R4G4B4:
			l_return = std::make_shared< PxBuffer< PixelFormat::eA4R4G4B4 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR5G6B5:
			l_return = std::make_shared< PxBuffer< PixelFormat::eR5G6B5 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eR8G8B8:
			l_return = std::make_shared< PxBuffer< PixelFormat::eR8G8B8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eB8G8R8:
			l_return = std::make_shared< PxBuffer< PixelFormat::eB8G8R8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eA8R8G8B8:
			l_return = std::make_shared< PxBuffer< PixelFormat::eA8R8G8B8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eA8B8G8R8:
			l_return = std::make_shared< PxBuffer< PixelFormat::eA8B8G8R8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eRGB16F:
			l_return = std::make_shared< PxBuffer< PixelFormat::eRGB16F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eRGBA16F:
			l_return = std::make_shared< PxBuffer< PixelFormat::eRGBA16F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eRGB16F32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::eRGB16F32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eRGBA16F32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::eRGBA16F32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eRGB32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::eRGB32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eRGBA32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::eRGBA32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eDXTC1:
			l_return = std::make_shared< PxBuffer< PixelFormat::eDXTC1 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eDXTC3:
			l_return = std::make_shared< PxBuffer< PixelFormat::eDXTC3 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eDXTC5:
			l_return = std::make_shared< PxBuffer< PixelFormat::eDXTC5 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eYUY2:
			l_return = std::make_shared< PxBuffer< PixelFormat::eYUY2 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD16:
			l_return = std::make_shared< PxBuffer< PixelFormat::eD16 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD24:
			l_return = std::make_shared< PxBuffer< PixelFormat::eD24 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD24S8:
			l_return = std::make_shared< PxBuffer< PixelFormat::eD24S8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD32:
			l_return = std::make_shared< PxBuffer< PixelFormat::eD32 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::eD32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eD32FS8:
			l_return = std::make_shared< PxBuffer< PixelFormat::eD32FS8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eS1:
			l_return = std::make_shared< PxBuffer< PixelFormat::eS1 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::eS8:
			l_return = std::make_shared< PxBuffer< PixelFormat::eS8 > >( p_size, p_buffer, p_eBufferFormat );
			break;
		}

		return l_return;
	}
}
