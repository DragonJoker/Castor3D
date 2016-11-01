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

#if defined( __linux__ )
#	include <X11/X.h>
#	include <X11/Xlib.h>
#	include <GL/glx.h>
#endif

#undef CreateWindow

using namespace Castor;

namespace
{
	void DoLoadPlugins( Castor3D::Engine & p_engine )
	{
		PathArray l_arrayFiles;
		File::ListDirectoryFiles( Castor3D::Engine::GetPluginsDirectory(), l_arrayFiles );
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

	std::unique_ptr< Castor3D::Engine > DoInitialiseCastor( Castor3D::IWindowHandleSPtr p_handle )
	{
		if ( !File::DirectoryExists( Castor3D::Engine::GetEngineDirectory() ) )
		{
			File::DirectoryCreate( Castor3D::Engine::GetEngineDirectory() );
		}

		auto l_return = std::make_unique< Castor3D::Engine >();
		DoLoadPlugins( *l_return );

		auto l_renderers = l_return->GetPluginCache().GetPlugins( Castor3D::PluginType::Renderer );

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
			auto l_target = l_return->GetRenderTargetCache().Add( Castor3D::TargetType::Window );
			l_target->SetPixelFormat( PixelFormat::A8R8G8B8 );
			l_target->SetSize( Size{ 1024, 1024 } );
			l_target->SetScene( l_scene );
			l_window->SetRenderTarget( l_target );
			l_window->Initialise( Size{ 1024, 1024 }, Castor3D::WindowHandle{ p_handle } );
		}
		else
		{
			CASTOR_EXCEPTION( "Couldn't load renderer." );
		}

		return l_return;
	}

#if defined( _WIN32 )

	class RenderWindow
	{
	public:
		RenderWindow()
		{
			MSG msg = { 0 };
			WNDCLASS wc = { 0 };
			wc.lpfnWndProc = RenderWindow::WndProc;
			wc.hInstance = ::GetModuleHandle( NULL );
			wc.hbrBackground = ( HBRUSH )( COLOR_BACKGROUND );
			wc.lpszClassName = "GlRenderSystemTests";
			wc.style = CS_OWNDC;

			if ( !RegisterClass( &wc ) )
			{
				CASTOR_EXCEPTION( "Couldn't register window class" );
			}

			m_hWnd = CreateWindowA( wc.lpszClassName, "GlRenderSystemTests", WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, 0, 0, wc.hInstance, 0 );

			if ( !m_hWnd )
			{
				CASTOR_EXCEPTION( "Couldn't create window" );
			}
		}

		~RenderWindow()
		{
			::DestroyWindow( m_hWnd );
		}

		Castor3D::IWindowHandleSPtr CreateWindowHandle()
		{
			Castor3D::IWindowHandleSPtr l_handle;

			if ( m_hWnd )
			{
				l_handle = std::make_shared< Castor3D::IMswWindowHandle >( m_hWnd );
			}

			return l_handle;
		}

	private:
		static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
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

	private:
		HWND m_hWnd;
	};

#elif defined( __linux__ )

	class RenderWindow
	{
	public:
		RenderWindow()
		{
			try
			{
				m_display = XOpenDisplay( NULL );

				if ( !m_display )
				{
					CASTOR_EXCEPTION( "Couldn't open X Display" );
				}

				int l_attributes[] =
				{
					GLX_X_RENDERABLE, True,
					GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
					GLX_RENDER_TYPE, GLX_RGBA_BIT,
					GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
					GLX_DOUBLEBUFFER, GL_TRUE,
					GLX_RED_SIZE, 1,
					GLX_GREEN_SIZE, 1,
					GLX_BLUE_SIZE, 1,
					0
				};

				int l_fbcount = 0;
				GLXFBConfig * l_config = glXChooseFBConfig( m_display, DefaultScreen( m_display ), l_attributes, &l_fbcount );
				int l_bestFbcIndex = -1;
				int l_worstFbcIndex = -1;
				int l_bestNumSamp = -1;
				int l_worstNumSamp = 999;
				Logger::LogDebug( StringStream() << cuT( "Configs count: " ) << l_fbcount );

				for ( int i = 0; i < l_fbcount; ++i )
				{
					XVisualInfo * l_vi = glXGetVisualFromFBConfig( m_display, l_config[i] );

					if ( l_vi )
					{
						int l_sampleBuffers;
						int l_samples;
						glXGetFBConfigAttrib( m_display, l_config[i], GLX_SAMPLE_BUFFERS, &l_sampleBuffers );
						glXGetFBConfigAttrib( m_display, l_config[i], GLX_SAMPLES, &l_samples );

						if ( l_bestFbcIndex < 0 || l_sampleBuffers && l_samples > l_bestNumSamp )
						{
							l_bestFbcIndex = i;
							l_bestNumSamp = l_samples;
						}

						if ( l_worstFbcIndex < 0 || !l_sampleBuffers || l_samples < l_worstNumSamp )
						{
							l_worstFbcIndex = i;
							l_worstNumSamp = l_samples;
						}
					}

					XFree( l_vi );
				}

				if ( l_bestFbcIndex == -1 )
				{
					CASTOR_EXCEPTION( "Couldn't find appropriate GLXFBConfig" );
				}

				m_fbConfig = l_config[l_bestFbcIndex];
				XVisualInfo * l_vi = glXGetVisualFromFBConfig( m_display, m_fbConfig );

				if ( !l_vi )
				{
					CASTOR_EXCEPTION( "Couldn't find get XVisualInfo" );
				}

				Window l_root = RootWindow( m_display, l_vi->screen );
				m_map = XCreateColormap( m_display, l_root, l_vi->visual, AllocNone );

				if ( !m_map )
				{
					CASTOR_EXCEPTION( "Couldn't create X Colormap" );
				}

				XSetWindowAttributes l_swa;
				l_swa.colormap = m_map;
				l_swa.background_pixmap = 0;
				l_swa.border_pixel = 0;
				l_swa.event_mask = StructureNotifyMask;
				m_xWindow = XCreateWindow( m_display, l_root, 0, 0, 640, 480, 0, l_vi->depth, InputOutput, l_vi->visual, CWBorderPixel | CWColormap | CWEventMask, &l_swa );

				if ( !m_xWindow )
				{
					CASTOR_EXCEPTION( "Couldn't create X Window" );
				}

				XFree( l_vi );
				XStoreName( m_display, m_xWindow, "GlRenderSystemTests" );
				XMapWindow( m_display, m_xWindow );
  				XSync( m_display, False );
			}
			catch( Castor::Exception & p_exc )
			{
				if ( m_xWindow )
				{
					XDestroyWindow( m_display, m_xWindow );
				}

				if ( m_map )
				{
					XFreeColormap( m_display, m_map );
				}

				if ( m_display )
				{
					XCloseDisplay( m_display );
				}

				throw;
			}
		}

		~RenderWindow()
		{
			if ( m_xWindow )
			{
				XDestroyWindow( m_display, m_xWindow );
			}

			if ( m_map )
			{
				XFreeColormap( m_display, m_map );
			}

			if ( m_display )
			{
				XCloseDisplay( m_display );
			}
		}

		Castor3D::IWindowHandleSPtr CreateWindowHandle()
		{
			REQUIRE( m_xWindow );
			REQUIRE( m_display );
			Castor3D::IWindowHandleSPtr l_handle= std::make_shared< Castor3D::IXWindowHandle >( (GLXDrawable)m_xWindow, m_display );
			return l_handle;
		}

	private:
		Colormap m_map{ 0 };
		Display * m_display{ nullptr };
		Window m_xWindow{ 0 };
		GLXWindow m_glxWindow{ 0 };
		GLXFBConfig m_fbConfig{ nullptr };
	};

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

	Castor::Logger::Initialise( Castor::ELogType_DEBUG );
	Castor::Logger::SetFileName( Castor::File::GetExecutableDirectory() / cuT( "GlRenderSystemTests.log" ) );
	{
		try
		{
			RenderWindow l_window;
			auto l_handle = l_window.CreateWindowHandle();

			if ( l_handle )
			{
				auto l_engine = DoInitialiseCastor( l_handle );

				if ( l_engine )
				{
					// Test cases.
					Testing::Register( std::make_unique< Testing::GlTransformFeedbackTest >( *l_engine ) );

					// Tests loop.
					BENCHLOOP( l_count, l_return );

					l_engine->Cleanup();
				}
			}
		}
		catch( Castor::Exception & p_exc )
		{
			Logger::LogError( p_exc.what() );
		}
	}
	Castor::Logger::Cleanup();
	return l_return;
}
