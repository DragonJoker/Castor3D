#include "CastorUtils/Graphics/ExrImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

#define TINYEXR_IMPLEMENTATION
#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include "tinyexr.h"
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor
{
	//************************************************************************************************

	namespace exrl
	{
		static PxBufferBaseUPtr doLoad32BitsPerChannel( uint8_t const * input
			, uint32_t size )
		{
			const char * error{ nullptr };
			int w{};
			int h{};
			float * data{ nullptr };
			LoadEXRFromMemory( &data
				, &w
				, &h
				, input
				, size
				, &error );

			if ( error )
			{
				MbString msg = error;
				FreeEXRErrorMessage( error );
				CU_LoaderError( "Can't load image:\n" + msg );
			}

			using ByteCPtr = uint8_t const *;
			auto result = PxBufferBase::create( Size{ uint32_t( w ), uint32_t( h ) }
				, PixelFormat::eR32G32B32A32_SFLOAT
				, ByteCPtr( data )
				, PixelFormat::eR32G32B32A32_SFLOAT );
			result->flip();
			free( data );

			return result;
		}

		static StringArray const ExrExtensions
		{
			cuT( "exr" ),
		};

		static StringArray const & listExtensions()
		{
			return ExrExtensions;
		}
	}

	//************************************************************************************************

	void ExrImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( exrl::listExtensions()
			, castor::make_unique< ExrImageLoader >() );
	}

	void ExrImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( exrl::listExtensions() );
	}

	ImageLayout ExrImageLoader::load( String const & imageFormat
		, uint8_t const * data
		, uint32_t size
		, PxBufferBaseUPtr & buffer )const
	{
		if ( string::upperCase( imageFormat ).find( cuT( "EXR" ) ) != String::npos )
		{
			buffer = exrl::doLoad32BitsPerChannel( data, size );
		}

		return ImageLayout{ ImageLayout::e2D, *buffer };
	}
}
