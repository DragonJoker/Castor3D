#include <Engine.hpp>
#include <Plugin/RendererPlugin.hpp>
#include <Scene/Scene.hpp>

#include <Material/Material.hpp>
#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Skeleton/Skeleton.hpp>

using StringArray = std::vector< std::string >;

struct Options
{
	castor::Path input;
	castor::Path output;
};

void printUsage()
{
	std::cout << "Castor Mesh Converter is a tool that allows you to convert any mesh file to the CMSH files." << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "CastorMeshConverter FILE [-o NAME]" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -o NAME     Allows you to specify the output file name." << std::endl;
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

bool doLoadRenderer( castor3d::Engine & engine )
{
	castor::PathArray arrayFiles;
	castor::File::listDirectoryFiles( castor3d::Engine::getPluginsDirectory(), arrayFiles );
	castor::PathArray arrayKept;

	// Exclude debug plug-in in release builds, and release plug-ins in debug builds
	for ( auto file : arrayFiles )
	{
#if defined( NDEBUG )

		if ( file.find( castor::String( cuT( "d." ) ) + CASTOR_DLL_EXT ) == castor::String::npos )
#else

		if ( file.find( castor::String( cuT( "d." ) ) + CASTOR_DLL_EXT ) != castor::String::npos )

#endif
		{
			arrayKept.push_back( file );
		}
	}

	castor::PathArray arrayFailed;

	if ( !arrayKept.empty() )
	{
		castor::PathArray otherPlugins;

		for ( auto file : arrayKept )
		{
			if ( file.getExtension() == CASTOR_DLL_EXT )
			{
				if ( !engine.getPluginCache().loadPlugin( file ) )
				{
					arrayFailed.push_back( file );
				}
			}
		}
	}

	return arrayFailed.empty();
}

bool doInitialiseEngine( castor3d::Engine & engine )
{
	if ( !castor::File::directoryExists( castor3d::Engine::getEngineDirectory() ) )
	{
		castor::File::directoryCreate( castor3d::Engine::getEngineDirectory() );
	}

	doLoadRenderer( engine );

	auto renderers = engine.getPluginCache().getPlugins( castor3d::PluginType::eRenderer );
	bool result = false;

	if ( renderers.empty() )
	{
		std::cerr << "No renderer plug-ins" << std::endl;
	}
	else
	{
		auto renderer = std::find_if( renderers.begin()
			, renderers.end()
			, []( std::pair< castor::String, castor3d::PluginSPtr > const & pair )
		{
			return pair.first.find( "Test" ) != castor::String::npos;
		} );

		if ( renderer != renderers.end() )
		{
			if ( engine.loadRenderer( std::static_pointer_cast< castor3d::RendererPlugin >( renderer->second )->getRendererType() ) )
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

template< typename ObjType, typename ViewType >
bool writeView( ViewType const & view
	, castor::TextFile & file )
{
	bool result = true;

	if ( !view.isEmpty() )
	{
		if ( result )
		{
			for ( auto const & name : view )
			{
				auto elem = view.find( name );
				result &= typename ObjType::TextWriter{ castor::cuEmptyString }( *elem, file );
			}
		}
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
	auto name = path.getFileName();
	auto folder = path.getPath();

	if ( skeleton )
	{
		auto newPath = folder / ( name + cuT( ".cskl" ) );
		result = doWriteObject( newPath, *skeleton );
	}

	if ( result )
	{
		auto & scene = *mesh.getScene();
		auto newPath = folder / ( name + cuT( "Materials.cscn" ) );
		castor::TextFile file{ newPath, castor::File::OpenMode::eWrite };
		result = writeView< castor3d::Material >( scene.getMaterialView()
			, file );
	}

	if ( result )
	{
		auto & scene = *mesh.getScene();
		auto newPath = folder / ( name + cuT( "Integration.cscn" ) );
		castor::TextFile file{ newPath, castor::File::OpenMode::eWrite };
		file.writeText( cuT( "object \"" ) + name + cuT( "\"\n" ) );
		file.writeText( cuT( "{\n" ) );
		file.writeText( cuT( "\tmesh \"" ) + name + cuT( "\"\n" ) );
		file.writeText( cuT( "\tmaterials\n" ) );
		file.writeText( cuT( "\t{\n" ) );
		uint32_t index = 0u;

		for ( auto & submesh : mesh )
		{
			file.writeText( cuT( "\t\tmaterial " ) + castor::string::toString( index++ ) + cuT( "\"" ) + submesh->getDefaultMaterial()->getName() + cuT( "\"\n" ) );
		}

		file.writeText( cuT( "\t}\n" ) );
		file.writeText( cuT( "}\n" ) );
		result = writeView< castor3d::Material >( scene.getMaterialView()
			, file );
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
		auto newPath = path.getPath() / ( path.getFileName() + cuT( ".cmsh" ) );
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

#if defined( NDEBUG )
		castor::Logger::initialise( castor::LogType::eInfo );
#else
		castor::Logger::initialise( castor::LogType::eDebug );
#endif

		castor::Logger::setFileName( castor::File::getExecutableDirectory() / cuT( "Tests.log" ) );
		castor3d::Engine engine;

		if ( doInitialiseEngine( engine ) )
		{
			castor3d::Scene scene{ cuT( "DummyScene" ), engine };
			auto name = path.getFileName();
			auto extension = castor::string::lowerCase( path.getExtension() );
			castor3d::MeshSPtr mesh;

			if ( !engine.getImporterFactory().isTypeRegistered( extension ) )
			{
				std::cerr << "Importer for [" << extension << "] files is not registered, make sure you've got the matching plug-in installed." << std::endl;
			}
			else
			{
				mesh = scene.getMeshCache().add( name );
				auto importer = engine.getImporterFactory().create( extension, engine );

				if ( !importer->importMesh( *mesh, path, castor3d::Parameters{}, true ) )
				{
					std::cerr << "Mesh Import failed" << std::endl;
					scene.getMeshCache().remove( name );
					mesh.reset();
				}
			}

			if ( mesh )
			{
				for ( auto & submesh : *mesh )
				{
					submesh->initialise();
				}

				doWriteObject( path, *mesh );
			}

			engine.cleanup();
		}

		castor::Logger::cleanup();
	}

	return EXIT_SUCCESS;
}

//******************************************************************************
