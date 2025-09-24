#include <Castor3D/Engine.hpp>

#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <ashespp/Core/PlatformWindowHandle.hpp>

namespace
{
#if defined( VK_USE_PLATFORM_ANDROID_KHR )
		c3d::MbString const KHR_PLATFORM_SURFACE_EXTENSION_NAME = "VK_KHR_android_surface";
#elif defined( VK_USE_PLATFORM_FUCHSIA )
		c3d::MbString const KHR_PLATFORM_SURFACE_EXTENSION_NAME = "VK_FUCHSIA_imagepipe_surface";
#elif defined( VK_USE_PLATFORM_IOS_MVK )
		c3d::MbString const KHR_PLATFORM_SURFACE_EXTENSION_NAME = "VK_MVK_ios_surface";
#elif defined( VK_USE_PLATFORM_MACOS_MVK )
		c3d::MbString const KHR_PLATFORM_SURFACE_EXTENSION_NAME = "VK_MVK_macos_surface";
#elif defined( VK_USE_PLATFORM_VI_NN )
		c3d::MbString const KHR_PLATFORM_SURFACE_EXTENSION_NAME = "VK_NN_vi_surface";
#elif defined( VK_USE_PLATFORM_XLIB_KHR )
		c3d::MbString const KHR_PLATFORM_SURFACE_EXTENSION_NAME = "VK_KHR_xlib_surface";
#elif defined( VK_USE_PLATFORM_XCB_KHR )
		c3d::MbString const KHR_PLATFORM_SURFACE_EXTENSION_NAME = "VK_KHR_xcb_surface";
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
		c3d::MbString const KHR_PLATFORM_SURFACE_EXTENSION_NAME = "VK_KHR_wayland_surface";
#elif defined( VK_USE_PLATFORM_WIN32_KHR )
		c3d::MbString const KHR_PLATFORM_SURFACE_EXTENSION_NAME = "VK_KHR_win32_surface";
#endif
	class DummyWindowHandle
		: public ashes::IWindowHandle
	{
	public:
		DummyWindowHandle()
			: ashes::IWindowHandle{ KHR_PLATFORM_SURFACE_EXTENSION_NAME }
		{
		}

		operator bool()noexcept override
		{
			return true;
		}
	};
	using StringArray = c3d::Vector< c3d::MbString >;

	struct Options
	{
		c3d::Path input;
		c3d::Path output;
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
		options.input = c3d::Path{ c3d::makeString( args[0] ) };

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
			options.output = c3d::Path{ c3d::makeString( *it ) };

			if ( options.output.getExtension().empty() )
			{
				options.output += cuT( "." ) + options.input.getExtension();
			}
		}

		return true;
	}

	bool doInitialiseEngine( c3d::Engine & engine )
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

			if ( renderer != renderers.end() )
			{
				if ( engine.loadRenderer( c3d::makeString( renderer->name ) ) )
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

	void doInitialise( c3d::RenderDevice const & device
		, c3d::Mesh const & mesh )
	{
		for ( auto & submesh : mesh )
		{
			submesh->initialise( device );
		}
	}

	void doInitialise( c3d::RenderDevice const & device
		, [[maybe_unused]] c3d::Skeleton const & skeleton )
	{
	}

	template< typename T >
	bool doParseObject( c3d::RenderDevice const & device
		, c3d::Path const & path
		, T & object )
	{
		bool result = false;

		try
		{
			c3d::BinaryFile file{ path, c3d::File::OpenMode::eRead };
			c3d::BinaryParser< T > parser;
			result = parser.parse( object, file );
		}
		catch ( c3d::Exception & exc )
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
			doInitialise( device, object );
		}

		return result;
	}

	template< typename T >
	bool doWriteObject( c3d::Path const & path
		, T & object );

	bool doPostWrite( c3d::Path const & path
		, c3d::Mesh & mesh )
	{
		auto skeleton = mesh.getSkeleton();
		bool result = true;

		if ( skeleton )
		{
			auto newPath = path.getPath() / ( path.getFileName() + cuT( ".cskl" ) );
			result = doWriteObject( newPath, *skeleton );
		}

		mesh.cleanup();
		return result;
	}

	bool doPostWrite( c3d::Path const & path
		, [[maybe_unused]] c3d::Skeleton const & skeleton )
	{
		return true;
	}

	template< typename T >
	bool doWriteObject( c3d::Path const & path
		, T & object )
	{
		bool result = false;

		try
		{
			auto newPath = path.getPath() / ( path.getFileName() + cuT( "Upgraded." ) + path.getExtension() );
			c3d::BinaryFile file{ newPath, c3d::File::OpenMode::eWrite };
			c3d::BinaryWriter< T > writer;
			result = writer.write( object, file );

			if ( result )
			{
				result = doPostWrite( path, object );
			}
		}
		catch ( c3d::Exception & exc )
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
}

int main( int argc, char * argv[] )
{
	Options options;

	if ( doParseArgs( argc, argv, options ) )
	{
		auto inputPath = options.input;

		if ( !c3d::File::fileExists( inputPath ) )
		{
			inputPath = c3d::File::getExecutableDirectory() / inputPath;
		}

		auto outputPath = options.output;

		if ( !c3d::File::fileExists( outputPath ) )
		{
			outputPath = c3d::File::getExecutableDirectory() / outputPath;
		}

		if ( !c3d::File::fileExists( inputPath ) )
		{
			std::cerr << "File [" << c3d::toUtf8( inputPath ) << "] does not exist." << std::endl << std::endl;
			printUsage();
			return EXIT_SUCCESS;
		}

		auto extension = c3d::string::lowerCase( inputPath.getExtension() );

		if ( extension != cuT( "cmsh" ) && extension != cuT( "cskl" ) )
		{
			std::cerr << "Wrong file type (expect .cmsh or .cskl extensions)." << std::endl << std::endl;
			printUsage();
			return EXIT_SUCCESS;
		}

#if defined( NDEBUG )
		c3d::Logger::initialise( c3d::LogType::eInfo );
#else
		c3d::Logger::initialise( c3d::LogType::eDebug );
#endif

		c3d::Logger::setFileName( c3d::File::getExecutableDirectory() / cuT( "CastorMeshUpgrader.log" ) );
		c3d::EngineConfig config{ cuT( "CastorMeshUpgrader" )
			, c3d::Version{ CastorMeshUpgrader_VERSION_MAJOR, CastorMeshUpgrader_VERSION_MINOR, CastorMeshUpgrader_VERSION_BUILD }
			, false
			, false };
		c3d::Engine engine{ c3d::move( config ) };

		if ( doInitialiseEngine( engine ) )
		{
			c3d::Scene scene{ cuT( "DummyScene" ), engine };
			auto name = c3d::String{ inputPath.getFileName() };
			auto & renderSystem = *engine.getRenderSystem();
			auto surface( renderSystem.getInstance().createSurface( renderSystem.getPhysicalDevice()
				, ashes::WindowHandle{ c3d::makeRawUnique< DummyWindowHandle >() } ) );
			auto & device = renderSystem.getRenderDevice();

			if ( extension == cuT( "cmsh" ) )
			{
				c3d::Mesh mesh{ name, scene };

				if ( doParseObject( device, inputPath, mesh ) )
				{
					doWriteObject( outputPath, mesh );
				}
			}
			else if ( extension == cuT( "cskl" ) )
			{
				c3d::Skeleton skeleton{ name, scene };

				if ( doParseObject( device, inputPath, skeleton ) )
				{
					doWriteObject( outputPath, skeleton );
				}
			}

			engine.cleanup();
		}

		c3d::Logger::cleanup();
	}

	return EXIT_SUCCESS;
}

//******************************************************************************
