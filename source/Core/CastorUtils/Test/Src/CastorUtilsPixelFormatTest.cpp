#include "CastorUtilsPixelFormatTest.hpp"

#include <Graphics/PixelBuffer.hpp>

using namespace Castor;

namespace
{
	template< TPL_PIXEL_FORMAT PF, typename Enable = void > struct PixelStreamer;

	template< TPL_PIXEL_FORMAT PF >
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

	template< TPL_PIXEL_FORMAT PF >
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

	template< typename CharType, TPL_PIXEL_FORMAT PF >
	std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & p_stream, Pixel< PF > const & p_pixel )
	{
		return PixelStreamer< PF >()( p_stream, p_pixel );
	}

	template< TPL_PIXEL_FORMAT PF, typename Enable = void > struct BufferStreamer;

	template< TPL_PIXEL_FORMAT PF >
	struct BufferStreamer< PF, typename std::enable_if< is_colour_format< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & p_stream, PxBuffer< PF > const & p_buffer )
		{
			uint8_t const * l_data = p_buffer.const_ptr();
			uint32_t l_width = p_buffer.dimensions().width();
			uint32_t l_height = p_buffer.dimensions().height();
			CharType l_fill = p_stream.fill( '0' );

			for ( uint32_t x = 0; x < l_width; ++x )
			{
				typename PxBuffer< PF >::column const & l_column = p_buffer[x];

				for ( uint32_t y = 0; y < l_height; ++y )
				{
					typename PxBuffer< PF >::pixel const & l_pixel = l_column[y];
					p_stream << "0x";
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::GetByteAlpha( l_pixel ) );
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::GetByteRed( l_pixel ) );
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::GetByteGreen( l_pixel ) );
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::GetByteBlue( l_pixel ) ) << " ";
				}

				p_stream << std::endl;
			}

			p_stream.fill( l_fill );
			return p_stream;
		}
	};

	template< TPL_PIXEL_FORMAT PF >
	struct BufferStreamer< PF, typename std::enable_if< is_depth_stencil_format< PF >::value >::type >
	{
		template< typename CharType >
		std::basic_ostream< CharType > & operator()( std::basic_ostream< CharType > & p_stream, PxBuffer< PF > const & p_buffer )
		{
			uint8_t const * l_data = p_buffer.const_ptr();
			uint32_t l_width = p_buffer.dimensions().width();
			uint32_t l_height = p_buffer.dimensions().height();
			CharType l_fill = p_stream.fill( '0' );

			for ( uint32_t x = 0; x < l_width; ++x )
			{
				typename PxBuffer< PF >::column const & l_column = p_buffer[x];

				for ( uint32_t y = 0; y < l_height; ++y )
				{
					typename PxBuffer< PF >::pixel const & l_pixel = l_column[y];
					p_stream << "0x";
					p_stream.width( 6 );
					p_stream << std::hex << int( PF::GetUInt24Depth( l_pixel ) );
					p_stream.width( 2 );
					p_stream << std::hex << int( PF::GetByteStencil( l_pixel ) ) << " ";
				}

				p_stream << std::endl;
			}

			p_stream.fill( l_fill );
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

	template< typename CharType, TPL_PIXEL_FORMAT PF >
	std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & p_stream, PxBuffer< PF > const & p_buffer )
	{
		p_stream << static_cast< PxBufferBase const & >( p_buffer );
		return BufferStreamer< PF >()( p_stream, p_buffer );
	}

	template< TPL_PIXEL_FORMAT PFDst, TPL_PIXEL_FORMAT PFSrc >
	struct PixelConverter
	{
		void operator()( Pixel< PFSrc > const & p_source )
		{
			Pixel< PFDst > l_dest( p_source );
			StringStream l_stream;
			l_stream.width( 20 );
			l_stream << "Converted pixel : " << l_dest;
			Logger::LogDebug( l_stream );
		}
	};

	template< TPL_PIXEL_FORMAT PFSrc >
	struct PixelConverter< PFSrc, PFSrc >
	{
		void operator()( Pixel< PFSrc > const & p_source )
		{
		}
	};

	template< TPL_PIXEL_FORMAT PFDst, TPL_PIXEL_FORMAT PFSrc >
	void ConvertPixel( Pixel< PFSrc > const & p_source )
	{
		PixelConverter< PFDst, PFSrc >()( p_source );
	}

	template< TPL_PIXEL_FORMAT PFSrc, typename Enable = void > struct PixelConversionChecker;

	template< TPL_PIXEL_FORMAT PFSrc >
	struct PixelConversionChecker< PFSrc, typename std::enable_if< is_colour_format< PFSrc >::value >::type >
	{
		void operator()()
		{
			Pixel< PFSrc > l_source( true );
			PF::SetByteRed( l_source, 0x40 );
			PF::SetByteGreen( l_source, 0x80 );
			PF::SetByteBlue( l_source, 0xBF );
			PF::SetByteAlpha( l_source, 0xFF );
			StringStream l_stream;
			l_stream.width( 20 );
			l_stream << "Source pixel : " << l_source;
			Logger::LogDebug( l_stream );
			ConvertPixel< PixelFormat::eL8 >( l_source );
			ConvertPixel< PixelFormat::eL32F >( l_source );
			ConvertPixel< PixelFormat::eA8L8 >( l_source );
			ConvertPixel< PixelFormat::eAL32F >( l_source );
			ConvertPixel< PixelFormat::eA1R5G5B5 >( l_source );
			ConvertPixel< PixelFormat::eA4R4G4B4 >( l_source );
			ConvertPixel< PixelFormat::eR5G6B5 >( l_source );
			ConvertPixel< PixelFormat::eR8G8B8 >( l_source );
			ConvertPixel< PixelFormat::eA8R8G8B8 >( l_source );
			ConvertPixel< PixelFormat::eRGB32F >( l_source );
			ConvertPixel< PixelFormat::eRGBA32F >( l_source );
			Logger::LogDebug( StringStream() << std::endl );
		}
	};

	template< TPL_PIXEL_FORMAT PFSrc >
	struct PixelConversionChecker< PFSrc, typename std::enable_if< is_depth_stencil_format< PFSrc >::value >::type >
	{
		void operator()()
		{
			Pixel< PFSrc > l_source( true );
			PF::SetUInt32Depth( l_source, 0x10204080 );
			PF::SetByteStencil( l_source, 0x80 );
			StringStream l_stream;
			l_stream.width( 20 );
			l_stream << "Source pixel : " << l_source;
			Logger::LogDebug( l_stream );
			ConvertPixel< PixelFormat::eD16 >( l_source );
			ConvertPixel< PixelFormat::eD24 >( l_source );
			ConvertPixel< PixelFormat::eD32 >( l_source );
			ConvertPixel< PixelFormat::eD24S8 >( l_source );
			ConvertPixel< PixelFormat::eS8 >( l_source );
			Logger::LogDebug( StringStream() << std::endl );
		}
	};

	template< TPL_PIXEL_FORMAT PF >
	void CheckPixelConversions()
	{
		PixelConversionChecker< PF >()();
	}

	template< TPL_PIXEL_FORMAT PFDst, TPL_PIXEL_FORMAT PFSrc >
	struct BufferConverter
	{
		void operator()( std::shared_ptr< PxBuffer< PFSrc > > p_source )
		{
			std::shared_ptr< PxBuffer< PFDst > > l_dest = std::static_pointer_cast< PxBuffer< PFDst > >( PxBufferBase::create( p_source->dimensions(), PixelFormat( PFDst ), p_source->ptr(), PixelFormat( PFSrc ) ) );
			StringStream l_stream;
			l_stream.width( 20 );
			l_stream << "Converted buffer : " << *l_dest;
			Logger::LogDebug( l_stream );
		}
	};

	template< TPL_PIXEL_FORMAT PFSrc >
	struct BufferConverter< PFSrc, PFSrc >
	{
		void operator()( std::shared_ptr< PxBuffer< PFSrc > > p_source )
		{
		}
	};

	template< TPL_PIXEL_FORMAT PFDst, TPL_PIXEL_FORMAT PFSrc >
	void ConvertBuffer( std::shared_ptr< PxBuffer< PFSrc > > p_source )
	{
		BufferConverter< PFDst, PFSrc >()( p_source );
	}

	template< TPL_PIXEL_FORMAT PFSrc, typename Enable = void > struct BufferConversionChecker;

	template< TPL_PIXEL_FORMAT PFSrc >
	struct BufferConversionChecker< PFSrc, typename std::enable_if< is_colour_format< PFSrc >::value >::type >
	{
		void operator()()
		{
			Size l_size = { 16, 16 };
			std::vector< uint8_t > l_buffer;
			size_t l_count = l_size.width() * l_size.height() * pixel_definitions< PFSrc >::Size;
			l_buffer.resize( l_count );
			uint8_t l_value = 0;
			Pixel< PFSrc > l_pixel;

			for ( size_t i = 0; i < l_count; i += pixel_definitions< PFSrc >::Size )
			{
				l_pixel.link( l_buffer.data() + i );
				PF::SetByteAlpha( l_pixel, l_value++ );
				PF::SetByteRed( l_pixel, l_value++ );
				PF::SetByteGreen( l_pixel, l_value++ );
				PF::SetByteBlue( l_pixel, l_value++ );
			}

			std::shared_ptr< PxBuffer< PFSrc > > l_source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( l_size, PixelFormat( PFSrc ), l_buffer.data(), PixelFormat( PFSrc ) ) );
			StringStream l_stream;
			l_stream.width( 20 );
			l_stream << "Source buffer : " << *l_source;
			Logger::LogDebug( l_stream );
			ConvertBuffer< PixelFormat::eL8 >( l_source );
			ConvertBuffer< PixelFormat::eL32F >( l_source );
			ConvertBuffer< PixelFormat::eA8L8 >( l_source );
			ConvertBuffer< PixelFormat::eAL32F >( l_source );
			ConvertBuffer< PixelFormat::eA1R5G5B5 >( l_source );
			ConvertBuffer< PixelFormat::eA4R4G4B4 >( l_source );
			ConvertBuffer< PixelFormat::eR5G6B5 >( l_source );
			ConvertBuffer< PixelFormat::eR8G8B8 >( l_source );
			ConvertBuffer< PixelFormat::eA8R8G8B8 >( l_source );
			ConvertBuffer< PixelFormat::eRGB32F >( l_source );
			ConvertBuffer< PixelFormat::eRGBA32F >( l_source );
			Logger::LogDebug( StringStream() << std::endl );
		}
	};

	template< TPL_PIXEL_FORMAT PFSrc >
	struct BufferConversionChecker < PFSrc, typename std::enable_if < is_depth_format< PFSrc >::value && PFSrc != PixelFormat::eD24S8 >::type >
	{
		void operator()()
		{
			Size l_size = { 16, 16 };
			std::vector< uint8_t > l_buffer;
			size_t l_count = l_size.width() * l_size.height() * pixel_definitions< PFSrc >::Size;
			l_buffer.resize( l_count );
			uint32_t l_depth = 0;
			uint8_t l_stencil = 0;
			Pixel< PFSrc > l_pixel;

			for ( size_t i = 0; i < l_count; i += pixel_definitions< PFSrc >::Size )
			{
				l_pixel.link( l_buffer.data() + i );
				PF::SetUInt32Depth( l_pixel, l_depth );
				PF::SetByteStencil( l_pixel, l_stencil );
				l_depth += 0x02468ACE;
				l_stencil++;
			}

			std::shared_ptr< PxBuffer< PFSrc > > l_source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( l_size, PixelFormat( PFSrc ), l_buffer.data(), PixelFormat( PFSrc ) ) );
			StringStream l_stream;
			l_stream.width( 20 );
			l_stream << "Source buffer : " << *l_source;
			Logger::LogDebug( l_stream );
			ConvertBuffer< PixelFormat::eD16 >( l_source );
			ConvertBuffer< PixelFormat::eD24 >( l_source );
			ConvertBuffer< PixelFormat::eD32 >( l_source );
			ConvertBuffer< PixelFormat::eD24S8 >( l_source );
			Logger::LogDebug( StringStream() << std::endl );
		}
	};

	template< TPL_PIXEL_FORMAT PFSrc >
	struct BufferConversionChecker< PFSrc, typename std::enable_if< PFSrc == PixelFormat::eD24S8 >::type >
	{
		void operator()()
		{
			Size l_size = { 16, 16 };
			std::vector< uint8_t > l_buffer;
			size_t l_count = l_size.width() * l_size.height() * pixel_definitions< PFSrc >::Size;
			l_buffer.resize( l_count );
			uint32_t l_depth = 0;
			uint8_t l_stencil = 0;
			Pixel< PFSrc > l_pixel;

			for ( size_t i = 0; i < l_count; i += pixel_definitions< PFSrc >::Size )
			{
				l_pixel.link( l_buffer.data() + i );
				PF::SetUInt32Depth( l_pixel, l_depth );
				PF::SetByteStencil( l_pixel, l_stencil );
				l_depth += 0x02468ACE;
				l_stencil++;
			}

			std::shared_ptr< PxBuffer< PFSrc > > l_source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( l_size, PixelFormat( PFSrc ), l_buffer.data(), PixelFormat( PFSrc ) ) );
			StringStream l_stream;
			l_stream.width( 20 );
			l_stream << "Source buffer : " << *l_source;
			Logger::LogDebug( l_stream );
			ConvertBuffer< PixelFormat::eD16 >( l_source );
			ConvertBuffer< PixelFormat::eD24 >( l_source );
			ConvertBuffer< PixelFormat::eD32 >( l_source );
			ConvertBuffer< PixelFormat::eD24S8 >( l_source );
			ConvertBuffer< PixelFormat::eS8 >( l_source );
			Logger::LogDebug( StringStream() << std::endl );
		}
	};

	template< TPL_PIXEL_FORMAT PFSrc >
	struct BufferConversionChecker < PFSrc, typename std::enable_if < is_stencil_format< PFSrc >::value && PFSrc != PixelFormat::eD24S8 >::type >
	{
		void operator()()
		{
			Size l_size = { 16, 16 };
			std::vector< uint8_t > l_buffer;
			size_t l_count = l_size.width() * l_size.height() * pixel_definitions< PFSrc >::Size;
			l_buffer.resize( l_count );
			uint8_t l_value = 0;
			Pixel< PFSrc > l_pixel;

			for ( size_t i = 0; i < l_count; i += pixel_definitions< PFSrc >::Size )
			{
				l_pixel.link( l_buffer.data() + i );
				PF::SetUInt32Depth( l_pixel, l_value++ );
				PF::SetByteStencil( l_pixel, l_value++ );
			}

			std::shared_ptr< PxBuffer< PFSrc > > l_source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( l_size, PixelFormat( PFSrc ), l_buffer.data(), PixelFormat( PFSrc ) ) );
			StringStream l_stream;
			l_stream.width( 20 );
			l_stream << "Source buffer : " << *l_source;
			Logger::LogDebug( l_stream );
			ConvertBuffer< PixelFormat::eD24S8 >( l_source );
			ConvertBuffer< PixelFormat::eS8 >( l_source );
			Logger::LogDebug( StringStream() << std::endl );
		}
	};

	template< TPL_PIXEL_FORMAT PF >
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
