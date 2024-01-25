#include "CastorUtils/Graphics/DataImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

namespace castor
{
	namespace datal
	{
		static StringArray const DataExtensions
		{
#define CUPF_ENUM_VALUE( name, value, components, alpha, colour, depth, stencil, compressed )\
				getFormatName( PixelFormat::e##name ),
#include "CastorUtils/Graphics/PixelFormat.enum"
		};

		static StringArray const & listExtensions()
		{
			return DataExtensions;
		}

		using R8G8B8Pixel = Pixel< PixelFormat::eR8G8B8_UNORM >;
	}

	void DataImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( datal::listExtensions()
			, castor::make_unique< DataImageLoader >() );
	}

	void DataImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( datal::listExtensions() );
	}

	ImageLayout DataImageLoader::load( String const & imageFormat
		, uint8_t const * input
		, uint32_t size
		, PxBufferBaseUPtr & outbuffer )const
	{
		auto format = getFormatByName( imageFormat );
		auto bitsize = getBytesPerPixel( format );
		auto dim = uint32_t( sqrt( double( size / bitsize ) ) );
		outbuffer = PxBufferBase::create( Size{ dim, dim }
			, format
			, input
			, format );
		return ImageLayout{ ImageLayout::e2D, *outbuffer };
	}
}
