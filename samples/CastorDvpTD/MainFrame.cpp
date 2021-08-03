#include "MainFrame.hpp"

#include "RenderPanel.hpp"
#include "CastorDvpTD.hpp"
#include "Game.hpp"

#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>

#include <wx/sizer.h>

using namespace castor;
using namespace castor3d;

namespace castortd
{
	namespace
	{
		static const wxSize MainFrameSize{ 1024, 768 };

		typedef enum eID
		{
			eID_RENDER_TIMER,
		}	eID;

		void doUpdate( Game & p_game )
		{
			auto & engine = *wxGetApp().getCastor();

			if ( !engine.isCleaned() )
			{
				p_game.update();
				engine.postEvent( makeCpuFunctorEvent( EventType::ePostRender, [&p_game]()
				{
					doUpdate( p_game );
				} ) );
			}
		}
	}

	MainFrame::MainFrame()
		: wxFrame{ nullptr, wxID_ANY, ApplicationName, wxDefaultPosition, MainFrameSize, wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxRESIZE_BORDER | wxMAXIMIZE_BOX }
	{
		SetClientSize( MainFrameSize );
		Show( true );

		try
		{
			doLoadScene();
			wxBoxSizer * sizer{ new wxBoxSizer{ wxHORIZONTAL } };
			sizer->Add( m_panel.get(), wxSizerFlags( 1 ).Shaped().Centre() );
			sizer->SetSizeHints( this );
			SetSizer( sizer );
		}
		catch ( std::exception & p_exc )
		{
			wxMessageBox( p_exc.what() );
		}
	}

	MainFrame::~MainFrame()
	{
	}

	void MainFrame::doLoadScene()
	{
		auto & engine = *wxGetApp().getCastor();
		auto target = GuiCommon::loadScene( engine
			, File::getExecutableDirectory().getPath() / cuT( "share" ) / cuT( "CastorDvpTD" ) / cuT( "Data.zip" )
			, nullptr );

		if ( target )
		{
			m_game = std::make_unique< Game >( *target->getScene() );
			m_panel = wxMakeWindowPtr< RenderPanel >( this, MainFrameSize, *m_game );
			m_panel->setRenderTarget( target );
			auto & window = m_panel->getRenderWindow();

			if ( window.isFullscreen() )
			{
				ShowFullScreen( true, wxFULLSCREEN_ALL );
			}

			if ( !IsMaximized() )
			{
				SetClientSize( window.getSize().getWidth()
					, window.getSize().getHeight() );
			}
			else
			{
				Maximize( false );
				SetClientSize( window.getSize().getWidth()
					, window.getSize().getHeight() );
				Maximize();
			}

			Logger::logInfo( cuT( "Scene file read" ) );

#if wxCHECK_VERSION( 2, 9, 0 )

			wxSize size = GetClientSize();
			SetMinClientSize( size );

#endif
			auto & engine = *wxGetApp().getCastor();

			if ( engine.isThreaded() )
			{
				engine.getRenderLoop().beginRendering();
				engine.postEvent( makeCpuFunctorEvent( EventType::ePostRender
					, [this]()
					{
						doUpdate( *m_game );
					} ) );
			}
			else
			{
				m_timer = new wxTimer( this, eID_RENDER_TIMER );
				m_timer->Start( 1000 / engine.getRenderLoop().getWantedFps(), true );
			}
		}
	}

	BEGIN_EVENT_TABLE( MainFrame, wxFrame )
		EVT_PAINT( MainFrame::OnPaint )
		EVT_CLOSE( MainFrame::OnClose )
		EVT_ERASE_BACKGROUND( MainFrame::OnEraseBackground )
		EVT_TIMER( eID_RENDER_TIMER, MainFrame::OnRenderTimer )
	END_EVENT_TABLE()

	void MainFrame::OnPaint( wxPaintEvent & p_event )
	{
		wxPaintDC paintDC( this );
		p_event.Skip();
	}

	void MainFrame::OnClose( wxCloseEvent & p_event )
	{
		Hide();

		if ( m_timer )
		{
			m_timer->Stop();
			delete m_timer;
			m_timer = nullptr;
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
		p_event.Skip();
	}

	void MainFrame::OnEraseBackground( wxEraseEvent & p_event )
	{
		p_event.Skip();
	}

	void MainFrame::OnRenderTimer( wxTimerEvent & p_event )
	{
		if ( wxGetApp().getCastor() )
		{
			auto & castor = *wxGetApp().getCastor();

			if ( !castor.isCleaned() )
			{
				if ( !castor.isThreaded() )
				{
					castor.getRenderLoop().renderSyncFrame();
					m_game->update();
					m_timer->Start( 1000 / castor.getRenderLoop().getWantedFps(), true );
				}
			}
		}
	}
}
