#include "CastorUtils/Graphics/StbImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"
#include "CastorUtils/Miscellaneous/BitSize.hpp"

#include <ashes/common/Format.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#pragma warning( push )
#pragma warning( disable: 4100 )
#pragma warning( disable: 4365 )
#pragma warning( disable: 5054 )
#pragma warning( disable: 5219 )
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Walloc-zero"
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough="
#pragma GCC diagnostic ignored "-Wshift-negative-value"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include "stb_image.h"
#pragma GCC diagnostic pop
#pragma clang diagnostic pop
#pragma warning( pop )

namespace castor
{
	//************************************************************************************************

	namespace
	{
		PxBufferBaseSPtr doLoad8BitsPerChannel( uint8_t const * input
			, uint32_t size )
		{
			PxBufferBaseSPtr result;
			int width = 0;
			int height = 0;
			int channels = 0;
			stbi_set_flip_vertically_on_load( 1u );
			uint8_t * data = stbi_load_from_memory( input
				, int( size )
				, &width
				, &height
				, &channels
				, 0 );

			if ( data )
			{
				PixelFormat format;

				switch ( channels )
				{
				case 1:
					format = PixelFormat::eR8_SRGB;
					break;
				case 2:
					format = PixelFormat::eR8G8_SRGB;
					break;
				case 3:
					format = PixelFormat::eR8G8B8_SRGB;
					break;
				case 4:
				default:
					format = PixelFormat::eR8G8B8A8_SRGB;
					break;
				}

				result = PxBufferBase::create( Size{ uint32_t( width ), uint32_t( height ) }
					, format
					, data
					, format );
				stbi_image_free( data );
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
			int width = 0;
			int height = 0;
			int channels = 0;
			stbi_set_flip_vertically_on_load( 1u );
			float * data = stbi_loadf_from_memory( input
				, int( size )
				, &width
				, &height
				, &channels
				, 0 );

			if ( data )
			{
				PixelFormat format;

				switch ( channels )
				{
				case 1:
					format = PixelFormat::eR32_SFLOAT;
					break;
				case 2:
					format = PixelFormat::eR32G32_SFLOAT;
					break;
				case 3:
					format = PixelFormat::eR32G32B32_SFLOAT;
					break;
				case 4:
				default:
					format = PixelFormat::eR32G32B32A32_SFLOAT;
					break;
				}

				result = PxBufferBase::create( Size{ uint32_t( width ), uint32_t( height ) }
					, format
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
			{
				cuT( "jpeg" ),
				cuT( "jpg" ),
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
#if !C3D_UseFreeImage
				cuT( "png" ),
#endif
			}
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
