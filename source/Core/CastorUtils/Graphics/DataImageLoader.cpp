#include "CastorUtils/Graphics/DataImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

namespace castor
{
	namespace xpml
	{
		static StringArray const & listExtensions()
		{
			static StringArray const list
			{
#define CUPF_ENUM_VALUE( name, value, components, alpha, colour, depth, stencil, compressed )\
				getFormatName( PixelFormat::e##name ),
#include "CastorUtils/Graphics/PixelFormat.enum"
			};
			return list;
		}

		using R8G8B8Pixel = Pixel< PixelFormat::eR8G8B8_UNORM >;
	}

	void DataImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( xpml::listExtensions()
			, std::make_unique< DataImageLoader >() );
	}

	void DataImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( xpml::listExtensions() );
	}

	ImageLayout DataImageLoader::load( String const & imageFormat
		, uint8_t const * input
		, uint32_t size
		, PxBufferBaseSPtr & outbuffer )const
	{
		auto format = getFormatByName( imageFormat );
		auto bitsize = getBytesPerPixel( format );
		auto dim = uint32_t( sqrt( double( size / bitsize ) ) );
		outbuffer = PxBufferBase::create( castor::Size{ dim, dim }
			, format
			, input
			, format );
		return ImageLayout{ ImageLayout::e2D, *outbuffer };
	}
}
