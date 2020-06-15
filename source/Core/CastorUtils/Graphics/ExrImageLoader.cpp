#include "CastorUtils/Graphics/ExrImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"

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

		PxBufferBaseSPtr doLoad32BitsPerChannel( uint8_t const * input
			, uint32_t size )
		{
			const char * error{ nullptr };
			int w, h;
			float * data{ nullptr };
			LoadEXRFromMemory( &data
				, &w
				, &h
				, input
				, size
				, &error );

			if ( error )
			{
				String err{ error };
				free( (void*)error );
				CU_LoaderError( String( cuT( "Can't load image:\n" ) ) + err );
			}

			PxBufferBaseSPtr result;

			PixelFormat format;
			format = PixelFormat::eR32G32B32A32_SFLOAT;
			result = PxBufferBase::create( Size( w, h )
				, format
				, reinterpret_cast< uint8_t * >( data )
				, format );
			result->flip();
			free( data );

			return result;
		}

		StringArray const & listExtensions()
		{
			static StringArray const list
			{
				cuT( "exr" ),
			};
			return list;
		}
	}

	//************************************************************************************************

	void ExrImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( listExtensions()
			, std::make_unique< ExrImageLoader >() );
	}

	void ExrImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( listExtensions() );
	}

	ImageLayout ExrImageLoader::load( String const & imageFormat
		, uint8_t const * data
		, uint32_t size
		, PxBufferBaseSPtr & buffer )const
	{
		if ( string::upperCase( imageFormat ).find( cuT( "EXR" ) ) != String::npos )
		{
			buffer = doLoad32BitsPerChannel( data, size );
		}

		return ImageLayout{ *buffer };
	}
}
