#include "GlRenderSystemTestPrerequisites.hpp"

#include <Log/Logger.hpp>
#include <Data/File.hpp>

#include <Engine.hpp>
#include <Cache/PluginCache.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Miscellaneous/PlatformWindowHandle.hpp>

#include <BenchManager.hpp>

#include <Render/GlRenderSystem.hpp>

#include "GlAtomicCounterBufferTest.hpp"
#include "ComputeShaderTest.hpp"
#include "GlTransformFeedbackTest.hpp"
#include "GlTextureTest.hpp"

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
		PathArray arrayFiles;
		File::ListDirectoryFiles( Castor3D::Engine::GetPluginsDirectory(), arrayFiles );
		PathArray arrayKept;

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		for ( auto file : arrayFiles )
		{
#if defined( NDEBUG )

			if ( file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) == String::npos )
#else

			if ( file.find( String( cuT( "d." ) ) + CASTOR_DLL_EXT ) != String::npos )

#endif
			{
				arrayKept.push_back( file );
			}
		}

		if ( !arrayKept.empty() )
		{
			PathArray arrayFailed;
			PathArray otherPlugins;

			for ( auto file : arrayKept )
			{
				if ( file.GetExtension() == CASTOR_DLL_EXT )
				{
					// Since techniques depend on renderers, we load these first
					if ( file.find( cuT( "RenderSystem" ) ) != String::npos )
					{
						if ( !p_engine.GetPluginCache().LoadPlugin( file ) )
						{
							arrayFailed.push_back( file );
						}
					}
					else
					{
						otherPlugins.push_back( file );
					}
				}
			}

			// Then we load other plug-ins
			for ( auto file : otherPlugins )
			{
				if ( !p_engine.GetPluginCache().LoadPlugin( file ) )
				{
					arrayFailed.push_back( file );
				}
			}

			if ( !arrayFailed.empty() )
			{
				Logger::LogWarning( cuT( "Some plug-ins couldn't be loaded :" ) );

				for ( auto file : arrayFailed )
				{
					Logger::LogWarning( Path( file ).GetFileName() );
				}

				arrayFailed.clear();
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

		auto result = std::make_unique< Castor3D::Engine >();
		DoLoadPlugins( *result );

		auto renderers = result->GetPluginCache().GetPlugins( Castor3D::PluginType::eRenderer );

		if ( renderers.empty() )
		{
			CASTOR_EXCEPTION( "No renderer plug-ins" );
		}

		if ( result->LoadRenderer( GlRender::GlRenderSystem::Type ) )
		{
			result->Initialise( 1, false );
			auto context = result->GetRenderSystem()->CreateContext();
			auto scene = result->GetSceneCache().Add( cuT( "Test" ) );
			auto window = result->GetRenderWindowCache().Add( cuT( "Window" ) );
			auto target = result->GetRenderTargetCache().Add( Castor3D::TargetType::eWindow );
			target->SetPixelFormat( PixelFormat::eA8R8G8B8 );
			target->SetSize( Size{ 1024, 1024 } );
			target->SetScene( scene );
			window->SetRenderTarget( target );
			window->Initialise( Size{ 1024, 1024 }, Castor3D::WindowHandle{ p_handle } );
		}
		else
		{
			CASTOR_EXCEPTION( "Couldn't load renderer." );
		}

		return result;
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
			Castor3D::IWindowHandleSPtr handle;

			if ( m_hWnd )
			{
				handle = std::make_shared< Castor3D::IMswWindowHandle >( m_hWnd );
			}

			return handle;
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

				int attributes[] =
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

				int fbcount = 0;
				GLXFBConfig * config = glXChooseFBConfig( m_display, DefaultScreen( m_display ), attributes, &fbcount );
				int bestFbcIndex = -1;
				int worstFbcIndex = -1;
				int bestNumSamp = -1;
				int worstNumSamp = 999;
				Logger::LogDebug( StringStream() << cuT( "Configs count: " ) << fbcount );

				for ( int i = 0; i < fbcount; ++i )
				{
					XVisualInfo * vi = glXGetVisualFromFBConfig( m_display, config[i] );

					if ( vi )
					{
						int sampleBuffers;
						int samples;
						glXGetFBConfigAttrib( m_display, config[i], GLX_SAMPLE_BUFFERS, &sampleBuffers );
						glXGetFBConfigAttrib( m_display, config[i], GLX_SAMPLES, &samples );

						if ( bestFbcIndex < 0 || sampleBuffers && samples > bestNumSamp )
						{
							bestFbcIndex = i;
							bestNumSamp = samples;
						}

						if ( worstFbcIndex < 0 || !sampleBuffers || samples < worstNumSamp )
						{
							worstFbcIndex = i;
							worstNumSamp = samples;
						}
					}

					XFree( vi );
				}

				if ( bestFbcIndex == -1 )
				{
					CASTOR_EXCEPTION( "Couldn't find appropriate GLXFBConfig" );
				}

				m_fbConfig = config[bestFbcIndex];
				XVisualInfo * vi = glXGetVisualFromFBConfig( m_display, m_fbConfig );

				if ( !vi )
				{
					CASTOR_EXCEPTION( "Couldn't find get XVisualInfo" );
				}

				Window root = RootWindow( m_display, vi->screen );
				m_map = XCreateColormap( m_display, root, vi->visual, AllocNone );

				if ( !m_map )
				{
					CASTOR_EXCEPTION( "Couldn't create X Colormap" );
				}

				XSetWindowAttributes swa;
				swa.colormap = m_map;
				swa.background_pixmap = 0;
				swa.border_pixel = 0;
				swa.event_mask = StructureNotifyMask;
				m_xWindow = XCreateWindow( m_display, root, 0, 0, 640, 480, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa );

				if ( !m_xWindow )
				{
					CASTOR_EXCEPTION( "Couldn't create X Window" );
				}

				XFree( vi );
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
			Castor3D::IWindowHandleSPtr handle= std::make_shared< Castor3D::IXWindowHandle >( (GLXDrawable)m_xWindow, m_display );
			return handle;
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
	int result = EXIT_SUCCESS;
	int count = 1;

	if ( argc == 2 )
	{
		count = std::max< int >( 1, atoi( argv[2] ) );
	}

	Castor::Logger::Initialise( Castor::LogType::eDebug );
	Castor::Logger::SetFileName( Castor::File::GetExecutableDirectory() / cuT( "GlRenderSystemTests.log" ) );
	{
		try
		{
			RenderWindow window;
			auto handle = window.CreateWindowHandle();

			if ( handle )
			{
				auto engine = DoInitialiseCastor( handle );

				if ( engine )
				{
					// Test cases.
					Testing::Register( std::make_unique< Testing::GlTextureTest >( *engine ) );

					if ( engine->GetRenderSystem()->GetGpuInformations().HasFeature( Castor3D::GpuFeature::eTransformFeedback ) )
					{
						//Testing::Register( std::make_unique< Testing::GlTransformFeedbackTest >( *engine ) );
					}

					if ( engine->GetRenderSystem()->GetGpuInformations().HasFeature( Castor3D::GpuFeature::eAtomicCounterBuffers ) )
					{
						Testing::Register( std::make_unique< Testing::GlAtomicCounterBufferTest >( *engine ) );
					}

					if ( engine->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel() >= Castor3D::ShaderModel::eModel5 )
					{
						Testing::Register( std::make_unique< Testing::GlComputeShaderTest >( *engine ) );
					}

					// Tests loop.
					BENCHLOOP( count, result );

					engine->Cleanup();
				}
			}
		}
		catch( Castor::Exception & p_exc )
		{
			Logger::LogError( p_exc.what() );
		}
	}
	Castor::Logger::Cleanup();
	return result;
}
