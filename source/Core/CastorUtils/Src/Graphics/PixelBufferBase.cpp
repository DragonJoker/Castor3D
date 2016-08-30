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
		init( nullptr, PixelFormat::A8R8G8B8 );
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
		case PixelFormat::L8:
			l_return = std::make_shared< PxBuffer< PixelFormat::L8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::L16F32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::L16F32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::L32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::L32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::A8L8:
			l_return = std::make_shared< PxBuffer< PixelFormat::A8L8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::AL16F32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::AL16F32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::AL32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::AL32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::A1R5G5B5:
			l_return = std::make_shared< PxBuffer< PixelFormat::A1R5G5B5 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::A4R4G4B4:
			l_return = std::make_shared< PxBuffer< PixelFormat::A4R4G4B4 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::R5G6B5:
			l_return = std::make_shared< PxBuffer< PixelFormat::R5G6B5 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::R8G8B8:
			l_return = std::make_shared< PxBuffer< PixelFormat::R8G8B8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::B8G8R8:
			l_return = std::make_shared< PxBuffer< PixelFormat::B8G8R8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::A8R8G8B8:
			l_return = std::make_shared< PxBuffer< PixelFormat::A8R8G8B8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::A8B8G8R8:
			l_return = std::make_shared< PxBuffer< PixelFormat::A8B8G8R8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::RGB16F:
			l_return = std::make_shared< PxBuffer< PixelFormat::RGB16F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::RGBA16F:
			l_return = std::make_shared< PxBuffer< PixelFormat::RGBA16F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::RGB16F32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::RGB16F32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::RGBA16F32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::RGBA16F32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::RGB32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::RGB32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::RGBA32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::RGBA32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::DXTC1:
			l_return = std::make_shared< PxBuffer< PixelFormat::DXTC1 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::DXTC3:
			l_return = std::make_shared< PxBuffer< PixelFormat::DXTC3 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::DXTC5:
			l_return = std::make_shared< PxBuffer< PixelFormat::DXTC5 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::YUY2:
			l_return = std::make_shared< PxBuffer< PixelFormat::YUY2 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::D16:
			l_return = std::make_shared< PxBuffer< PixelFormat::D16 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::D24:
			l_return = std::make_shared< PxBuffer< PixelFormat::D24 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::D24S8:
			l_return = std::make_shared< PxBuffer< PixelFormat::D24S8 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::D32:
			l_return = std::make_shared< PxBuffer< PixelFormat::D32 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::D32F:
			l_return = std::make_shared< PxBuffer< PixelFormat::D32F > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::S1:
			l_return = std::make_shared< PxBuffer< PixelFormat::S1 > >( p_size, p_buffer, p_eBufferFormat );
			break;

		case PixelFormat::S8:
			l_return = std::make_shared< PxBuffer< PixelFormat::S8 > >( p_size, p_buffer, p_eBufferFormat );
			break;
		}

		return l_return;
	}
}
