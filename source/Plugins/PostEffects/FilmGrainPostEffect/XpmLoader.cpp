#include "FilmGrainPostEffect/XpmLoader.hpp"

#include <CastorUtils/Graphics/PixelBuffer.hpp>

using namespace castor;

namespace film_grain
{
	using Pixel = castor::Pixel< PixelFormat::eR8G8B8_UNORM >;

	XpmLoader::XpmLoader()
	{
	}

	XpmLoader::~XpmLoader()
	{
	}

	PxBufferBaseSPtr XpmLoader::loadImage( char const ** data
		, size_t size )
	{
		if ( !data )
		{
			CU_Exception( "XpmImage - Null data" );
		}

		if ( !size )
		{
			CU_Exception( "XpmImage - Empty file" );
		}

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
			CU_Exception( "XpmImage - Invalid header" );
		}

		if ( imgSize.getHeight() + coloursCount != size - 1 )
		{
			CU_Exception( "XpmImage - Invalid data size" );
		}

		// Parse colours
		std::map< std::string, Pixel > colours;
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
						CU_Exception( "XpmImage - Malformed colour line" );
					}
				}

				std::string value( it + 2, &line[std::strlen( line )] );
				Pixel pixel{ true };

				if ( value.find( '#' ) != std::string::npos )
				{
					uint32_t r, g, b;

					if ( std::sscanf( value.c_str(), "#%02X%02X%02X", &r, &g, &b ) == EOF )
					{
						CU_Exception( "XpmImage - Invalid image data" );
					}

					std::array< uint8_t, 3u > components{ { uint8_t( r ), uint8_t( g ), uint8_t( b ) } };
					std::memcpy( pixel.ptr(), components.data(), 3 );
				}

				colours[code] = pixel;
			} );

		// Parse image
		auto pixels = std::static_pointer_cast< PxBuffer< PixelFormat::eR8G8B8_UNORM > >( PxBufferBase::create( imgSize, PixelFormat::eR8G8B8_UNORM ) );
		auto buffer = pixels->begin();

		for ( char const ** it = &data[1 + coloursCount]; it != &data[1 + coloursCount + h]; ++it )
		{
			char const * line = *it;

			for ( uint32_t x = 0; x < w; ++x )
			{
				buffer->set( colours[std::string( line, line + charCount )] );
				line += charCount;
				buffer++;
			}
		}

		return PxBufferBase::create( pixels->dimensions()
			, PixelFormat::eR8G8B8A8_UNORM
			, pixels->constPtr()
			, pixels->format() );
	}
}
