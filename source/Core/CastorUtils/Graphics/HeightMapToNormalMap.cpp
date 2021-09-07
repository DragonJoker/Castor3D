#include "CastorUtils/Graphics/HeightMapToNormalMap.hpp"

#include "CastorUtils/Graphics/Image.hpp"

#include <vector>
#include <string>

namespace castor
{
	namespace
	{
		int clamp( int value, int max )
		{
			if ( value >= max )
			{
				value %= max;
			}

			while ( value < 0 )
			{
				value += max;
			}

			return value;
		}

		size_t getIndex( int x, int y, int width, int CU_UnusedParam( height ) )
		{
			return size_t( y * width + x );
		}

		float getHeight( castor::ArrayView< uint8_t const > const & data
			, int width
			, int height
			, int x
			, int y )
		{
			x = clamp( x, width );
			y = clamp( y, height );
			return float( data[getIndex( x, y, width, height )] ) / 255.0f;
		}

		uint8_t textureCoordinateToRgb( float value )
		{
			return uint8_t( ( value * 0.5 + 0.5 ) * 255.0 );
		}

		castor::Point4f calculateNormal( float strength
			, castor::ArrayView< uint8_t const > const & data
			, int width
			, int height
			, int x
			, int y )
		{
			// surrounding pixels
			float tl = getHeight( data, width, height, x - 1, y - 1 ); // top left
			float  l = getHeight( data, width, height, x - 1, y );   // left
			float bl = getHeight( data, width, height, x - 1, y + 1 ); // bottom left
			float  t = getHeight( data, width, height, x, y - 1 );   // top
			float  c = getHeight( data, width, height, x, y );   // center
			float  b = getHeight( data, width, height, x, y + 1 );   // bottom
			float tr = getHeight( data, width, height, x + 1, y - 1 ); // top right
			float  r = getHeight( data, width, height, x + 1, y );   // right
			float br = getHeight( data, width, height, x + 1, y + 1 ); // bottom right

			// sobel filter
			const float dX = float( ( tr + 2.0 * r + br ) - ( tl + 2.0 * l + bl ) );
			const float dY = float( ( bl + 2.0 * b + br ) - ( tl + 2.0 * t + tr ) );
			const float dZ = float( 1.0 / strength );

			castor::Point3f n{ dX, dY, dZ };
			castor::point::normalise( n );
			return { n->x, n->y, n->z, c };
		}

		castor::ByteArray calculateNormals( float strength
			, castor::ArrayView< uint8_t const > const & data
			, castor::Size const & size )
		{
			castor::ByteArray result;
			result.resize( data.size() * 4u );
			auto width = int( size.getWidth() );
			auto height = int( size.getHeight() );

			for ( int x = 0; x < width; ++x )
			{
				for ( int y = 0; y < height; ++y )
				{
					auto n = calculateNormal( strength, data, width, height, x, y );

					 // convert to rgb
					auto coord = getIndex( x, y, width, height ) * 4;
					result[coord + 0] = textureCoordinateToRgb( n->x );
					result[coord + 1] = textureCoordinateToRgb( n->y );
					result[coord + 2] = textureCoordinateToRgb( n->z );
					result[coord + 3] = uint8_t( n->w * 255.0f );
				}
			}

			return result;
		}
	}

	bool convertToNormalMap( float strength
		, castor::Image & image )
	{
		try
		{
			castor::Size origDimensions{ image.getDimensions() };
			castor::Size dimensions{ origDimensions.getWidth() * 16u
				, origDimensions.getHeight() * 16u };
			image.resample( dimensions );
			auto buffer = castor::PxBufferBase::create( dimensions
				, castor::PixelFormat::eR8_UNORM
				, image.getPxBuffer().getConstPtr()
				, image.getPixelFormat()
				, image.getPxBuffer().getAlign() );
			auto normals = calculateNormals( strength
				, castor::makeArrayView( buffer->getConstPtr(), buffer->getSize() )
				, dimensions );
			auto nmlHeights = castor::PxBufferBase::create( buffer->getDimensions()
				, castor::PixelFormat::eR8G8B8A8_UNORM
				, normals.data()
				, castor::PixelFormat::eR8G8B8A8_UNORM );
			image = castor::Image{ image.getName(), image.getPath(), *nmlHeights };
			image.resample( origDimensions );
		}
		catch ( castor::Exception & exc )
		{
			std::cerr << cuT( "Error encountered while converting image [" ) << image.getName() << cuT( "] to a normal map: " ) << exc.what() << std::endl;
			return false;
		}
		catch ( std::exception & exc )
		{
			std::cerr << cuT( "Error encountered while converting image [" ) << image.getName() << cuT( "] to a normal map: " ) << exc.what() << std::endl;
			return false;
		}
		catch ( ... )
		{
			std::cerr << cuT( "Error encountered while converting image [" ) << image.getName() << cuT( "] to a normal map: Unknown error" ) << std::endl;
			return false;
		}

		return true;
	}
}
