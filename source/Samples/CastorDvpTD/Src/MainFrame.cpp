#include "MainFrame.hpp"

#include "RenderPanel.hpp"
#include "CastorDvpTD.hpp"
#include "Game.hpp"

#include <wx/sizer.h>

#include <Event/Frame/FunctorEvent.hpp>
#include <Render/RenderLoop.hpp>
#include <Render/RenderWindow.hpp>

using namespace Castor;
using namespace Castor3D;

namespace castortd
{
	namespace
	{
		static const wxSize MainFrameSize{ 1024, 768 };
		static const bool CASTOR3D_THREADED = true;

		typedef enum eID
		{
			eID_RENDER_TIMER,
		}	eID;

		void DoUpdate( Game & p_game )
		{
			if ( !wxGetApp().GetCastor()->IsCleaned() )
			{
				p_game.Update();
				wxGetApp().GetCastor()->PostEvent( MakeFunctorEvent( EventType::ePostRender, [&p_game]()
				{
					DoUpdate( p_game );
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
			wxGetApp().GetCastor()->Initialise( 120, CASTOR3D_THREADED );
			DoLoadScene();
			wxBoxSizer * l_sizer{ new wxBoxSizer{ wxHORIZONTAL } };
			l_sizer->Add( m_panel.get(), wxSizerFlags( 1 ).Shaped().Centre() );
			l_sizer->SetSizeHints( this );
			SetSizer( l_sizer );
		}
		catch ( std::exception & p_exc )
		{
			wxMessageBox( p_exc.what() );
		}
	}

	MainFrame::~MainFrame()
	{
	}

	void MainFrame::DoLoadScene()
	{
		auto & l_engine = *wxGetApp().GetCastor();
		auto l_window = GuiCommon::LoadScene( l_engine
											  , File::GetExecutableDirectory().GetPath() / cuT( "share" ) / cuT( "CastorDvpTD" ) / cuT( "Data.zip" )
											  , l_engine.GetRenderLoop().GetWantedFps()
											  , l_engine.IsThreaded() );

		if ( l_window )
		{
			m_game = std::make_unique< Game >( *l_window->GetScene() );
			m_panel = wxMakeWindowPtr< RenderPanel >( this, MainFrameSize, *m_game );
			m_panel->SetRenderWindow( l_window );

			if ( l_window->IsInitialised() )
			{
				if ( l_window->IsFullscreen() )
				{
					ShowFullScreen( true, wxFULLSCREEN_ALL );
				}

				if ( !IsMaximized() )
				{
					SetClientSize( l_window->GetSize().width(), l_window->GetSize().height() );
				}
				else
				{
					Maximize( false );
					SetClientSize( l_window->GetSize().width(), l_window->GetSize().height() );
					Maximize();
				}

				Logger::LogInfo( cuT( "Scene file read" ) );
			}
			else
			{
				throw std::runtime_error{ "Impossible d'initialiser la fenêtre de rendu." };
			}

#if wxCHECK_VERSION( 2, 9, 0 )

			wxSize l_size = GetClientSize();
			SetMinClientSize( l_size );

#endif

			if ( CASTOR3D_THREADED )
			{
				wxGetApp().GetCastor()->GetRenderLoop().StartRendering();
				wxGetApp().GetCastor()->PostEvent( MakeFunctorEvent( EventType::ePostRender, [this]()
				{
					DoUpdate( *m_game );
				} ) );
			}
			else
			{
				m_timer = new wxTimer( this, eID_RENDER_TIMER );
				m_timer->Start( 1000 / wxGetApp().GetCastor()->GetRenderLoop().GetWantedFps(), true );
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
		wxPaintDC l_paintDC( this );
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

		if ( m_panel )
		{
			if ( wxGetApp().GetCastor()->IsThreaded() )
			{
				wxGetApp().GetCastor()->GetRenderLoop().Pause();
			}

			m_panel->SetRenderWindow( nullptr );

			if ( wxGetApp().GetCastor()->IsThreaded() )
			{
				wxGetApp().GetCastor()->GetRenderLoop().Resume();
			}
		}

		if ( wxGetApp().GetCastor() )
		{
			wxGetApp().GetCastor()->Cleanup();
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
		if ( wxGetApp().GetCastor() )
		{
			auto & l_castor = *wxGetApp().GetCastor();

			if ( !l_castor.IsCleaned() )
			{
				if ( !l_castor.IsThreaded() )
				{
					l_castor.GetRenderLoop().RenderSyncFrame();
					m_game->Update();
					m_timer->Start( 1000 / l_castor.GetRenderLoop().GetWantedFps(), true );
				}
			}
		}
	}
}
