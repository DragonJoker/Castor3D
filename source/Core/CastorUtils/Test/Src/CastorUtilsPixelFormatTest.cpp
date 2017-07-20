#include "CastorUtilsPixelFormatTest.hpp"

#include <Graphics/PixelBuffer.hpp>

using namespace Castor;

namespace
{
	template< PixelFormat PF, typename Enable = void > struct PixelStreamer;

	template< PixelFormat PF >
	struct PixelStreamer< PF, typename std::enable_if< is_colour_format< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & p_stream, Pixel< PF > const & p_pixel )
		{
			p_stream << "BPP : ";
			p_stream.width( 2 );
			p_stream << uint32_t( PF::GetBytesPerPixel( p_pixel.get_format() ) );
			p_stream << ", Format : ";
			p_stream.width( 10 );
			p_stream << string::string_cast< CharType >( PF::GetFormatName( p_pixel.get_format() ) );
			p_stream << ", Value : (";
			p_stream.width( 3 );
			p_stream << int( PF::GetByteRed( p_pixel ) );
			p_stream << ", ";
			p_stream.width( 3 );
			p_stream << int( PF::GetByteGreen( p_pixel ) );
			p_stream << ", ";
			p_stream.width( 3 );
			p_stream << int( PF::GetByteBlue( p_pixel ) );
			p_stream << ", ";
			p_stream.width( 3 );
			p_stream << int( PF::GetByteAlpha( p_pixel ) );
			p_stream << ") (";
			p_stream.precision( 3 );
			p_stream.width( 5 );
			p_stream << PF::GetFloatRed( p_pixel );
			p_stream << ", ";
			p_stream.precision( 3 );
			p_stream.width( 5 );
			p_stream << PF::GetFloatGreen( p_pixel );
			p_stream << ", ";
			p_stream.precision( 3 );
			p_stream.width( 5 );
			p_stream << PF::GetFloatBlue( p_pixel );
			p_stream << ", ";
			p_stream.precision( 3 );
			p_stream.width( 5 );
			p_stream << PF::GetFloatAlpha( p_pixel );
			p_stream << ")";
			return p_stream;
		}
	};

	template< PixelFormat PF >
	struct PixelStreamer< PF, typename std::enable_if< is_depth_stencil_format< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & p_stream, Pixel< PF > const & p_pixel )
		{
			p_stream << "BPP : ";
			p_stream.width( 2 );
			p_stream << uint32_t( PF::GetBytesPerPixel( p_pixel.get_format() ) );
			p_stream << ", Format : ";
			p_stream.width( 10 );
			p_stream << string::string_cast< CharType >( PF::GetFormatName( p_pixel.get_format() ) );
			p_stream << ", Value : (";
			p_stream.width( 11 );
			p_stream << int( PF::GetUInt32Depth( p_pixel ) );
			p_stream << ", ";
			p_stream.width( 3 );
			p_stream << int( PF::GetByteStencil( p_pixel ) );
			p_stream << ") (";
			p_stream.precision( 3 );
			p_stream.width( 10 );
			p_stream << PF::GetFloatDepth( p_pixel );
			p_stream << ", ";
			p_stream.precision( 3 );
			p_stream.width( 5 );
			p_stream << PF::GetFloatStencil( p_pixel );
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
	struct BufferStreamer< PF, typename std::enable_if< is_colour_format< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & p_stream, PxBuffer< PF > const & p_buffer )
		{
			uint8_t const * data = p_buffer.const_ptr();
			uint32_t width = p_buffer.dimensions().width();
			uint32_t height = p_buffer.dimensions().height();
			CharType fill = p_stream.fill( '0' );

			for ( uint32_t x = 0; x < width; ++x )
			{
				typename PxBuffer< PF >::column const & column = p_buffer[x];

				for ( uint32_t y = 0; y < height; ++y )
				{
					typename PxBuffer< PF >::pixel const & pixel = column[y];
					p_stream << "0x";
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::GetByteAlpha( pixel ) );
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::GetByteRed( pixel ) );
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::GetByteGreen( pixel ) );
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::GetByteBlue( pixel ) ) << " ";
				}

				p_stream << std::endl;
			}

			p_stream.fill( fill );
			return p_stream;
		}
	};

	template< PixelFormat PF >
	struct BufferStreamer< PF, typename std::enable_if< is_depth_stencil_format< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & p_stream, PxBuffer< PF > const & p_buffer )
		{
			uint8_t const * data = p_buffer.const_ptr();
			uint32_t width = p_buffer.dimensions().width();
			uint32_t height = p_buffer.dimensions().height();
			CharType fill = p_stream.fill( '0' );

			for ( uint32_t x = 0; x < width; ++x )
			{
				typename PxBuffer< PF >::column const & column = p_buffer[x];

				for ( uint32_t y = 0; y < height; ++y )
				{
					typename PxBuffer< PF >::pixel const & pixel = column[y];
					p_stream << "0x";
					p_stream.width( 6 );
					p_stream << std::hex << int( PF::GetUInt24Depth( pixel ) );
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::GetByteStencil( pixel ) ) << " ";
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
		p_stream << uint32_t( PF::GetBytesPerPixel( p_buffer.format() ) );
		p_stream << ", Format : ";
		p_stream.width( 10 );
		p_stream << string::string_cast< CharType >( PF::GetFormatName( p_buffer.format() ) );
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
			Logger::LogTrace( stream );
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
	void ConvertPixel( Pixel< PFSrc > const & p_source )
	{
		PixelConverter< PFDst, PFSrc >()( p_source );
	}

	template< PixelFormat PFSrc, typename Enable = void > struct PixelConversionChecker;

	template< PixelFormat PFSrc >
	struct PixelConversionChecker< PFSrc, typename std::enable_if< is_colour_format< PFSrc >::value >::type >
	{
		void operator()()
		{
			Pixel< PFSrc > source( true );
			PF::SetByteRed( source, 0x40 );
			PF::SetByteGreen( source, 0x80 );
			PF::SetByteBlue( source, 0xBF );
			PF::SetByteAlpha( source, 0xFF );
			StringStream stream;
			stream.width( 20 );
			stream << "Source pixel : " << source;
			Logger::LogTrace( stream );
			ConvertPixel< PixelFormat::eL8 >( source );
			ConvertPixel< PixelFormat::eL32F >( source );
			ConvertPixel< PixelFormat::eA8L8 >( source );
			ConvertPixel< PixelFormat::eAL32F >( source );
			ConvertPixel< PixelFormat::eA1R5G5B5 >( source );
			ConvertPixel< PixelFormat::eA4R4G4B4 >( source );
			ConvertPixel< PixelFormat::eR5G6B5 >( source );
			ConvertPixel< PixelFormat::eR8G8B8 >( source );
			ConvertPixel< PixelFormat::eA8R8G8B8 >( source );
			ConvertPixel< PixelFormat::eRGB32F >( source );
			ConvertPixel< PixelFormat::eRGBA32F >( source );
			Logger::LogTrace( StringStream() << std::endl );
		}
	};

	template< PixelFormat PFSrc >
	struct PixelConversionChecker< PFSrc, typename std::enable_if< is_depth_stencil_format< PFSrc >::value >::type >
	{
		void operator()()
		{
			Pixel< PFSrc > source( true );
			PF::SetUInt32Depth( source, 0x10204080 );
			PF::SetByteStencil( source, 0x80 );
			StringStream stream;
			stream.width( 20 );
			stream << "Source pixel : " << source;
			Logger::LogTrace( stream );
			ConvertPixel< PixelFormat::eD16 >( source );
			ConvertPixel< PixelFormat::eD24 >( source );
			ConvertPixel< PixelFormat::eD32 >( source );
			ConvertPixel< PixelFormat::eD24S8 >( source );
			ConvertPixel< PixelFormat::eS8 >( source );
			Logger::LogTrace( StringStream() << std::endl );
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
			Logger::LogTrace( stream );
		}
	};

	template< PixelFormat PFSrc >
	struct BufferConverter< PFSrc, PFSrc >
	{
		void operator()( std::shared_ptr< PxBuffer< PFSrc > > p_source )
		{
		}
	};

	template< PixelFormat PFDst, PixelFormat PFSrc >
	void ConvertBuffer( std::shared_ptr< PxBuffer< PFSrc > > p_source )
	{
		BufferConverter< PFDst, PFSrc >()( p_source );
	}

	template< PixelFormat PFSrc, typename Enable = void > struct BufferConversionChecker;

	template< PixelFormat PFSrc >
	struct BufferConversionChecker< PFSrc, typename std::enable_if< is_colour_format< PFSrc >::value >::type >
	{
		void operator()()
		{
			Size size = { 16, 16 };
			std::vector< uint8_t > buffer;
			size_t count = size.width() * size.height() * pixel_definitions< PFSrc >::Size;
			buffer.resize( count );
			uint8_t value = 0;
			Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += pixel_definitions< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				PF::SetByteAlpha( pixel, value++ );
				PF::SetByteRed( pixel, value++ );
				PF::SetByteGreen( pixel, value++ );
				PF::SetByteBlue( pixel, value++ );
			}

			std::shared_ptr< PxBuffer< PFSrc > > source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( size, PixelFormat( PFSrc ), buffer.data(), PixelFormat( PFSrc ) ) );
			StringStream stream;
			stream.width( 20 );
			stream << "Source buffer : " << *source;
			Logger::LogTrace( stream );
			ConvertBuffer< PixelFormat::eL8 >( source );
			ConvertBuffer< PixelFormat::eL32F >( source );
			ConvertBuffer< PixelFormat::eA8L8 >( source );
			ConvertBuffer< PixelFormat::eAL32F >( source );
			ConvertBuffer< PixelFormat::eA1R5G5B5 >( source );
			ConvertBuffer< PixelFormat::eA4R4G4B4 >( source );
			ConvertBuffer< PixelFormat::eR5G6B5 >( source );
			ConvertBuffer< PixelFormat::eR8G8B8 >( source );
			ConvertBuffer< PixelFormat::eA8R8G8B8 >( source );
			ConvertBuffer< PixelFormat::eRGB32F >( source );
			ConvertBuffer< PixelFormat::eRGBA32F >( source );
			Logger::LogTrace( StringStream() << std::endl );
		}
	};

	template< PixelFormat PFSrc >
	struct BufferConversionChecker < PFSrc, typename std::enable_if < is_depth_format< PFSrc >::value && PFSrc != PixelFormat::eD24S8 >::type >
	{
		void operator()()
		{
			Size size = { 16, 16 };
			std::vector< uint8_t > buffer;
			size_t count = size.width() * size.height() * pixel_definitions< PFSrc >::Size;
			buffer.resize( count );
			uint32_t depth = 0;
			uint8_t stencil = 0;
			Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += pixel_definitions< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				PF::SetUInt32Depth( pixel, depth );
				PF::SetByteStencil( pixel, stencil );
				depth += 0x02468ACE;
				stencil++;
			}

			std::shared_ptr< PxBuffer< PFSrc > > source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( size, PixelFormat( PFSrc ), buffer.data(), PixelFormat( PFSrc ) ) );
			StringStream stream;
			stream.width( 20 );
			stream << "Source buffer : " << *source;
			Logger::LogTrace( stream );
			ConvertBuffer< PixelFormat::eD16 >( source );
			ConvertBuffer< PixelFormat::eD24 >( source );
			ConvertBuffer< PixelFormat::eD32 >( source );
			ConvertBuffer< PixelFormat::eD24S8 >( source );
			Logger::LogTrace( StringStream() << std::endl );
		}
	};

	template< PixelFormat PFSrc >
	struct BufferConversionChecker< PFSrc, typename std::enable_if< PFSrc == PixelFormat::eD24S8 >::type >
	{
		void operator()()
		{
			Size size = { 16, 16 };
			std::vector< uint8_t > buffer;
			size_t count = size.width() * size.height() * pixel_definitions< PFSrc >::Size;
			buffer.resize( count );
			uint32_t depth = 0;
			uint8_t stencil = 0;
			Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += pixel_definitions< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				PF::SetUInt32Depth( pixel, depth );
				PF::SetByteStencil( pixel, stencil );
				depth += 0x02468ACE;
				stencil++;
			}

			std::shared_ptr< PxBuffer< PFSrc > > source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( size, PixelFormat( PFSrc ), buffer.data(), PixelFormat( PFSrc ) ) );
			StringStream stream;
			stream.width( 20 );
			stream << "Source buffer : " << *source;
			Logger::LogTrace( stream );
			ConvertBuffer< PixelFormat::eD16 >( source );
			ConvertBuffer< PixelFormat::eD24 >( source );
			ConvertBuffer< PixelFormat::eD32 >( source );
			ConvertBuffer< PixelFormat::eD24S8 >( source );
			ConvertBuffer< PixelFormat::eS8 >( source );
			Logger::LogTrace( StringStream() << std::endl );
		}
	};

	template< PixelFormat PFSrc >
	struct BufferConversionChecker < PFSrc, typename std::enable_if < is_stencil_format< PFSrc >::value && PFSrc != PixelFormat::eD24S8 >::type >
	{
		void operator()()
		{
			Size size = { 16, 16 };
			std::vector< uint8_t > buffer;
			size_t count = size.width() * size.height() * pixel_definitions< PFSrc >::Size;
			buffer.resize( count );
			uint8_t value = 0;
			Pixel< PFSrc > pixel;

			for ( size_t i = 0; i < count; i += pixel_definitions< PFSrc >::Size )
			{
				pixel.link( buffer.data() + i );
				PF::SetUInt32Depth( pixel, value++ );
				PF::SetByteStencil( pixel, value++ );
			}

			std::shared_ptr< PxBuffer< PFSrc > > source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( size, PixelFormat( PFSrc ), buffer.data(), PixelFormat( PFSrc ) ) );
			StringStream stream;
			stream.width( 20 );
			stream << "Source buffer : " << *source;
			Logger::LogTrace( stream );
			ConvertBuffer< PixelFormat::eD24S8 >( source );
			ConvertBuffer< PixelFormat::eS8 >( source );
			Logger::LogTrace( StringStream() << std::endl );
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

	void CastorUtilsPixelFormatTest::DoRegisterTests()
	{
		DoRegisterTest( "TestPixelConversions", std::bind( &CastorUtilsPixelFormatTest::TestPixelConversions, this ) );
		DoRegisterTest( "TestBufferConversions", std::bind( &CastorUtilsPixelFormatTest::TestBufferConversions, this ) );
	}

	void CastorUtilsPixelFormatTest::TestPixelConversions()
	{
		CheckPixelConversions< PixelFormat::eL8 >();
		CheckPixelConversions< PixelFormat::eL32F >();
		CheckPixelConversions< PixelFormat::eA8L8 >();
		CheckPixelConversions< PixelFormat::eAL32F >();
		CheckPixelConversions< PixelFormat::eA1R5G5B5 >();
		CheckPixelConversions< PixelFormat::eA4R4G4B4 >();
		CheckPixelConversions< PixelFormat::eR5G6B5 >();
		CheckPixelConversions< PixelFormat::eR8G8B8 >();
		CheckPixelConversions< PixelFormat::eA8R8G8B8 >();
		CheckPixelConversions< PixelFormat::eRGB32F >();
		CheckPixelConversions< PixelFormat::eRGBA32F >();
		CheckPixelConversions< PixelFormat::eD16 >();
		CheckPixelConversions< PixelFormat::eD24 >();
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
		CheckBufferConversions< PixelFormat::eA4R4G4B4 >();
		CheckBufferConversions< PixelFormat::eR5G6B5 >();
		CheckBufferConversions< PixelFormat::eR8G8B8 >();
		CheckBufferConversions< PixelFormat::eA8R8G8B8 >();
		CheckBufferConversions< PixelFormat::eRGB32F >();
		CheckBufferConversions< PixelFormat::eRGBA32F >();
		CheckBufferConversions< PixelFormat::eD16 >();
		CheckBufferConversions< PixelFormat::eD24 >();
		CheckBufferConversions< PixelFormat::eD32 >();
		CheckBufferConversions< PixelFormat::eD24S8 >();
		CheckBufferConversions< PixelFormat::eS8 >();
	}
}
