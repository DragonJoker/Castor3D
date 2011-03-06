#include "CastorViewer/PrecompiledHeader.h"

#include "CastorViewer/RenderPanel.h"
#include "CastorViewer/CastorViewer.h"
#include "CastorViewer/MainFrame.h"
#include "CastorViewer/MouseEvent.h"

#define ID_NEW_WINDOW 10000

using namespace CastorViewer;
using namespace Castor3D;

DECLARE_APP( CastorViewerApp)

RenderPanel :: RenderPanel( wxWindow * parent, wxWindowID p_id,
							    Viewport::eTYPE p_renderType, ScenePtr p_scene,
								const wxPoint & pos, const wxSize & size,
								ePROJECTION_DIRECTION p_look, long style)
	:	wxPanel( parent, p_id, pos, size, style),
		m_timer( NULL),
		m_renderType( p_renderType),
		m_lookAt( p_look),
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
	if ( ! m_renderWindow.expired())
	{
		m_renderWindow.lock()->Focus();
	}
}

void RenderPanel :: UnFocus()
{
	if ( ! m_renderWindow.expired())
	{
		m_renderWindow.lock()->UnFocus();
	}
}

void RenderPanel :: DrawOneFrame()
{
	wxClientDC l_dc( this);

	if ( ! m_renderWindow.expired())
	{
		m_renderWindow.lock()->SetToUpdate();
	}
}

void RenderPanel :: _initialiseRenderWindow()
{
	Logger::LogMessage( CU_T( "Initialising RenderWindow"));
	m_renderWindow = Root::GetSingletonPtr()->CreateRenderWindow( m_mainScene, (void *)GetHandle(), GetClientSize().x, GetClientSize().y,
																  m_renderType, eA8R8G8B8, m_lookAt);
	m_listener = m_renderWindow.lock()->GetListener();
	m_pRotateCamEvent = shared_ptr<CameraRotateEvent>( new CameraRotateEvent( m_renderWindow.lock()->GetCamera(), 0, 0, 0));
	m_pTranslateCamEvent = shared_ptr<CameraTranslateEvent>( new CameraTranslateEvent( m_renderWindow.lock()->GetCamera(), 0, 0, 0));

	if (m_timer == NULL)
	{
		m_timer = new wxTimer( this, 1);
		m_timer->Start( 40);
	}

	Logger::LogMessage( CU_T( "RenderWindow Initialised"));
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

	if ( ! m_renderWindow.expired())
	{
		m_renderWindow.lock()->Resize( GetClientSize().x, GetClientSize().y);
	}
}

void RenderPanel :: _onMove( wxMoveEvent & event)
{
	wxClientDC l_dc( this);

	if ( ! m_renderWindow.expired())
	{
		m_renderWindow.lock()->SetToUpdate();
	}
}

void RenderPanel :: _onPaint( wxPaintEvent & WXUNUSED(event))
{
	wxPaintDC l_dc( this);

	if ( ! m_renderWindow.expired())
	{
		m_renderWindow.lock()->SetToUpdate();
	}
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
	if ( ! m_renderWindow.expired())
	{
		m_renderWindow.lock()->SetToUpdate();
	}
}

void RenderPanel :: _onEnterWindow( wxMouseEvent & WXUNUSED(event))
{
	if ( ! m_renderWindow.expired())
	{
		m_renderWindow.lock()->Focus();
	}
}

void RenderPanel :: _onLeaveWindow( wxMouseEvent & WXUNUSED(event))
{
	if ( ! m_renderWindow.expired())
	{
		m_renderWindow.lock()->UnFocus();
	}
}

void RenderPanel :: _onEraseBackground(wxEraseEvent& event)
{
}

void RenderPanel :: _onSetFocus( wxFocusEvent & event)
{
	if ( ! m_renderWindow.expired())
	{
		m_renderWindow.lock()->Focus();
	}
}

void RenderPanel :: _onKillFocus( wxFocusEvent & event)
{
	if ( ! m_renderWindow.expired())
	{
		m_renderWindow.lock()->UnFocus();
	}
}

void RenderPanel :: _onKeyDown(wxKeyEvent& event)
{
	int l_keyCode = event.GetKeyCode();

	if ( ! m_renderWindow.expired())
	{
		RenderWindowPtr l_pWindow = m_renderWindow.lock();

		if (l_keyCode == 78)// n
		{
			l_pWindow->SetNormalsVisibility( l_pWindow->IsNormalsVisible());
		}
		else if (l_keyCode == 82)// r
		{
			l_pWindow->GetCamera()->ResetPosition();
			l_pWindow->GetCamera()->ResetOrientation();
			l_pWindow->GetCamera()->GetParent()->Translate( 0.0f, 0.0f, -5.0f);
		}
		else if (l_keyCode == 83)// s
		{
			l_pWindow->SetNormalsMode( eNORMALS_MODE( eSmooth - l_pWindow->GetNormalsMode()));
		}
		else if (l_keyCode == 87)// w
		{
			switch (l_pWindow->GetDrawType())
			{
			case eTriangles:
				l_pWindow->SetDrawType( eLines);
				break;

			case eLines:
				l_pWindow->SetDrawType( ePoints);
				break;

			case ePoints:
				l_pWindow->SetDrawType( eTriangles);
				break;
			}
		}
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

	if ( ! m_renderWindow.expired())
	{
		if (m_mouseLeftDown)
		{
			if (m_renderWindow.lock()->GetType() == Viewport::e3DView)
			{
				MouseCameraEvent::Add( m_pRotateCamEvent, m_listener, m_deltaX * Math::Angle::DegreesToRadians, m_deltaY * Math::Angle::DegreesToRadians, 0);
			}
			else
			{
				MouseCameraEvent::Add( m_pRotateCamEvent, m_listener, 0, 0, m_deltaX * Math::Angle::DegreesToRadians);
			}
		}
		else if (m_mouseRightDown)
		{
			MouseCameraEvent::Add( m_pTranslateCamEvent, m_listener, -m_deltaX / real( 40), m_deltaY / real( 40), 0);
		}
	}
}

void RenderPanel :: _onMouseWheel( wxMouseEvent & event)
{
	int l_wheelRotation = event.GetWheelRotation();
	if ( ! m_renderWindow.expired())
	{
		const Point3r & l_cameraPos = m_renderWindow.lock()->GetCamera()->GetParent()->GetPosition();

		if (l_wheelRotation < 0)
		{
			MouseCameraEvent::Add( m_pTranslateCamEvent, m_listener, 0, 0, (l_cameraPos[2] - real( 1.0)) / 10);
		}
		else if (l_wheelRotation > 0)
		{
			MouseCameraEvent::Add( m_pTranslateCamEvent, m_listener, 0, 0, (real( 1.0) - l_cameraPos[2]) / 10);
		}
	}
}

void RenderPanel :: _onMenuClose( wxCommandEvent & event)
{
	Close( true);
}
