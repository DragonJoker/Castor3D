#include <Castor3D/Engine.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/Phong/BlinnPhongPass.hpp>
#include <Castor3D/Material/Pass/Phong/PhongPass.hpp>
#include <Castor3D/Material/Pass/PBR/MetallicRoughnessPbrPass.hpp>
#include <Castor3D/Material/Pass/PBR/SpecularGlossinessPbrPass.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Mesh/Importer.hpp>
#include <Castor3D/Model/Mesh/ImporterFactory.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

#include <SceneExporter/CscnExporter.hpp>

namespace convert
{
	using StringArray = std::vector< std::string >;

	struct Options
	{
		castor::Path input;
		castor::String output;
		castor::String passType{ castor3d::SpecularGlossinessPbrPass::Type };
		castor3d::exporter::ExportOptions options;
		castor3d::Parameters params;
	};

	static void printUsage()
	{
		std::cout << "Castor Mesh Converter is a tool that allows you to convert any mesh file to the CMSH file format." << std::endl;
		std::cout << "Usage:" << std::endl;
		std::cout << "CastorMeshConverter FILE [-o NAME] [-s] [-c] [-p DEGREES] [-y DEGREES] [-r DEGREES] [-a VALUE] [-m VALUE]" << std::endl;
		std::cout << "Options:" << std::endl;
		std::cout << "  -o NAME     Allows you to specify the output file name." << std::endl;
		std::cout << "              NAME can omit the extension." << std::endl << std::endl;
		std::cout << "  -s          Splits the mesh per material." << std::endl;
		std::cout << "  -c          Recenters the submesh in its bounding box." << std::endl;
		std::cout << "              Only useful whe -s is specified" << std::endl;
		std::cout << "  -p DEGREES  Pitch model by given angle in floating point degrees" << std::endl;
		std::cout << "  -y DEGREES  Yaw model by given angle in floating point degrees" << std::endl;
		std::cout << "  -r DEGREES  Roll model by given angle in floating point degrees" << std::endl;
		std::cout << "  -a SCALE    Rescales model by given floating point value" << std::endl;
		std::cout << "  -m VALUE    Specify materials type" << std::endl;
		std::cout << "              VALUE can be one of:" << std::endl;
		std::cout << "              - phong : Phong" << std::endl;
		std::cout << "              - blinn_phong : Blinn-Phong" << std::endl;
		std::cout << "              - pbr_sg : PBR Specular Glossiness (default value)" << std::endl;
		std::cout << "              - pbr_mr : PBR Metallic Roughness (default value for glTF files)" << std::endl;
	}

	static bool parseSwitchOption( castor::String const & option
		, castor::StringArray & args )
	{
		auto it = std::find( args.begin(), args.end(), "-" + option );
		auto result = it != args.end();

		if ( result )
		{
			args.erase( it );
		}

		return result;
	}

	static bool parseValueOption( castor::String const & option
		, castor::StringArray & args
		, castor::String & value )
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
			value = *it;
			args.erase( it );
		}

		return result;
	}

	static bool parseArgs( int argc
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

		castor::String value;
		bool overridePassType{ false };

		if ( parseValueOption( "o", args, value ) )
		{
			options.output = castor::Path{ value }.getFileName();
		}

		if ( parseValueOption( "m", args, value ) )
		{
			overridePassType = true;

			if ( value == "phong" )
			{
				options.passType = castor3d::PhongPass::Type;
			}
			else if ( value == "blinn_phong" )
			{
				options.passType = castor3d::BlinnPhongPass::Type;
			}
			else if ( value == "pbr_sg" )
			{
				options.passType = castor3d::SpecularGlossinessPbrPass::Type;
			}
			else if ( value == "pbr_mr" )
			{
				options.passType = castor3d::MetallicRoughnessPbrPass::Type;
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
			options.params.add( cuT( "pitch" ),  castor::string::toFloat( value ) );
		}

		if ( parseValueOption( "y", args, value ) )
		{
			options.params.add( cuT( "yaw" ), castor::string::toFloat( value ) );
		}

		if ( parseValueOption( "r", args, value ) )
		{
			options.params.add( cuT( "roll" ), castor::string::toFloat( value ) );
		}

		if ( parseValueOption( "a", args, value ) )
		{
			options.params.add( cuT( "rescale" ), castor::string::toFloat( value ) );
		}

		options.options.splitPerMaterial = parseSwitchOption( "s", args );
		options.options.recenter = parseSwitchOption( "c", args );

		if ( args.empty() )
		{
			std::cerr << "Missing mesh file parameter." << std::endl << std::endl;
			printUsage();
			return false;
		}

		options.input = castor::Path{ args.front() };

		if ( options.output.empty() )
		{
			options.output = options.input.getFileName();
		}

		auto extension = castor::string::lowerCase( options.input.getExtension() );

		if ( !overridePassType
			&& ( extension == "gltf" || extension == "glb" ) )
		{
			options.passType = castor3d::MetallicRoughnessPbrPass::Type;
		}

		return true;
	}

	static castor::PathArray listPluginsFiles( castor::Path const & folder )
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

	static void loadPlugins( castor3d::Engine & engine )
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
					castor::Logger::logWarning( file.getFileName() );
				}

				arrayFailed.clear();
			}
		}

		castor::Logger::logInfo( cuT( "Plugins loaded" ) );
	}

	static bool initialiseEngine( castor3d::Engine & engine )
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
				std::cerr << "Couldn't load test renderer." << std::endl;
			}
		}

		return result;
	}

	static castor::Point3f getCameraPosition( castor::BoundingBox const & aabb
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

		if ( !castor::File::fileExists( path ) )
		{
			path = castor::File::getExecutableDirectory() / path;
		}

		if ( !castor::File::fileExists( path ) )
		{
			std::cerr << "File [" << path << "] does not exist." << std::endl << std::endl;
			convert::printUsage();
			return EXIT_SUCCESS;
		}

#if defined( NDEBUG )
		castor::Logger::initialise( castor::LogType::eInfo );
#else
		castor::Logger::initialise( castor::LogType::eDebug );
#endif

		castor::Logger::setFileName( castor::File::getExecutableDirectory() / cuT( "Tests.log" ) );
		{
			castor3d::Engine engine
			{
				cuT( "CastorMeshConverter" ),
				castor3d::Version{ CastorMeshConverter_VERSION_MAJOR, CastorMeshConverter_VERSION_MINOR, CastorMeshConverter_VERSION_BUILD },
				false
			};

			if ( convert::initialiseEngine( engine ) )
			{
				auto name = path.getFileName();
				auto extension = castor::string::lowerCase( path.getExtension() );

				if ( extension == "cscn" )
				{
					try
					{
						castor3d::SceneFileParser parser{ engine };
						auto preprocessed = parser.processFile( path );

						if ( preprocessed.parse() )
						{
							auto begin = parser.scenesBegin();

							if ( begin != parser.scenesEnd() )
							{
								auto scene = parser.scenesBegin()->second;
								auto rootFolder = path.getPath() / name;

								if ( !castor::File::directoryExists( rootFolder ) )
								{
									castor::File::directoryCreate( rootFolder );
								}

								castor3d::exporter::CscnSceneExporter exporter{ options.options };
								exporter.exportScene( *scene, rootFolder / ( scene->getName() + ".cscn" ) );
								scene->cleanup();
							}
							else
							{
								castor::Logger::logError( castor::makeStringStream() << cuT( "No scene was imported" ) );
							}
						}
						else
						{
							castor::Logger::logError( castor::makeStringStream() << cuT( "Can't read scene file" ) );
						}
					}
					catch ( std::exception & exc )
					{
						castor::Logger::logError( castor::makeStringStream() << "Failed to parse the scene file, with following error:\n" << exc.what() );
					}
				}
				else
				{
					castor3d::Scene scene{ cuT( "DummyScene" ), engine };
					scene.setAmbientLight( castor::RgbColour::fromComponents( 1.0f, 1.0f, 1.0f ) );
					scene.setBackgroundColour( castor::RgbColour::fromComponents( 0.5f, 0.5f, 0.5f ) );

					if ( !engine.getImporterFactory().isTypeRegistered( extension ) )
					{
						castor::Logger::logError( castor::makeStringStream() << "Importer for [" << extension << "] files is not registered, make sure you've got the matching plug-in installed." );
					}
					else
					{
						scene.setPassesType( scene.getEngine()->getPassFactory().getNameId( options.passType ) );
						auto importer = engine.getImporterFactory().create( extension, engine );

						if ( !importer->import( scene, path, options.params, {} ) )
						{
							castor::Logger::logError( castor::makeStringStream() << "Mesh Import failed" );
							scene.removeMesh( name );
						}
						else
						{
							scene.initialise();
							auto rootFolder = path.getPath() / name;

							if ( !castor::File::directoryExists( rootFolder ) )
							{
								castor::File::directoryCreate( rootFolder );
							}

							if ( scene.getCameraCache().isEmpty() )
							{
								float farPlane = 0.0f;
								auto cameraNode = scene.createSceneNode( "MainCameraNode", scene );
								cameraNode->setPosition( convert::getCameraPosition( scene.getBoundingBox(), farPlane ) );
								cameraNode->attachTo( *scene.getCameraRootNode() );
								cameraNode = scene.addSceneNode( "MainCameraNode", cameraNode ).lock();
								castor3d::Viewport viewport{ *scene.getEngine() };
								viewport.setPerspective( 45.0_degrees
									, 1.7778f
									, std::max( 0.1f, farPlane / 1000.0f )
									, std::min( farPlane, 1000.0f ) );
								auto camera = scene.createCamera( "MainCamera"
									, scene
									, *cameraNode
									, viewport );
								camera->attachTo( *cameraNode );
								scene.addCamera( "MainCamera", camera, false );
							}

							if ( scene.getLightCache().isEmpty() )
							{
								auto lightNode = scene.createSceneNode( "LightNode", scene );
								lightNode->setOrientation( castor::Quaternion::fromAxisAngle( castor::Point3f{ 1.0, 0.0, 0.0 }, 90.0_degrees ) );
								lightNode->attachTo( *scene.getObjectRootNode() );
								lightNode = scene.addSceneNode( "LightNode", lightNode ).lock();
								auto light = scene.createLight( "SunLight"
									, scene
									, *lightNode
									, scene.getLightsFactory()
									, castor3d::LightType::eDirectional );
								light->setColour( castor::RgbColour::fromComponents( 1.0f, 1.0f, 1.0f ) );
								light->setIntensity( { 8.0f, 10.0f } );
								light->attachTo( *lightNode );
								scene.addLight( "SunLight", light, false );
							}

							castor3d::exporter::CscnSceneExporter exporter{ options.options };
							engine.getRenderLoop().renderSyncFrame();
							exporter.exportScene( scene, rootFolder / options.output );
							scene.cleanup();
							engine.getRenderLoop().renderSyncFrame();
						}
					}
				}

				engine.cleanup();
			}
		}

		castor::Logger::cleanup();
	}

	return EXIT_SUCCESS;
}

//******************************************************************************
