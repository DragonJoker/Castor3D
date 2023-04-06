#include "CastorTestLauncher/MainFrame.hpp"

#include "CastorTestLauncher/CastorTestLauncher.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/Graphics/PixelBufferBase.hpp>

#include <ashespp/Core/PlatformWindowHandle.hpp>
#include <ashespp/Core/WindowHandle.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/display.h>
#include <wx/mstream.h>
#include <wx/renderer.h>
#include <wx/rawbmp.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#if defined( CU_PlatformApple )
#	include "MetalLayer.h"
#elif defined( CU_PlatformLinux )
#	include <gdk/gdkx.h>
#	include <gdk/gdkwayland.h>
#	include <gtk/gtk.h>
#	include <GL/glx.h>
#	undef None
#	undef Bool
#	undef Always
using Bool = int;
#endif

#include <castor.xpm>

namespace test_launcher
{
	namespace
	{
		castor3d::RenderTargetRPtr doLoadScene( castor3d::Engine & engine
			, castor::Path const & fileName )
		{
			castor3d::RenderTargetRPtr result{};

			if ( castor::File::fileExists( fileName ) )
			{
				castor::Logger::logInfo( cuT( "Loading scene file : " ) + fileName );

				if ( fileName.getExtension() == cuT( "cscn" ) || fileName.getExtension() == cuT( "zip" ) )
				{
					try
					{
						castor3d::SceneFileParser parser( engine );

						if ( parser.parseFile( fileName ) )
						{
							result = parser.getRenderWindow().renderTarget;
						}
						else
						{
							castor::Logger::logWarning( cuT( "Can't read scene file" ) );
						}
					}
					catch ( std::exception & exc )
					{
						castor::Logger::logError( castor::makeStringStream() << cuT( "Failed to parse the scene file, with following error: " ) << exc.what() );
					}
				}
				else
				{
					castor::Logger::logError( cuT( "Unsupported scene file type: " ) + fileName.getFileName() );
				}
			}
			else
			{
				castor::Logger::logError( cuT( "Scene file doesn't exist: " ) + fileName );
			}

			return result;
		}

		void doCreateBitmapFromBuffer( uint8_t const * input
			, uint32_t width
			, uint32_t height
			, bool flip
			, wxBitmap & output )
		{
			output.Create( int( width ), int( height ), 24 );
			wxNativePixelData data( output );

			if ( output.IsOk()
				&& uint32_t( data.GetWidth() ) == width
				&& uint32_t( data.GetHeight() ) == height )
			{
				wxNativePixelData::Iterator it( data );

				try
				{
					if ( flip )
					{
						uint32_t pitch = width * 4;
						uint8_t const * buffer = input + ( height - 1ull ) * pitch;

						for ( uint32_t i = 0; i < height && it.IsOk(); i++ )
						{
							uint8_t const * line = buffer;
#if defined( CU_PlatformWindows )
							wxNativePixelData::Iterator rowStart = it;
#endif

							for ( uint32_t j = 0; j < width && it.IsOk(); j++ )
							{
								it.Red() = *line;
								line++;
								it.Green() = *line;
								line++;
								it.Blue() = *line;
								line++;
								// don't write the alpha.
								line++;
								it++;
							}

							buffer -= pitch;

#if defined( CU_PlatformWindows )
							it = rowStart;
							it.OffsetY( data, 1 );
#endif
						}
					}
					else
					{
						uint8_t const * buffer = input;

						for ( uint32_t i = 0; i < height && it.IsOk(); i++ )
						{
#if defined( CU_PlatformWindows )
							wxNativePixelData::Iterator rowStart = it;
#endif

							for ( uint32_t j = 0; j < width && it.IsOk(); j++ )
							{
								it.Red() = *buffer;
								buffer++;
								it.Green() = *buffer;
								buffer++;
								it.Blue() = *buffer;
								buffer++;
								// don't write the alpha.
								buffer++;
								it++;
							}

#if defined( CU_PlatformWindows )
							it = rowStart;
							it.OffsetY( data, 1 );
#endif
						}
					}
				}
				catch ( ... )
				{
					castor::Logger::logWarning( cuT( "doCreateBitmapFromBuffer encountered an exception" ) );
				}
			}
		}

		void doCreateBitmapFromBuffer( castor::PxBufferBaseRPtr input
			, bool flip
			, wxBitmap & output )
		{
			castor::PxBufferBaseUPtr buffer{};

			if ( input->getFormat() != castor::PixelFormat::eR8G8B8A8_UNORM )
			{
				buffer = castor::PxBufferBase::create( castor::Size( input->getWidth(), input->getHeight() )
					, castor::PixelFormat::eR8G8B8A8_UNORM
					, input->getConstPtr()
					, input->getFormat() );
			}
			else
			{
				buffer = input->clone();
			}

			doCreateBitmapFromBuffer( buffer->getConstPtr()
				, buffer->getWidth()
				, buffer->getHeight()
				, flip
				, output );
		}

		ashes::WindowHandle makeWindowHandle( wxWindow * window )
		{
#if defined( CU_PlatformWindows )

			return ashes::WindowHandle( std::make_unique< ashes::IMswWindowHandle >( ::GetModuleHandle( nullptr )
				, window->GetHandle() ) );

#elif defined( CU_PlatformApple )

			auto handle = window->GetHandle();
			makeViewMetalCompatible( handle );
			return ashes::WindowHandle( std::make_unique< ashes::IMacOsWindowHandle >( handle ) );

#elif defined( CU_PlatformLinux )

			auto gtkWidget = window->GetHandle();

			if ( gtkWidget )
			{
				gtk_widget_realize( gtkWidget );
				auto gdkDisplay = gtk_widget_get_display( gtkWidget );

				if ( gdkDisplay )
				{
#if defined( VK_USE_PLATFORM_WAYLAND_KHR )
#	ifdef GDK_WINDOWING_WAYLAND
					if ( GDK_IS_WAYLAND_DISPLAY( gdkDisplay ) )
					{
						auto gdkWindow = gtk_widget_get_window( gtkWidget );
						auto display = gdk_wayland_display_get_wl_display( gdkDisplay );
						auto surface = gdkWindow
							? gdk_wayland_window_get_wl_surface( gdkWindow )
							: nullptr;
						return ashes::WindowHandle( std::make_unique< ashes::IWaylandWindowHandle >( display, surface ) );
					}
#	endif
#endif
#if defined( VK_USE_PLATFORM_XLIB_KHR )
#	ifdef GDK_WINDOWING_X11
					if ( GDK_IS_X11_DISPLAY( gdkDisplay ) )
					{
						auto gdkWindow = gtk_widget_get_window( gtkWidget );
						auto display = gdk_x11_display_get_xdisplay( gdkDisplay );
						GLXDrawable drawable = gdkWindow
							? gdk_x11_window_get_xid( gdkWindow )
							: 0;
						return ashes::WindowHandle( std::make_unique< ashes::IXWindowHandle >( drawable, display ) );
					}
#	endif
#endif
					CU_Exception( "Unsupported GTK surface type." );
				}
			}

			return ashes::WindowHandle{ nullptr };

#endif
		}
	}

	MainFrame::MainFrame( castor3d::Engine & engine
		, uint32_t maxFrameCount )
		: wxFrame{ nullptr, wxID_ANY, wxT( "Castor3D Test Launcher" ), wxDefaultPosition, wxSize( 800, 700 ) }
		, m_engine{ engine }
		, m_maxFrameCount{ maxFrameCount }
	{
		SetClientSize( 800, 600 );
	}

	bool MainFrame::initialise()
	{
		wxIcon icon = wxIcon( castor_xpm );
		SetIcon( icon );
		bool result = true;
		castor::Logger::logInfo( cuT( "Initialising Castor3D" ) );

		try
		{
			m_engine.initialise( 60, false );
			castor::Logger::logInfo( cuT( "Castor3D Initialised" ) );
		}
		catch ( std::exception & exc )
		{
			castor::Logger::logError( castor::makeStringStream() << cuT( "Problem occured while initialising Castor3D: " ) << exc.what() );
			result = false;
		}
		catch ( ... )
		{
			castor::Logger::logError( cuT( "Problem occured while initialising Castor3D." ) );
			result = false;
		}

		return result;
	}

	bool MainFrame::loadScene( wxString const & fileName )
	{
		if ( !fileName.empty() )
		{
			m_filePath = castor::Path{ static_cast< wxChar const * >( fileName.c_str() ) };
		}

		if ( !m_filePath.empty() )
		{
			auto sizewx = GetClientSize();
			castor::Size sizeWnd{ uint32_t( sizewx.GetWidth() ), uint32_t( sizewx.GetHeight() ) };
			m_renderWindow = std::make_unique< castor3d::RenderWindow >( "CastorTest"
				, m_engine
				, sizeWnd
				, makeWindowHandle( this ) );
			auto target = doLoadScene( m_engine, m_filePath );

			if ( !target )
			{
				castor::Logger::logError( cuT( "Can't initialise the render window." ) );
			}
			else
			{
				m_renderWindow->initialise( *target );
			}
		}
		else
		{
			castor::Logger::logError( cuT( "Can't open a scene file : empty file name." ) );
		}

		return m_renderWindow != nullptr;
	}

	void MainFrame::saveFrame( castor::String const & suffix
		, FrameTimes & times )
	{
		if ( m_renderWindow )
		{
			wxBitmap bitmap;

			for ( uint32_t i = 0u; i <= m_maxFrameCount; ++i )
			{
				m_engine.getRenderLoop().renderSyncFrame( 25_ms );
			}

			m_engine.getRenderLoop().dumpFrameTimes( times.params );
			m_renderWindow->enableSaveFrame();
			m_engine.getRenderLoop().renderSyncFrame( 25_ms );
			auto buffer = m_renderWindow->getSavedFrame();
			doCreateBitmapFromBuffer( buffer
				, false
				, bitmap );
			auto image = bitmap.ConvertToImage();
			auto folder = m_filePath.getPath() / cuT( "Compare" );

			if ( !castor::File::directoryExists( folder ) )
			{
				castor::File::directoryCreate( folder );
			}

			castor::Path outputPath = folder / ( m_filePath.getFileName() + cuT( "_" ) + suffix + cuT( ".png" ) );

			image.SaveFile( wxString( outputPath ) );
		}
	}

	void MainFrame::cleanup( castor::String const & suffix
		, FrameTimes const & times )
	{
		try
		{
			m_renderWindow->cleanup();
		}
		catch ( ... )
		{
		}

		m_renderWindow.reset();
		auto stop = Clock::now();
		auto totalTime = std::chrono::duration_cast< castor::Microseconds >( stop - times.start );
		castor::Nanoseconds avg{};
		castor::Nanoseconds last{};

		if ( times.params.get( "Average", avg )
			&& times.params.get( "Last", last ) )
		{
			std::ofstream stream{ m_filePath.getPath() / cuT( "Compare" ) / ( m_filePath.getFileName() + cuT( "_" ) + suffix + cuT( ".times" ) ) };

			if ( stream.is_open() )
			{
				stream << castor::System::getOSName() << "\n";
				stream << m_engine.getCpuInformations().getModel() << "\n";
				stream << m_engine.getRenderSystem()->getGpuInformations().getRenderer() << "\n";
				stream << totalTime.count()
					<< " " << std::chrono::duration_cast< castor::Microseconds >( avg ).count()
					<< " " << std::chrono::duration_cast< castor::Microseconds >( last ).count();
			}
		}

		m_engine.cleanup();
	}
}
