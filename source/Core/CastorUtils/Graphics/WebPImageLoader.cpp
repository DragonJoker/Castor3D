#include "CastorUtils/Graphics/WebPImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <webp/decode.h>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor
{
	//************************************************************************************************

	namespace webpl
	{
		static StringArray const WebPExtensions
		{
			cuT( "webp" ),
		};

		static StringArray const & listExtensions()
		{
			return WebPExtensions;
		}
	}

	//************************************************************************************************

	void WebPImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( webpl::listExtensions()
			, castor::make_unique< WebPImageLoader >() );
	}

	void WebPImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( webpl::listExtensions() );
	}

	ImageLayout WebPImageLoader::load( String const & imageFormat
		, uint8_t const * data
		, uint32_t size
		, PxBufferBaseUPtr & buffer )const
	{
		WebPDecoderConfig config;
		if ( !WebPInitDecoderConfig( &config ) )
		{
			CU_LoaderError( "Can't initialise libwebp" );
		}

		if ( WebPGetFeatures( data, size, &config.input ) != VP8_STATUS_OK )
		{
			CU_LoaderError( "Can't retrieve image features" );
		}

		ImageLayout result;
		result.type = ImageLayout::Type::e2D;
		result.format = PixelFormat::eR8G8B8A8_UNORM;
		result.extent = { config.input.width, config.input.height, 1u };
		result.layers = 1u;
		result.levels = 1u;
		result.alignment = uint32_t( getBytesPerPixel( result.format ) );
		buffer = PxBufferBase::create( result.dimensions()
			, result.layers
			, result.levels
			, result.format );
		buffer->flip();
		config.output.colorspace = MODE_RGBA;
		config.output.u.RGBA.rgba = buffer->getPtr();
		config.output.u.RGBA.stride = int( buffer->getAlign() * buffer->getWidth() );
		config.output.u.RGBA.size = size_t( buffer->getSize() );
		config.output.is_external_memory = 1;

		if ( WebPDecode( data, size, &config ) != VP8_STATUS_OK )
		{
			CU_LoaderError( "Can't decode image" );
		}

		return result;
	}
}
