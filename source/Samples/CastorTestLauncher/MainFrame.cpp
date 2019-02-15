#include "CastorTestLauncher/MainFrame.hpp"

#include "CastorTestLauncher/CastorTestLauncher.hpp"

#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/Graphics/PixelBufferBase.hpp>

#include <Ashes/Core/PlatformWindowHandle.hpp>
#include <Ashes/Core/WindowHandle.hpp>

#include <wx/display.h>
#include <wx/mstream.h>
#include <wx/renderer.h>
#include <wx/rawbmp.h>

#include <xpms/castor.xpm>

using namespace castor3d;
using namespace castor;

namespace test_launcher
{
	namespace
	{
		RenderWindowSPtr doLoadScene( Engine & engine
			, Path const & fileName )
		{
			RenderWindowSPtr result;

			if ( File::fileExists( fileName ) )
			{
				Logger::logInfo( cuT( "Loading scene file : " ) + fileName );

				if ( fileName.getExtension() == cuT( "cscn" ) || fileName.getExtension() == cuT( "zip" ) )
				{
					try
					{
						SceneFileParser parser( engine );

						if ( parser.parseFile( fileName ) )
						{
							result = parser.getRenderWindow();
						}
						else
						{
							Logger::logWarning( cuT( "Can't read scene file" ) );
						}
					}
					catch ( std::exception & exc )
					{
						Logger::logError( makeStringStream() << cuT( "Failed to parse the scene file, with following error: " ) << exc.what() );
					}
				}
				else
				{
					Logger::logError( cuT( "Unsupported scene file type: " ) + fileName.getFileName() );
				}
			}
			else
			{
				Logger::logError( cuT( "Scene file doesn't exist: " ) + fileName );
			}

			return result;
		}

		void doCreateBitmapFromBuffer( uint8_t const * input
			, uint32_t width
			, uint32_t height
			, bool flip
			, wxBitmap & output )
		{
			output.Create( width, height, 24 );
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
						uint8_t const * buffer = input + ( height - 1 ) * pitch;

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
					Logger::logWarning( cuT( "doCreateBitmapFromBuffer encountered an exception" ) );
				}
			}
		}

		void doCreateBitmapFromBuffer( PxBufferBaseSPtr input
			, bool flip
			, wxBitmap & output )
		{
			PxBufferBaseSPtr buffer;

			if ( input->format() != PixelFormat::eR8G8B8A8_UNORM )
			{
				buffer = PxBufferBase::create( Size( input->getWidth(), input->getHeight() )
					, PixelFormat::eR8G8B8A8_UNORM
					, input->constPtr()
					, input->format() );
			}
			else
			{
				buffer = input;
			}

			doCreateBitmapFromBuffer( buffer->constPtr()
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

#elif defined( CU_PlatformLinux )

			GtkWidget * gtkWidget = static_cast< GtkWidget * >( window->GetHandle() );
			GLXDrawable drawable = 0;
			Display * display = nullptr;

			if ( gtkWidget && gtkWidget->window )
			{
				drawable = GDK_WINDOW_XID( gtkWidget->window );
				GdkDisplay * gtkDisplay = gtk_widget_get_display( gtkWidget );

				if ( gtkDisplay )
				{
					display = gdk_x11_display_get_xdisplay( gtkDisplay );
				}
			}

			return ashes::WindowHandle( std::make_unique< ashes::IXWindowHandle >( drawable, display ) );

#endif
		}
	}

	MainFrame::MainFrame( castor3d::Engine & engine )
		: wxFrame{ nullptr, wxID_ANY, wxT( "Castor3D Test Launcher" ), wxDefaultPosition, wxSize( 800, 700 ) }
		, m_engine{ engine }
	{
		SetClientSize( 800, 600 );
	}

	MainFrame::~MainFrame()
	{
	}

	bool MainFrame::initialise()
	{
		wxIcon icon = wxIcon( castor_xpm );
		SetIcon( icon );
		bool result = true;
		Logger::logInfo( cuT( "Initialising Castor3D" ) );

		try
		{
			m_engine.initialise( 60, false );
			Logger::logInfo( cuT( "Castor3D Initialised" ) );
		}
		catch ( std::exception & exc )
		{
			Logger::logError( makeStringStream() << cuT( "Problem occured while initialising Castor3D: " ) << exc.what() );
			result = false;
		}
		catch ( ... )
		{
			Logger::logError( cuT( "Problem occured while initialising Castor3D." ) );
			result = false;
		}

		return result;
	}

	bool MainFrame::loadScene( wxString const & fileName )
	{
		if ( !fileName.empty() )
		{
			m_filePath = Path{ ( wxChar const * )fileName.c_str() };
		}

		if ( !m_filePath.empty() )
		{
			RenderWindowSPtr window = doLoadScene( m_engine, m_filePath );

			auto sizewx = GetClientSize();
			castor::Size sizeWnd{ uint32_t( sizewx.GetWidth() ), uint32_t( sizewx.GetHeight() ) };

			if ( !window || !window->initialise( sizeWnd, makeWindowHandle( this ) ) )
			{
				Logger::logError( cuT( "Can't initialise the render window." ) );
			}
			else
			{
				m_renderWindow = window;
			}
		}
		else
		{
			Logger::logError( cuT( "Can't open a scene file : empty file name." ) );
		}

		return m_renderWindow != nullptr;
	}

	void MainFrame::saveFrame( castor::String const & renderer )
	{
		if ( m_renderWindow )
		{
			wxBitmap bitmap;

			// Prerender 10 frames, for environment maps.
			for ( uint32_t i = 0; i < 10; ++i )
			{
				m_engine.getRenderLoop().renderSyncFrame();
			}

			m_renderWindow->enableSaveFrame();
			m_engine.getRenderLoop().renderSyncFrame();
			auto buffer = m_renderWindow->getSavedFrame();
			Size size = buffer->dimensions();
			doCreateBitmapFromBuffer( buffer
				, false
				, bitmap );
			auto image = bitmap.ConvertToImage();
			auto folder = m_filePath.getPath() / cuT( "Compare" );

			if ( !castor::File::directoryExists( folder ) )
			{
				castor::File::directoryCreate( folder );
			}

			Path outputPath = folder / ( m_filePath.getFileName() + cuT( "_" ) + renderer + cuT( ".png" ) );
			image.SaveFile( wxString( outputPath ) );
		}
	}

	void MainFrame::cleanup()
	{
		m_renderWindow.reset();
		m_engine.cleanup();
	}
}
