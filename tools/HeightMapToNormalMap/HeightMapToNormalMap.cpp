#include <DiffImageLib/DiffImageLib.hpp>

#include <CastorUtils/Graphics/ExrImageLoader.hpp>
#include <CastorUtils/Graphics/FreeImageLoader.hpp>
#include <CastorUtils/Graphics/GliImageLoader.hpp>
#include <CastorUtils/Graphics/StbImageLoader.hpp>
#include <CastorUtils/Graphics/StbImageWriter.hpp>
#include <CastorUtils/Graphics/XpmImageLoader.hpp>

#include <vector>
#include <string>

void printUsage()
{
	std::cout << "HeightMapToNormalMap is a tool used to convert a height map to a normal map." << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "HeightMapToNormalMap -s strength FILE" << std::endl;
	std::cout << "  FILE must be an image file." << std::endl;
	std::cout << "  strength is a floating point value defining the normals strength (default is 8.0)." << std::endl;
}

struct Options
{
	float normalStrength{ 8.0f };
	castor::PathArray paths;
};

bool parseArgs( int argc
	, char * argv[]
	, Options & options )
{
	castor::StringArray args{ argv + 1, argv + argc };

	if ( args.empty() )
	{
		std::cerr << "Missing parameters." << std::endl << std::endl;
		printUsage();
		return false;
	}

	auto it = std::find( args.begin(), args.end(), "-h" );

	if ( it == args.end() )
	{
		it = std::find( args.begin(), args.end(), "--help" );
	}

	if ( it != args.end() )
	{
		args.erase( it );
		printUsage();
		return false;
	}

	it = std::find( args.begin(), args.end(), "-s" );

	if ( it == args.end() )
	{
		it = std::find( args.begin(), args.end(), "--strength" );
	}

	if ( it != args.end()
		&& std::distance( args.begin(), it ) == args.size() - 1 )
	{
		std::cerr << "-s option is missing strength parameter" << std::endl << std::endl;
		printUsage();
		return false;
	}

	if ( it != args.end() )
	{
		args.erase( it );
		++it;
		options.normalStrength = castor::string::toFloat( *it );
		args.erase( it );
	}

	if ( args.empty() )
	{
		std::cerr << "Missing file parameter" << std::endl << std::endl;
		printUsage();
		return false;
	}

	it = args.begin() + args.size() - 1u;

	if ( std::distance( args.begin(), it ) == 0
		&& args.size() > 1 )
	{
		std::cerr << "Missing file parameter" << std::endl << std::endl;
		printUsage();
		return false;
	}

	for ( auto & param : args )
	{
		options.paths.emplace_back( param );
	}

	return true;
}

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

size_t getIndex( int x, int y, int width, int height )
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
	return float( 0xFF - data[getIndex( x, y, width, height )] ) / 255.0f;
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
	const float dX = ( tr + 2.0 * r + br ) - ( tl + 2.0 * l + bl );
	const float dY = ( bl + 2.0 * b + br ) - ( tl + 2.0 * t + tr );
	const float dZ = 1.0 / strength;

	castor::Point3f n{ dX, dY, dZ };
	castor::point::normalise( n );
	return { n->x, n->y, n->z, 1.0f - c };
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

bool convertToNormalMap( float strength
	, castor::Path & path
	, castor::ImageLoader const & loader
	, castor::ImageWriter const & writer )
{
	try
	{
		auto image = loader.load( path.getFileName(), path, false, false );
		castor::Size origDimensions{ image.getDimensions() };
		castor::Size dimensions{ origDimensions.getWidth() * uint32_t( strength ) * 2u
			, origDimensions.getHeight() * uint32_t( strength ) * 2u };
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
		castor::Image imageNml{ cuT( "" ), *nmlHeights };
		imageNml.resample( origDimensions );
		path = image.getPath();
		path = path.getPath() / ( "N_" + path.getFileName() + ".png" );
		writer.write( path, imageNml.getPxBuffer() );
	}
	catch ( castor::Exception & exc )
	{
		std::cerr << cuT( "Error encountered while loading image file [" ) << path << cuT( "]: " ) << exc.what() << std::endl;
		return false;
	}
	catch ( std::exception & exc )
	{
		std::cerr << cuT( "Error encountered while loading image file [" ) << path << cuT( "]: " ) << exc.what() << std::endl;
		return false;
	}
	catch ( ... )
	{
		std::cerr << cuT( "Error encountered while loading image file [" ) << path << cuT( "]: Unknown error" ) << std::endl;
		return false;
	}

	return true;
}

int main( int argc, char * argv[] )
{
	Options options;

	if ( !parseArgs( argc, argv, options )
		|| options.paths.empty() )
	{
		return -1;
	}

	castor::ImageLoader loader;
	castor::ExrImageLoader::registerLoader( loader );
	castor::FreeImageLoader::registerLoader( loader );
	castor::GliImageLoader::registerLoader( loader );
	castor::StbImageLoader::registerLoader( loader );
	castor::XpmImageLoader::registerLoader( loader );
	castor::ImageWriter writer;
	castor::StbImageWriter::registerWriter( writer );

	for ( auto & path : options.paths )
	{
		convertToNormalMap( options.normalStrength, path, loader, writer );
	}

	return EXIT_SUCCESS;
}

//******************************************************************************
