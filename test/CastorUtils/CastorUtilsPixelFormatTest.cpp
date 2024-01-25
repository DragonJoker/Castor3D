#include "CastorUtilsPixelFormatTest.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

namespace
{
	template< castor::PixelFormat PF, typename Enable = void > struct PixelStreamer;

	template< castor::PixelFormat PF >
	struct PixelStreamer< PF, std::enable_if_t< castor::IsColourFormat< PF >::value > >
	{
		castor::OutputStream & operator()( castor::OutputStream & stream, castor::Pixel< PF > const & pixel )
		{
			stream << "BPP : ";
			stream.width( 2 );
			stream << uint32_t( getBytesPerPixel( pixel.getFormat() ) );
			stream << ", Format : ";
			stream.width( 10 );
			stream << getFormatName( pixel.getFormat() );
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

	template< castor::PixelFormat PF >
	struct PixelStreamer< PF, std::enable_if_t< castor::IsDepthStencilFormat< PF >::value > >
	{
		castor::OutputStream & operator()( castor::OutputStream & stream, castor::Pixel< PF > const & pixel )
		{
			stream << "BPP : ";
			stream.width( 2 );
			stream << uint32_t( getBytesPerPixel( pixel.getFormat() ) );
			stream << ", Format : ";
			stream.width( 10 );
			stream << getFormatName( pixel.getFormat() );
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

	template< castor::PixelFormat PF >
	castor::OutputStream & operator<<( castor::OutputStream & stream
		, castor::Pixel< PF > const & pixel )
	{
		static PixelStreamer< PF > streamer;
		return streamer( stream, pixel );
	}

	template< castor::PixelFormat PF, typename Enable = void > struct BufferStreamer;

	template< castor::PixelFormat PF >
	struct BufferStreamer< PF, std::enable_if_t< castor::IsColourFormat< PF >::value > >
	{
		castor::OutputStream & operator()( castor::OutputStream & stream, castor::PxBuffer< PF > const & buffer )
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

	template< castor::PixelFormat PF >
	struct BufferStreamer< PF, std::enable_if_t< castor::IsDepthStencilFormat< PF >::value > >
	{
		castor::OutputStream & operator()( castor::OutputStream & stream, castor::PxBuffer< PF > const & buffer )
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
					stream << std::hex << int( castor::getD24U( pixel ) );
					stream.width( 2 );
					stream << std::hex << int( castor::getS8U( pixel ) ) << " ";
				}

				stream << std::endl;
			}

			stream.fill( fill );
			return stream;
		}
	};

	castor::OutputStream & operator <<( castor::OutputStream & stream, castor::PxBufferBase const & buffer )
	{
		stream << "BPP : ";
		stream.width( 2 );
		stream << uint32_t( getBytesPerPixel( buffer.getFormat() ) );
		stream << ", Format : ";
		stream.width( 10 );
		stream << getFormatName( buffer.getFormat() );
		return stream;
	}

	template< castor::PixelFormat PF >
	castor::OutputStream & operator <<( castor::OutputStream & stream, castor::PxBuffer< PF > const & buffer )
	{
		static BufferStreamer< PF > streamer;
		stream << static_cast< castor::PxBufferBase const & >( buffer );
		return streamer( stream, buffer );
	}

	template< castor::PixelFormat PFDst, castor::PixelFormat PFSrc >
	struct PixelConverter
	{
		void operator()( castor::Pixel< PFSrc > const & source )
		{
			castor::Pixel< PFDst > dest( source );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Converted pixel : " << dest;
			castor::Logger::logTrace( stream );
		}
	};

	template< castor::PixelFormat PFSrc >
	struct PixelConverter< PFSrc, PFSrc >
	{
		void operator()( castor::Pixel< PFSrc > const & source )
		{
		}
	};

	template< castor::PixelFormat PFDst, castor::PixelFormat PFSrc >
	void convertPixel( castor::Pixel< PFSrc > const & source )
	{
		PixelConverter< PFDst, PFSrc >()( source );
	}

	template< castor::PixelFormat PF >
	void CheckPixelConversions()
	{
		if constexpr ( castor::isColourFormatV< PF > )
		{
			castor::Pixel< PF > source( true );
			castor::setR8U( source, 0x40 );
			castor::setG8U( source, 0x80 );
			castor::setB8U( source, 0xBF );
			castor::setA8U( source, 0xFF );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Source pixel : " << source;
			castor::Logger::logTrace( stream );
			convertPixel< castor::PixelFormat::eR8_UNORM >( source );
			convertPixel< castor::PixelFormat::eR32_SFLOAT >( source );
			convertPixel< castor::PixelFormat::eR8G8_UNORM >( source );
			convertPixel< castor::PixelFormat::eR32G32_SFLOAT >( source );
			convertPixel< castor::PixelFormat::eR5G5B5A1_UNORM >( source );
			convertPixel< castor::PixelFormat::eR5G6B5_UNORM >( source );
			convertPixel< castor::PixelFormat::eR8G8B8_UNORM >( source );
			convertPixel< castor::PixelFormat::eR8G8B8A8_UNORM >( source );
			convertPixel< castor::PixelFormat::eR32G32B32_SFLOAT >( source );
			convertPixel< castor::PixelFormat::eR32G32B32A32_SFLOAT >( source );
			castor::Logger::logTrace( castor::StringStream() << std::endl );
		}
		else if constexpr ( castor::isDepthStencilFormatV< PF > )
		{
			castor::Pixel< PF > source( true );
			castor::setD24U( source, 0x00102040 );
			castor::setS8U( source, 0x80 );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Source pixel : " << source;
			castor::Logger::logTrace( stream );
			convertPixel< castor::PixelFormat::eD16_UNORM >( source );
			convertPixel< castor::PixelFormat::eX8_D24_UNORM >( source );
			convertPixel< castor::PixelFormat::eD24_UNORM_S8_UINT >( source );
			convertPixel< castor::PixelFormat::eS8_UINT >( source );
			castor::Logger::logTrace( castor::StringStream() << std::endl );
		}
	}

	template< castor::PixelFormat PFDst, castor::PixelFormat PFSrc >
	struct BufferConverter
	{
		void operator()( castor::PxBuffer< PFSrc > & source )
		{
			auto destination = castor::PxBufferBase::create( source.getDimensions()
				, castor::PixelFormat( PFDst )
				, source.getPtr()
				, PFSrc );
			auto dest = static_cast< castor::PxBuffer< PFDst > & >( *destination );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Converted buffer : " << dest;
			castor::Logger::logTrace( stream );
		}
	};

	template< castor::PixelFormat PFSrc >
	struct BufferConverter< PFSrc, PFSrc >
	{
		using PixelBuffer = castor::PxBuffer< PFSrc >;

		void operator()( PixelBuffer & source )
		{
		}
	};

	template< castor::PixelFormat PFDst, castor::PixelFormat PFSrc >
	void convertBuffer( castor::PxBuffer< PFSrc > & source )
	{
		BufferConverter< PFDst, PFSrc >()( source );
	}

	template< castor::PixelFormat PFSrc, typename Enable = void >
	struct BufferConversionChecker;

	template< castor::PixelFormat PFSrc >
	struct BufferConversionChecker< PFSrc, std::enable_if_t< castor::IsColourFormat< PFSrc >::value > >
	{
		void operator()()
		{
			castor::Size size = { 16, 16 };
			castor::Vector< uint8_t > buffer;
			size_t count = size_t( size.getWidth() ) * size.getHeight() * castor::PixelDefinitionsT< PFSrc >::Size;
			buffer.resize( count );
			uint8_t value = 0;
			castor::Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += castor::PixelDefinitionsT< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				castor::setA8U( pixel, value++ );
				castor::setR8U( pixel, value++ );
				castor::setG8U( pixel, value++ );
				castor::setB8U( pixel, value++ );
			}

			auto src = castor::PxBufferBase::create( size
				, PFSrc
				, buffer.data()
				, PFSrc );
			auto & source = static_cast< castor::PxBuffer< PFSrc > & >( *src );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Source buffer : " << source;
			castor::Logger::logTrace( stream );
			convertBuffer< castor::PixelFormat::eR8_UNORM >( source );
			convertBuffer< castor::PixelFormat::eR32_SFLOAT >( source );
			convertBuffer< castor::PixelFormat::eR8G8_UNORM >( source );
			convertBuffer< castor::PixelFormat::eR32G32_SFLOAT >( source );
			convertBuffer< castor::PixelFormat::eR5G5B5A1_UNORM >( source );
			convertBuffer< castor::PixelFormat::eR5G6B5_UNORM >( source );
			convertBuffer< castor::PixelFormat::eR8G8B8_UNORM >( source );
			convertBuffer< castor::PixelFormat::eR8G8B8A8_UNORM >( source );
			convertBuffer< castor::PixelFormat::eR32G32B32_SFLOAT >( source );
			convertBuffer< castor::PixelFormat::eR32G32B32A32_SFLOAT >( source );
			castor::Logger::logTrace( castor::StringStream() << std::endl );
		}
	};

	template< castor::PixelFormat PFSrc >
	struct BufferConversionChecker < PFSrc, std::enable_if_t < castor::IsDepthFormat< PFSrc >::value && PFSrc != castor::PixelFormat::eD24_UNORM_S8_UINT > >
	{
		void operator()()
		{
			castor::Size size = { 16, 16 };
			castor::Vector< uint8_t > buffer;
			size_t count = size_t( size.getWidth() ) * size.getHeight() * castor::PixelDefinitionsT< PFSrc >::Size;
			buffer.resize( count );
			uint32_t depth = 0;
			uint8_t stencil = 0;
			castor::Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += castor::PixelDefinitionsT< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				castor::setD24U( pixel, depth );
				castor::setS8U( pixel, stencil );
				depth += 0x02468ACE;
				stencil++;
			}

			auto src = castor::PxBufferBase::create( size
				, PFSrc
				, buffer.data()
				, PFSrc );
			auto & source = static_cast< castor::PxBuffer< PFSrc > & >( *src );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Source buffer : " << source;
			castor::Logger::logTrace( stream );
			convertBuffer< castor::PixelFormat::eD16_UNORM >( source );
			convertBuffer< castor::PixelFormat::eX8_D24_UNORM >( source );
			convertBuffer< castor::PixelFormat::eD24_UNORM_S8_UINT >( source );
			castor::Logger::logTrace( castor::StringStream() << std::endl );
		}
	};

	template< castor::PixelFormat PFSrc >
	struct BufferConversionChecker< PFSrc, std::enable_if_t< PFSrc == castor::PixelFormat::eD24_UNORM_S8_UINT > >
	{
		void operator()()
		{
			castor::Size size = { 16, 16 };
			castor::Vector< uint8_t > buffer;
			size_t count = size_t( size.getWidth() ) * size.getHeight() * castor::PixelDefinitionsT< PFSrc >::Size;
			buffer.resize( count );
			uint32_t depth = 0;
			uint8_t stencil = 0;
			castor::Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += castor::PixelDefinitionsT< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				castor::setD24U( pixel, depth );
				castor::setS8U( pixel, stencil );
				depth += 0x02468ACE;
				stencil++;
			}

			auto src = castor::PxBufferBase::create( size
				, PFSrc
				, buffer.data()
				, PFSrc );
			auto & source = static_cast< castor::PxBuffer< PFSrc > & >( *src );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Source buffer : " << source;
			castor::Logger::logTrace( stream );
			convertBuffer< castor::PixelFormat::eD16_UNORM >( source );
			convertBuffer< castor::PixelFormat::eX8_D24_UNORM >( source );
			convertBuffer< castor::PixelFormat::eD24_UNORM_S8_UINT >( source );
			convertBuffer< castor::PixelFormat::eS8_UINT >( source );
			castor::Logger::logTrace( castor::StringStream() << std::endl );
		}
	};

	template< castor::PixelFormat PFSrc >
	struct BufferConversionChecker < PFSrc, std::enable_if_t < castor::IsStencilFormat< PFSrc >::value && PFSrc != castor::PixelFormat::eD24_UNORM_S8_UINT > >
	{
		void operator()()
		{
			castor::Size size = { 16, 16 };
			castor::Vector< uint8_t > buffer;
			size_t count = size_t( size.getWidth()) * size.getHeight() * castor::PixelDefinitionsT< PFSrc >::Size;
			buffer.resize( count );
			uint8_t value = 0;
			castor::Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += castor::PixelDefinitionsT< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				castor::setD24U( pixel, value++ );
				castor::setS8U( pixel, value++ );
			}

			auto src = castor::PxBufferBase::create( size
				, PFSrc
				, buffer.data()
				, PFSrc );
			auto & source = static_cast< castor::PxBuffer< PFSrc > & >( *src );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Source buffer : " << source;
			castor::Logger::logTrace( stream );
			convertBuffer< castor::PixelFormat::eD24_UNORM_S8_UINT >( source );
			convertBuffer< castor::PixelFormat::eS8_UINT >( source );
			castor::Logger::logTrace( castor::StringStream() << std::endl );
		}
	};

	template< castor::PixelFormat PF >
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
		CheckPixelConversions< castor::PixelFormat::eR8_UNORM >();
		CheckPixelConversions< castor::PixelFormat::eR32_SFLOAT >();
		CheckPixelConversions< castor::PixelFormat::eR8G8_UNORM >();
		CheckPixelConversions< castor::PixelFormat::eR32G32_SFLOAT >();
		CheckPixelConversions< castor::PixelFormat::eR5G5B5A1_UNORM >();
		CheckPixelConversions< castor::PixelFormat::eR5G6B5_UNORM >();
		CheckPixelConversions< castor::PixelFormat::eR8G8B8_UNORM >();
		CheckPixelConversions< castor::PixelFormat::eR8G8B8A8_UNORM >();
		CheckPixelConversions< castor::PixelFormat::eR32G32B32_SFLOAT >();
		CheckPixelConversions< castor::PixelFormat::eR32G32B32A32_SFLOAT >();
		CheckPixelConversions< castor::PixelFormat::eD16_UNORM >();
		CheckPixelConversions< castor::PixelFormat::eX8_D24_UNORM >();
		CheckPixelConversions< castor::PixelFormat::eD24_UNORM_S8_UINT >();
		CheckPixelConversions< castor::PixelFormat::eS8_UINT >();
	}

	void CastorUtilsPixelFormatTest::TestBufferConversions()
	{
		CheckBufferConversions< castor::PixelFormat::eR8_UNORM >();
		CheckBufferConversions< castor::PixelFormat::eR32_SFLOAT >();
		CheckBufferConversions< castor::PixelFormat::eR8G8_UNORM >();
		CheckBufferConversions< castor::PixelFormat::eR32G32_SFLOAT >();
		CheckBufferConversions< castor::PixelFormat::eR5G5B5A1_UNORM >();
		CheckBufferConversions< castor::PixelFormat::eR5G6B5_UNORM >();
		CheckBufferConversions< castor::PixelFormat::eR8G8B8_UNORM >();
		CheckBufferConversions< castor::PixelFormat::eR8G8B8A8_UNORM >();
		CheckBufferConversions< castor::PixelFormat::eR32G32B32_SFLOAT >();
		CheckBufferConversions< castor::PixelFormat::eR32G32B32A32_SFLOAT >();
		CheckBufferConversions< castor::PixelFormat::eD16_UNORM >();
		CheckBufferConversions< castor::PixelFormat::eX8_D24_UNORM >();
		CheckBufferConversions< castor::PixelFormat::eD24_UNORM_S8_UINT >();
		CheckBufferConversions< castor::PixelFormat::eS8_UINT >();
	}
}
