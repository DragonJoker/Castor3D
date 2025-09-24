#include "MainFrame.hpp"

#include "RenderPanel.hpp"
#include "CastorDvpTD.hpp"
#include "Game.hpp"

#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>

#include <wx/sizer.h>

namespace castortd
{
	namespace main
	{
		static const wxSize MainFrameSize{ 1024, 768 };

		enum class eID
		{
			eRenderTimer,
		};

		static void doUpdate( Game & game )
		{
			if ( auto const & engine = *wxGetApp().getCastor();
				!engine.isCleaned() )
			{
				game.update();
				engine.postEvent( c3d::makeCpuFunctorEvent( c3d::CpuEventType::ePostCpuStep
					, [&game]()
					{
						doUpdate( game );
					} ) );
			}
		}
	}

	MainFrame::MainFrame()
		: wxFrame{ nullptr, wxID_ANY, ApplicationName, wxDefaultPosition, main::MainFrameSize, wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxRESIZE_BORDER | wxMAXIMIZE_BOX }
	{
		SetClientSize( FromDIP( main::MainFrameSize ) );
		Show( true );

		try
		{
			doLoadScene();
			wxBoxSizer * sizer{ new wxBoxSizer{ wxHORIZONTAL } };
			sizer->Add( m_panel.get(), wxSizerFlags( 1 ).Shaped().Centre() );
			sizer->SetSizeHints( this );
			SetSizer( sizer );
		}
		catch ( std::exception & exc )
		{
			wxMessageBox( exc.what() );
		}
	}

	void MainFrame::doLoadScene()
	{
		auto & engine = *wxGetApp().getCastor();
		auto window = GuiCommon::loadScene( engine
			, cuT( "CastorDvpTD" )
			, c3d::File::getExecutableDirectory().getPath() / cuT( "share" ) / cuT( "CastorDvpTD" ) / cuT( "Data.zip" )
			, nullptr );
		c3d::Logger::logInfo( cuT( "Scene file read" ) );

		if ( auto target = window.renderTarget )
		{
			m_game = c3d::makeRawUnique< Game >( *target->getScene() );
			m_panel = wxMakeWindowPtr< RenderPanel >( this, main::MainFrameSize, *m_game );
			m_panel->updateRenderWindow( window );
			auto & renderWindow = m_panel->getRenderWindow();

			if ( renderWindow.isFullscreen() )
			{
				ShowFullScreen( true, wxFULLSCREEN_ALL );
			}

			auto size = FromDIP( GuiCommon::make_wxSize( renderWindow.getSize() ) );

			if ( !IsMaximized() )
			{
				SetClientSize( size );
			}
			else
			{
				Maximize( false );
				SetClientSize( size );
				Maximize();
			}
#if wxCHECK_VERSION( 2, 9, 0 )
			SetMinClientSize( size );
#endif

			if ( engine.isThreaded() )
			{
				engine.getRenderLoop().beginRendering();
				engine.postEvent( c3d::makeCpuFunctorEvent( c3d::CpuEventType::ePostCpuStep
					, [this]()
					{
						main::doUpdate( *m_game );
					} ) );
			}
			else
			{
				m_timer = c3d::makeRawUnique< wxTimer >( this, int( main::eID::eRenderTimer ) );
				m_timer->Start( 1000 / int( engine.getRenderLoop().getWantedFps() ), true );
			}

			c3d::Logger::logInfo( cuT( "Initialised render panel." ) );
		}
		else
		{
			c3d::Logger::logWarning( cuT( "No render target found in the scene" ) );
		}
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( MainFrame, wxFrame )
		EVT_PAINT( MainFrame::onPaint )
		EVT_CLOSE( MainFrame::onClose )
		EVT_ERASE_BACKGROUND( MainFrame::onEraseBackground )
		EVT_TIMER( int( main::eID::eRenderTimer ), MainFrame::onRenderTimer )
		EVT_KEY_DOWN( MainFrame::onKeyDown )
		EVT_KEY_UP( MainFrame::onKeyUp )
		EVT_LEFT_DOWN( MainFrame::onMouseLDown )
		EVT_LEFT_UP( MainFrame::onMouseLUp )
		EVT_RIGHT_UP( MainFrame::onMouseRUp )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void MainFrame::onPaint( wxPaintEvent & event )
	{
		wxPaintDC paintDC( this );
		event.Skip();
	}

	void MainFrame::onClose( wxCloseEvent & event )
	{
		Hide();

		if ( m_timer )
		{
			m_timer->Stop();
			m_timer = {};
		}

		auto & engine = *wxGetApp().getCastor();

		if ( m_panel )
		{
			if ( engine.isThreaded() )
			{
				engine.getRenderLoop().pause();
			}

			m_panel->reset();

			if ( engine.isThreaded() )
			{
				engine.getRenderLoop().resume();
			}
		}

		if ( wxGetApp().getCastor() )
		{
			engine.cleanup();
		}

		if ( m_panel )
		{
			m_panel->Close( true );
			m_panel = nullptr;
		}

		DestroyChildren();
		event.Skip();
	}

	void MainFrame::onEraseBackground( wxEraseEvent & event )
	{
		event.Skip();
	}

	void MainFrame::onRenderTimer( wxTimerEvent & event )
	{
		if ( wxGetApp().getCastor() )
		{
			auto const & castor = *wxGetApp().getCastor();

			if ( !castor.isCleaned()
				&& !castor.isThreaded() )
			{
				castor.getRenderLoop().renderSyncFrame();
				m_game->update();
				m_timer->Start( 1000 / int( castor.getRenderLoop().getWantedFps() ), true );
			}
		}

		event.Skip( false );
	}

	void MainFrame::onKeyDown( wxKeyEvent & event )
	{
		if ( m_panel )
		{
			m_panel->GetEventHandler()->QueueEvent( new wxKeyEvent{ event } );
			event.Skip();
		}
	}

	void MainFrame::onKeyUp( wxKeyEvent & event )
	{
		if ( m_panel )
		{
			m_panel->GetEventHandler()->QueueEvent( new wxKeyEvent{ event } );
			event.Skip();
		}
	}

	void MainFrame::onMouseLDown( wxMouseEvent & event )
	{
		if ( m_panel )
		{
			m_panel->GetEventHandler()->QueueEvent( new wxMouseEvent{ event } );
			event.Skip();
		}
	}

	void MainFrame::onMouseLUp( wxMouseEvent & event )
	{
		if ( m_panel )
		{
			m_panel->GetEventHandler()->QueueEvent( new wxMouseEvent{ event } );
			event.Skip();
		}
	}

	void MainFrame::onMouseRUp( wxMouseEvent & event )
	{
		if ( m_panel )
		{
			m_panel->GetEventHandler()->QueueEvent( new wxMouseEvent{ event } );
			event.Skip();
		}
	}

	void MainFrame::onMouseWheel( wxMouseEvent & event )
	{
		if ( m_panel )
		{
			m_panel->GetEventHandler()->QueueEvent( new wxMouseEvent{ event } );
			event.Skip();
		}
	}
}
