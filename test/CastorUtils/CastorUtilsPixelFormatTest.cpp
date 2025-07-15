#include "CastorUtilsPixelFormatTest.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

namespace
{
	template< c3d::PixelFormat PF, typename Enable = void > struct PixelStreamer;

	template< c3d::PixelFormat PF >
	struct PixelStreamer< PF, std::enable_if_t< c3d::IsColourFormat< PF >::value > >
	{
		c3d::OutputStream & operator()( c3d::OutputStream & stream, c3d::Pixel< PF > const & pixel )
		{
			stream << "BPP : ";
			stream.width( 2 );
			stream << uint32_t( c3d::getBytesPerPixel( pixel.getFormat() ) );
			stream << ", Format : ";
			stream.width( 10 );
			stream << c3d::getFormatName( pixel.getFormat() );
			stream << ", Value : (";
			stream.width( 3 );
			stream << int( getR8U( pixel ) );
			stream << ", ";
			stream.width( 3 );
			stream << int( getG8U( pixel ) );
			stream << ", ";
			stream.width( 3 );
			stream << int( getB8U( pixel ) );
			stream << ", ";
			stream.width( 3 );
			stream << int( getA8U( pixel ) );
			stream << ") (";
			stream.precision( 3 );
			stream.width( 5 );
			stream << getR32F( pixel );
			stream << ", ";
			stream.precision( 3 );
			stream.width( 5 );
			stream << getG32F( pixel );
			stream << ", ";
			stream.precision( 3 );
			stream.width( 5 );
			stream << getB32F( pixel );
			stream << ", ";
			stream.precision( 3 );
			stream.width( 5 );
			stream << getA32F( pixel );
			stream << ")";
			return stream;
		}
	};

	template< c3d::PixelFormat PF >
	struct PixelStreamer< PF, std::enable_if_t< c3d::IsDepthStencilFormat< PF >::value > >
	{
		c3d::OutputStream & operator()( c3d::OutputStream & stream, c3d::Pixel< PF > const & pixel )
		{
			stream << "BPP : ";
			stream.width( 2 );
			stream << uint32_t( c3d::getBytesPerPixel( pixel.getFormat() ) );
			stream << ", Format : ";
			stream.width( 10 );
			stream << c3d::getFormatName( pixel.getFormat() );
			stream << ", Value : (";
			stream.precision( 3 );
			stream.width( 10 );
			stream << int( getD32F( pixel ) );
			stream << ", ";
			stream.width( 3 );
			stream << int( getS8U( pixel ) );
			stream << ")";
			return stream;
		}
	};

	template< c3d::PixelFormat PF >
	c3d::OutputStream & operator<<( c3d::OutputStream & stream
		, c3d::Pixel< PF > const & pixel )
	{
		static PixelStreamer< PF > streamer;
		return streamer( stream, pixel );
	}

	template< c3d::PixelFormat PF, typename Enable = void > struct BufferStreamer;

	template< c3d::PixelFormat PF >
	struct BufferStreamer< PF, std::enable_if_t< c3d::IsColourFormat< PF >::value > >
	{
		c3d::OutputStream & operator()( c3d::OutputStream & stream, c3d::PxBuffer< PF > const & buffer )
		{
			auto width = buffer.getWidth();
			auto height = buffer.getHeight();
			auto fill = stream.fill( cuT( '0' ) );

			for ( uint32_t x = 0; x < width; ++x )
			{
				for ( uint32_t y = 0; y < height; ++y )
				{
					auto const & pixel = buffer.at( x, y );
					stream << "0x";
					stream.width( 2 );
					stream << std::hex << int( getA8U( pixel ) );
					stream.width( 2 );
					stream << std::hex << int( getR8U( pixel ) );
					stream.width( 2 );
					stream << std::hex << int( getG8U( pixel ) );
					stream.width( 2 );
					stream << std::hex << int( getB8U( pixel ) ) << " ";
				}

				stream << std::endl;
			}

			stream.fill( fill );
			return stream;
		}
	};

	template< c3d::PixelFormat PF >
	struct BufferStreamer< PF, std::enable_if_t< c3d::IsDepthStencilFormat< PF >::value > >
	{
		c3d::OutputStream & operator()( c3d::OutputStream & stream, c3d::PxBuffer< PF > const & buffer )
		{
			auto width = buffer.getWidth();
			auto height = buffer.getHeight();
			auto fill = stream.fill( cuT( '0' ) );

			for ( uint32_t x = 0; x < width; ++x )
			{
				for ( uint32_t y = 0; y < height; ++y )
				{
					auto const & pixel = buffer.at( x, y );
					stream << "0x";
					stream.width( 6 );
					stream << std::hex << int( c3d::getD24U( pixel ) );
					stream.width( 2 );
					stream << std::hex << int( c3d::getS8U( pixel ) ) << " ";
				}

				stream << std::endl;
			}

			stream.fill( fill );
			return stream;
		}
	};

	c3d::OutputStream & operator <<( c3d::OutputStream & stream, c3d::PxBufferBase const & buffer )
	{
		stream << "BPP : ";
		stream.width( 2 );
		stream << uint32_t( c3d::getBytesPerPixel( buffer.getFormat() ) );
		stream << ", Format : ";
		stream.width( 10 );
		stream << c3d::getFormatName( buffer.getFormat() );
		return stream;
	}

	template< c3d::PixelFormat PF >
	c3d::OutputStream & operator <<( c3d::OutputStream & stream, c3d::PxBuffer< PF > const & buffer )
	{
		static BufferStreamer< PF > streamer;
		stream << static_cast< c3d::PxBufferBase const & >( buffer );
		return streamer( stream, buffer );
	}

	template< c3d::PixelFormat PFDst, c3d::PixelFormat PFSrc >
	struct PixelConverter
	{
		void operator()( c3d::Pixel< PFSrc > const & source )
		{
			c3d::Pixel< PFDst > dest( source );
			auto stream = c3d::makeStringStream();
			stream.width( 20 );
			stream << "Converted pixel : " << dest;
			c3d::Logger::logTrace( stream );
		}
	};

	template< c3d::PixelFormat PFSrc >
	struct PixelConverter< PFSrc, PFSrc >
	{
		void operator()( c3d::Pixel< PFSrc > const & source )
		{
		}
	};

	template< c3d::PixelFormat PFDst, c3d::PixelFormat PFSrc >
	void convertPixel( c3d::Pixel< PFSrc > const & source )
	{
		PixelConverter< PFDst, PFSrc >()( source );
	}

	template< c3d::PixelFormat PF >
	void CheckPixelConversions()
	{
		if constexpr ( c3d::isColourFormatV< PF > )
		{
			c3d::Pixel< PF > source( true );
			c3d::setR8U( source, 0x40 );
			c3d::setG8U( source, 0x80 );
			c3d::setB8U( source, 0xBF );
			c3d::setA8U( source, 0xFF );
			auto stream = c3d::makeStringStream();
			stream.width( 20 );
			stream << "Source pixel : " << source;
			c3d::Logger::logTrace( stream );
			convertPixel< c3d::PixelFormat::eR8_UNORM >( source );
			convertPixel< c3d::PixelFormat::eR32_SFLOAT >( source );
			convertPixel< c3d::PixelFormat::eR8G8_UNORM >( source );
			convertPixel< c3d::PixelFormat::eR32G32_SFLOAT >( source );
			convertPixel< c3d::PixelFormat::eR5G5B5A1_UNORM >( source );
			convertPixel< c3d::PixelFormat::eR5G6B5_UNORM >( source );
			convertPixel< c3d::PixelFormat::eR8G8B8_UNORM >( source );
			convertPixel< c3d::PixelFormat::eR8G8B8A8_UNORM >( source );
			convertPixel< c3d::PixelFormat::eR32G32B32_SFLOAT >( source );
			convertPixel< c3d::PixelFormat::eR32G32B32A32_SFLOAT >( source );
			c3d::Logger::logTrace( c3d::StringStream() << std::endl );
		}
		else if constexpr ( c3d::isDepthStencilFormatV< PF > )
		{
			c3d::Pixel< PF > source( true );
			c3d::setD24U( source, 0x00102040 );
			c3d::setS8U( source, 0x80 );
			auto stream = c3d::makeStringStream();
			stream.width( 20 );
			stream << "Source pixel : " << source;
			c3d::Logger::logTrace( stream );
			convertPixel< c3d::PixelFormat::eD16_UNORM >( source );
			convertPixel< c3d::PixelFormat::eX8_D24_UNORM >( source );
			convertPixel< c3d::PixelFormat::eD24_UNORM_S8_UINT >( source );
			convertPixel< c3d::PixelFormat::eS8_UINT >( source );
			c3d::Logger::logTrace( c3d::StringStream() << std::endl );
		}
	}

	template< c3d::PixelFormat PFDst, c3d::PixelFormat PFSrc >
	struct BufferConverter
	{
		void operator()( c3d::PxBuffer< PFSrc > & source )
		{
			auto destination = c3d::PxBufferBase::create( source.getDimensions()
				, c3d::PixelFormat( PFDst )
				, source.getPtr()
				, PFSrc );
			auto dest = static_cast< c3d::PxBuffer< PFDst > & >( *destination );
			auto stream = c3d::makeStringStream();
			stream.width( 20 );
			stream << "Converted buffer : " << dest;
			c3d::Logger::logTrace( stream );
		}
	};

	template< c3d::PixelFormat PFSrc >
	struct BufferConverter< PFSrc, PFSrc >
	{
		using PixelBuffer = c3d::PxBuffer< PFSrc >;

		void operator()( PixelBuffer & source )
		{
		}
	};

	template< c3d::PixelFormat PFDst, c3d::PixelFormat PFSrc >
	void convertBuffer( c3d::PxBuffer< PFSrc > & source )
	{
		BufferConverter< PFDst, PFSrc >()( source );
	}

	template< c3d::PixelFormat PFSrc, typename Enable = void >
	struct BufferConversionChecker;

	template< c3d::PixelFormat PFSrc >
	struct BufferConversionChecker< PFSrc, std::enable_if_t< c3d::IsColourFormat< PFSrc >::value > >
	{
		void operator()()
		{
			c3d::Size size = { 16, 16 };
			c3d::Vector< uint8_t > buffer;
			size_t count = size_t( size.getWidth() ) * size.getHeight() * c3d::PixelDefinitionsT< PFSrc >::Size;
			buffer.resize( count );
			uint8_t value = 0;
			c3d::Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += c3d::PixelDefinitionsT< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				c3d::setA8U( pixel, value++ );
				c3d::setR8U( pixel, value++ );
				c3d::setG8U( pixel, value++ );
				c3d::setB8U( pixel, value++ );
			}

			auto src = c3d::PxBufferBase::create( size
				, PFSrc
				, buffer.data()
				, PFSrc );
			auto & source = static_cast< c3d::PxBuffer< PFSrc > & >( *src );
			auto stream = c3d::makeStringStream();
			stream.width( 20 );
			stream << "Source buffer : " << source;
			c3d::Logger::logTrace( stream );
			convertBuffer< c3d::PixelFormat::eR8_UNORM >( source );
			convertBuffer< c3d::PixelFormat::eR32_SFLOAT >( source );
			convertBuffer< c3d::PixelFormat::eR8G8_UNORM >( source );
			convertBuffer< c3d::PixelFormat::eR32G32_SFLOAT >( source );
			convertBuffer< c3d::PixelFormat::eR5G5B5A1_UNORM >( source );
			convertBuffer< c3d::PixelFormat::eR5G6B5_UNORM >( source );
			convertBuffer< c3d::PixelFormat::eR8G8B8_UNORM >( source );
			convertBuffer< c3d::PixelFormat::eR8G8B8A8_UNORM >( source );
			convertBuffer< c3d::PixelFormat::eR32G32B32_SFLOAT >( source );
			convertBuffer< c3d::PixelFormat::eR32G32B32A32_SFLOAT >( source );
			c3d::Logger::logTrace( c3d::StringStream() << std::endl );
		}
	};

	template< c3d::PixelFormat PFSrc >
	struct BufferConversionChecker < PFSrc, std::enable_if_t < c3d::IsDepthFormat< PFSrc >::value && PFSrc != c3d::PixelFormat::eD24_UNORM_S8_UINT > >
	{
		void operator()()
		{
			c3d::Size size = { 16, 16 };
			c3d::Vector< uint8_t > buffer;
			size_t count = size_t( size.getWidth() ) * size.getHeight() * c3d::PixelDefinitionsT< PFSrc >::Size;
			buffer.resize( count );
			uint32_t depth = 0;
			uint8_t stencil = 0;
			c3d::Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += c3d::PixelDefinitionsT< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				c3d::setD24U( pixel, depth );
				c3d::setS8U( pixel, stencil );
				depth += 0x02468ACE;
				stencil++;
			}

			auto src = c3d::PxBufferBase::create( size
				, PFSrc
				, buffer.data()
				, PFSrc );
			auto & source = static_cast< c3d::PxBuffer< PFSrc > & >( *src );
			auto stream = c3d::makeStringStream();
			stream.width( 20 );
			stream << "Source buffer : " << source;
			c3d::Logger::logTrace( stream );
			convertBuffer< c3d::PixelFormat::eD16_UNORM >( source );
			convertBuffer< c3d::PixelFormat::eX8_D24_UNORM >( source );
			convertBuffer< c3d::PixelFormat::eD24_UNORM_S8_UINT >( source );
			c3d::Logger::logTrace( c3d::StringStream() << std::endl );
		}
	};

	template< c3d::PixelFormat PFSrc >
	struct BufferConversionChecker< PFSrc, std::enable_if_t< PFSrc == c3d::PixelFormat::eD24_UNORM_S8_UINT > >
	{
		void operator()()
		{
			c3d::Size size = { 16, 16 };
			c3d::Vector< uint8_t > buffer;
			size_t count = size_t( size.getWidth() ) * size.getHeight() * c3d::PixelDefinitionsT< PFSrc >::Size;
			buffer.resize( count );
			uint32_t depth = 0;
			uint8_t stencil = 0;
			c3d::Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += c3d::PixelDefinitionsT< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				c3d::setD24U( pixel, depth );
				c3d::setS8U( pixel, stencil );
				depth += 0x02468ACE;
				stencil++;
			}

			auto src = c3d::PxBufferBase::create( size
				, PFSrc
				, buffer.data()
				, PFSrc );
			auto & source = static_cast< c3d::PxBuffer< PFSrc > & >( *src );
			auto stream = c3d::makeStringStream();
			stream.width( 20 );
			stream << "Source buffer : " << source;
			c3d::Logger::logTrace( stream );
			convertBuffer< c3d::PixelFormat::eD16_UNORM >( source );
			convertBuffer< c3d::PixelFormat::eX8_D24_UNORM >( source );
			convertBuffer< c3d::PixelFormat::eD24_UNORM_S8_UINT >( source );
			convertBuffer< c3d::PixelFormat::eS8_UINT >( source );
			c3d::Logger::logTrace( c3d::StringStream() << std::endl );
		}
	};

	template< c3d::PixelFormat PFSrc >
	struct BufferConversionChecker < PFSrc, std::enable_if_t < c3d::IsStencilFormat< PFSrc >::value && PFSrc != c3d::PixelFormat::eD24_UNORM_S8_UINT > >
	{
		void operator()()
		{
			c3d::Size size = { 16, 16 };
			c3d::Vector< uint8_t > buffer;
			size_t count = size_t( size.getWidth()) * size.getHeight() * c3d::PixelDefinitionsT< PFSrc >::Size;
			buffer.resize( count );
			uint8_t value = 0;
			c3d::Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += c3d::PixelDefinitionsT< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				c3d::setD24U( pixel, value++ );
				c3d::setS8U( pixel, value++ );
			}

			auto src = c3d::PxBufferBase::create( size
				, PFSrc
				, buffer.data()
				, PFSrc );
			auto & source = static_cast< c3d::PxBuffer< PFSrc > & >( *src );
			auto stream = c3d::makeStringStream();
			stream.width( 20 );
			stream << "Source buffer : " << source;
			c3d::Logger::logTrace( stream );
			convertBuffer< c3d::PixelFormat::eD24_UNORM_S8_UINT >( source );
			convertBuffer< c3d::PixelFormat::eS8_UINT >( source );
			c3d::Logger::logTrace( c3d::StringStream() << std::endl );
		}
	};

	template< c3d::PixelFormat PF >
	void CheckBufferConversions()
	{
		BufferConversionChecker< PF >()();
	}
}

namespace Testing
{
	CastorUtilsPixelFormatTest::CastorUtilsPixelFormatTest()
		:	TestCase( "CastorUtilsPixelFormatTest" )
	{
	}

	void CastorUtilsPixelFormatTest::doRegisterTests()
	{
		doRegisterTest( "TestPixelConversions", [this](){ TestPixelConversions(); } );
		doRegisterTest( "TestBufferConversions", [this](){ TestBufferConversions(); } );
	}

	void CastorUtilsPixelFormatTest::TestPixelConversions()
	{
		CheckPixelConversions< c3d::PixelFormat::eR8_UNORM >();
		CheckPixelConversions< c3d::PixelFormat::eR32_SFLOAT >();
		CheckPixelConversions< c3d::PixelFormat::eR8G8_UNORM >();
		CheckPixelConversions< c3d::PixelFormat::eR32G32_SFLOAT >();
		CheckPixelConversions< c3d::PixelFormat::eR5G5B5A1_UNORM >();
		CheckPixelConversions< c3d::PixelFormat::eR5G6B5_UNORM >();
		CheckPixelConversions< c3d::PixelFormat::eR8G8B8_UNORM >();
		CheckPixelConversions< c3d::PixelFormat::eR8G8B8A8_UNORM >();
		CheckPixelConversions< c3d::PixelFormat::eR32G32B32_SFLOAT >();
		CheckPixelConversions< c3d::PixelFormat::eR32G32B32A32_SFLOAT >();
		CheckPixelConversions< c3d::PixelFormat::eD16_UNORM >();
		CheckPixelConversions< c3d::PixelFormat::eX8_D24_UNORM >();
		CheckPixelConversions< c3d::PixelFormat::eD24_UNORM_S8_UINT >();
		CheckPixelConversions< c3d::PixelFormat::eS8_UINT >();
	}

	void CastorUtilsPixelFormatTest::TestBufferConversions()
	{
		CheckBufferConversions< c3d::PixelFormat::eR8_UNORM >();
		CheckBufferConversions< c3d::PixelFormat::eR32_SFLOAT >();
		CheckBufferConversions< c3d::PixelFormat::eR8G8_UNORM >();
		CheckBufferConversions< c3d::PixelFormat::eR32G32_SFLOAT >();
		CheckBufferConversions< c3d::PixelFormat::eR5G5B5A1_UNORM >();
		CheckBufferConversions< c3d::PixelFormat::eR5G6B5_UNORM >();
		CheckBufferConversions< c3d::PixelFormat::eR8G8B8_UNORM >();
		CheckBufferConversions< c3d::PixelFormat::eR8G8B8A8_UNORM >();
		CheckBufferConversions< c3d::PixelFormat::eR32G32B32_SFLOAT >();
		CheckBufferConversions< c3d::PixelFormat::eR32G32B32A32_SFLOAT >();
		CheckBufferConversions< c3d::PixelFormat::eD16_UNORM >();
		CheckBufferConversions< c3d::PixelFormat::eX8_D24_UNORM >();
		CheckBufferConversions< c3d::PixelFormat::eD24_UNORM_S8_UINT >();
		CheckBufferConversions< c3d::PixelFormat::eS8_UINT >();
	}
}
