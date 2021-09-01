#include <Castor3D/Engine.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PBR/SpecularGlossinessPbrPass.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Mesh/Importer.hpp>
#include <Castor3D/Model/Mesh/ImporterFactory.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Text/TextMaterial.hpp>
#include <Castor3D/Text/TextScene.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

#include <SceneExporter/CscnExporter.hpp>

using StringArray = std::vector< std::string >;

struct Options
{
	castor::Path input;
	castor::String output;
	castor::String passType{ castor3d::SpecularGlossinessPbrPass::Type };
	castor3d::exporter::ExportOptions options;
};

void printUsage()
{
	std::cout << "Castor Mesh Converter is a tool that allows you to convert any mesh file to the CMSH file format." << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "CastorMeshConverter FILE [-o NAME] [-s] [-r]" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -o NAME     Allows you to specify the output file name." << std::endl;
	std::cout << "              NAME can omit the extension." << std::endl << std::endl;
	std::cout << "  -s          Splits the mesh per material." << std::endl;
	std::cout << "  -r          Recenters the submesh in its bounding box." << std::endl;
	std::cout << "              Only useful whe -s is specified" << std::endl;
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

	options.input = castor::Path{ castor::string::stringCast< castor::xchar >( args[0] ) };
	it = std::find( args.begin(), args.end(), "-o" );

	if ( it == args.end() )
	{
		options.output = options.input.getFileName();
	}
	else if ( ++it == args.end() )
	{
		std::cerr << "Missing NAME parameter for -o option." << std::endl << std::endl;
		printUsage();
		return false;
	}
	else
	{
		options.output = castor::Path{ *it }.getFileName();
	}

	it = std::find( args.begin(), args.end(), "-s" );
	options.options.splitPerMaterial = it != args.end();
	it = std::find( args.begin(), args.end(), "-r" );
	options.options.recenter = it != args.end();

	return true;
}

castor::PathArray listPluginsFiles( castor::Path const & folder )
{
	castor::PathArray files;
	castor::File::listDirectoryFiles( folder, files );
	castor::PathArray result;

	// Exclude debug plug-in in release builds, and release plug-ins in debug builds
	for ( auto file : files )
	{
		if ( file.find( CU_SharedLibExt ) != castor::String::npos
			&& file.getFileName().find( cuT( "castor3d" ) ) == 0u )
		{
			result.push_back( file );
		}
	}

	return result;
}

void loadPlugins( castor3d::Engine & engine )
{
	castor::PathArray arrayKept = listPluginsFiles( castor3d::Engine::getPluginsDirectory() );

#if !defined( NDEBUG )

		// When debug is installed, plugins are installed in lib/Debug/Castor3D
	if ( arrayKept.empty() )
	{
		castor::Path pathBin = castor::File::getExecutableDirectory();

		while ( pathBin.getFileName() != cuT( "bin" ) )
		{
			pathBin = pathBin.getPath();
		}

		castor::Path pathUsr = pathBin.getPath();
		arrayKept = listPluginsFiles( pathUsr / cuT( "lib" ) / cuT( "Debug" ) / cuT( "Castor3D" ) );
	}

#endif

	if ( !arrayKept.empty() )
	{
		castor::PathArray arrayFailed;
		castor::PathArray otherPlugins;

		for ( auto file : arrayKept )
		{
			if ( file.getExtension() == CU_SharedLibExt )
			{
				// Only load importer plugins.
				if ( file.find( cuT( "Importer" ) ) != castor::String::npos )
				{
					if ( !engine.getPluginCache().loadPlugin( file ) )
					{
						arrayFailed.push_back( file );
					}
				}
			}
		}

		if ( !arrayFailed.empty() )
		{
			castor::Logger::logWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

			for ( auto file : arrayFailed )
			{
				castor::Logger::logWarning( castor::Path( file ).getFileName() );
			}

			arrayFailed.clear();
		}
	}

	castor::Logger::logInfo( cuT( "Plugins loaded" ) );
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
		auto renderer = renderers.find( "vk" );

		if ( renderer != renderers.end() )
		{
			if ( engine.loadRenderer( renderer->name ) )
			{
				engine.initialise( 1, false );
				loadPlugins( engine );
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

#if defined( NDEBUG )
		castor::Logger::initialise( castor::LogType::eInfo );
#else
		castor::Logger::initialise( castor::LogType::eDebug );
#endif

		castor::Logger::setFileName( castor::File::getExecutableDirectory() / cuT( "Tests.log" ) );
		castor3d::Engine engine
		{
			cuT( "CastorMeshConverter" ),
			castor3d::Version{ CastorMeshConverter_VERSION_MAJOR, CastorMeshConverter_VERSION_MINOR, CastorMeshConverter_VERSION_BUILD },
			false
		};

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
				scene.setPassesType( scene.getEngine()->getPassFactory().getNameId( options.passType ) );
				mesh = scene.getMeshCache().add( name );
				auto importer = engine.getImporterFactory().create( extension, engine );

				if ( !importer->import( *mesh, path, castor3d::Parameters{}, true ) )
				{
					std::cerr << "Mesh Import failed" << std::endl;
					scene.getMeshCache().remove( name );
					mesh.reset();
				}
			}

			if ( mesh )
			{
				auto rootFolder = path.getPath() / name;

				if ( !castor::File::directoryExists( rootFolder ) )
				{
					castor::File::directoryCreate( rootFolder );
				}

				castor3d::exporter::CscnSceneExporter exporter{ options.options };
				exporter.exportMesh( scene, *mesh, rootFolder, options.output );
				mesh->cleanup();
			}

			engine.cleanup();
		}

		castor::Logger::cleanup();
	}

	return EXIT_SUCCESS;
}

//******************************************************************************
