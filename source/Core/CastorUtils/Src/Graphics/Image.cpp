#include "Graphics/Image.hpp"

#include "Data/LoaderException.hpp"
#include "Data/Path.hpp"
#include "Graphics/Rectangle.hpp"
#include "Log/Logger.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#if defined( CU_CompilerMSVC )
#	define STBI_MSC_SECURE_CRT
#endif
#include "stb_image_write.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

namespace castor
{
	//************************************************************************************************

	namespace
	{
		bool preMultiplyWithAlpha( PxBufferBaseSPtr pixelBuffer )
		{
			struct Pixel
			{
				uint8_t r;
				uint8_t g;
				uint8_t b;
				uint8_t a;
			};
			int width = pixelBuffer->getWidth();
			int height = pixelBuffer->getHeight();
			auto buffer = reinterpret_cast< Pixel * >( pixelBuffer->ptr() );

			for ( int y = 0; y < height; ++y )
			{
				for ( int x = 0; x < width; ++x )
				{
					const uint8_t alpha = buffer->a;

					// slightly faster: care for two special cases
					if ( alpha == 0x00 )
					{
						// special case for alpha == 0x00
						// color * 0x00 / 0xFF = 0x00
						buffer->r = 0x00;
						buffer->g = 0x00;
						buffer->b = 0x00;
					}
					else if ( alpha == 0xFF )
					{
						// nothing to do for alpha == 0xFF
						// color * 0xFF / 0xFF = color
						continue;
					}
					else
					{
						buffer->r = uint8_t( ( alpha * uint16_t( buffer->r ) + 127 ) / 255 );
						buffer->g = uint8_t( ( alpha * uint16_t( buffer->g ) + 127 ) / 255 );
						buffer->b = uint8_t( ( alpha * uint16_t( buffer->b ) + 127 ) / 255 );
					}

					++buffer;
				}
			}

			return true;
		}

		PxBufferBaseSPtr doLoad8BitsPerChannel( Path const & path
			, bool dropAlpha )
		{
			PxBufferBaseSPtr result;
			int x = 0;
			int y = 0;
			int n = 0;
			stbi_set_flip_vertically_on_load( 1u );
			uint8_t * data = stbi_load( string::stringCast< char >( path ).c_str()
				, &x
				, &y
				, &n
				, 0 );

			if ( data )
			{
				PixelFormat format;

				switch ( n )
				{
				case 1:
					format = PixelFormat::eR8_UNORM;
					break;

				case 2:
					format = PixelFormat::eR8A8_UNORM;
					break;

				case 3:
					format = PixelFormat::eR8G8B8_UNORM;
					break;

				case 4:
					format = PixelFormat::eR8G8B8A8_UNORM;
					break;
				}

				result = PxBufferBase::create( Size( x, y ), format, data, format );
				stbi_image_free( data );

				if ( !dropAlpha && ( n % 2 ) == 0 )
				{
					preMultiplyWithAlpha( result );
				}
			}
			else
			{
				CU_LoaderError( cuT( "Couldn't load file [" ) + path + cuT( "]:\n" ) + stbi_failure_reason() );
			}

			return result;
		}

		PxBufferBaseSPtr doLoad32BitsPerChannel( Path const & path
			, bool dropAlpha )
		{
			PxBufferBaseSPtr result;
			int x = 0;
			int y = 0;
			int n = 0;
			stbi_set_flip_vertically_on_load( 1u );
			float * data = stbi_loadf( string::stringCast< char >( path ).c_str()
				, &x
				, &y
				, &n
				, 0 );

			if ( data )
			{
				PixelFormat format;

				switch ( n )
				{
				case 1:
					format = PixelFormat::eR32_SFLOAT;
					break;

				case 2:
					format = PixelFormat::eR32A32_SFLOAT;
					break;

				case 3:
					format = PixelFormat::eR32G32B32_SFLOAT;
					break;

				case 4:
					format = PixelFormat::eR32G32B32A32_SFLOAT;
					break;
				}

				result = PxBufferBase::create( Size( x, y )
					, format
					, reinterpret_cast< uint8_t * >( data )
					, format );
				stbi_image_free( data );
			}
			else
			{
				CU_LoaderError( cuT( "Couldn't load file [" ) + path + cuT( "]:\n" ) + stbi_failure_reason() );
			}

			return result;
		}

		bool doSave8BitsPerChannel( Path const & path
			, PxBufferBaseSPtr buffer )
		{
			bool result = buffer != nullptr;

			if ( result )
			{
				int n = 0;

				switch ( buffer->format() )
				{
				case PixelFormat::eR8_UNORM:
					n = 1;
					break;

				case PixelFormat::eR8A8_UNORM:
					n = 2;
					break;

				case PixelFormat::eR8G8B8_UNORM:
				case PixelFormat::eR8G8B8_SRGB:
				case PixelFormat::eB8G8R8_UNORM:
				case PixelFormat::eB8G8R8_SRGB:
					n = 3;
					break;

				case PixelFormat::eA8B8G8R8_UNORM:
				case PixelFormat::eA8B8G8R8_SRGB:
				case PixelFormat::eR8G8B8A8_UNORM:
				case PixelFormat::eR8G8B8A8_SRGB:
					n = 4;
					break;
				}

				if ( !n )
				{
					CU_LoaderError( cuT( "Couldn't save file " ) + path + cuT( ":\nUnsupported pixel format." ) );
				}

				int x = int( buffer->getWidth() );
				int y = int( buffer->getHeight() );
				stbi_flip_vertically_on_write( 1u );
				auto extension = string::upperCase( path.getExtension() );

				if ( extension == "PNG" )
				{
					result = stbi_write_png( string::stringCast< char >( path ).c_str()
						, x
						, y
						, n
						, buffer->constPtr()
						, int( buffer->size() / buffer->getHeight() ) ) != 0;
				}
				else if ( extension == "BMP" )
				{
					result = stbi_write_bmp( string::stringCast< char >( path ).c_str()
						, x
						, y
						, n
						, buffer->constPtr() ) != 0;
				}
				else if ( extension == "TGA" )
				{
					result = stbi_write_tga( string::stringCast< char >( path ).c_str()
						, x
						, y
						, n
						, buffer->constPtr() ) != 0;
				}
				else if ( extension == "JPG"
					|| extension == "JPEG" )
				{
					result = stbi_write_jpg( string::stringCast< char >( path ).c_str()
						, x
						, y
						, n
						, buffer->constPtr()
						, 90 ) != 0;
				}
				else
				{
					CU_LoaderError( cuT( "Couldn't save file " ) + path + cuT( ":\nUnsupported image format." ) );
				}

				if ( !result )
				{
					CU_LoaderError( cuT( "Couldn't load file [" ) + path + cuT( "]:\n" ) + stbi_failure_reason() );
				}
			}
			else
			{
				CU_LoaderError( cuT( "Couldn't save file " ) + path + cuT( ":\nNull buffer given for save." ) );
			}

			return result;
		}

		bool doSave32BitsPerChannel( Path const & path
			, PxBufferBaseSPtr buffer )
		{
			bool result = buffer != nullptr;

			if ( result )
			{
				int n = 0;

				switch ( buffer->format() )
				{
				case PixelFormat::eR16_SFLOAT:
				case PixelFormat::eR32_SFLOAT:
					n = 1;
					break;

				case PixelFormat::eR16A16_SFLOAT:
				case PixelFormat::eR32A32_SFLOAT:
					n = 2;
					break;

				case PixelFormat::eR16G16B16_SFLOAT:
				case PixelFormat::eR32G32B32_SFLOAT:
					n = 3;
					break;

				case PixelFormat::eR16G16B16A16_SFLOAT:
				case PixelFormat::eR32G32B32A32_SFLOAT:
					n = 4;
					break;
				}

				if ( !n )
				{
					CU_LoaderError( cuT( "Couldn't save file " ) + path + cuT( ":\nUnsupported pixel format." ) );
				}

				int x = int( buffer->getWidth() );
				int y = int( buffer->getHeight() );
				Path realPath = path.getPath() / ( path.getFileName() + ".hdr" );
				stbi_flip_vertically_on_write( 1u );
				auto data = reinterpret_cast< float const * >( buffer->constPtr() );
				result = stbi_write_hdr( string::stringCast< char >( realPath ).c_str()
					, x
					, y
					, n
					, data ) != 0;

				if ( !result )
				{
					CU_LoaderError( cuT( "Couldn't load file [" ) + path + cuT( "]:\n" ) + stbi_failure_reason() );
				}
			}
			else
			{
				CU_LoaderError( cuT( "Couldn't save file " ) + path + cuT( ":\nNull buffer given for save." ) );
			}

			return result;
		}
	}

	//************************************************************************************************

	Image::BinaryLoader::BinaryLoader()
	{
	}

	bool Image::BinaryLoader::operator()( Image & image
		, Path const & path
		, bool dropAlpha )
	{
		if ( path.empty() )
		{
			CU_LoaderError( "Can't load image : path is empty" );
		}

		image.m_buffer.reset();
		auto extension = string::upperCase( path.getExtension() );

		if ( extension.find( cuT( "HDR" ) ) != String::npos )
		{
			image.m_buffer = doLoad32BitsPerChannel( path, dropAlpha );
		}
		else
		{
			image.m_buffer = doLoad8BitsPerChannel( path, dropAlpha );
		}

		return image.m_buffer != nullptr;
	}

	//************************************************************************************************

	Image::BinaryWriter::BinaryWriter()
	{
	}

	bool Image::BinaryWriter::operator()( Image const & image, Path const & path )
	{
		bool result = false;

		if ( path.empty() )
		{
			CU_LoaderError( "Can't save image : path is empty" );
		}

		if ( image.getPixelFormat() == PixelFormat::eR16A16_SFLOAT
			|| image.getPixelFormat() == PixelFormat::eR32A32_SFLOAT
			|| image.getPixelFormat() == PixelFormat::eR16_SFLOAT
			|| image.getPixelFormat() == PixelFormat::eR32_SFLOAT
			|| image.getPixelFormat() == PixelFormat::eR16G16B16_SFLOAT
			|| image.getPixelFormat() == PixelFormat::eR32G32B32_SFLOAT
			|| image.getPixelFormat() == PixelFormat::eR16G16B16A16_SFLOAT
			|| image.getPixelFormat() == PixelFormat::eR32G32B32A32_SFLOAT )
		{
			result = doSave32BitsPerChannel( path, image.m_buffer );
		}
		else
		{
			result = doSave8BitsPerChannel( path, image.m_buffer );
		}

		return result;
	}

	//************************************************************************************************

	Image::Image( String const & name
		, Size const & size
		, PixelFormat format
		, ByteArray const & buffer
		, PixelFormat bufferFormat )
		: Resource< Image >( name )
		, m_buffer( PxBufferBase::create( size, format, &buffer[0], bufferFormat ) )
	{
		CU_CheckInvariants();
	}

	Image::Image( String const & name
		, Size const & size
		, PixelFormat format
		, uint8_t const * buffer
		, PixelFormat bufferFormat )
		: Resource< Image >( name )
		, m_buffer( PxBufferBase::create( size, format, buffer, bufferFormat ) )
	{
		CU_CheckInvariants();
	}

	Image::Image( String const & name
		, PxBufferBase const & buffer )
		: Resource< Image >( name )
		, m_buffer( buffer.clone() )
	{
		CU_CheckInvariants();
	}

	Image::Image( String const & name
		, Path const & pathFile
		, bool dropAlpha )
		: Resource< Image >( name )
		, m_pathFile( pathFile )
	{
		Image::BinaryLoader()( *this, pathFile, dropAlpha );
	}

	Image::Image( Image const & image )
		: Resource< Image >( image )
		, m_pathFile( image.m_pathFile )
		, m_buffer( image.m_buffer->clone() )
	{
		CU_CheckInvariants();
	}

	Image::Image( Image && image )
		: Resource< Image >( std::move( image ) )
		, m_pathFile( std::move( image.m_pathFile ) )
		, m_buffer( std::move( image.m_buffer ) )
	{
		image.m_pathFile.clear();
		image.m_buffer.reset();
		CU_CheckInvariants();
	}

	Image & Image::operator=( Image const & image )
	{
		Resource< Image >::operator=( image );
		m_pathFile = image.m_pathFile;
		m_buffer = image.m_buffer->clone();
		CU_CheckInvariants();
		return * this;
	}

	Image & Image::operator =( Image && image )
	{
		Resource< Image >::operator=( std::move( image ) );
		m_pathFile = std::move( image.m_pathFile );
		m_buffer = std::move( image.m_buffer );
		image.m_pathFile.clear();
		image.m_buffer.reset();
		CU_CheckInvariants();
		return * this;
	}

	Image::~Image()
	{
	}

	Image & Image::resample( Size const & size )
	{
		using ResizeFunc = int( *)( const unsigned char * input_pixels, int input_w, int input_h, int input_stride_in_bytes,
			unsigned char *output_pixels, int output_w, int output_h, int output_stride_in_bytes,
			int num_channels, int alpha_channel, int flags,
			stbir_edge edge_wrap_mode, stbir_filter filter, stbir_colorspace space,
			void *alloc_context );

		auto srcBuffer = getPixels();
		int channels = int( PF::getComponentsCount( srcBuffer->format() ) );
		int alpha = STBIR_ALPHA_CHANNEL_NONE;
		stbir_colorspace colorSpace{ STBIR_COLORSPACE_LINEAR };
		stbir_datatype dataType = STBIR_TYPE_UINT8;

		switch ( srcBuffer->format() )
		{
		case PixelFormat::eR8_UNORM:
		case PixelFormat::eR8A8_UNORM:
		case PixelFormat::eA8B8G8R8_UNORM:
		case PixelFormat::eR8G8B8A8_UNORM:
			alpha = 1;
			break;

		case PixelFormat::eR8G8B8_SRGB:
		case PixelFormat::eB8G8R8_SRGB:
			colorSpace = STBIR_COLORSPACE_SRGB;
			break;

		case PixelFormat::eA8B8G8R8_SRGB:
		case PixelFormat::eR8G8B8A8_SRGB:
			colorSpace = STBIR_COLORSPACE_SRGB;
			alpha = 1;
			break;

		case PixelFormat::eR16_SFLOAT:
		case PixelFormat::eR32_SFLOAT:
		case PixelFormat::eR16G16B16_SFLOAT:
		case PixelFormat::eR32G32B32_SFLOAT:
			dataType = STBIR_TYPE_FLOAT;
			break;

		case PixelFormat::eR16A16_SFLOAT:
		case PixelFormat::eR32A32_SFLOAT:
		case PixelFormat::eR16G16B16A16_SFLOAT:
		case PixelFormat::eR32G32B32A32_SFLOAT:
			dataType = STBIR_TYPE_FLOAT;
			alpha = 1;
			break;

		default:
			CU_LoaderError( "Unsupported image format for resize." );
			break;
		}

		auto dstBuffer = PxBufferBase::create( size
			, srcBuffer->format() );
		auto result = stbir_resize( srcBuffer->constPtr(), int( srcBuffer->getWidth() ), int( srcBuffer->getHeight() ), 0
			, dstBuffer->ptr(), int( dstBuffer->getWidth() ), int( dstBuffer->getHeight() ), 0
			, dataType
			, channels, alpha, 0
			, STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP
			, STBIR_FILTER_CATMULLROM, STBIR_FILTER_CATMULLROM
			, colorSpace, nullptr );

		if ( !result )
		{
			CU_LoaderError( stbi_failure_reason() );
		}

		m_buffer = dstBuffer;
		return *this;
	}

	CU_BeginInvariantBlock( Image )
	CU_CheckInvariant( m_buffer->count() > 0 );
	CU_EndInvariantBlock()

	Image & Image::fill( RgbColour const & colour )
	{
		CU_CheckInvariants();
		setPixel( 0, 0, colour );
		uint32_t uiBpp = PF::getBytesPerPixel( getPixelFormat() );

		for ( uint32_t i = uiBpp; i < m_buffer->size(); i += uiBpp )
		{
			memcpy( &m_buffer->ptr()[i], &m_buffer->constPtr()[0], uiBpp );
		}

		CU_CheckInvariants();
		return * this;
	}

	Image & Image::fill( RgbaColour const & colour )
	{
		CU_CheckInvariants();
		setPixel( 0, 0, colour );
		uint32_t uiBpp = PF::getBytesPerPixel( getPixelFormat() );

		for ( uint32_t i = uiBpp; i < m_buffer->size(); i += uiBpp )
		{
			memcpy( &m_buffer->ptr()[i], &m_buffer->constPtr()[0], uiBpp );
		}

		CU_CheckInvariants();
		return * this;
	}

	Image & Image::setPixel( uint32_t x
		, uint32_t y
		, uint8_t const * pixel
		, PixelFormat format )
	{
		CU_CheckInvariants();
		CU_Require( x < getWidth() && y < getHeight() && pixel );
		uint8_t const * src = pixel;
		uint8_t * dst = &( *m_buffer->getAt( x, y ) );
		PF::convertPixel( format, src, getPixelFormat(), dst );
		CU_CheckInvariants();
		return * this;
	}

	Image & Image::setPixel( uint32_t x
		, uint32_t y
		, RgbColour const & colour )
	{
		CU_CheckInvariants();
		CU_Require( x < getWidth() && y < getHeight() );
		Point4ub components = toBGRAByte( colour );
		uint8_t const * src = components.constPtr();
		uint8_t * dst = &( *m_buffer->getAt( x, y ) );
		PF::convertPixel( PixelFormat::eR8G8B8A8_UNORM, src, getPixelFormat(), dst );
		CU_CheckInvariants();
		return * this;
	}

	Image & Image::setPixel( uint32_t x
		, uint32_t y
		, RgbaColour const & colour )
	{
		CU_CheckInvariants();
		CU_Require( x < getWidth() && y < getHeight() );
		Point3ub components = toBGRByte( colour );
		uint8_t const * src = components.constPtr();
		uint8_t * dst = &( *m_buffer->getAt( x, y ) );
		PF::convertPixel( PixelFormat::eR8G8B8_UNORM, src, getPixelFormat(), dst );
		CU_CheckInvariants();
		return * this;
	}

	void Image::getPixel( uint32_t x
		, uint32_t y
		, uint8_t * pixel
		, PixelFormat format )const
	{
		CU_CheckInvariants();
		CU_Require( x < getWidth() && y < getHeight() && pixel );
		uint8_t const * src = &( *m_buffer->getAt( x, y ) );
		uint8_t * dst = pixel;
		PF::convertPixel( getPixelFormat(), src, format, dst );
		CU_CheckInvariants();
	}

	RgbaColour Image::getPixel( uint32_t x
		, uint32_t y )const
	{
		CU_CheckInvariants();
		CU_Require( x < getWidth() && y < getHeight() );
		Point4ub components;
		uint8_t const * src = &( *m_buffer->getAt( x, y ) );
		uint8_t * dst = components.ptr();
		PF::convertPixel( getPixelFormat(), src, PixelFormat::eR8G8B8A8_UNORM, dst );
		CU_CheckInvariants();
		return RgbaColour::fromBGRA( components );
	}

	Image & Image::copyImage( Image const & toCopy )
	{
		CU_CheckInvariants();

		if ( getDimensions() == toCopy.getDimensions() )
		{
			if ( getPixelFormat() == toCopy.getPixelFormat() )
			{
				memcpy( m_buffer->ptr(), toCopy.m_buffer->ptr(), m_buffer->size() );
			}
			else
			{
				for ( uint32_t i = 0; i < getWidth(); ++i )
				{
					for ( uint32_t j = 0; j < getHeight(); ++j )
					{
						uint8_t const * src = &( *toCopy.m_buffer->getAt( i, j ) );
						uint8_t * dst = &( *m_buffer->getAt( i, j ) );
						PF::convertPixel( getPixelFormat(), src, getPixelFormat(), dst );
					}
				}
			}
		}
		else
		{
			Point2d srcStep( static_cast< double >( toCopy.getWidth() ) / getWidth(), static_cast< double >( toCopy.getHeight() ) / getHeight() );
			ByteArray srcPix( PF::getBytesPerPixel( toCopy.getPixelFormat() ), 0 );

			for ( uint32_t i = 0; i < getWidth(); ++i )
			{
				for ( uint32_t j = 0; j < getHeight(); ++j )
				{
					toCopy.getPixel( static_cast< uint32_t >( i * srcStep[0] ), static_cast< uint32_t >( j * srcStep[1] ), srcPix.data(), toCopy.getPixelFormat() );
					setPixel( i, j, srcPix.data(), toCopy.getPixelFormat() );
				}
			}
		}

		CU_CheckInvariants();
		return * this;
	}

	Image Image::subImage( Rectangle const & rect )const
	{
		CU_CheckInvariants();
		CU_Require( Rectangle( 0, 0 , getWidth(), getHeight() ).intersects( rect ) == Intersection::eIn );
		Size size( rect.getWidth(), rect.getHeight() );
		// Création de la sous-image à remplir
		Image img( m_name + cuT( "_Sub" ) + string::toString( rect[0] ) + cuT( "x" ) + string::toString( rect[1] ) + cuT( ":" ) + string::toString( size.getWidth() ) + cuT( "x" ) + string::toString( size.getHeight() ), size, getPixelFormat() );
		// Calcul de variables temporaires
		uint8_t const * src = &( *m_buffer->getAt( rect.left(), rect.top() ) );
		uint8_t * dst = &( *img.m_buffer->getAt( 0, 0 ) );
		uint32_t srcPitch = getWidth() * PF::getBytesPerPixel( getPixelFormat() );
		uint32_t dstPitch = img.getWidth() * PF::getBytesPerPixel( img.getPixelFormat() );

		// Copie des pixels de l'image originale dans la sous-image
		for ( int i = rect.left(); i < rect.right(); ++i )
		{
			std::memcpy( dst, src, dstPitch );
			src += srcPitch;
			dst += dstPitch;
		}

		CU_CheckInvariants();
		return img;
	}

	Image & Image::flip()
	{
		CU_CheckInvariants();
		m_buffer->flip();
		CU_CheckInvariants();
		return * this;
	}

	Image & Image::mirror()
	{
		CU_CheckInvariants();
		m_buffer->mirror();
		CU_CheckInvariants();
		return * this;
	}
}
