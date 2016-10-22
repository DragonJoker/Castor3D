#include "GlRenderSystemTestPrerequisites.hpp"

#include <Log/Logger.hpp>
#include <Data/File.hpp>

#include <Engine.hpp>
#include <PluginCache.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Miscellaneous/PlatformWindowHandle.hpp>

#include <BenchManager.hpp>

#include <Render/GlRenderSystem.hpp>

#include "GlTransformFeedbackTest.hpp"

#undef CreateWindow

using namespace Castor;
using namespace Castor3D;

namespace
{
	void DoLoadPlugins( Engine & p_engine )
	{
		PathArray l_arrayFiles;
		File::ListDirectoryFiles( Engine::GetPluginsDirectory(), l_arrayFiles );
		PathArray l_arrayKept;

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		for ( auto l_file : l_arrayFiles )
		{
#if defined( NDEBUG )

			if ( l_file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) == String::npos )
#else

			if ( l_file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) != String::npos )

#endif
			{
				l_arrayKept.push_back( l_file );
			}
		}

		if ( !l_arrayKept.empty() )
		{
			PathArray l_arrayFailed;
			PathArray l_otherPlugins;

			for ( auto l_file : l_arrayKept )
			{
				if ( l_file.GetExtension() == CASTOR_DLL_EXT )
				{
					// Since techniques depend on renderers, we load these first
					if ( l_file.find( cuT( "RenderSystem" ) ) != String::npos )
					{
						if ( !p_engine.GetPluginCache().LoadPlugin( l_file ) )
						{
							l_arrayFailed.push_back( l_file );
						}
					}
					else
					{
						l_otherPlugins.push_back( l_file );
					}
				}
			}

			// Then we load other plug-ins
			for ( auto l_file : l_otherPlugins )
			{
				if ( !p_engine.GetPluginCache().LoadPlugin( l_file ) )
				{
					l_arrayFailed.push_back( l_file );
				}
			}

			if ( !l_arrayFailed.empty() )
			{
				Logger::LogWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

				for ( auto l_file : l_arrayFailed )
				{
					Logger::LogWarning( Path( l_file ).GetFileName() );
				}

				l_arrayFailed.clear();
			}
		}

		Logger::LogInfo( cuT( "Plugins loaded" ) );
	}

	std::unique_ptr< Engine > DoInitialiseCastor( IWindowHandleSPtr p_handle )
	{
		if ( !File::DirectoryExists( Engine::GetEngineDirectory() ) )
		{
			File::DirectoryCreate( Engine::GetEngineDirectory() );
		}

		std::unique_ptr< Engine > l_return = std::make_unique< Engine >();
		DoLoadPlugins( *l_return );

		auto l_renderers = l_return->GetPluginCache().GetPlugins( PluginType::Renderer );

		if ( l_renderers.empty() )
		{
			CASTOR_EXCEPTION( "No renderer plug-ins" );
		}

		if ( l_return->LoadRenderer( GlRender::GlRenderSystem::Name ) )
		{
			l_return->Initialise( 1, false );
			auto l_context = l_return->GetRenderSystem()->CreateContext();
			auto l_scene = l_return->GetSceneCache().Add( cuT( "Test" ) );
			auto l_window = l_scene->GetRenderWindowCache().Add( cuT( "Window" ) );
			auto l_target = l_return->GetRenderTargetCache().Add( TargetType::Window );
			l_target->SetPixelFormat( PixelFormat::A8R8G8B8 );
			l_target->SetSize( Size{ 1024, 1024 } );
			l_window->SetRenderTarget( l_target );
			l_window->Initialise( Size{ 1024, 1024 }, WindowHandle{ p_handle } );
		}
		else
		{
			CASTOR_EXCEPTION( "Couldn't load renderer." );
		}

		return l_return;
	}

#if defined( _WIN32 )

	LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		switch ( message )
		{
		case WM_CREATE:
			break;

		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
		}
		return 0;
	}

	IWindowHandleSPtr CreateWindowHandle()
	{
		IWindowHandleSPtr l_handle;
		MSG msg = { 0 };
		WNDCLASS wc = { 0 };
		wc.lpfnWndProc = WndProc;
		wc.hInstance = ::GetModuleHandle( NULL );
		wc.hbrBackground = ( HBRUSH )( COLOR_BACKGROUND );
		wc.lpszClassName = "GlRenderSystemTests";
		wc.style = CS_OWNDC;

		if ( RegisterClass( &wc ) )
		{
			auto l_hWnd = CreateWindowA( wc.lpszClassName, "GlRenderSystemTests", WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, 0, 0, wc.hInstance, 0 );
			l_handle = std::make_shared< IMswWindowHandle >( l_hWnd );
		}

		return l_handle;
	}

	void DestroyWindowHandle( IWindowHandleSPtr p_handle )
	{
		auto l_handle = std::static_pointer_cast< IMswWindowHandle >( p_handle );
		::DestroyWindow( l_handle->GetHwnd() );
	}

#elif ( __linux__ )
	
	IWindowHandleSPtr CreateWindowHandle()
	{
		Display *display = XOpenDisplay( NULL );

		if ( !display )
		{
			printf( "Failed to open X display\n" );
			exit( 1 );
		}

		// Get a matching FB config
		static int visual_attribs[] =
		{
			GLX_X_RENDERABLE    , True,
			GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
			GLX_RENDER_TYPE     , GLX_RGBA_BIT,
			GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
			GLX_RED_SIZE        , 8,
			GLX_GREEN_SIZE      , 8,
			GLX_BLUE_SIZE       , 8,
			GLX_ALPHA_SIZE      , 8,
			GLX_DEPTH_SIZE      , 24,
			GLX_STENCIL_SIZE    , 8,
			GLX_DOUBLEBUFFER    , True,
			//GLX_SAMPLE_BUFFERS  , 1,
			//GLX_SAMPLES         , 4,
			None
		};

		int glx_major, glx_minor;

		// FBConfigs were added in GLX version 1.3.
		if ( !glXQueryVersion( display, &glx_major, &glx_minor ) ||
			( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
		{
			printf( "Invalid GLX version" );
			exit( 1 );
		}

		printf( "Getting matching framebuffer configs\n" );
		int fbcount;
		GLXFBConfig* fbc = glXChooseFBConfig( display, DefaultScreen( display ), visual_attribs, &fbcount );
		if ( !fbc )
		{
			printf( "Failed to retrieve a framebuffer config\n" );
			exit( 1 );
		}
		printf( "Found %d matching FB configs.\n", fbcount );

		// Pick the FB config/visual with the most samples per pixel
		printf( "Getting XVisualInfos\n" );
		int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

		int i;
		for ( i = 0; i<fbcount; ++i )
		{
			XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
			if ( vi )
			{
				int samp_buf, samples;
				glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
				glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES, &samples );

				printf( "  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
						" SAMPLES = %d\n",
						i, vi->visualid, samp_buf, samples );

				if ( best_fbc < 0 || samp_buf && samples > best_num_samp )
					best_fbc = i, best_num_samp = samples;
				if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
					worst_fbc = i, worst_num_samp = samples;
			}
			XFree( vi );
		}

		GLXFBConfig bestFbc = fbc[best_fbc];

		// Be sure to free the FBConfig list allocated by glXChooseFBConfig()
		XFree( fbc );

		// Get a visual
		XVisualInfo *vi = glXGetVisualFromFBConfig( display, bestFbc );
		printf( "Chosen visual ID = 0x%x\n", vi->visualid );

		printf( "Creating colormap\n" );
		XSetWindowAttributes swa;
		Colormap cmap;
		swa.colormap = cmap = XCreateColormap( display,
											   RootWindow( display, vi->screen ),
											   vi->visual, AllocNone );
		swa.background_pixmap = None;
		swa.border_pixel = 0;
		swa.event_mask = StructureNotifyMask;

		printf( "Creating window\n" );
		Window win = XCreateWindow( display, RootWindow( display, vi->screen ),
									0, 0, 100, 100, 0, vi->depth, InputOutput,
									vi->visual,
									CWBorderPixel | CWColormap | CWEventMask, &swa );
		if ( !win )
		{
			printf( "Failed to create window.\n" );
			exit( 1 );
		}

		// Done with the visual info data
		XFree( vi );

		XStoreName( display, win, "GL 3.0 Window" );

		printf( "Mapping window\n" );
		XMapWindow( display, win );

		if ( RegisterClass( &wc ) )
		{
			auto l_hWnd = CreateWindowA( wc.lpszClassName, "GlRenderSystemTests", WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, 0, 0, wc.hInstance, 0 );
			l_handle = std::make_shared< IMswWindowHandle >( l_hWnd );
		}

		return l_handle;
	}

	void DestroyWindowHandle( IWindowHandleSPtr p_handle )
	{
		auto l_handle = std::static_pointer_cast< IMswWindowHandle >( p_handle );
		XDestroyWindow( display, win );
		XFreeColormap( display, cmap );
		XCloseDisplay( display );
	}

#endif

}

int main( int argc, char const * argv[] )
{
	int l_return = EXIT_SUCCESS;
	int l_count = 1;

	if ( argc == 2 )
	{
		l_count = std::max< int >( 1, atoi( argv[2] ) );
	}

#if defined( NDEBUG )
	Castor::Logger::Initialise( Castor::ELogType_INFO );
#else
	Logger::Initialise( Castor::ELogType_DEBUG );
#endif

	Logger::SetFileName( Castor::File::GetExecutableDirectory() / cuT( "GlRenderSystemTests.log" ) );
	{
		auto l_handle = CreateWindowHandle();
		std::unique_ptr< Engine > l_engine = DoInitialiseCastor( l_handle );

		// Test cases.
		Testing::Register( std::make_shared< Testing::GlTransformFeedbackTest >( *l_engine ) );

		// Tests loop.
		BENCHLOOP( l_count, l_return );

		l_engine->Cleanup();
		DestroyWindowHandle( l_handle );
	}
	Logger::Cleanup();
	return l_return;
}
