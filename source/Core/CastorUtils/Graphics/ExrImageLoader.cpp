#include "CastorUtils/Graphics/ExrImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

#define TINYEXR_IMPLEMENTATION
#pragma warning( push )
#pragma warning( disable: 4242 )
#pragma warning( disable: 4365 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Walloc-zero"
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include "tinyexr.h"
#pragma GCC diagnostic pop
#pragma warning( pop )

namespace castor
{
	//************************************************************************************************

	namespace
	{
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
				auto msg = string::stringCast< xchar >( error );
				FreeEXRErrorMessage( error );
				CU_LoaderError( String( cuT( "Can't load image:\n" ) ) + msg );
			}

			PxBufferBaseSPtr result;

			PixelFormat format;
			format = PixelFormat::eR32G32B32A32_SFLOAT;
			result = PxBufferBase::create( Size{ uint32_t( w ), uint32_t( h ) }
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

		return ImageLayout{ ImageLayout::e2D, *buffer };
	}

	PixelFormat ExrImageLoader::getFormat( Path const & imagePath )const
	{
		return PixelFormat::eR32G32B32A32_SFLOAT;
	}
}
