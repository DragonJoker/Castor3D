#include "CastorUtilsTest.hpp"

#include <Matrix.hpp>
#include <TransformationMatrix.hpp>
#include <ZipArchive.hpp>
#include <BinaryFile.hpp>
#include <TextFile.hpp>
#include <PixelBuffer.hpp>

#if defined( CASTOR_USE_GLM )
#	include <glm/glm.hpp>
#endif

using namespace Castor;

namespace
{
	inline void randomInit( float * p_pData1, float * p_pData2, int size )
	{
		for ( int i = 0; i < size; ++i )
		{
			p_pData1[i] = rand() / float( RAND_MAX );
			p_pData2[i] = p_pData1[i];
		}
	}

	template< typename InChar, typename OutChar >
	inline void convert( std::basic_string< InChar > const & p_strIn, std::basic_string< OutChar > & p_strOut, std::locale const & p_locale = std::locale() )
	{
		if ( !p_strIn.empty() )
		{
			typedef typename std::codecvt< OutChar, InChar, std::mbstate_t > facet_type;
			typedef typename facet_type::result result_type;
			std::mbstate_t l_state = std::mbstate_t();
			result_type l_result;
			std::vector< OutChar > l_buffer( p_strIn.size() );
			InChar const * l_pEndIn = NULL;
			OutChar * l_pEndOut = NULL;
			l_result = std::use_facet< facet_type >( p_locale ).in( l_state,
					   p_strIn.data(),		p_strIn.data() + p_strIn.length(),		l_pEndIn,
					   &l_buffer.front(),	&l_buffer.front() + l_buffer.size(),	l_pEndOut
																  );
			p_strOut = std::basic_string< OutChar >( &l_buffer.front(), l_pEndOut );
		}
		else
		{
			p_strOut.clear();
		}
	}

	inline void narrow( std::wstring const & p_wstrIn, std::string & p_strOut )
	{
		p_strOut.resize( p_wstrIn.size() + 1, '\0' );
		std::ostringstream l_stream;
		std::ctype< wchar_t > const & l_ctfacet = std::use_facet< std::ctype< wchar_t > >( l_stream.getloc() );

		if ( p_wstrIn.size() > 0 )
		{
			l_ctfacet.narrow( p_wstrIn.data(), p_wstrIn.data() + p_wstrIn.size(), '?', &p_strOut[0] );
		}
	}

	inline void widen( std::string const & p_strIn, std::wstring & p_wstrOut )
	{
		p_wstrOut.resize( p_strIn.size() + 1, L'\0' );
		std::wostringstream l_wstream;
		std::ctype< wchar_t > const & l_ctfacet = std::use_facet< std::ctype< wchar_t > >( l_wstream.getloc() );

		if ( p_strIn.size() > 0 )
		{
			l_ctfacet.widen( p_strIn.data(), p_strIn.data() + p_strIn.size(), &p_wstrOut[0] );
		}
	}
    
#if defined( CASTOR_USE_GLM )
	inline bool operator ==( Matrix4x4r const & a, glm::mat4x4 const & b )
	{
		return a[0][0] == b[0][0] && a[1][0] == b[1][0] &&	a[2][0] == b[2][0] &&	a[3][0] == b[3][0]
			   && a[0][1] == b[0][1] && a[1][1] == b[1][1] &&	a[2][1] == b[2][1] &&	a[3][1] == b[3][1]
			   && a[0][2] == b[0][2] && a[1][2] == b[1][2] &&	a[2][2] == b[2][2] &&	a[3][2] == b[3][2]
			   && a[0][3] == b[0][3] && a[1][3] == b[1][3] &&	a[2][3] == b[2][3] &&	a[3][3] == b[3][3];
	}

	inline std::ostream & operator <<( std::ostream & p_stream, glm::mat4 p_mtx )
	{
		for ( uint32_t i = 0; i < 4; i++ )
		{
			for ( uint32_t j = 0; j < 4; j++ )
			{
				p_stream << "\t" << p_mtx[j][i];
			}

			p_stream << std::endl;
		}

		return p_stream;
	}

	inline std::ostream & operator <<( std::ostream & p_stream, Castor::String const & p_str )
	{
		p_stream << Castor::str_utils::to_str( p_str );
		return p_stream;
	}
#endif

	template< TPL_PIXEL_FORMAT PF, typename Enable=void > struct PixelStreamer;
	
	template< TPL_PIXEL_FORMAT PF >
	struct PixelStreamer< PF, typename std::enable_if< is_colour_format< PF >::value >::type >
	{
		std::ostream & operator()( std::ostream & p_stream, Pixel< PF > const & p_pixel )
		{
			p_stream << "BPP : ";
			p_stream.width( 2 );
			p_stream << uint32_t( PF::GetBytesPerPixel( p_pixel.get_format() ) );
			p_stream << ", Format : ";
			p_stream.width( 10 );
			p_stream << PF::GetFormatName( p_pixel.get_format() );
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
		std::ostream & operator()( std::ostream & p_stream, Pixel< PF > const & p_pixel )
		{
			p_stream << "BPP : ";
			p_stream.width( 2 );
			p_stream << uint32_t( PF::GetBytesPerPixel( p_pixel.get_format() ) );
			p_stream << ", Format : ";
			p_stream.width( 10 );
			p_stream << str_utils::to_str( PF::GetFormatName( p_pixel.get_format() ) );
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
	
	template< TPL_PIXEL_FORMAT PF >
	std::ostream & operator <<( std::ostream & p_stream, Pixel< PF > const & p_pixel )
	{
		return PixelStreamer< PF >()( p_stream, p_pixel );
	}
	
	template< TPL_PIXEL_FORMAT PF, typename Enable=void > struct BufferStreamer;
	
	template< TPL_PIXEL_FORMAT PF >
	struct BufferStreamer< PF, typename std::enable_if< is_colour_format< PF >::value >::type >
	{
		std::ostream & operator()( std::ostream & p_stream, PxBuffer< PF > const & p_buffer )
		{
			uint8_t const * l_data = p_buffer.const_ptr();
			uint32_t l_width = p_buffer.dimensions().width();
			uint32_t l_height = p_buffer.dimensions().height();
			char l_fill = p_stream.fill( '0' );
			
			for ( uint32_t x = 0; x < l_width; ++x )
			{
				PxBuffer< PF >::column const & l_column = p_buffer[x];
				
				for ( uint32_t y = 0; y < l_height; ++y )
				{
					PxBuffer< PF >::pixel const & l_pixel = l_column[y];
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
		std::ostream & operator()( std::ostream & p_stream, PxBuffer< PF > const & p_buffer )
		{
			uint8_t const * l_data = p_buffer.const_ptr();
			uint32_t l_width = p_buffer.dimensions().width();
			uint32_t l_height = p_buffer.dimensions().height();
			char l_fill = p_stream.fill( '0' );
			
			for ( uint32_t x = 0; x < l_width; ++x )
			{
				PxBuffer< PF >::column const & l_column = p_buffer[x];
				
				for ( uint32_t y = 0; y < l_height; ++y )
				{
					PxBuffer< PF >::pixel const & l_pixel = l_column[y];
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
	
	std::ostream & operator <<( std::ostream & p_stream, PxBufferBase const & p_buffer )
	{
		p_stream << "BPP : ";
		p_stream.width( 2 );
		p_stream << uint32_t( PF::GetBytesPerPixel( p_buffer.format() ) );
		p_stream << ", Format : ";
		p_stream.width( 10 );
		p_stream << str_utils::to_str( PF::GetFormatName( p_buffer.format() ) ) << std::endl;
		return p_stream;
	}
	
	template< TPL_PIXEL_FORMAT PF >
	std::ostream & operator <<( std::ostream & p_stream, PxBuffer< PF > const & p_buffer )
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
			std::cout.width( 20 );
			std::cout << "Converted pixel : ";
			std::cout << l_dest << std::endl;
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
	
	template< TPL_PIXEL_FORMAT PFSrc, typename Enable=void > struct PixelConversionChecker;
	
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
			std::cout.width( 20 );
			std::cout << "Source pixel : ";
			std::cout << l_source << std::endl;
			ConvertPixel< ePIXEL_FORMAT_L8 >( l_source );
			ConvertPixel< ePIXEL_FORMAT_L32F >( l_source );
			ConvertPixel< ePIXEL_FORMAT_A8L8 >( l_source );
			ConvertPixel< ePIXEL_FORMAT_AL32F >( l_source );
			ConvertPixel< ePIXEL_FORMAT_A1R5G5B5 >( l_source );
			ConvertPixel< ePIXEL_FORMAT_A4R4G4B4 >( l_source );
			ConvertPixel< ePIXEL_FORMAT_R5G6B5 >( l_source );
			ConvertPixel< ePIXEL_FORMAT_R8G8B8 >( l_source );
			ConvertPixel< ePIXEL_FORMAT_A8R8G8B8 >( l_source );
			ConvertPixel< ePIXEL_FORMAT_RGB32F >( l_source );
			ConvertPixel< ePIXEL_FORMAT_ARGB32F >( l_source );
			std::cout << std::endl;
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
			std::cout.width( 20 );
			std::cout << "Source pixel : ";
			std::cout << l_source << std::endl;
			ConvertPixel< ePIXEL_FORMAT_DEPTH16 >( l_source );
			ConvertPixel< ePIXEL_FORMAT_DEPTH24 >( l_source );
			ConvertPixel< ePIXEL_FORMAT_DEPTH32 >( l_source );
			ConvertPixel< ePIXEL_FORMAT_DEPTH24S8 >( l_source );
			ConvertPixel< ePIXEL_FORMAT_STENCIL8 >( l_source );
			std::cout << std::endl;
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
			std::shared_ptr< PxBuffer< PFDst > > l_dest = std::static_pointer_cast< PxBuffer< PFDst > >( PxBufferBase::create( p_source->dimensions(), ePIXEL_FORMAT( PFDst ), p_source->ptr(), ePIXEL_FORMAT( PFSrc ) ) );
			std::cout.width( 20 );
			std::cout << "Converted buffer : ";
			std::cout << *l_dest << std::endl;
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
	
	template< TPL_PIXEL_FORMAT PFSrc, typename Enable=void > struct BufferConversionChecker;
	
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
			
			std::shared_ptr< PxBuffer< PFSrc > > l_source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( l_size, ePIXEL_FORMAT( PFSrc ), l_buffer.data(), ePIXEL_FORMAT( PFSrc ) ) );
			std::cout.width( 20 );
			std::cout << "Source buffer : ";
			std::cout << *l_source << std::endl;
			ConvertBuffer< ePIXEL_FORMAT_L8 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_L32F >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_A8L8 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_AL32F >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_A1R5G5B5 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_A4R4G4B4 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_R5G6B5 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_R8G8B8 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_A8R8G8B8 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_RGB32F >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_ARGB32F >( l_source );
			std::cout << std::endl;
		}
	};
	
	template< TPL_PIXEL_FORMAT PFSrc >
	struct BufferConversionChecker< PFSrc, typename std::enable_if< is_depth_format< PFSrc >::value && PFSrc != ePIXEL_FORMAT_DEPTH24S8 >::type >
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
			
			std::shared_ptr< PxBuffer< PFSrc > > l_source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( l_size, ePIXEL_FORMAT( PFSrc ), l_buffer.data(), ePIXEL_FORMAT( PFSrc ) ) );
			std::cout.width( 20 );
			std::cout << "Source buffer : ";
			std::cout << *l_source << std::endl;
			ConvertBuffer< ePIXEL_FORMAT_DEPTH16 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_DEPTH24 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_DEPTH32 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_DEPTH24S8 >( l_source );
			std::cout << std::endl;
		}
	};
	
	template< TPL_PIXEL_FORMAT PFSrc >
	struct BufferConversionChecker< PFSrc, typename std::enable_if< PFSrc == ePIXEL_FORMAT_DEPTH24S8 >::type >
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
			
			std::shared_ptr< PxBuffer< PFSrc > > l_source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( l_size, ePIXEL_FORMAT( PFSrc ), l_buffer.data(), ePIXEL_FORMAT( PFSrc ) ) );
			std::cout.width( 20 );
			std::cout << "Source buffer : ";
			std::cout << *l_source << std::endl;
			ConvertBuffer< ePIXEL_FORMAT_DEPTH16 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_DEPTH24 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_DEPTH32 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_DEPTH24S8 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_STENCIL8 >( l_source );
			std::cout << std::endl;
		}
	};
	
	template< TPL_PIXEL_FORMAT PFSrc >
	struct BufferConversionChecker< PFSrc, typename std::enable_if< is_stencil_format< PFSrc >::value && PFSrc != ePIXEL_FORMAT_DEPTH24S8 >::type >
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
			
			std::shared_ptr< PxBuffer< PFSrc > > l_source = std::static_pointer_cast< PxBuffer< PFSrc > >( PxBufferBase::create( l_size, ePIXEL_FORMAT( PFSrc ), l_buffer.data(), ePIXEL_FORMAT( PFSrc ) ) );
			std::cout.width( 20 );
			std::cout << "Source buffer : ";
			std::cout << *l_source << std::endl;
			ConvertBuffer< ePIXEL_FORMAT_DEPTH24S8 >( l_source );
			ConvertBuffer< ePIXEL_FORMAT_STENCIL8 >( l_source );
			std::cout << std::endl;
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
	CastorUtilsTest::CastorUtilsTest()
		:	TestCase( "CastorUtilsTest" )
	{
	}

	CastorUtilsTest::~CastorUtilsTest()
	{
	}

	void CastorUtilsTest::Execute( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		EXECUTE_TEST( StringConversions, p_errCount, p_testCount );
		EXECUTE_TEST( MatrixInversion, p_errCount, p_testCount );
#if defined( CASTOR_USE_GLM )
		EXECUTE_TEST( MatrixInversionComparison, p_errCount, p_testCount );
#endif
		EXECUTE_TEST( ZipFile, p_errCount, p_testCount );
		EXECUTE_TEST( TestPixelConversions, p_errCount, p_testCount );
		EXECUTE_TEST( TestBufferConversions, p_errCount, p_testCount );
	}

	void CastorUtilsTest::StringConversions( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		String l_tstrOut;
		String l_tstrIn( cuT( "STR : Bonjoir éêèàÉÊÈÀ" ) );
		std::string l_strIn = "STR : Bonjoir éêèàÉÊÈÀ";
		std::wstring l_wstrIn = L"STR : Bonjoir éêèàÉÊÈÀ";
		std::wstring l_wstrOut;
		std::string l_strOut;
		l_tstrOut = str_utils::from_str( l_strIn );
		Logger::LogMessage(	"	Conversion from std::string to String" );
		Logger::LogMessage(	"		Entry  : " + l_strIn );
		Logger::LogMessage( cuT(	"		Result : " ) + l_tstrOut );
		Logger::LogMessage(	"" );
		TEST_EQUAL( l_tstrOut, l_tstrIn );
		l_tstrOut = str_utils::from_wstr( l_wstrIn );
		Logger::LogMessage(	"	Conversion from std::wstring to String" );
		Logger::LogMessage( L"		Entry  : " + l_wstrIn );
		Logger::LogMessage( cuT(	"		Result : " ) + l_tstrOut );
		Logger::LogMessage(	"" );
		TEST_EQUAL( l_tstrOut, l_tstrIn );
		l_strOut = str_utils::to_str( l_tstrIn );
		Logger::LogMessage(	"	Conversion from String to std::string" );
		Logger::LogMessage( cuT(	"		Entry  : " ) + l_tstrIn );
		Logger::LogMessage(	"		Result : " + l_strOut );
		Logger::LogMessage(	"" );
		TEST_EQUAL( l_strOut, l_strIn );
		l_wstrOut = str_utils::to_wstr( l_tstrIn );
		Logger::LogMessage(	"	Conversion from String to std::wstring" );
		Logger::LogMessage( cuT(	"		Entry  : " ) + l_tstrIn );
		Logger::LogMessage( L"		Result : " + l_wstrOut );
		Logger::LogMessage(	"" );
		TEST_EQUAL( l_wstrOut, l_wstrIn );
		convert( l_strIn, l_wstrOut );
		Logger::LogMessage(	"	Conversion from std::string to std::wstring" );
		Logger::LogMessage(	"		Entry  : " + l_strIn );
		Logger::LogMessage( L"		Result : " + l_wstrOut );
		Logger::LogMessage(	"" );
		TEST_EQUAL( l_wstrOut, l_wstrIn );
		convert( l_wstrIn, l_strOut );
		Logger::LogMessage(	"	Conversion from std::wstring to std::string" );
		Logger::LogMessage( L"		Entry  : " + l_wstrIn );
		Logger::LogMessage(	"		Result : " + l_strOut );
		TEST_EQUAL( l_strOut, l_strIn );
	}

	void CastorUtilsTest::MatrixInversion( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		Matrix3x3d l_mtxRGBtoYUV;
		l_mtxRGBtoYUV[0][0] =  0.299;
		l_mtxRGBtoYUV[1][0] =  0.587;
		l_mtxRGBtoYUV[2][0] =  0.114;
		l_mtxRGBtoYUV[0][1] = -0.14713;
		l_mtxRGBtoYUV[1][1] = -0.28886;
		l_mtxRGBtoYUV[2][1] =  0.436;
		l_mtxRGBtoYUV[0][2] =  0.615;
		l_mtxRGBtoYUV[1][2] = -0.51499;
		l_mtxRGBtoYUV[2][2] = -0.10001;
		Logger::LogMessage( cuT( "	RGB to YUV conversion matrix (BT 601) :" ) );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][0], l_mtxRGBtoYUV[1][0], l_mtxRGBtoYUV[2][0] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][1], l_mtxRGBtoYUV[1][1], l_mtxRGBtoYUV[2][1] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][2], l_mtxRGBtoYUV[1][2], l_mtxRGBtoYUV[2][2] );
		Matrix3x3d l_mtxYUVtoRGB( l_mtxRGBtoYUV.get_inverse() );
		Logger::LogMessage( cuT( "	YUV to RGB conversion matrix (BT 601) :" ) );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][0], l_mtxYUVtoRGB[1][0], l_mtxYUVtoRGB[2][0] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][1], l_mtxYUVtoRGB[1][1], l_mtxYUVtoRGB[2][1] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][2], l_mtxYUVtoRGB[1][2], l_mtxYUVtoRGB[2][2] );
		TEST_EQUAL( l_mtxRGBtoYUV, l_mtxYUVtoRGB.get_inverse() );
		l_mtxRGBtoYUV[0][0] =  0.2126;
		l_mtxRGBtoYUV[1][0] =  0.7152;
		l_mtxRGBtoYUV[2][0] =  0.0722;
		l_mtxRGBtoYUV[0][1] = -0.09991;
		l_mtxRGBtoYUV[1][1] = -0.33609;
		l_mtxRGBtoYUV[2][1] =  0.436;
		l_mtxRGBtoYUV[0][2] =  0.615;
		l_mtxRGBtoYUV[1][2] = -0.55861;
		l_mtxRGBtoYUV[2][2] = -0.05639;
		Logger::LogMessage( cuT( "	RGB to YUV conversion matrix (BT 709) :" ) );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][0], l_mtxRGBtoYUV[1][0], l_mtxRGBtoYUV[2][0] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][1], l_mtxRGBtoYUV[1][1], l_mtxRGBtoYUV[2][1] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][2], l_mtxRGBtoYUV[1][2], l_mtxRGBtoYUV[2][2] );
		l_mtxYUVtoRGB = l_mtxRGBtoYUV.get_inverse();
		Logger::LogMessage( cuT( "	YUV to RGB conversion matrix (BT 709) :" ) );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][0], l_mtxYUVtoRGB[1][0], l_mtxYUVtoRGB[2][0] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][1], l_mtxYUVtoRGB[1][1], l_mtxYUVtoRGB[2][1] );
		Logger::LogMessage( cuT( "		%.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][2], l_mtxYUVtoRGB[1][2], l_mtxYUVtoRGB[2][2] );
		TEST_EQUAL( l_mtxRGBtoYUV, l_mtxYUVtoRGB.get_inverse() );
	}

#if defined( CASTOR_USE_GLM )
	void CastorUtilsTest::MatrixInversionComparison( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		Matrix4x4r l_mtxRGBtoYUV;
		l_mtxRGBtoYUV[0][0] =  0.299f;
		l_mtxRGBtoYUV[1][0] =  0.587f;
		l_mtxRGBtoYUV[2][0] =  0.114f;
		l_mtxRGBtoYUV[3][0] = 0.0f;
		l_mtxRGBtoYUV[0][1] = -0.14713f;
		l_mtxRGBtoYUV[1][1] = -0.28886f;
		l_mtxRGBtoYUV[2][1] =  0.436f;
		l_mtxRGBtoYUV[3][1] = 0.0f;
		l_mtxRGBtoYUV[0][2] =  0.615f;
		l_mtxRGBtoYUV[1][2] = -0.51499f;
		l_mtxRGBtoYUV[2][2] = -0.10001f;
		l_mtxRGBtoYUV[3][2] = 0.0f;
		l_mtxRGBtoYUV[0][3] =  0.0f;
		l_mtxRGBtoYUV[1][3] = 0.0f;
		l_mtxRGBtoYUV[2][3] = 0.0f;
		l_mtxRGBtoYUV[3][3] = 1.0f;
		glm::mat4x4 l_glmRGBtoYUV;
		l_glmRGBtoYUV[0][0] =  0.299f;
		l_glmRGBtoYUV[1][0] =  0.587f;
		l_glmRGBtoYUV[2][0] =  0.114f;
		l_glmRGBtoYUV[3][0] = 0.0f;
		l_glmRGBtoYUV[0][1] = -0.14713f;
		l_glmRGBtoYUV[1][1] = -0.28886f;
		l_glmRGBtoYUV[2][1] =  0.436f;
		l_glmRGBtoYUV[3][1] = 0.0f;
		l_glmRGBtoYUV[0][2] =  0.615f;
		l_glmRGBtoYUV[1][2] = -0.51499f;
		l_glmRGBtoYUV[2][2] = -0.10001f;
		l_glmRGBtoYUV[3][2] = 0.0f;
		l_glmRGBtoYUV[0][3] =  0.0f;
		l_glmRGBtoYUV[1][3] = 0.0f;
		l_glmRGBtoYUV[2][3] = 0.0f;
		l_glmRGBtoYUV[3][3] = 1.0f;
		TEST_EQUAL( l_mtxRGBtoYUV, l_glmRGBtoYUV );
		Logger::LogMessage( cuT( "	Matrix Inversion with CastorUtils :" ) );
		Logger::LogMessage( cuT( "		Normal :" ) );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][0], l_mtxRGBtoYUV[1][0], l_mtxRGBtoYUV[2][0], l_mtxRGBtoYUV[3][0] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][1], l_mtxRGBtoYUV[1][1], l_mtxRGBtoYUV[2][1], l_mtxRGBtoYUV[3][1] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][2], l_mtxRGBtoYUV[1][2], l_mtxRGBtoYUV[2][2], l_mtxRGBtoYUV[3][2] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxRGBtoYUV[0][3], l_mtxRGBtoYUV[1][3], l_mtxRGBtoYUV[2][3], l_mtxRGBtoYUV[3][3] );
		Matrix4x4r l_mtxYUVtoRGB( l_mtxRGBtoYUV.get_inverse() );
		Logger::LogMessage( cuT( "		Inverted :" ) );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][0], l_mtxYUVtoRGB[1][0], l_mtxYUVtoRGB[2][0], l_mtxYUVtoRGB[3][0] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][1], l_mtxYUVtoRGB[1][1], l_mtxYUVtoRGB[2][1], l_mtxYUVtoRGB[3][1] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][2], l_mtxYUVtoRGB[1][2], l_mtxYUVtoRGB[2][2], l_mtxYUVtoRGB[3][2] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_mtxYUVtoRGB[0][3], l_mtxYUVtoRGB[1][3], l_mtxYUVtoRGB[2][3], l_mtxYUVtoRGB[3][3] );
		Logger::LogMessage( cuT( "	Matrix Inversion with GLM :" ) );
		Logger::LogMessage( cuT( "		Normal :" ) );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmRGBtoYUV[0][0], l_glmRGBtoYUV[1][0], l_glmRGBtoYUV[2][0], l_glmRGBtoYUV[3][0] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmRGBtoYUV[0][1], l_glmRGBtoYUV[1][1], l_glmRGBtoYUV[2][1], l_glmRGBtoYUV[3][1] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmRGBtoYUV[0][2], l_glmRGBtoYUV[1][2], l_glmRGBtoYUV[2][2], l_glmRGBtoYUV[3][2] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmRGBtoYUV[0][3], l_glmRGBtoYUV[1][3], l_glmRGBtoYUV[2][3], l_glmRGBtoYUV[3][3] );
		glm::mat4x4 l_glmYUVtoRGB( glm::inverse( l_glmRGBtoYUV ) );
		Logger::LogMessage( cuT( "		Inverted :" ) );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmYUVtoRGB[0][0], l_glmYUVtoRGB[1][0], l_glmYUVtoRGB[2][0], l_glmYUVtoRGB[3][0] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmYUVtoRGB[0][1], l_glmYUVtoRGB[1][1], l_glmYUVtoRGB[2][1], l_glmYUVtoRGB[3][1] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmYUVtoRGB[0][2], l_glmYUVtoRGB[1][2], l_glmYUVtoRGB[2][2], l_glmYUVtoRGB[3][2] );
		Logger::LogMessage( cuT( "			%.5f %.5f %.5f %.5f" ), l_glmYUVtoRGB[0][3], l_glmYUVtoRGB[1][3], l_glmYUVtoRGB[2][3], l_glmYUVtoRGB[3][3] );
		TEST_EQUAL( l_mtxYUVtoRGB, l_glmYUVtoRGB );
	}
#endif

	void CastorUtilsTest::ZipFile( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		Path l_folder1 = cuT( "test1" );
		Path l_folder2 = l_folder1 / cuT( "test2" );
		
		Logger::LogMessage( "	First folder creation" );

		if ( File::DirectoryExists( l_folder1 ) || File::DirectoryCreate( l_folder1 ) )
		{
			Logger::LogMessage( "	Second folder creation" );

			if ( File::DirectoryExists( l_folder2 ) || File::DirectoryCreate( l_folder2 ) )
			{
				Path l_binName = l_folder1 / cuT( "binFile.bin" );
				Path l_txtName = l_folder2 / cuT( "txtFile.txt" );
				String l_zipName = cuT( "zipFile.zip" );

				std::vector< uint8_t > l_inBinData( 1024 );
				String l_inTxtData( cuT( "Coucou, comment allez-vous?" ) );

				if ( !File::FileExists( l_binName ) )
				{
					Logger::LogMessage( "	Binary file creation" );
					BinaryFile l_binary( l_binName, File::eOPEN_MODE_WRITE );
					l_binary.WriteArray( l_inBinData.data(), l_inBinData.size() );
				}

				if ( !File::FileExists( l_txtName ) )
				{
					Logger::LogMessage( "	Text file creation" );
					TextFile l_text( l_txtName, File::eOPEN_MODE_WRITE );
					l_text.WriteText( l_inTxtData );
				}

				{
					Logger::LogMessage( "	Deflate the archive" );
					ZipArchive l_def( l_zipName, File::eOPEN_MODE_WRITE );
					l_def.AddFile( l_binName );
					l_def.AddFile( l_txtName );
					l_def.Deflate();
				}

				{
					Logger::LogMessage( "	Inflate the archive" );
					Path l_folder( cuT( "	inflated" ) );

					if ( File::DirectoryExists( l_folder ) || File::DirectoryCreate( l_folder ) )
					{
						ZipArchive l_inf( l_zipName, File::eOPEN_MODE_READ );
						l_inf.Inflate( l_folder );

						String l_outTxtData;

						{
							Logger::LogMessage( "	Check binary file content" );
							BinaryFile l_binary( l_folder / l_binName, File::eOPEN_MODE_READ );
							std::vector< uint8_t > l_outBinData( size_t( l_binary.GetLength() ) );
							l_binary.ReadArray( l_outBinData.data(), l_outBinData.size() );
							TEST_EQUAL( l_outBinData.size(), l_inBinData.size() );
							TEST_CHECK( !std::memcmp( l_outBinData.data(), l_inBinData.data(), std::min( l_outBinData.size(), l_inBinData.size() ) ) );
						}

						{
							Logger::LogMessage( "	Check text file content" );
							TextFile l_text( l_folder / l_txtName, File::eOPEN_MODE_READ );
							l_text.ReadLine( l_outTxtData, l_inTxtData.size() * 2 );
							TEST_EQUAL( l_outTxtData, l_inTxtData );
						}

						std::remove( str_utils::to_str( l_folder / l_binName ).c_str() );
						std::remove( str_utils::to_str( l_folder / l_txtName ).c_str() );
						File::DirectoryDelete( l_folder / l_folder2 );
						File::DirectoryDelete( l_folder / l_folder1 );
						File::DirectoryDelete( l_folder );
					}

					std::remove( str_utils::to_str( l_binName ).c_str() );
					std::remove( str_utils::to_str( l_txtName ).c_str() );
					std::remove( str_utils::to_str( l_zipName ).c_str() );
					File::DirectoryDelete( l_folder2 );
					File::DirectoryDelete( l_folder1 );
				}
			}
			else
			{
				Logger::LogError( "	Couldn't create second folder" );
			}
		}
		else
		{
			Logger::LogError( "	Couldn't create first folder" );
		}
	}

		Logger::LogMessage(	"End test case : ZipFile" );
	}

	void CastorUtilsTest::TestPixelConversions( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		CheckPixelConversions< ePIXEL_FORMAT_L8 >();
		CheckPixelConversions< ePIXEL_FORMAT_L32F >();
		CheckPixelConversions< ePIXEL_FORMAT_A8L8 >();
		CheckPixelConversions< ePIXEL_FORMAT_AL32F >();
		CheckPixelConversions< ePIXEL_FORMAT_A1R5G5B5 >();
		CheckPixelConversions< ePIXEL_FORMAT_A4R4G4B4 >();
		CheckPixelConversions< ePIXEL_FORMAT_R5G6B5 >();
		CheckPixelConversions< ePIXEL_FORMAT_R8G8B8 >();
		CheckPixelConversions< ePIXEL_FORMAT_A8R8G8B8 >();
		CheckPixelConversions< ePIXEL_FORMAT_RGB32F >();
		CheckPixelConversions< ePIXEL_FORMAT_ARGB32F >();
		CheckPixelConversions< ePIXEL_FORMAT_DEPTH16 >();
		CheckPixelConversions< ePIXEL_FORMAT_DEPTH24 >();
		CheckPixelConversions< ePIXEL_FORMAT_DEPTH32 >();
		CheckPixelConversions< ePIXEL_FORMAT_DEPTH24S8 >();
		CheckPixelConversions< ePIXEL_FORMAT_STENCIL8 >();
	}

	void CastorUtilsTest::TestBufferConversions( uint32_t & p_errCount, uint32_t & p_testCount )
	{
		CheckBufferConversions< ePIXEL_FORMAT_L8 >();
		CheckBufferConversions< ePIXEL_FORMAT_L32F >();
		CheckBufferConversions< ePIXEL_FORMAT_A8L8 >();
		CheckBufferConversions< ePIXEL_FORMAT_AL32F >();
		CheckBufferConversions< ePIXEL_FORMAT_A1R5G5B5 >();
		CheckBufferConversions< ePIXEL_FORMAT_A4R4G4B4 >();
		CheckBufferConversions< ePIXEL_FORMAT_R5G6B5 >();
		CheckBufferConversions< ePIXEL_FORMAT_R8G8B8 >();
		CheckBufferConversions< ePIXEL_FORMAT_A8R8G8B8 >();
		CheckBufferConversions< ePIXEL_FORMAT_RGB32F >();
		CheckBufferConversions< ePIXEL_FORMAT_ARGB32F >();
		CheckBufferConversions< ePIXEL_FORMAT_DEPTH16 >();
		CheckBufferConversions< ePIXEL_FORMAT_DEPTH24 >();
		CheckBufferConversions< ePIXEL_FORMAT_DEPTH32 >();
		CheckBufferConversions< ePIXEL_FORMAT_DEPTH24S8 >();
		CheckBufferConversions< ePIXEL_FORMAT_STENCIL8 >();
	}
}
