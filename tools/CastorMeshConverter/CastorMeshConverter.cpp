#include <Castor3D/Engine.hpp>
#include <Castor3D/ImporterFile.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Material/Pass/PbrPass.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Mesh/MeshImporter.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Light/DirectionalLight.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneImporter.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Shader/LightingModelFactory.hpp>

#include <CastorUtils/Design/CacheView.hpp>
#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

#include <SceneExporter/CscnExporter.hpp>

namespace convert
{
	using StringArray = c3d::Vector< c3d::MbString >;

	struct Options
	{
		c3d::Path input;
		c3d::String output;
		c3d::String passType{ c3d::PbrPass::LightingModel };
		c3d::exporter::ExportOptions options;
		c3d::Parameters params;
	};

	static void printUsage()
	{
		std::cout << "Castor Mesh Converter is a tool that allows you to convert any mesh file to the CMSH file format." << std::endl;
		std::cout << "Usage:" << std::endl;
		std::cout << "CastorMeshConverter FILE [-o NAME] [-s] [-c] [-p DEGREES] [-y DEGREES] [-r DEGREES] [-a VALUE] [-m VALUE]" << std::endl;
		std::cout << "Options:" << std::endl;
		std::cout << "  -o NAME     Allows you to specify the output file name." << std::endl;
		std::cout << "              NAME can omit the .cscn extension." << std::endl << std::endl;
		std::cout << "  -f          Enables stop at first failure." << std::endl;
		std::cout << "  -s          Splits the mesh per material." << std::endl;
		std::cout << "  -c          Recenters the submesh in its bounding box." << std::endl;
		std::cout << "              Only useful when -s is specified" << std::endl;
		std::cout << "  -p DEGREES  Pitch model by given angle in floating point degrees" << std::endl;
		std::cout << "  -y DEGREES  Yaw model by given angle in floating point degrees" << std::endl;
		std::cout << "  -r DEGREES  Roll model by given angle in floating point degrees" << std::endl;
		std::cout << "  -a SCALE    Rescales model by given floating point value" << std::endl;
		std::cout << "  -m VALUE    Specify materials type" << std::endl;
		std::cout << "              VALUE can be one of:" << std::endl;
		std::cout << "              - phong : Phong" << std::endl;
		std::cout << "              - pbr : PBR (default value)" << std::endl;
		std::cout << "  -t          Force export as text cscn files." << std::endl;
	}

	static bool parseSwitchOption( c3d::MbString const & option
		, StringArray & args )
	{
		auto it = std::find( args.begin(), args.end(), "-" + option );
		auto result = it != args.end();

		if ( result )
		{
			args.erase( it );
		}

		return result;
	}

	static bool parseValueOption( c3d::MbString const & option
		, StringArray & args
		, c3d::String & value )
	{
		auto it = std::find( args.begin(), args.end(), "-" + option );
		auto result = it != args.end();

		if ( it != args.end() )
		{
			if ( std::next( it ) == args.end() )
			{
				std::cerr << "Missing value parameter for -" << option << " option." << std::endl << std::endl;
				printUsage();
				return false;
			}

			it = args.erase( it );
			value = c3d::makeString( *it );
			args.erase( it );
		}

		return result;
	}

	static bool parseArgs( int argc
		, char const * const argv[]
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

		c3d::String value;
		bool overridePassType{ false };

		if ( parseValueOption( "o", args, value ) )
		{
			options.output = c3d::Path{ value }.getFileName();
		}

		if ( parseValueOption( "m", args, value ) )
		{
			overridePassType = true;

			if ( value == cuT( "blinn_phong" ) || value == cuT( "phong" ) )
			{
				options.passType = c3d::PhongPass::LightingModel;
			}
			else if ( value == cuT( "pbr" ) )
			{
				options.passType = c3d::PbrPass::LightingModel;
			}
			else
			{
				std::cerr << "Wrong VALUE parameter for -m option." << std::endl << std::endl;
				printUsage();
				return false;
			}
		}

		if ( parseValueOption( "p", args, value ) )
		{
			options.params.add( cuT( "pitch" ),  c3d::string::toFloat( value ) );
		}

		if ( parseValueOption( "y", args, value ) )
		{
			options.params.add( cuT( "yaw" ), c3d::string::toFloat( value ) );
		}

		if ( parseValueOption( "r", args, value ) )
		{
			options.params.add( cuT( "roll" ), c3d::string::toFloat( value ) );
		}

		if ( parseValueOption( "a", args, value ) )
		{
			options.params.add( cuT( "rescale" ), c3d::string::toFloat( value ) );
		}

		options.options.splitPerMaterial = parseSwitchOption( "s", args );
		options.options.recenter = parseSwitchOption( "c", args );
		options.options.ignoreFailures = !parseSwitchOption( "f", args );
		options.options.forceText = !parseSwitchOption( "t", args );

		if ( args.empty() )
		{
			std::cerr << "Missing mesh file parameter." << std::endl << std::endl;
			printUsage();
			return false;
		}

		options.input = c3d::Path{ c3d::makeString( args.front() ) };

		if ( options.output.empty() )
		{
			options.output = options.input.getFileName();
		}

		if ( auto extension = c3d::string::lowerCase( options.input.getExtension() );
			!overridePassType && ( extension == cuT( "gltf" ) || extension == cuT( "glb" ) ) )
		{
			options.passType = c3d::PbrPass::LightingModel;
		}

		return true;
	}

	static c3d::PathArray listPluginsFiles( c3d::Path const & folder )
	{
		c3d::PathArray files;
		c3d::File::listDirectoryFiles( folder, files );
		c3d::PathArray result;

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		for ( auto const & file : files )
		{
			if ( file.find( CU_SharedLibExt ) != c3d::String::npos
				&& file.getFileName().find( cuT( "castor3d" ) ) == 0u )
			{
				result.emplace_back( file );
			}
		}

		return result;
	}

	static void loadPlugins( c3d::Engine & engine )
	{
		c3d::PathArray arrayKept = listPluginsFiles( c3d::Engine::getPluginsDirectory() );

#if !defined( NDEBUG )

		// When debug is installed, plugins are installed in lib/Debug/Castor3D
		if ( arrayKept.empty() )
		{
			c3d::Path pathBin = c3d::File::getExecutableDirectory();

			while ( pathBin.getFileName() != cuT( "bin" ) )
			{
				pathBin = pathBin.getPath();
			}

			c3d::Path pathUsr = pathBin.getPath();
			arrayKept = listPluginsFiles( pathUsr / cuT( "lib" ) / cuT( "Debug" ) / cuT( "Castor3D" ) );
		}

#endif

		if ( !arrayKept.empty() )
		{
			c3d::PathArray arrayFailed;
			c3d::PathArray otherPlugins;

			for ( auto const & file : arrayKept )
			{
				if ( file.getExtension() == CU_SharedLibExt
					// Only load importer and material plugins.
					&& ( file.find( cuT( "Importer" ) ) != c3d::String::npos || file.find( cuT( "Material" ) ) != c3d::String::npos )
					&& !engine.getPluginCache().loadPlugin( file ) )
				{
					arrayFailed.emplace_back( file );
				}
			}

			if ( !arrayFailed.empty() )
			{
				c3d::Logger::logWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

				for ( auto const & file : arrayFailed )
				{
					c3d::Logger::logWarning( file.getFileName() );
				}

				arrayFailed.clear();
			}
		}

		c3d::Logger::logInfo( cuT( "Plugins loaded" ) );
	}

	static bool initialiseEngine( c3d::Engine & engine )
	{
		if ( !c3d::File::directoryExists( c3d::Engine::getEngineDirectory() ) )
		{
			c3d::File::directoryCreate( c3d::Engine::getEngineDirectory() );
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

			if ( renderer == renderers.end() )
			{
				std::cout << "Couldn't find test renderer, using Vulkan." << std::endl;
				renderer = renderers.find( "vk" );
			}

			if ( renderer != renderers.end() )
			{
				if ( engine.loadRenderer( c3d::makeString( renderer->name ) ) )
				{
					engine.initialise( 100, false );
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
				std::cerr << "Couldn't find renderer." << std::endl;
			}
		}

		return result;
	}

	static c3d::Point3f getCameraPosition( c3d::BoundingBox const & aabb
		, float & farPlane )
	{
		auto height = aabb.getDimensions()->y;
		auto z = -( height * 1.5f );
		farPlane = std::abs( z ) + std::max( aabb.getMax()->z, std::max( aabb.getMax()->x, aabb.getMax()->y ) ) * 2.0f;
		return { aabb.getCenter()->x
			, aabb.getCenter()->y
			, z };
	}
}

int main( int argc, char * argv[] )
{
	convert::Options options;

	if ( convert::parseArgs( argc, argv, options ) )
	{
		auto path = options.input;

		if ( !c3d::File::fileExists( path ) )
		{
			path = c3d::File::getExecutableDirectory() / path;
		}

		if ( !c3d::File::fileExists( path ) )
		{
			std::cerr << "File [" << c3d::toUtf8( path ) << "] does not exist." << std::endl << std::endl;
			convert::printUsage();
			return EXIT_SUCCESS;
		}

#if defined( NDEBUG )
		c3d::Logger::initialise( c3d::LogType::eInfo );
#else
		c3d::Logger::initialise( c3d::LogType::eDebug );
#endif

		c3d::Logger::setFileName( c3d::File::getExecutableDirectory() / cuT( "CastorMeshConverter.log" ) );
		{
			c3d::EngineConfig config{ cuT( "CastorMeshConverter" )
				, c3d::Version{ CastorMeshConverter_VERSION_MAJOR, CastorMeshConverter_VERSION_MINOR, CastorMeshConverter_VERSION_BUILD }
				, false
				, false };
			c3d::Engine engine{ c3d::move( config ) };

			if ( convert::initialiseEngine( engine ) )
			{
				auto name = path.getFileName();
				auto extension = c3d::string::lowerCase( path.getExtension() );

				if ( extension == cuT( "cscn" ) )
				{
					try
					{
						c3d::SceneFileParser parser{ engine };
						auto preprocessed = parser.processFile( path );

						if ( preprocessed.parse() )
						{
							auto begin = parser.scenesBegin();

							if ( begin != parser.scenesEnd() )
							{
								auto scene = parser.scenesBegin()->second;
								auto rootFolder = path.getPath() / name;

								if ( !c3d::File::directoryExists( rootFolder ) )
								{
									c3d::File::directoryCreate( rootFolder );
								}

								c3d::exporter::CscnSceneExporter exporter{ options.options };
								exporter.exportScene( *scene, rootFolder / ( scene->getName() + cuT( ".cscn" ) ) );
								scene->cleanup();
							}
							else
							{
								c3d::Logger::logError( c3d::makeStringStream() << cuT( "No scene was imported" ) );
							}
						}
						else
						{
							c3d::Logger::logError( c3d::makeStringStream() << cuT( "Can't read scene file" ) );
						}
					}
					catch ( std::exception & exc )
					{
						c3d::Logger::logError( c3d::makeStringStream() << "Failed to parse the scene file, with following error:\n" << exc.what() );
					}
				}
				else
				{
					c3d::Scene scene{ name, engine };
					scene.setAmbientLight( c3d::RgbColour::fromComponents( 1.0f, 1.0f, 1.0f ) );
					scene.setBackgroundColour( c3d::RgbColour::fromComponents( 0.5f, 0.5f, 0.5f ) );
					scene.setDefaultLightingModel( scene.getEngine()->getLightingModelFactory().getLightingModelId( options.passType ) );
					c3d::SceneImporter importer{ *scene.getEngine() };

					if ( !importer.importData( scene
						, path
						, options.params
						, {} ) )
					{
						c3d::Logger::logError( c3d::makeStringStream() << "Import failed" );
					}
					else
					{
						scene.initialise();
						auto rootFolder = path.getPath() / name;

						if ( !c3d::File::directoryExists( rootFolder ) )
						{
							c3d::File::directoryCreate( rootFolder );
						}

						if ( scene.getCameraCache().isEmpty() )
						{
							float farPlane = 0.0f;
							auto cameraNode = scene.createSceneNode( cuT( "MainCameraNode" ), scene );
							cameraNode->setPosition( convert::getCameraPosition( scene.getBoundingBox(), farPlane ) );
							cameraNode->attachTo( *scene.getCameraRootNode() );

							if ( auto camNode = scene.addSceneNode( cuT( "MainCameraNode" ), cameraNode ) )
							{
								c3d::Viewport viewport{ *scene.getEngine() };
								viewport.setPerspective( 45.0_degrees
									, 1.7778f
									, std::max( 0.1f, farPlane / 1000.0f )
									, std::min( farPlane, 1000.0f ) );
								auto camera = scene.createCamera( cuT( "MainCamera" )
									, scene
									, *camNode
									, viewport );
								camera->attachTo( *camNode );
								scene.addCamera( cuT( "MainCamera" ), camera, false );
							}
						}

						if ( scene.getLightCache().isEmpty() )
						{
							auto lightNode = scene.createSceneNode( cuT( "LightNode" ), scene );
							lightNode->setOrientation( c3d::Quaternion::fromAxisAngle( c3d::Point3f{ 1.0, 0.0, 0.0 }, 90.0_degrees ) );
							lightNode->attachTo( *scene.getObjectRootNode() );

							if ( auto lgtNode = scene.addSceneNode( cuT( "LightNode" ), lightNode ) )
							{
								auto light = scene.createLight( cuT( "SunLight" )
									, scene
									, *lgtNode
									, scene.getLightsFactory()
									, c3d::LightType::eDirectional );
								light->setColour( c3d::RgbColour::fromComponents( 1.0f, 1.0f, 1.0f ) );
								light->getDirectionalLight()->setIllumination( c3d::Illumination{ 8.0f } );
								light->attachTo( *lgtNode );
								scene.addLight( cuT( "SunLight" ), light, false );
							}
						}

						c3d::exporter::CscnSceneExporter exporter{ options.options };
						engine.getRenderLoop().renderSyncFrame();
						exporter.exportScene( scene, rootFolder / options.output );
						scene.cleanup();
						engine.getRenderLoop().renderSyncFrame();
					}
				}

				engine.cleanup();
			}
		}

		c3d::Logger::cleanup();
	}

	return EXIT_SUCCESS;
}

//******************************************************************************
