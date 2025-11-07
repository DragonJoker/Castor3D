#include <CastorUtils/Graphics/HeightMapToNormalMap.hpp>
#include <CastorUtils/Graphics/ExrImageLoader.hpp>
#include <CastorUtils/Graphics/FreeImageLoader.hpp>
#include <CastorUtils/Graphics/GliImageLoader.hpp>
#include <CastorUtils/Graphics/StbImageLoader.hpp>
#include <CastorUtils/Graphics/StbImageWriter.hpp>
#include <CastorUtils/Graphics/XpmImageLoader.hpp>
#include <CastorUtils/Miscellaneous/CpuInformations.hpp>
#include <CastorUtils/Multithreading/ThreadPool.hpp>

#include <vector>
#include <string>

namespace
{
	using StringArray = c3d::Vector< c3d::MbString >;

	void printUsage()
	{
		std::cout << "HeightMapToNormalMap is a tool used to convert a height map to a normal map." << std::endl;
		std::cout << "Usage:" << std::endl;
		std::cout << "HeightMapToNormalMap -s strength FILE" << std::endl;
		std::cout << "  FILE must be an image file." << std::endl;
		std::cout << "  strength is a floating point value defining the normals strength (default is 3.0)." << std::endl;
	}

	struct Options
	{
		float normalStrength{ 3.0f };
		c3d::PathArray paths;
	};

	bool parseArgs( int argc
		, char * argv[]
		, Options & options )
	{
		StringArray args{ argv + 1, argv + argc };

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
			&& std::distance( args.begin(), it ) == ptrdiff_t( args.size() - 1 ) )
		{
			std::cerr << "-s option is missing strength parameter" << std::endl << std::endl;
			printUsage();
			return false;
		}

		if ( it != args.end() )
		{
			it = args.erase( it );
			options.normalStrength = c3d::string::toFloat( c3d::makeString( *it ) );
			args.erase( it );
		}

		if ( args.empty() )
		{
			std::cerr << "Missing file parameter" << std::endl << std::endl;
			printUsage();
			return false;
		}

		it = std::next( args.begin(), ptrdiff_t( args.size() - 1u ) );

		if ( std::distance( args.begin(), it ) == 0
			&& args.size() > 1 )
		{
			std::cerr << "Missing file parameter" << std::endl << std::endl;
			printUsage();
			return false;
		}

		for ( auto & param : args )
		{
			options.paths.emplace_back( c3d::makeString( param ) );
		}

		return true;
	}

	void convertToNormalMap( float strength
		, c3d::Path path
		, c3d::ImageLoader const & loader
		, c3d::ImageWriter const & writer )
	{
		auto mbPath = c3d::toUtf8( path );
		try
		{
			std::cout << "Converting " << mbPath << std::endl;
			auto image = loader.load( path.getFileName(), path, {} );

			if ( c3d::convertToNormalMap( strength, image ) )
			{
				path = image.getPath();
				path = path.getPath() / ( cuT( "N_" ) + path.getFileName() + cuT( ".png" ) );
				writer.write( path, image.getPxBuffer() );
			}
		}
		catch ( c3d::Exception & exc )
		{
			std::cerr << "Error encountered while loading image file [" << mbPath << "]: " << exc.what() << std::endl;
		}
		catch ( std::exception & exc )
		{
			std::cerr << "Error encountered while loading image file [" << mbPath << "]: " << exc.what() << std::endl;
		}
		catch ( ... )
		{
			std::cerr << "Error encountered while loading image file [" << mbPath << "]: Unknown error" << std::endl;
		}
	}
}

int main( int argc, char * argv[] )
{
	Options options;

	if ( !parseArgs( argc, argv, options )
		|| options.paths.empty() )
	{
		return -1;
	}

	c3d::Logger::initialise( c3d::LogType::eInfo );
	c3d::Logger::setFileName( c3d::Path{ "HeightMapToNormalMap.log" } );

	c3d::ImageLoader loader;
	c3d::ExrImageLoader::registerLoader( loader );
	c3d::FreeImageLoader::registerLoader( loader );
	c3d::GliImageLoader::registerLoader( loader );
	c3d::StbImageLoader::registerLoader( loader );
	c3d::XpmImageLoader::registerLoader( loader );
	c3d::ImageWriter writer;
	c3d::StbImageWriter::registerWriter( writer );

	c3d::CpuInformations cpuInfos;
	c3d::ThreadPool pool{ cpuInfos.getCoreCount() };

	for ( auto & path : options.paths )
	{
		pool.pushJob( [&options, path, &loader, &writer]()
			{
				convertToNormalMap( options.normalStrength, path, loader, writer );
			} );
	}

	pool.waitAll( c3d::Milliseconds::max() );
	c3d::StbImageWriter::unregisterWriter( writer );
	c3d::ExrImageLoader::unregisterLoader( loader );
	c3d::FreeImageLoader::unregisterLoader( loader );
	c3d::GliImageLoader::unregisterLoader( loader );
	c3d::StbImageLoader::unregisterLoader( loader );
	c3d::XpmImageLoader::unregisterLoader( loader );
	c3d::Logger::cleanup();
	return EXIT_SUCCESS;
}
