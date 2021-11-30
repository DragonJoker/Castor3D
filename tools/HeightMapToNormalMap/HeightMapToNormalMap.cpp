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
		float normalStrength{ 3.0f };
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
			&& std::distance( args.begin(), it ) == ptrdiff_t( args.size() - 1 ) )
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
			options.paths.emplace_back( param );
		}

		return true;
	}

	void convertToNormalMap( float strength
		, castor::Path path
		, castor::ImageLoader const & loader
		, castor::ImageWriter const & writer )
	{
		try
		{
			std::cout << "Converting " << path << std::endl;
			auto image = loader.load( path.getFileName(), path, {} );

			if ( castor::convertToNormalMap( strength, image ) )
			{
				path = image.getPath();
				path = path.getPath() / ( "N_" + path.getFileName() + ".png" );
				writer.write( path, image.getPxBuffer() );
			}
		}
		catch ( castor::Exception & exc )
		{
			std::cerr << cuT( "Error encountered while loading image file [" ) << path << cuT( "]: " ) << exc.what() << std::endl;
		}
		catch ( std::exception & exc )
		{
			std::cerr << cuT( "Error encountered while loading image file [" ) << path << cuT( "]: " ) << exc.what() << std::endl;
		}
		catch ( ... )
		{
			std::cerr << cuT( "Error encountered while loading image file [" ) << path << cuT( "]: Unknown error" ) << std::endl;
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

	castor::Logger::initialise( castor::LogType::eInfo );
	castor::Logger::setFileName( castor::Path{ "HeightMapToNormalMap.log" } );

	castor::ImageLoader loader;
	castor::ExrImageLoader::registerLoader( loader );
	castor::FreeImageLoader::registerLoader( loader );
	castor::GliImageLoader::registerLoader( loader );
	castor::StbImageLoader::registerLoader( loader );
	castor::XpmImageLoader::registerLoader( loader );
	castor::ImageWriter writer;
	castor::StbImageWriter::registerWriter( writer );

	castor::CpuInformations cpuInfos;
	castor::ThreadPool pool{ cpuInfos.getCoreCount() };

	for ( auto & path : options.paths )
	{
		pool.pushJob( [&options, path, &loader, &writer]()
			{
				convertToNormalMap( options.normalStrength, path, loader, writer );
			} );
	}

	pool.waitAll( castor::Milliseconds::max() );
	castor::StbImageWriter::unregisterWriter( writer );
	castor::ExrImageLoader::unregisterLoader( loader );
	castor::FreeImageLoader::unregisterLoader( loader );
	castor::GliImageLoader::unregisterLoader( loader );
	castor::StbImageLoader::unregisterLoader( loader );
	castor::XpmImageLoader::unregisterLoader( loader );
	castor::Logger::cleanup();
	return EXIT_SUCCESS;
}
