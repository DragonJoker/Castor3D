#include "CastorUtils/Graphics/StbImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

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
			auto buffer = reinterpret_cast< Pixel * >( pixelBuffer->getPtr() );

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

		PxBufferBaseSPtr doLoad8BitsPerChannel( uint8_t const * input
			, uint32_t size )
		{
			PxBufferBaseSPtr result;
			int x = 0;
			int y = 0;
			int n = 0;
			stbi_set_flip_vertically_on_load( 1u );
			uint8_t * data = stbi_load_from_memory( input
				, size
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

				result = PxBufferBase::create( Size( x, y )
					, PF::getCompressed( format )
					, data
					, format );
				stbi_image_free( data );

				//if ( components != ImageComponents::eA
				//	&& components != ImageComponents::eAll
				//	&& ( n % 2 ) == 0 )
				//{
				//	preMultiplyWithAlpha( result );
				//}
			}
			else
			{
				CU_LoaderError( String( cuT( "Can't load image:\n" ) ) + stbi_failure_reason() );
			}

			return result;
		}

		PxBufferBaseSPtr doLoad32BitsPerChannel( uint8_t const * input
			, uint32_t size )
		{
			PxBufferBaseSPtr result;
			int x = 0;
			int y = 0;
			int n = 0;
			stbi_set_flip_vertically_on_load( 1u );
			float * data = stbi_loadf_from_memory( input
				, size
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
					, PF::getCompressed( format )
					, reinterpret_cast< uint8_t * >( data )
					, format );
				stbi_image_free( data );
			}
			else
			{
				CU_LoaderError( String( cuT( "Can't load image:\n" ) ) + stbi_failure_reason() );
			}

			return result;
		}

		StringArray const & listExtensions()
		{
			static StringArray const list
#if !C3D_UseFreeImage
			{
				cuT( "jpeg" ),
				cuT( "jpg" ),
				cuT( "png" ),
				cuT( "tga" ),
				cuT( "bmp" ),
				cuT( "psd" ),
				cuT( "gif" ),
				cuT( "hdr" ),
				cuT( "pic" ),
				cuT( "pnm" ),
				cuT( "ppm" ),
				cuT( "pgm" ),
				cuT( "tif" ),
			}
#endif
			;
			return list;
		}
	}

	//************************************************************************************************

	void StbImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( listExtensions()
			, std::make_unique< StbImageLoader >() );
	}

	void StbImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( listExtensions() );
	}

	ImageLayout StbImageLoader::load( String const & imageFormat
		, uint8_t const * data
		, uint32_t size
		, PxBufferBaseSPtr & buffer )const
	{
		if ( string::upperCase( imageFormat ).find( cuT( "HDR" ) ) != String::npos )
		{
			buffer = doLoad32BitsPerChannel( data, size );
		}
		else
		{
			buffer = doLoad8BitsPerChannel( data, size );
		}

		return ImageLayout{ ImageLayout::e2D, *buffer };
	}
}
