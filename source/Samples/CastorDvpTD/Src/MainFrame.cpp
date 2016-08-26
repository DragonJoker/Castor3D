#include "MainFrame.hpp"

#include "RenderPanel.hpp"
#include "CastorDvpTD.hpp"

#include <wx/sizer.h>

#include <Render/RenderLoop.hpp>
#include <Render/RenderWindow.hpp>

using namespace Castor;
using namespace Castor3D;

namespace castortd
{
	namespace
	{
		static const wxSize MainFrameSize{ 800, 600 };

#if defined( NDEBUG )

		static const ELogType ELogType_DEFAULT = ELogType_INFO;

#else

		static const ELogType ELogType_DEFAULT = ELogType_DEBUG;

#endif

	}

	MainFrame::MainFrame()
		: wxFrame{ nullptr, wxID_ANY, ApplicationName, wxDefaultPosition, MainFrameSize, wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN | wxRESIZE_BORDER | wxMAXIMIZE_BOX }
	{
		SetClientSize( MainFrameSize );
		Show( true );

		try
		{
			m_panel = wxMakeWindowPtr< RenderPanel >( this, MainFrameSize );

			wxBoxSizer * l_sizer{ new wxBoxSizer{ wxVERTICAL } };
			l_sizer->Add( m_panel.get(), wxSizerFlags{ 1 }.Expand() );
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

	void MainFrame::Initialise()
	{
		auto l_window = GuiCommon::LoadScene( *wxGetApp().GetCastor()
											  , File::GetExecutableDirectory().GetPath() / cuT( "share" ) / cuT( "CastorDvpTD" ) / cuT( "scene.cscn" )
											  , 60
											  , true );

		if ( l_window )
		{
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

			wxGetApp().GetCastor()->GetRenderLoop().StartRendering();

#if wxCHECK_VERSION( 2, 9, 0 )

			wxSize l_size = GetClientSize();
			SetMinClientSize( l_size );

#endif
		}
	}

	BEGIN_EVENT_TABLE( MainFrame, wxFrame )
		EVT_PAINT( MainFrame::OnPaint )
		EVT_CLOSE( MainFrame::OnClose )
		EVT_ERASE_BACKGROUND( MainFrame::OnEraseBackground )
	END_EVENT_TABLE()

	void MainFrame::OnPaint( wxPaintEvent & p_event )
	{
		wxPaintDC l_paintDC( this );
		p_event.Skip();
	}

	void MainFrame::OnClose( wxCloseEvent & p_event )
	{
		Hide();

		if ( m_panel )
		{
			m_panel->Close( true );
			m_panel = nullptr;
		}

		if ( wxGetApp().GetCastor() )
		{
			wxGetApp().GetCastor()->Cleanup();
		}

		DestroyChildren();
		p_event.Skip();
	}

	void MainFrame::OnEraseBackground( wxEraseEvent & p_event )
	{
		p_event.Skip();
	}
}
