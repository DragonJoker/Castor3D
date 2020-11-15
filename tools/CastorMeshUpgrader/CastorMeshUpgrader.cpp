#include <Castor3D/Engine.hpp>

#include <Castor3D/Binary/BinarySkeleton.hpp>
#include <Castor3D/Binary/BinaryMesh.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <ashespp/Core/PlatformWindowHandle.hpp>

class DummyWindowHandle
	: public ashes::IWindowHandle
{
public:
	DummyWindowHandle()
		: ashes::IWindowHandle{ ashes::KHR_PLATFORM_SURFACE_EXTENSION_NAME }
#if defined( VK_USE_PLATFORM_WIN32_KHR )
		, m_mswHandle{ nullptr, nullptr }
#elif defined( VK_USE_PLATFORM_ANDROID_KHR )
		, m_androidHandle{ nullptr }
#elif defined( VK_USE_PLATFORM_XCB_KHR )
		, m_xcbHandle{ nullptr, nullptr }
#elif defined( VK_USE_PLATFORM_MIR_KHR )
		, m_mirHandle{ nullptr, nullptr }
#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )
		, m_waylandHandle{ nullptr, nullptr }
#elif defined( VK_USE_PLATFORM_XLIB_KHR )
		, m_xlibHandle{ 0, nullptr }
#elif defined( VK_USE_PLATFORM_MACOS_MVK )
		, m_macosHandle{ nullptr }
#endif
	{
	}

	~DummyWindowHandle()
	{
	}

	operator bool()override
	{
		return true;
	}

#if defined( VK_USE_PLATFORM_WIN32_KHR )

	ashes::IMswWindowHandle m_mswHandle;

	operator ashes::IMswWindowHandle const & ( )const
	{
		return m_mswHandle;
	}

	operator ashes::IMswWindowHandle & ( )
	{
		return m_mswHandle;
	}

	ashes::IMswWindowHandle const * operator *()const
	{
		return &m_mswHandle;
	}

	ashes::IMswWindowHandle * operator *()
	{
		return &m_mswHandle;
	}

#elif defined( VK_USE_PLATFORM_ANDROID_KHR )

	ashes::IAndroidWindowHandle m_androidHandle;

	operator ashes::IAndroidWindowHandle const & ( )const
	{
		return m_androidHandle;
	}

	operator ashes::IAndroidWindowHandle & ( )
	{
		return m_androidHandle;
	}

	ashes::IAndroidWindowHandle const * operator *()const
	{
		return &m_androidHandle;
	}

	ashes::IAndroidWindowHandle * operator *()
	{
		return &m_androidHandle;
	}

#elif defined( VK_USE_PLATFORM_XCB_KHR )

	ashes::IXcbWindowHandle m_xcbHandle;

	operator ashes::IXcbWindowHandle const & ( )const
	{
		return m_xcbHandle;
	}

	operator ashes::IXcbWindowHandle & ( )
	{
		return m_xcbHandle;
	}

	ashes::IXcbWindowHandle const * operator *()const
	{
		return &m_xcbHandle;
	}

	ashes::IXcbWindowHandle * operator *()
	{
		return &m_xcbHandle;
	}

#elif defined( VK_USE_PLATFORM_MIR_KHR )

	ashes::IMirWindowHandle m_mirHandle;

	operator ashes::IMirWindowHandle const & ( )const
	{
		return m_mirHandle;
	}

	operator ashes::IMirWindowHandle & ( )
	{
		return m_mirHandle;
	}

	ashes::IMirWindowHandle const * operator *()const
	{
		return &m_mirHandle;
	}

	ashes::IMirWindowHandle * operator *()
	{
		return &m_mirHandle;
	}

#elif defined( VK_USE_PLATFORM_WAYLAND_KHR )

	ashes::IWaylandWindowHandle m_waylandHandle;

	operator ashes::IWaylandWindowHandle const & ( )const
	{
		return m_waylandHandle;
	}

	operator ashes::IWaylandWindowHandle & ( )
	{
		return m_waylandHandle;
	}

	ashes::IWaylandWindowHandle const * operator *()const
	{
		return &m_waylandHandle;
	}

	ashes::IWaylandWindowHandle * operator *()
	{
		return &m_waylandHandle;
	}

#elif defined( VK_USE_PLATFORM_XLIB_KHR )

	ashes::IXWindowHandle m_xlibHandle;

	operator ashes::IXWindowHandle const & ( )const
	{
		return m_xlibHandle;
	}

	operator ashes::IXWindowHandle & ( )
	{
		return m_xlibHandle;
	}

	ashes::IXWindowHandle const * operator *()const
	{
		return &m_xlibHandle;
	}

	ashes::IXWindowHandle * operator *()
	{
		return &m_xlibHandle;
	}

#elif defined( VK_USE_PLATFORM_MACOS_MVK )

	ashes::IMacOsWindowHandle m_macosHandle;

	operator ashes::IMacOsWindowHandle const & ( )const
	{
		return m_macosHandle;
	}

	operator ashes::IMacOsWindowHandle & ( )
	{
		return m_macosHandle;
	}

	ashes::IMacOsWindowHandle const * operator *()const
	{
		return &m_macosHandle;
	}

	ashes::IMacOsWindowHandle * operator *()
	{
		return &m_macosHandle;
	}

#else

#	error "Unsupported window system."

#endif
};
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
	options.input = castor::Path{ castor::string::stringCast< castor::xchar >( args[0] ) };

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

void doInitialise( castor3d::RenderDevice const & device
	, castor3d::Mesh & mesh )
{
	for ( auto & submesh : mesh )
	{
		submesh->initialise( device );
	}
}

void doInitialise( castor3d::RenderDevice const & device
	, castor3d::Skeleton & skeleton )
{
}

template< typename T >
bool doParseObject( castor3d::RenderDevice const & device
	, castor::Path const & path
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
		doInitialise( device, object );
	}

	return result;
}

template< typename T >
bool doWriteObject( castor::Path const & path
	, T & object );

bool doPostWrite( castor::Path const & path
	, castor3d::Mesh & mesh )
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

bool doPostWrite( castor::Path const & path
	, castor3d::Skeleton & skeleton )
{
	return true;
}

template< typename T >
bool doWriteObject( castor::Path const & path
	, T & object )
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
		auto inputPath = options.input;

		if ( !castor::File::fileExists( inputPath ) )
		{
			inputPath = castor::File::getExecutableDirectory() / inputPath;
		}

		auto outputPath = options.output;

		if ( !castor::File::fileExists( outputPath ) )
		{
			outputPath = castor::File::getExecutableDirectory() / outputPath;
		}

		if ( !castor::File::fileExists( inputPath ) )
		{
			std::cerr << "File [" << inputPath << "] does not exist." << std::endl << std::endl;
			printUsage();
			return EXIT_SUCCESS;
		}

		auto extension = castor::string::lowerCase( inputPath.getExtension() );

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
			auto name = inputPath.getFileName();
			auto device = engine.getRenderSystem()->createDevice( ashes::WindowHandle{ std::make_unique< DummyWindowHandle >() }, 0u );

			if ( extension == cuT( "cmsh" ) )
			{
				castor3d::Mesh mesh{ name, scene };

				if ( doParseObject( *device, inputPath, mesh ) )
				{
					doWriteObject( outputPath, mesh );
				}
			}
			else if ( extension == cuT( "cskl" ) )
			{
				castor3d::Skeleton skeleton{ scene };

				if ( doParseObject( *device, inputPath, skeleton ) )
				{
					doWriteObject( outputPath, skeleton );
				}
			}

			engine.cleanup();
		}

		castor::Logger::cleanup();
	}

	return EXIT_SUCCESS;
}

//******************************************************************************
