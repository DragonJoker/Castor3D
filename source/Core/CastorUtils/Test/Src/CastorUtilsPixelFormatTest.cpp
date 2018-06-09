#include "CastorUtilsPixelFormatTest.hpp"

#include <Graphics/PixelBuffer.hpp>

using namespace castor;

namespace
{
	template< PixelFormat PF, typename Enable = void > struct PixelStreamer;

	template< PixelFormat PF >
	struct PixelStreamer< PF, typename std::enable_if< IsColourFormat< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & p_stream, Pixel< PF > const & p_pixel )
		{
			p_stream << "BPP : ";
			p_stream.width( 2 );
			p_stream << uint32_t( PF::getBytesPerPixel( p_pixel.getFormat() ) );
			p_stream << ", Format : ";
			p_stream.width( 10 );
			p_stream << string::stringCast< CharType >( PF::getFormatName( p_pixel.getFormat() ) );
			p_stream << ", Value : (";
			p_stream.width( 3 );
			p_stream << int( PF::getByteRed( p_pixel ) );
			p_stream << ", ";
			p_stream.width( 3 );
			p_stream << int( PF::getByteGreen( p_pixel ) );
			p_stream << ", ";
			p_stream.width( 3 );
			p_stream << int( PF::getByteBlue( p_pixel ) );
			p_stream << ", ";
			p_stream.width( 3 );
			p_stream << int( PF::getByteAlpha( p_pixel ) );
			p_stream << ") (";
			p_stream.precision( 3 );
			p_stream.width( 5 );
			p_stream << PF::getFloatRed( p_pixel );
			p_stream << ", ";
			p_stream.precision( 3 );
			p_stream.width( 5 );
			p_stream << PF::getFloatGreen( p_pixel );
			p_stream << ", ";
			p_stream.precision( 3 );
			p_stream.width( 5 );
			p_stream << PF::getFloatBlue( p_pixel );
			p_stream << ", ";
			p_stream.precision( 3 );
			p_stream.width( 5 );
			p_stream << PF::getFloatAlpha( p_pixel );
			p_stream << ")";
			return p_stream;
		}
	};

	template< PixelFormat PF >
	struct PixelStreamer< PF, typename std::enable_if< IsDepthStencilFormat< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & p_stream, Pixel< PF > const & p_pixel )
		{
			p_stream << "BPP : ";
			p_stream.width( 2 );
			p_stream << uint32_t( PF::getBytesPerPixel( p_pixel.getFormat() ) );
			p_stream << ", Format : ";
			p_stream.width( 10 );
			p_stream << string::stringCast< CharType >( PF::getFormatName( p_pixel.getFormat() ) );
			p_stream << ", Value : (";
			p_stream.width( 11 );
			p_stream << int( PF::getUInt32Depth( p_pixel ) );
			p_stream << ", ";
			p_stream.width( 3 );
			p_stream << int( PF::getByteStencil( p_pixel ) );
			p_stream << ") (";
			p_stream.precision( 3 );
			p_stream.width( 10 );
			p_stream << PF::getFloatDepth( p_pixel );
			p_stream << ", ";
			p_stream.precision( 3 );
			p_stream.width( 5 );
			p_stream << PF::getFloatStencil( p_pixel );
			p_stream << ")";
			return p_stream;
		}
	};

	template< typename CharType, PixelFormat PF >
	std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & p_stream, Pixel< PF > const & p_pixel )
	{
		return PixelStreamer< PF >()( p_stream, p_pixel );
	}

	template< PixelFormat PF, typename Enable = void > struct BufferStreamer;

	template< PixelFormat PF >
	struct BufferStreamer< PF, typename std::enable_if< IsColourFormat< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & p_stream, PxBuffer< PF > const & p_buffer )
		{
			uint32_t width = p_buffer.dimensions().getWidth();
			uint32_t height = p_buffer.dimensions().getHeight();
			CharType fill = p_stream.fill( '0' );

			for ( uint32_t x = 0; x < width; ++x )
			{
				typename PxBuffer< PF >::column const & column = p_buffer[x];

				for ( uint32_t y = 0; y < height; ++y )
				{
					typename PxBuffer< PF >::pixel const & pixel = column[y];
					p_stream << "0x";
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::getByteAlpha( pixel ) );
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::getByteRed( pixel ) );
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::getByteGreen( pixel ) );
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::getByteBlue( pixel ) ) << " ";
				}

				p_stream << std::endl;
			}

			p_stream.fill( fill );
			return p_stream;
		}
	};

	template< PixelFormat PF >
	struct BufferStreamer< PF, typename std::enable_if< IsDepthStencilFormat< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & p_stream, PxBuffer< PF > const & p_buffer )
		{
			uint8_t const * data = p_buffer.constPtr();
			uint32_t width = p_buffer.dimensions().getWidth();
			uint32_t height = p_buffer.dimensions().getHeight();
			CharType fill = p_stream.fill( '0' );

			for ( uint32_t x = 0; x < width; ++x )
			{
				typename PxBuffer< PF >::column const & column = p_buffer[x];

				for ( uint32_t y = 0; y < height; ++y )
				{
					typename PxBuffer< PF >::pixel const & pixel = column[y];
					p_stream << "0x";
					p_stream.width( 6 );
					p_stream << std::hex << int( PF::getUInt24Depth( pixel ) );
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::getByteStencil( pixel ) ) << " ";
				}

				p_stream << std::endl;
			}

			p_stream.fill( fill );
			return p_stream;
		}
	};

	template< typename CharType >
	std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & p_stream, PxBufferBase const & p_buffer )
	{
		p_stream << "BPP : ";
		p_stream.width( 2 );
		p_stream << uint32_t( PF::getBytesPerPixel( p_buffer.format() ) );
		p_stream << ", Format : ";
		p_stream.width( 10 );
		p_stream << string::stringCast< CharType >( PF::getFormatName( p_buffer.format() ) );
		return p_stream;
	}

	template< typename CharType, PixelFormat PF >
	std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & p_stream, PxBuffer< PF > const & p_buffer )
	{
		p_stream << static_cast< PxBufferBase const & >( p_buffer );
		return BufferStreamer< PF >()( p_stream, p_buffer );
	}

	template< PixelFormat PFDst, PixelFormat PFSrc >
	struct PixelConverter
	{
		void operator()( Pixel< PFSrc > const & p_source )
		{
			Pixel< PFDst > dest( p_source );
			StringStream stream;
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

	template< PixelFormat PFSrc, typename Enable = void > struct PixelConversionChecker;

	template< PixelFormat PFSrc >
	struct PixelConversionChecker< PFSrc, typename std::enable_if< IsColourFormat< PFSrc >::value >::type >
	{
		void operator()()
		{
			Pixel< PFSrc > source( true );
			PF::setByteRed( source, 0x40 );
			PF::setByteGreen( source, 0x80 );
			PF::setByteBlue( source, 0xBF );
			PF::setByteAlpha( source, 0xFF );
			StringStream stream;
			stream.width( 20 );
			stream << "Source pixel : " << source;
			Logger::logTrace( stream );
			convertPixel< PixelFormat::eL8 >( source );
			convertPixel< PixelFormat::eL32F >( source );
			convertPixel< PixelFormat::eA8L8 >( source );
			convertPixel< PixelFormat::eAL32F >( source );
			convertPixel< PixelFormat::eA1R5G5B5 >( source );
			convertPixel< PixelFormat::eR5G6B5 >( source );
			convertPixel< PixelFormat::eR8G8B8 >( source );
			convertPixel< PixelFormat::eA8R8G8B8 >( source );
			convertPixel< PixelFormat::eRGB32F >( source );
			convertPixel< PixelFormat::eRGBA32F >( source );
			Logger::logTrace( StringStream() << std::endl );
		}
	};

	template< PixelFormat PFSrc >
	struct PixelConversionChecker< PFSrc, typename std::enable_if< IsDepthStencilFormat< PFSrc >::value >::type >
	{
		void operator()()
		{
			Pixel< PFSrc > source( true );
			PF::setUInt32Depth( source, 0x10204080 );
			PF::setByteStencil( source, 0x80 );
			StringStream stream;
			stream.width( 20 );
			stream << "Source pixel : " << source;
			Logger::logTrace( stream );
			convertPixel< PixelFormat::eD16 >( source );
			convertPixel< PixelFormat::eD32 >( source );
			convertPixel< PixelFormat::eD24S8 >( source );
			convertPixel< PixelFormat::eS8 >( source );
			Logger::logTrace( StringStream() << std::endl );
		}
	};

	template< PixelFormat PF >
	void CheckPixelConversions()
	{
		PixelConversionChecker< PF >()();
	}

	template< PixelFormat PFDst, PixelFormat PFSrc >
	struct BufferConverter
	{
		void operator()( std::shared_ptr< PxBuffer< PFSrc > > p_source )
		{
			std::shared_ptr< PxBuffer< PFDst > > dest = std::static_pointer_cast< PxBuffer< PFDst > >( PxBufferBase::create( p_source->dimensions(), PixelFormat( PFDst ), p_source->ptr(), PixelFormat( PFSrc ) ) );
			StringStream stream;
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
			size_t count = size.getWidth() * size.getHeight() * PixelDefinitions< PFSrc >::Size;
			buffer.resize( count );
			uint8_t value = 0;
			Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += PixelDefinitions< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				PF::setByteAlpha( pixel, value++ );
				PF::setByteRed( pixel, value++ );
				PF::setByteGreen( pixel, value++ );
				PF::setByteBlue( pixel, value++ );
			}

			std::shared_ptr< PxBuffer< PFSrc > > source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( size, PixelFormat( PFSrc ), buffer.data(), PixelFormat( PFSrc ) ) );
			StringStream stream;
			stream.width( 20 );
			stream << "Source buffer : " << *source;
			Logger::logTrace( stream );
			convertBuffer< PixelFormat::eL8 >( source );
			convertBuffer< PixelFormat::eL32F >( source );
			convertBuffer< PixelFormat::eA8L8 >( source );
			convertBuffer< PixelFormat::eAL32F >( source );
			convertBuffer< PixelFormat::eA1R5G5B5 >( source );
			convertBuffer< PixelFormat::eR5G6B5 >( source );
			convertBuffer< PixelFormat::eR8G8B8 >( source );
			convertBuffer< PixelFormat::eA8R8G8B8 >( source );
			convertBuffer< PixelFormat::eRGB32F >( source );
			convertBuffer< PixelFormat::eRGBA32F >( source );
			Logger::logTrace( StringStream() << std::endl );
		}
	};

	template< PixelFormat PFSrc >
	struct BufferConversionChecker < PFSrc, typename std::enable_if < IsDepthFormat< PFSrc >::value && PFSrc != PixelFormat::eD24S8 >::type >
	{
		void operator()()
		{
			Size size = { 16, 16 };
			std::vector< uint8_t > buffer;
			size_t count = size.getWidth() * size.getHeight() * PixelDefinitions< PFSrc >::Size;
			buffer.resize( count );
			uint32_t depth = 0;
			uint8_t stencil = 0;
			Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += PixelDefinitions< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				PF::setUInt32Depth( pixel, depth );
				PF::setByteStencil( pixel, stencil );
				depth += 0x02468ACE;
				stencil++;
			}

			std::shared_ptr< PxBuffer< PFSrc > > source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( size, PixelFormat( PFSrc ), buffer.data(), PixelFormat( PFSrc ) ) );
			StringStream stream;
			stream.width( 20 );
			stream << "Source buffer : " << *source;
			Logger::logTrace( stream );
			convertBuffer< PixelFormat::eD16 >( source );
			convertBuffer< PixelFormat::eD32 >( source );
			convertBuffer< PixelFormat::eD24S8 >( source );
			Logger::logTrace( StringStream() << std::endl );
		}
	};

	template< PixelFormat PFSrc >
	struct BufferConversionChecker< PFSrc, typename std::enable_if< PFSrc == PixelFormat::eD24S8 >::type >
	{
		void operator()()
		{
			Size size = { 16, 16 };
			std::vector< uint8_t > buffer;
			size_t count = size.getWidth() * size.getHeight() * PixelDefinitions< PFSrc >::Size;
			buffer.resize( count );
			uint32_t depth = 0;
			uint8_t stencil = 0;
			Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += PixelDefinitions< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				PF::setUInt32Depth( pixel, depth );
				PF::setByteStencil( pixel, stencil );
				depth += 0x02468ACE;
				stencil++;
			}

			std::shared_ptr< PxBuffer< PFSrc > > source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( size, PixelFormat( PFSrc ), buffer.data(), PixelFormat( PFSrc ) ) );
			StringStream stream;
			stream.width( 20 );
			stream << "Source buffer : " << *source;
			Logger::logTrace( stream );
			convertBuffer< PixelFormat::eD16 >( source );
			convertBuffer< PixelFormat::eD32 >( source );
			convertBuffer< PixelFormat::eD24S8 >( source );
			convertBuffer< PixelFormat::eS8 >( source );
			Logger::logTrace( StringStream() << std::endl );
		}
	};

	template< PixelFormat PFSrc >
	struct BufferConversionChecker < PFSrc, typename std::enable_if < IsStencilFormat< PFSrc >::value && PFSrc != PixelFormat::eD24S8 >::type >
	{
		void operator()()
		{
			Size size = { 16, 16 };
			std::vector< uint8_t > buffer;
			size_t count = size.getWidth() * size.getHeight() * PixelDefinitions< PFSrc >::Size;
			buffer.resize( count );
			uint8_t value = 0;
			Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += PixelDefinitions< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				PF::setUInt32Depth( pixel, value++ );
				PF::setByteStencil( pixel, value++ );
			}

			std::shared_ptr< PxBuffer< PFSrc > > source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( size, PixelFormat( PFSrc ), buffer.data(), PixelFormat( PFSrc ) ) );
			StringStream stream;
			stream.width( 20 );
			stream << "Source buffer : " << *source;
			Logger::logTrace( stream );
			convertBuffer< PixelFormat::eD24S8 >( source );
			convertBuffer< PixelFormat::eS8 >( source );
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
		CheckPixelConversions< PixelFormat::eL8 >();
		CheckPixelConversions< PixelFormat::eL32F >();
		CheckPixelConversions< PixelFormat::eA8L8 >();
		CheckPixelConversions< PixelFormat::eAL32F >();
		CheckPixelConversions< PixelFormat::eA1R5G5B5 >();
		CheckPixelConversions< PixelFormat::eR5G6B5 >();
		CheckPixelConversions< PixelFormat::eR8G8B8 >();
		CheckPixelConversions< PixelFormat::eA8R8G8B8 >();
		CheckPixelConversions< PixelFormat::eRGB32F >();
		CheckPixelConversions< PixelFormat::eRGBA32F >();
		CheckPixelConversions< PixelFormat::eD16 >();
		CheckPixelConversions< PixelFormat::eD32 >();
		CheckPixelConversions< PixelFormat::eD24S8 >();
		CheckPixelConversions< PixelFormat::eS8 >();
	}

	void CastorUtilsPixelFormatTest::TestBufferConversions()
	{
		CheckBufferConversions< PixelFormat::eL8 >();
		CheckBufferConversions< PixelFormat::eL32F >();
		CheckBufferConversions< PixelFormat::eA8L8 >();
		CheckBufferConversions< PixelFormat::eAL32F >();
		CheckBufferConversions< PixelFormat::eA1R5G5B5 >();
		CheckBufferConversions< PixelFormat::eR5G6B5 >();
		CheckBufferConversions< PixelFormat::eR8G8B8 >();
		CheckBufferConversions< PixelFormat::eA8R8G8B8 >();
		CheckBufferConversions< PixelFormat::eRGB32F >();
		CheckBufferConversions< PixelFormat::eRGBA32F >();
		CheckBufferConversions< PixelFormat::eD16 >();
		CheckBufferConversions< PixelFormat::eD32 >();
		CheckBufferConversions< PixelFormat::eD24S8 >();
		CheckBufferConversions< PixelFormat::eS8 >();
	}
}
