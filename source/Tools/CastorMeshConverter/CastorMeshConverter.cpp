#include <Castor3D/Engine.hpp>

#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Mesh/Mesh.hpp>
#include <Castor3D/Mesh/Submesh.hpp>
#include <Castor3D/Mesh/Skeleton/Skeleton.hpp>
#include <Castor3D/Plugin/RendererPlugin.hpp>
#include <Castor3D/Scene/Scene.hpp>

using StringArray = std::vector< std::string >;

struct Options
{
	castor::Path input;
	castor::String output;
	bool split{ false };
	uint32_t submesh{ 0u };
	bool recenter{ false };
};

void printUsage()
{
	std::cout << "Castor Mesh Converter is a tool that allows you to convert any mesh file to the CMSH files." << std::endl;
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

	options.input = castor::Path{ castor::string::stringCast< xchar >( args[0] ) };
	it = std::find( args.begin(), args.end(), "-o" );

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
		options.output = *it;
	}

	it = std::find( args.begin(), args.end(), "-s" );
	options.split = it != args.end();
	it = std::find( args.begin(), args.end(), "-r" );
	options.recenter = it != args.end();

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

		if ( file.find( castor::String( cuT( "d." ) ) + CU_SharedLibExt ) == castor::String::npos )
#else

		if ( file.find( castor::String( cuT( "d." ) ) + CU_SharedLibExt ) != castor::String::npos )

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
			if ( file.getExtension() == CU_SharedLibExt )
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
			return pair.first.find( "Vk" ) != castor::String::npos;
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

template< bool Split, typename T >
struct ObjectWriter;

template< bool Split, typename T >
bool doWriteObject( castor::Path const & path
	, T const & object
	, Options options )
{
	bool result = false;
	auto meshFolder = path / cuT( "Meshes" );

	if ( !castor::File::directoryExists( meshFolder ) )
	{
		castor::File::directoryCreate( meshFolder );
	}

	try
	{
		ObjectWriter< Split, T >{}( meshFolder, object, options );
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

template< bool Split, typename T >
struct ObjectPostWriter;

template<>
struct ObjectPostWriter< false, castor3d::Mesh >
{
	bool operator()( castor::Path const & path
		, castor3d::Mesh const & object
		, Options const & options )
	{
		auto skeleton = object.getSkeleton();
		bool result = true;

		if ( skeleton )
		{
			result = doWriteObject< false >( path, *skeleton, options );
		}

		if ( result )
		{
			auto materialsFolder = path / cuT( "Materials" );

			if ( !castor::File::directoryExists( materialsFolder ) )
			{
				castor::File::directoryCreate( materialsFolder );
			}

			auto & scene = *object.getScene();
			auto newPath = materialsFolder / ( options.output + cuT( ".cscn" ) );
			castor::TextFile file{ newPath, castor::File::OpenMode::eWrite };
			result = writeView< castor3d::Material >( scene.getMaterialView()
				, file );
		}

		if ( result && !options.split )
		{
			auto newPath = path / ( options.output + cuT( "Integration.cscn" ) );
			castor::TextFile file{ newPath, castor::File::OpenMode::eWrite };
			result = file.writeText( cuT( "\tmesh \"" ) + options.output + cuT( "\"\n" ) ) > 0
				&& file.writeText( cuT( "\t{\n" ) ) > 0
				&& file.writeText( cuT( "\t\timport \"Meshes/" ) + options.output + cuT( ".cmsh\"\n" ) ) > 0
				&& file.writeText( cuT( "\t}\n" ) ) > 0
				&& file.writeText( cuT( "\tscene_node \"" ) + options.output + cuT( "\"\n" ) ) > 0
				&& file.writeText( cuT( "\t{\n" ) ) > 0
				&& file.writeText( cuT( "\t\tposition 0.0 0.0 0.0\n" ) ) > 0
				&& file.writeText( cuT( "\t}\n" ) ) > 0
				&& file.writeText( cuT( "\tobject \"" ) + options.output + cuT( "\"\n" ) ) > 0
				&& file.writeText( cuT( "\t{\n" ) ) > 0
				&& file.writeText( cuT( "\t\tparent \"" ) + options.output + cuT( "\"\n" ) ) > 0
				&& file.writeText( cuT( "\t\tmesh \"" ) + options.output + cuT( "\"\n" ) ) > 0
				&& file.writeText( cuT( "\t\tmaterials\n" ) ) > 0
				&& file.writeText( cuT( "\t\t{\n\n" ) ) > 0;
			uint32_t index = 0u;

			for ( auto & submesh : object )
			{
				result &= file.writeText( cuT( "\t\tmaterial " ) + castor::string::toString( index++, std::locale{ "C" } ) + cuT( " \"" ) + submesh->getDefaultMaterial()->getName() + cuT( "\"\n" ) ) > 0;
			}

			result &= file.writeText( cuT( "\t}\n" ) ) > 0
				&& file.writeText( cuT( "}\n" ) ) > 0;
		}

		return result;
	}
};

template<>
struct ObjectPostWriter< true, castor3d::Mesh >
{
	bool operator()( castor::Path const & path
		, castor3d::Mesh const & object
		, Options const & options )
	{
		auto name = options.output + cuT( "_" ) + castor::string::toString( options.submesh, std::locale{ "C" } );
		auto newPath = path / ( options.output + cuT( "Integration.cscn" ) );
		auto & submesh = *object.getSubmesh( 0u );
		castor::Point3f position;
			
		if ( options.recenter )
		{
			position = submesh.getBoundingBox().getCenter();
		}

		auto stream = castor::makeStringStream();
		stream << position[0] << cuT( " " ) << position[1] << cuT( " " ) << position[2];

		castor::TextFile file{ newPath, castor::File::OpenMode::eAppend };
		return file.writeText( cuT( "\tmesh \"" ) + name + cuT( "\"\n" ) )
			&& file.writeText( cuT( "\t{\n" ) )
			&& file.writeText( cuT( "\t\timport \"Meshes/" ) + name + cuT( ".cmsh\"\n" ) )
			&& file.writeText( cuT( "\t}\n" ) )
			&& file.writeText( cuT( "\tscene_node \"" ) + name + cuT( "\"\n" ) )
			&& file.writeText( cuT( "\t{\n" ) )
			&& file.writeText( cuT( "\t\tposition " ) + stream.str() + cuT( "\n" ) )
			&& file.writeText( cuT( "\t}\n" ) )
			&& file.writeText( cuT( "\tobject \"" ) + name + cuT( "\"\n" ) )
			&& file.writeText( cuT( "\t{\n" ) )
			&& file.writeText( cuT( "\t\tparent \"" ) + name + cuT( "\"\n" ) )
			&& file.writeText( cuT( "\t\tmesh \"" ) + name + cuT( "\"\n" ) )
			&& file.writeText( cuT( "\t\tmaterial \"" ) + object.getSubmesh( 0u )->getDefaultMaterial()->getName() + cuT( "\"\n" ) )
			&& file.writeText( cuT( "\t}\n\n" ) );
	}
};

template< bool Split >
struct ObjectPostWriter< Split, castor3d::Skeleton >
{
	bool operator()( castor::Path const & path
		, castor3d::Skeleton const & object
		, Options const & options )
	{
		return true;
	}
};

template< bool Split, typename T >
bool doPostWrite( castor::Path const & path
	, T const & object
	, Options const & options )
{
	return ObjectPostWriter< Split, T >{}( path, object, options );
}

template<>
struct ObjectWriter< false, castor3d::Mesh >
{
	bool operator()( castor::Path const & path
		, castor3d::Mesh const & object
		, Options options )
	{
		auto newPath = path / ( options.output + cuT( ".cmsh" ) );
		castor::BinaryFile file{ newPath, castor::File::OpenMode::eWrite };
		castor3d::BinaryWriter< castor3d::Mesh > writer;
		auto result = writer.write( object, file );

		if ( result )
		{
			result = doPostWrite< false >( path, object, options );
		}

		return result;
	}
};

template<>
struct ObjectWriter< true, castor3d::Mesh >
{
	bool operator()( castor::Path const & path
		, castor3d::Mesh const & object
		, Options options )
	{
		bool result = true;
		uint32_t index = 0u;
		auto dstFilePath = path / ( options.output + cuT( "Integration.cscn" ) );

		if ( castor::File::fileExists( dstFilePath ) )
		{
			castor::File::deleteFile( dstFilePath );
		}

		for ( auto & srcSubmesh : object )
		{
			if ( result )
			{
				castor::String name = options.output + cuT( "_" );

				if ( srcSubmesh->getDefaultMaterial() )
				{
					name += srcSubmesh->getDefaultMaterial()->getName();
				}

				name += castor::string::toString( index, std::locale{ "C" } );
				auto newPath = path / ( name + cuT( ".cmsh" ) );
				auto mesh = std::make_unique< castor3d::Mesh >( name, *object.getScene() );

				if ( auto skeleton = object.getSkeleton() )
				{
					mesh->setSkeleton( skeleton );
				}

				auto dstSubmesh = mesh->createSubmesh();
				dstSubmesh->setDefaultMaterial( srcSubmesh->getDefaultMaterial() );
				dstSubmesh->addPoints( srcSubmesh->getPoints() );
				auto & indexMapping = *srcSubmesh->getIndexMapping();
				castor3d::SubmeshComponentSPtr indexMappingComponent;

				for ( auto & component : srcSubmesh->getComponents() )
				{
					if ( component.second.get() != &indexMapping )
					{
						dstSubmesh->addComponent( component.first, component.second );
					}
					else
					{
						dstSubmesh->setIndexMapping( std::static_pointer_cast< castor3d::IndexMapping >( component.second ) );
					}
				}

				dstSubmesh->computeContainers();

				if ( options.recenter )
				{
					castor::Point3f position = dstSubmesh->getBoundingBox().getCenter();

					for ( auto & point : dstSubmesh->getPoints() )
					{
						point.pos -= position;
					}
				}

				castor::BinaryFile file{ newPath, castor::File::OpenMode::eWrite };
				castor3d::BinaryWriter< castor3d::Mesh > writer;
				result = writer.write( *mesh, file );

				if ( result )
				{
					options.submesh = index;
					result = doPostWrite< true >( path.getPath(), *mesh, options );
				}
			}

			++index;
		}

		if ( result )
		{
			result = doPostWrite< false >( path.getPath(), object, options );
		}

		return result;
	}
};

template< bool Split >
struct ObjectWriter< Split, castor3d::Skeleton >
{
	bool operator()( castor::Path const & path
		, castor3d::Skeleton const & object
		, Options options )
	{
		auto newPath = path / ( options.output + cuT( ".cskl" ) );
		castor::BinaryFile file{ newPath, castor::File::OpenMode::eWrite };
		castor3d::BinaryWriter< castor3d::Skeleton > writer;
		auto result = writer.write( object, file );

		if ( result )
		{
			result = doPostWrite< false >( path, object, options );
		}

		return result;
	}
};

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
		castor3d::Engine engine{ cuT( "MeshConverter" ), false };

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
				name = castor::Path{ options.output };
				auto rootFolder = path.getPath() / name;

				if ( !castor::File::directoryExists( rootFolder ) )
				{
					castor::File::directoryCreate( rootFolder );
				}

				if ( options.split )
				{
					doWriteObject< true >( rootFolder, *mesh, options );
				}
				else
				{
					doWriteObject< false >( rootFolder, *mesh, options );
				}
			}

			engine.cleanup();
		}

		castor::Logger::cleanup();
	}

	return EXIT_SUCCESS;
}

//******************************************************************************
