#include "CastorUtilsPixelFormatTest.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

using namespace castor;

namespace
{
	template< PixelFormat PF, typename Enable = void > struct PixelStreamer;

	template< PixelFormat PF >
	struct PixelStreamer< PF, typename std::enable_if< IsColourFormat< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & stream, Pixel< PF > const & pixel )
		{
			stream << "BPP : ";
			stream.width( 2 );
			stream << uint32_t( getBytesPerPixel( pixel.getFormat() ) );
			stream << ", Format : ";
			stream.width( 10 );
			stream << string::stringCast< CharType >( getFormatName( pixel.getFormat() ) );
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

	template< PixelFormat PF >
	struct PixelStreamer< PF, typename std::enable_if< IsDepthStencilFormat< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & stream, Pixel< PF > const & pixel )
		{
			stream << "BPP : ";
			stream.width( 2 );
			stream << uint32_t( getBytesPerPixel( pixel.getFormat() ) );
			stream << ", Format : ";
			stream.width( 10 );
			stream << string::stringCast< CharType >( getFormatName( pixel.getFormat() ) );
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

	template< typename CharType, PixelFormat PF >
	std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & stream
		, Pixel< PF > const & pixel )
	{
		static PixelStreamer< PF > streamer;
		return streamer( stream, pixel );
	}

	template< PixelFormat PF, typename Enable = void > struct BufferStreamer;

	template< PixelFormat PF >
	struct BufferStreamer< PF, typename std::enable_if< IsColourFormat< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & stream, PxBuffer< PF > const & buffer )
		{
			uint32_t width = buffer.getWidth();
			uint32_t height = buffer.getHeight();
			CharType fill = stream.fill( '0' );

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

	template< PixelFormat PF >
	struct BufferStreamer< PF, typename std::enable_if< IsDepthStencilFormat< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & stream, PxBuffer< PF > const & buffer )
		{
			uint32_t width = buffer.getWidth();
			uint32_t height = buffer.getHeight();
			CharType fill = stream.fill( '0' );

			for ( uint32_t x = 0; x < width; ++x )
			{
				for ( uint32_t y = 0; y < height; ++y )
				{
					auto const & pixel = buffer.at( x, y );
					stream << "0x";
					stream.width( 6 );
					stream << std::hex << int( getD24U( pixel ) );
					stream.width( 2 );
					stream << std::hex << int( getS8U( pixel ) ) << " ";
				}

				stream << std::endl;
			}

			stream.fill( fill );
			return stream;
		}
	};

	template< typename CharType >
	std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & stream, PxBufferBase const & buffer )
	{
		stream << "BPP : ";
		stream.width( 2 );
		stream << uint32_t( getBytesPerPixel( buffer.getFormat() ) );
		stream << ", Format : ";
		stream.width( 10 );
		stream << string::stringCast< CharType >( getFormatName( buffer.getFormat() ) );
		return stream;
	}

	template< typename CharType, PixelFormat PF >
	std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & stream, PxBuffer< PF > const & buffer )
	{
		static BufferStreamer< PF > streamer;
		stream << static_cast< PxBufferBase const & >( buffer );
		return streamer( stream, buffer );
	}

	template< PixelFormat PFDst, PixelFormat PFSrc >
	struct PixelConverter
	{
		void operator()( Pixel< PFSrc > const & p_source )
		{
			Pixel< PFDst > dest( p_source );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Converted pixel : " << dest;
			Logger::logTrace( stream );
		}
	};

	template< PixelFormat PFSrc >
	struct PixelConverter< PFSrc, PFSrc >
	{
		void operator()( Pixel< PFSrc > const & p_source )
		{
		}
	};

	template< PixelFormat PFDst, PixelFormat PFSrc >
	void convertPixel( Pixel< PFSrc > const & p_source )
	{
		PixelConverter< PFDst, PFSrc >()( p_source );
	}

	template< PixelFormat PF >
	void CheckPixelConversions()
	{
		if constexpr ( isColourFormatV< PF > )
		{
			Pixel< PF > source( true );
			setR8U( source, 0x40 );
			setG8U( source, 0x80 );
			setB8U( source, 0xBF );
			setA8U( source, 0xFF );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Source pixel : " << source;
			Logger::logTrace( stream );
			convertPixel< PixelFormat::eR8_UNORM >( source );
			convertPixel< PixelFormat::eR32_SFLOAT >( source );
			convertPixel< PixelFormat::eR8G8_UNORM >( source );
			convertPixel< PixelFormat::eR32G32_SFLOAT >( source );
			convertPixel< PixelFormat::eR5G5B5A1_UNORM >( source );
			convertPixel< PixelFormat::eR5G6B5_UNORM >( source );
			convertPixel< PixelFormat::eR8G8B8_UNORM >( source );
			convertPixel< PixelFormat::eR8G8B8A8_UNORM >( source );
			convertPixel< PixelFormat::eR32G32B32_SFLOAT >( source );
			convertPixel< PixelFormat::eR32G32B32A32_SFLOAT >( source );
			Logger::logTrace( StringStream() << std::endl );
		}
		else if constexpr ( isDepthStencilFormatV< PF > )
		{
			Pixel< PF > source( true );
			setD24U( source, 0x00102040 );
			setS8U( source, 0x80 );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Source pixel : " << source;
			Logger::logTrace( stream );
			convertPixel< PixelFormat::eD16_UNORM >( source );
			convertPixel< PixelFormat::eX8_D24_UNORM >( source );
			convertPixel< PixelFormat::eD24_UNORM_S8_UINT >( source );
			convertPixel< PixelFormat::eS8_UINT >( source );
			Logger::logTrace( StringStream() << std::endl );
		}
	}

	template< PixelFormat PFDst, PixelFormat PFSrc >
	struct BufferConverter
	{
		void operator()( std::shared_ptr< PxBuffer< PFSrc > > source )
		{
			std::shared_ptr< PxBuffer< PFDst > > dest = std::static_pointer_cast< PxBuffer< PFDst > >( PxBufferBase::create( source->getDimensions()
				, PixelFormat( PFDst )
				, source->getPtr()
				, PFSrc ) );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Converted buffer : " << *dest;
			Logger::logTrace( stream );
		}
	};

	template< PixelFormat PFSrc >
	struct BufferConverter< PFSrc, PFSrc >
	{
		using PixelBuffer = PxBuffer< PFSrc >;
		using PixelBufferPtr = std::shared_ptr< PixelBuffer >;

		void operator()( PixelBufferPtr p_source )
		{
		}
	};

	template< PixelFormat PFDst, PixelFormat PFSrc >
	void convertBuffer( std::shared_ptr< PxBuffer< PFSrc > > p_source )
	{
		BufferConverter< PFDst, PFSrc >()( p_source );
	}

	template< PixelFormat PFSrc, typename Enable = void > struct BufferConversionChecker;

	template< PixelFormat PFSrc >
	struct BufferConversionChecker< PFSrc, typename std::enable_if< IsColourFormat< PFSrc >::value >::type >
	{
		void operator()()
		{
			Size size = { 16, 16 };
			std::vector< uint8_t > buffer;
			size_t count = size_t( size.getWidth() ) * size.getHeight() * PixelDefinitionsT< PFSrc >::Size;
			buffer.resize( count );
			uint8_t value = 0;
			Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += PixelDefinitionsT< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				setA8U( pixel, value++ );
				setR8U( pixel, value++ );
				setG8U( pixel, value++ );
				setB8U( pixel, value++ );
			}

			auto source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( size
				, PFSrc
				, buffer.data()
				, PFSrc ) );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Source buffer : " << *source;
			Logger::logTrace( stream );
			convertBuffer< PixelFormat::eR8_UNORM >( source );
			convertBuffer< PixelFormat::eR32_SFLOAT >( source );
			convertBuffer< PixelFormat::eR8G8_UNORM >( source );
			convertBuffer< PixelFormat::eR32G32_SFLOAT >( source );
			convertBuffer< PixelFormat::eR5G5B5A1_UNORM >( source );
			convertBuffer< PixelFormat::eR5G6B5_UNORM >( source );
			convertBuffer< PixelFormat::eR8G8B8_UNORM >( source );
			convertBuffer< PixelFormat::eR8G8B8A8_UNORM >( source );
			convertBuffer< PixelFormat::eR32G32B32_SFLOAT >( source );
			convertBuffer< PixelFormat::eR32G32B32A32_SFLOAT >( source );
			Logger::logTrace( StringStream() << std::endl );
		}
	};

	template< PixelFormat PFSrc >
	struct BufferConversionChecker < PFSrc, typename std::enable_if < IsDepthFormat< PFSrc >::value && PFSrc != PixelFormat::eD24_UNORM_S8_UINT >::type >
	{
		void operator()()
		{
			Size size = { 16, 16 };
			std::vector< uint8_t > buffer;
			size_t count = size_t( size.getWidth() ) * size.getHeight() * PixelDefinitionsT< PFSrc >::Size;
			buffer.resize( count );
			uint32_t depth = 0;
			uint8_t stencil = 0;
			Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += PixelDefinitionsT< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				setD24U( pixel, depth );
				setS8U( pixel, stencil );
				depth += 0x02468ACE;
				stencil++;
			}

			auto source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( size
				, PFSrc
				, buffer.data()
				, PFSrc ) );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Source buffer : " << *source;
			Logger::logTrace( stream );
			convertBuffer< PixelFormat::eD16_UNORM >( source );
			convertBuffer< PixelFormat::eX8_D24_UNORM >( source );
			convertBuffer< PixelFormat::eD24_UNORM_S8_UINT >( source );
			Logger::logTrace( StringStream() << std::endl );
		}
	};

	template< PixelFormat PFSrc >
	struct BufferConversionChecker< PFSrc, typename std::enable_if< PFSrc == PixelFormat::eD24_UNORM_S8_UINT >::type >
	{
		void operator()()
		{
			Size size = { 16, 16 };
			std::vector< uint8_t > buffer;
			size_t count = size_t( size.getWidth() ) * size.getHeight() * PixelDefinitionsT< PFSrc >::Size;
			buffer.resize( count );
			uint32_t depth = 0;
			uint8_t stencil = 0;
			Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += PixelDefinitionsT< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				setD24U( pixel, depth );
				setS8U( pixel, stencil );
				depth += 0x02468ACE;
				stencil++;
			}

			auto source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( size
				, PFSrc
				, buffer.data()
				, PFSrc ) );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Source buffer : " << *source;
			Logger::logTrace( stream );
			convertBuffer< PixelFormat::eD16_UNORM >( source );
			convertBuffer< PixelFormat::eX8_D24_UNORM >( source );
			convertBuffer< PixelFormat::eD24_UNORM_S8_UINT >( source );
			convertBuffer< PixelFormat::eS8_UINT >( source );
			Logger::logTrace( StringStream() << std::endl );
		}
	};

	template< PixelFormat PFSrc >
	struct BufferConversionChecker < PFSrc, typename std::enable_if < IsStencilFormat< PFSrc >::value && PFSrc != PixelFormat::eD24_UNORM_S8_UINT >::type >
	{
		void operator()()
		{
			Size size = { 16, 16 };
			std::vector< uint8_t > buffer;
			size_t count = size_t( size.getWidth()) * size.getHeight() * PixelDefinitionsT< PFSrc >::Size;
			buffer.resize( count );
			uint8_t value = 0;
			Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += PixelDefinitionsT< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				setD24U( pixel, value++ );
				setS8U( pixel, value++ );
			}

			auto source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( size
				, PFSrc
				, buffer.data()
				, PFSrc ) );
			auto stream = castor::makeStringStream();
			stream.width( 20 );
			stream << "Source buffer : " << *source;
			Logger::logTrace( stream );
			convertBuffer< PixelFormat::eD24_UNORM_S8_UINT >( source );
			convertBuffer< PixelFormat::eS8_UINT >( source );
			Logger::logTrace( StringStream() << std::endl );
		}
	};

	template< PixelFormat PF >
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

	CastorUtilsPixelFormatTest::~CastorUtilsPixelFormatTest()
	{
	}

	void CastorUtilsPixelFormatTest::doRegisterTests()
	{
		doRegisterTest( "TestPixelConversions", std::bind( &CastorUtilsPixelFormatTest::TestPixelConversions, this ) );
		doRegisterTest( "TestBufferConversions", std::bind( &CastorUtilsPixelFormatTest::TestBufferConversions, this ) );
	}

	void CastorUtilsPixelFormatTest::TestPixelConversions()
	{
		CheckPixelConversions< PixelFormat::eR8_UNORM >();
		CheckPixelConversions< PixelFormat::eR32_SFLOAT >();
		CheckPixelConversions< PixelFormat::eR8G8_UNORM >();
		CheckPixelConversions< PixelFormat::eR32G32_SFLOAT >();
		CheckPixelConversions< PixelFormat::eR5G5B5A1_UNORM >();
		CheckPixelConversions< PixelFormat::eR5G6B5_UNORM >();
		CheckPixelConversions< PixelFormat::eR8G8B8_UNORM >();
		CheckPixelConversions< PixelFormat::eR8G8B8A8_UNORM >();
		CheckPixelConversions< PixelFormat::eR32G32B32_SFLOAT >();
		CheckPixelConversions< PixelFormat::eR32G32B32A32_SFLOAT >();
		CheckPixelConversions< PixelFormat::eD16_UNORM >();
		CheckPixelConversions< PixelFormat::eX8_D24_UNORM >();
		CheckPixelConversions< PixelFormat::eD24_UNORM_S8_UINT >();
		CheckPixelConversions< PixelFormat::eS8_UINT >();
	}

	void CastorUtilsPixelFormatTest::TestBufferConversions()
	{
		CheckBufferConversions< PixelFormat::eR8_UNORM >();
		CheckBufferConversions< PixelFormat::eR32_SFLOAT >();
		CheckBufferConversions< PixelFormat::eR8G8_UNORM >();
		CheckBufferConversions< PixelFormat::eR32G32_SFLOAT >();
		CheckBufferConversions< PixelFormat::eR5G5B5A1_UNORM >();
		CheckBufferConversions< PixelFormat::eR5G6B5_UNORM >();
		CheckBufferConversions< PixelFormat::eR8G8B8_UNORM >();
		CheckBufferConversions< PixelFormat::eR8G8B8A8_UNORM >();
		CheckBufferConversions< PixelFormat::eR32G32B32_SFLOAT >();
		CheckBufferConversions< PixelFormat::eR32G32B32A32_SFLOAT >();
		CheckBufferConversions< PixelFormat::eD16_UNORM >();
		CheckBufferConversions< PixelFormat::eX8_D24_UNORM >();
		CheckBufferConversions< PixelFormat::eD24_UNORM_S8_UINT >();
		CheckBufferConversions< PixelFormat::eS8_UINT >();
	}
}
