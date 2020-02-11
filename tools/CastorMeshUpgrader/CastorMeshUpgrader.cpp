#include <Castor3D/Engine.hpp>

#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>

using StringArray = std::vector< std::string >;

struct Options
{
	castor::Path input;
	castor::Path output;
};

void printUsage()
{
	std::cout << "Castor Mesh Upgrader is a tool that allows you to upgrade your CMSH files to the latest CMSH version (works for CMSH and CSKL files)." << std::endl;
	std::cout << "Note that if the .cmsh file contains a skeleton, it will be written in its own .cskl file." << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "CastorMeshUpgrader FILE [-o NAME]" << std::endl;
	std::cout << "  FILE must be a .cmsh or .cskl file." << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -o NAME     Allows you to specify the output file name." << std::endl;
	std::cout << "              If you don't use this option, the original file will be overwritten." << std::endl;
	std::cout << "              NAME can omit the extension." << std::endl << std::endl;
}

bool doParseArgs( int argc
	, char * argv[]
	, Options & options )
{
	StringArray args{ argv + 1, argv + argc };

	if ( args.empty() )
	{
		std::cerr << "Missing mesh file parameter." << std::endl << std::endl;
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

	it = std::find( args.begin(), args.end(), "-o" );
	options.input = castor::Path{ castor::string::stringCast< xchar >( args[0] ) };

	if ( it == args.end() )
	{
		options.output = options.input;
	}
	else if ( ++it == args.end() )
	{
		std::cerr << "Missing NAME parameter for -o option." << std::endl << std::endl;
		printUsage();
		return false;
	}
	else
	{
		options.output = castor::Path{ *it };

		if ( options.output.getExtension().empty() )
		{
			options.output += cuT( "." ) + options.input.getExtension();
		}
	}

	return true;
}

bool doInitialiseEngine( castor3d::Engine & engine )
{
	if ( !castor::File::directoryExists( castor3d::Engine::getEngineDirectory() ) )
	{
		castor::File::directoryCreate( castor3d::Engine::getEngineDirectory() );
	}

	auto & renderers = engine.getRenderersList();
	bool result = false;

	if ( renderers.empty() )
	{
		std::cerr << "No renderer plug-ins" << std::endl;
	}
	else
	{
		auto renderer = renderers.find( "test" );

		if ( renderer != renderers.end() )
		{
			if ( engine.loadRenderer( renderer->name ) )
			{
				engine.initialise( 1, false );
				result = true;
			}
			else
			{
				std::cerr << "Couldn't load renderer." << std::endl;
			}
		}
		else
		{
			std::cerr << "Couldn't load test renderer." << std::endl;
		}
	}

	return result;
}

void doInitialise( castor3d::Mesh & mesh )
{
	for ( auto & submesh : mesh )
	{
		submesh->initialise();
	}
}

void doInitialise( castor3d::Skeleton & skeleton )
{
}

template< typename T >
bool doParseObject( castor::Path const & path
	, T & object )
{
	bool result = false;

	try
	{
		castor::BinaryFile file{ path, castor::File::OpenMode::eRead };
		castor3d::BinaryParser< T > parser;
		result = parser.parse( object, file );
	}
	catch ( castor::Exception & exc )
	{
		std::cerr << "Error encountered while parsing file : " << exc.what() << std::endl;
	}
	catch ( std::exception & exc )
	{
		std::cerr << "Error encountered while parsing file : " << exc.what() << std::endl;
	}
	catch ( ... )
	{
		std::cerr << "Error encountered while parsing file : Unknown exception" << std::endl;
	}

	if ( result )
	{
		doInitialise( object );
	}

	return result;
}

template< typename T >
bool doWriteObject( castor::Path const & path
	, T const & object );

bool doPostWrite( castor::Path const & path
	, castor3d::Mesh const & mesh )
{
	auto skeleton = mesh.getSkeleton();
	bool result = true;

	if ( skeleton )
	{
		auto newPath = path.getPath() / ( path.getFileName() + cuT( ".cskl" ) );
		result = doWriteObject( newPath, *skeleton );
	}

	return result;
}

bool doPostWrite( castor::Path const & path
	, castor3d::Skeleton const & skeleton )
{
	return true;
}

template< typename T >
bool doWriteObject( castor::Path const & path
	, T const & object )
{
	bool result = false;

	try
	{
		auto newPath = path.getPath() / ( path.getFileName() + cuT( "Upgraded." ) + path.getExtension() );
		castor::BinaryFile file{ newPath, castor::File::OpenMode::eWrite };
		castor3d::BinaryWriter< T > writer;
		result = writer.write( object, file );

		if ( result )
		{
			result = doPostWrite( path, object );
		}
	}
	catch ( castor::Exception & exc )
	{
		std::cerr << "Error encountered while writing file : " << exc.what() << std::endl;
	}
	catch ( std::exception & exc )
	{
		std::cerr << "Error encountered while writing file : " << exc.what() << std::endl;
	}
	catch ( ... )
	{
		std::cerr << "Error encountered while writing file : Unknown exception" << std::endl;
	}

	return result;
}

int main( int argc, char * argv[] )
{
	Options options;

	if ( doParseArgs( argc, argv, options ) )
	{
		auto path = options.input;

		if ( !castor::File::fileExists( path ) )
		{
			path = castor::File::getExecutableDirectory() / path;
		}

		if ( !castor::File::fileExists( path ) )
		{
			std::cerr << "File [" << path << "] does not exist." << std::endl << std::endl;
			printUsage();
			return EXIT_SUCCESS;
		}

		auto extension = castor::string::lowerCase( path.getExtension() );

		if ( extension != cuT( "cmsh" ) && extension != cuT( "cskl" ) )
		{
			std::cerr << "Wrong file type (expect .cmsh or .cskl extensions)." << std::endl << std::endl;
			printUsage();
			return EXIT_SUCCESS;
		}

#if defined( NDEBUG )
		castor::Logger::initialise( castor::LogType::eInfo );
#else
		castor::Logger::initialise( castor::LogType::eDebug );
#endif

		castor::Logger::setFileName( castor::File::getExecutableDirectory() / cuT( "CastorMeshUpgrader.log" ) );
		castor3d::Engine engine
		{
			cuT( "CastorMeshUpgrader" ),
			castor3d::Version{ CastorMeshUpgrader_VERSION_MAJOR, CastorMeshUpgrader_VERSION_MINOR, CastorMeshUpgrader_VERSION_BUILD },
			false
		};

		if ( doInitialiseEngine( engine ) )
		{
			castor3d::Scene scene{ cuT( "DummyScene" ), engine };
			auto name = path.getFileName();

			if ( extension == cuT( "cmsh" ) )
			{
				castor3d::Mesh mesh{ name, scene };

				if ( doParseObject( path, mesh ) )
				{
					doWriteObject( path, mesh );
				}
			}
			else if ( extension == cuT( "cskl" ) )
			{
				castor3d::Skeleton skeleton{ scene };

				if ( doParseObject( path, skeleton ) )
				{
					doWriteObject( path, skeleton );
				}
			}

			engine.cleanup();
		}

		castor::Logger::cleanup();
	}

	return EXIT_SUCCESS;
}

//******************************************************************************
