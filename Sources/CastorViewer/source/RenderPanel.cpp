#include "PrecompiledHeader.h"

#include "RenderPanel.h"
#include "CastorViewer.h"
#include "MainFrame.h"

#define ID_NEW_WINDOW 10000

using namespace CastorViewer;
using namespace Castor3D;

DECLARE_APP( CastorViewerApp)

RenderPanel :: RenderPanel( wxWindow * parent, wxWindowID p_id,
							    ProjectionType p_renderType, Scene * p_scene,
								const wxPoint & pos, const wxSize & size,
								ProjectionDirection p_look, long style)
	:	wxPanel( parent, p_id, pos, size, style),
		m_timer( NULL),
		m_renderType( p_renderType),
		m_lookAt( p_look),
		m_renderWindow( NULL),
		m_mainScene( p_scene),
		m_mouseLeftDown( false),
		m_mouseRightDown( false),
		m_mouseMiddleDown( false)
{
	_initialiseRenderWindow();
}

RenderPanel :: ~RenderPanel()
{
	if (m_timer != NULL)
	{
		m_timer->Stop();
		delete m_timer;
		m_timer = NULL;
	}
}

void RenderPanel :: Focus()
{
	m_renderWindow->Focus();
}

void RenderPanel :: UnFocus()
{
	m_renderWindow->UnFocus();
}

void RenderPanel :: DrawOneFrame()
{
	wxClientDC l_dc( this);
	m_renderWindow->SetToUpdate();
}

void RenderPanel :: _initialiseRenderWindow()
{
	Log::LogMessage( C3D_T( "Initialising RenderWindow"));
	m_renderWindow = Root::GetSingletonPtr()->CreateRenderWindow( m_mainScene,
																  (void *)GetHandle(),
																  GetClientSize().x,
																  GetClientSize().y,
																  m_renderType,
																  m_lookAt);
	m_listener = m_renderWindow->GetListener();

	if (m_timer == NULL)
	{
		m_timer = new wxTimer( this, 1);
		m_timer->Start( 40);
	}

	Log::LogMessage( "RenderWindow Initialised");
}

BEGIN_EVENT_TABLE( RenderPanel, wxPanel)
	EVT_TIMER(	1,			RenderPanel::_onTimer)

	EVT_SIZE(				RenderPanel::_onSize)
	EVT_MOVE(				RenderPanel::_onMove)
	EVT_CLOSE(				RenderPanel::_onClose)
	EVT_PAINT(				RenderPanel::_onPaint)
	EVT_ENTER_WINDOW(		RenderPanel::_onEnterWindow)
	EVT_LEAVE_WINDOW(		RenderPanel::_onLeaveWindow)
	EVT_ERASE_BACKGROUND(	RenderPanel::_onEraseBackground)
	
	EVT_SET_FOCUS(			RenderPanel::_onSetFocus)
	EVT_KILL_FOCUS(			RenderPanel::_onKillFocus)

	EVT_KEY_DOWN(			RenderPanel::_onKeyDown)
	EVT_KEY_UP(				RenderPanel::_onKeyUp)
	EVT_LEFT_DOWN(			RenderPanel::_onMouseLDown)
	EVT_LEFT_UP(			RenderPanel::_onMouseLUp)
	EVT_MIDDLE_DOWN(		RenderPanel::_onMouseMDown)
	EVT_MIDDLE_UP(			RenderPanel::_onMouseMUp)
	EVT_RIGHT_DOWN(			RenderPanel::_onMouseRDown)
	EVT_RIGHT_UP(			RenderPanel::_onMouseRUp)
	EVT_MOTION(				RenderPanel::_onMouseMove)
	EVT_MOUSEWHEEL(			RenderPanel::_onMouseWheel)

    EVT_MENU(	wxID_EXIT,	RenderPanel::_onMenuClose)
END_EVENT_TABLE()

void RenderPanel :: _onSize( wxSizeEvent & event)
{
	wxClientDC l_dc( this);
	m_renderWindow->Resize( GetClientSize().x, GetClientSize().y);
}

void RenderPanel :: _onMove( wxMoveEvent & event)
{
	wxClientDC l_dc( this);
	m_renderWindow->SetToUpdate();
}

void RenderPanel :: _onPaint( wxPaintEvent & WXUNUSED(event))
{
	wxPaintDC l_dc( this);
	m_renderWindow->SetToUpdate();
}

void RenderPanel :: _onClose( wxCloseEvent & event)
{
	if (m_timer != NULL)
	{
		m_timer->Stop();
		delete m_timer;
		m_timer = NULL;
	}

	Destroy();
}

void RenderPanel :: _onTimer( wxTimerEvent & WXUNUSED(event))
{
	wxClientDC l_dc( this);
	m_renderWindow->SetToUpdate();
}

void RenderPanel :: _onEnterWindow( wxMouseEvent & WXUNUSED(event))
{
	m_renderWindow->Focus();
}

void RenderPanel :: _onLeaveWindow( wxMouseEvent & WXUNUSED(event))
{
	m_renderWindow->UnFocus();
}

void RenderPanel :: _onEraseBackground(wxEraseEvent& event)
{
}

void RenderPanel :: _onSetFocus( wxFocusEvent & event)
{
	m_renderWindow->Focus();
}

void RenderPanel :: _onKillFocus( wxFocusEvent & event)
{
	m_renderWindow->UnFocus();
}

void RenderPanel :: _onKeyDown(wxKeyEvent& event)
{
	int l_keyCode = event.GetKeyCode();
	if (l_keyCode == 83)// s
	{
		if (m_renderWindow->GetNormalsMode() == nmFace)
		{
			m_renderWindow->SetNormalsMode( nmSmoothGroups);
		}
		else if (m_renderWindow->GetNormalsMode() == nmSmoothGroups)
		{
			m_renderWindow->SetNormalsMode( nmFace);
		}
	}
	else if (l_keyCode == 82)// s
	{
		m_renderWindow->GetCamera()->ResetPosition();
		m_renderWindow->GetCamera()->ResetOrientation();
		m_renderWindow->GetCamera()->Translate( 0.0f, 0.0f, -5.0f);
	}
	else if (l_keyCode == 78)// n
	{
		m_renderWindow->SetNormalsVisibility( m_renderWindow->IsNormalsVisible());
	}
}

void RenderPanel :: _onKeyUp(wxKeyEvent& event)
{
	switch (event.GetKeyCode())
	{
	case WXK_F5:
		wxGetApp().GetMainFrame()->LoadScene();
		break;
	}
}

void RenderPanel :: _onMouseLDown( wxMouseEvent & event)
{
	m_mouseLeftDown = true;
	m_oldX = event.GetX();
	m_oldY = event.GetY();
}

void RenderPanel :: _onMouseLUp( wxMouseEvent & event)
{
	m_mouseLeftDown = false;
}

void RenderPanel :: _onMouseMDown( wxMouseEvent & event)
{
	m_mouseMiddleDown = true;
	m_oldX = event.GetX();
	m_oldY = event.GetY();
}

void RenderPanel :: _onMouseMUp( wxMouseEvent & event)
{
	m_mouseMiddleDown = false;
}

void RenderPanel :: _onMouseRDown( wxMouseEvent & event)
{
	m_mouseRightDown = true;
	m_oldX = event.GetX();
	m_oldY = event.GetY();
}

void RenderPanel :: _onMouseRUp( wxMouseEvent & event)
{
	m_mouseRightDown = false;
}

void RenderPanel :: _onMouseMove( wxMouseEvent & event)
{
	m_x = event.GetX();
	m_y = event.GetY();
	m_deltaX = (m_oldX - m_x) / 2.0f;
	m_deltaY = (m_oldY - m_y) / 2.0f;
	m_oldX = m_x;
	m_oldY = m_y;

	if (m_mouseLeftDown)
	{
		if (m_renderWindow->GetType() == pt3DView)
		{
			m_renderWindow->GetCamera()->Yaw( m_deltaX);
			m_renderWindow->GetCamera()->Pitch( m_deltaY);
		}
		else
		{
			m_renderWindow->GetCamera()->Roll( m_deltaX);
		}
	}
	else if (m_mouseRightDown)
	{
		m_renderWindow->GetCamera()->Translate( Vector3f( -m_deltaX / 20.0f, m_deltaY / 20.0f, 0.0f));
	}
}

void RenderPanel :: _onMouseWheel( wxMouseEvent & event)
{
	int l_wheelRotation = event.GetWheelRotation();
	const Vector3f * l_cameraPos = m_renderWindow->GetCamera()->GetPosition();

	if (l_wheelRotation < 0)
	{
		m_renderWindow->GetCamera()->Translate( Vector3f( 0.0f, 0.0f, (l_cameraPos->z - 1.0f) / 10));
	}
	else if (l_wheelRotation > 0)
	{
		m_renderWindow->GetCamera()->Translate( Vector3f( 0.0f, 0.0f, (1.0f - l_cameraPos->z) / 10));
	}
}

void RenderPanel :: _onMenuClose( wxCommandEvent & event)
{
	Close( true);
}