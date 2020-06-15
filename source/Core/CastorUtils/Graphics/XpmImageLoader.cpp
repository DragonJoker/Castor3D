#include "CastorUtils/Graphics/XpmImageLoader.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/PixelBuffer.hpp"

namespace castor
{
	namespace
	{
		StringArray const & listExtensions()
		{
			static StringArray const list
			{
				cuT( "xpm" ),
			};
			return list;
		}

		using R8G8B8Pixel = Pixel< PixelFormat::eR8G8B8_UNORM >;
	}

	void XpmImageLoader::registerLoader( ImageLoader & reg )
	{
		reg.registerLoader( listExtensions()
			, std::make_unique< XpmImageLoader >() );
	}

	void XpmImageLoader::unregisterLoader( ImageLoader & reg )
	{
		reg.unregisterLoader( listExtensions() );
	}

	ImageLayout XpmImageLoader::load( String const & imageFormat
		, uint8_t const * input
		, uint32_t size
		, PxBufferBaseSPtr & outbuffer )const
	{
		auto data = reinterpret_cast< char * const * >( input );
		uint32_t coloursCount = 0;
		uint32_t charCount = 0;
		std::stringstream stream( data[0] );
		uint32_t w, h;
		stream >> w >> h >> coloursCount >> charCount;
		Size imgSize{ w, h };

		if ( !imgSize.getWidth()
			|| !imgSize.getHeight()
			|| !coloursCount
			|| !charCount )
		{
			CU_LoaderError( "Can't load XPM image: Invalid header" );
		}

		if ( imgSize.getHeight() + coloursCount != size - 1 )
		{
			CU_LoaderError( "Can't load XPM image: Invalid data size" );
		}

		// Parse colours
		std::map< std::string, R8G8B8Pixel > colours;
		std::for_each( &data[1], &data[1 + coloursCount]
			, [&colours, &charCount]( char const * line )
			{
				std::string code( &line[0], &line[charCount] );
				char const * it = std::strstr( line, "c " );

				if ( !it )
				{
					it = std::strstr( line, "g " );

					if ( !it )
					{
						CU_LoaderError( "Can't load XPM image: Ill-formed colour line" );
					}
				}

				std::string value( it + 2, &line[std::strlen( line )] );
				R8G8B8Pixel pixel{ true };

				if ( value.find( '#' ) != std::string::npos )
				{
					uint32_t r, g, b;

					if ( std::sscanf( value.c_str(), "#%02X%02X%02X", &r, &g, &b ) == EOF )
					{
						CU_LoaderError( "Can't load XPM image: Invalid image data" );
					}

					std::array< uint8_t, 3u > components{ { uint8_t( r ), uint8_t( g ), uint8_t( b ) } };
					std::memcpy( pixel.ptr(), components.data(), 3 );
				}

				colours[code] = pixel;
			} );

		// Parse image
		auto pixels = std::static_pointer_cast< PxBuffer< PixelFormat::eR8G8B8_UNORM > >( PxBufferBase::create( imgSize, PixelFormat::eR8G8B8_UNORM ) );
		auto buffer = pixels->begin();

		for ( auto it = &data[1 + coloursCount]; it != &data[1 + coloursCount + h]; ++it )
		{
			char const * line = *it;

			for ( uint32_t x = 0; x < w; ++x )
			{
				buffer->set( colours[std::string( line, line + charCount )] );
				line += charCount;
				buffer++;
			}
		}

		outbuffer = PxBufferBase::create( pixels->getDimensions()
			, PixelFormat::eR8G8B8A8_UNORM
			, pixels->getConstPtr()
			, pixels->getFormat() );
		return ImageLayout{ *outbuffer };
	}
}
