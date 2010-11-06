#include "PrecompiledHeaders.h"

#include "main/RenderPanel.h"
#include "main/MainFrame.h"
#include "main/FrameListener.h"

#define ID_NEW_WINDOW 10000

extern CSMainFrame * g_mainFrame;

using namespace Castor3D;

BEGIN_EVENT_TABLE( CSRenderPanel, wxPanel)
	EVT_TIMER(	csTimer,	CSRenderPanel::_onTimer)

	EVT_SIZE(				CSRenderPanel::_onSize)
	EVT_MOVE(				CSRenderPanel::_onMove)
	EVT_CLOSE(				CSRenderPanel::_onClose)
	EVT_PAINT(				CSRenderPanel::_onPaint)
	EVT_ENTER_WINDOW(		CSRenderPanel::_onEnterWindow)
	EVT_LEAVE_WINDOW(		CSRenderPanel::_onLeaveWindow)
	EVT_ERASE_BACKGROUND(	CSRenderPanel::_onEraseBackground)
	
	EVT_SET_FOCUS(			CSRenderPanel::_onSetFocus)
	EVT_KILL_FOCUS(			CSRenderPanel::_onKillFocus)

	EVT_KEY_DOWN(			CSRenderPanel::_onKeyDown)
	EVT_KEY_UP(				CSRenderPanel::_onKeyUp)
	EVT_LEFT_DOWN(			CSRenderPanel::_onMouseLDown)
	EVT_LEFT_UP(			CSRenderPanel::_onMouseLUp)
	EVT_MIDDLE_DOWN(		CSRenderPanel::_onMouseMDown)
	EVT_MIDDLE_UP(			CSRenderPanel::_onMouseMUp)
	EVT_RIGHT_DOWN(			CSRenderPanel::_onMouseRDown)
	EVT_RIGHT_UP(			CSRenderPanel::_onMouseRUp)
	EVT_MOTION(				CSRenderPanel::_onMouseMove)
	EVT_MOUSEWHEEL(			CSRenderPanel::_onMouseWheel)

    EVT_MENU(	wxID_EXIT,	CSRenderPanel::_onMenuClose)
END_EVENT_TABLE()

CSRenderPanel :: CSRenderPanel( wxWindow * parent, wxWindowID p_id,
							    Viewport::eTYPE p_renderType, ScenePtr p_scene,
								const wxPoint & pos, const wxSize & size,
								ProjectionDirection p_look, long style)
	:	wxPanel( parent, p_id, pos, size, style),
		m_actionType( atNone),
		m_selectionType( stNone),
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

CSRenderPanel :: ~CSRenderPanel()
{
	if (m_timer != NULL)
	{
		m_timer->Stop();
		delete m_timer;
		m_timer = NULL;
	}
}

void CSRenderPanel :: Focus()
{
	m_renderWindow->Focus();
}

void CSRenderPanel :: UnFocus()
{
	m_renderWindow->UnFocus();
}

void CSRenderPanel :: DrawOneFrame()
{
	wxClientDC l_dc( this);
	m_renderWindow->SetToUpdate();
}

void CSRenderPanel :: SelectGeometry( GeometryPtr p_geometry)
{
	g_mainFrame->SelectGeometry( p_geometry);
}

void CSRenderPanel :: SelectVertex( Point3rPtr p_vertex)
{
	g_mainFrame->SelectVertex( p_vertex);
}

void CSRenderPanel :: _initialiseRenderWindow()
{
	Log::LogMessage( CU_T( "Initialising RenderWindow"));
	m_renderWindow = Root::GetSingletonPtr()->CreateRenderWindow( m_mainScene,
																  (void *)GetHandle(),
																  GetClientSize().x,
																  GetClientSize().y,
																  m_renderType,
																  m_lookAt);
	m_listener = m_renderWindow->GetListener();

	if (m_timer == NULL)
	{
		m_timer = new wxTimer( this, csTimer);
		m_timer->Start( 40);
	}

	Log::LogMessage( "RenderWindow Initialised");
}

void CSRenderPanel :: _onSize( wxSizeEvent & event)
{
	wxClientDC l_dc( this);
	m_renderWindow->Resize( GetClientSize().x, GetClientSize().y);
}

void CSRenderPanel :: _onMove( wxMoveEvent & event)
{
	wxClientDC l_dc( this);
	m_renderWindow->SetToUpdate();
}

void CSRenderPanel :: _onPaint( wxPaintEvent & WXUNUSED(event))
{
	wxPaintDC l_dc( this);
	m_renderWindow->SetToUpdate();
}

void CSRenderPanel :: _onClose( wxCloseEvent & event)
{
	if (m_timer != NULL)
	{
		m_timer->Stop();
		delete m_timer;
		m_timer = NULL;
	}

	Destroy();
}

void CSRenderPanel :: _onTimer( wxTimerEvent & WXUNUSED(event))
{
	wxClientDC l_dc( this);
	m_renderWindow->SetToUpdate();
}

void CSRenderPanel :: _onEnterWindow( wxMouseEvent & WXUNUSED(event))
{
	m_renderWindow->Focus();
}

void CSRenderPanel :: _onLeaveWindow( wxMouseEvent & WXUNUSED(event))
{
	m_renderWindow->UnFocus();
}

void CSRenderPanel :: _onEraseBackground(wxEraseEvent& event)
{
}

void CSRenderPanel :: _onSetFocus( wxFocusEvent & event)
{
	m_renderWindow->Focus();
}

void CSRenderPanel :: _onKillFocus( wxFocusEvent & event)
{
	m_renderWindow->UnFocus();
}

void CSRenderPanel :: _onKeyDown(wxKeyEvent& event)
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

void CSRenderPanel :: _onKeyUp(wxKeyEvent& event)
{
}

void CSRenderPanel :: _onMouseLDown( wxMouseEvent & event)
{
	m_mouseLeftDown = true;
	m_oldX = event.GetX();
	m_oldY = event.GetY();
}

void CSRenderPanel :: _onMouseLUp( wxMouseEvent & event)
{
	m_mouseLeftDown = false;
	if (m_actionType == atSelect)
	{
		if (m_selectionType == stGeometry)
		{
			_selectGeometry( event.GetX(), event.GetY());
		}
		else if (m_selectionType == stVertex)
		{
			_selectVertex( event.GetX(), event.GetY());
		}
		else if (m_selectionType == stAll)
		{
			_selectAll();
		}
		else if (m_selectionType == stClone)
		{
			_cloneSelected();
		}
	}
	else if (m_actionType == atModify)
	{
	}
}

void CSRenderPanel :: _onMouseMDown( wxMouseEvent & event)
{
	m_mouseMiddleDown = true;
	m_oldX = event.GetX();
	m_oldY = event.GetY();
}

void CSRenderPanel :: _onMouseMUp( wxMouseEvent & event)
{
	m_mouseMiddleDown = false;
}

void CSRenderPanel :: _onMouseRDown( wxMouseEvent & event)
{
	m_mouseRightDown = true;
	m_oldX = event.GetX();
	m_oldY = event.GetY();
}

void CSRenderPanel :: _onMouseRUp( wxMouseEvent & event)
{
	m_mouseRightDown = false;
}

void CSRenderPanel :: _onMouseMove( wxMouseEvent & event)
{
	m_x = event.GetX();
	m_y = event.GetY();
	m_deltaX = m_oldX - m_x;
	m_deltaY = m_oldY - m_y;
	m_oldX = m_x;
	m_oldY = m_y;
	if (m_mouseLeftDown)
	{
		if (m_renderWindow->GetType() == Viewport::pt3DView)
		{
			m_renderWindow->GetCamera()->Yaw( m_deltaX * Angle::DegreesToRadians);
			m_renderWindow->GetCamera()->Pitch( m_deltaY * Angle::DegreesToRadians);
		}
		else
		{
			m_renderWindow->GetCamera()->Roll( m_deltaX * Angle::DegreesToRadians);
		}
	}
	else if (m_mouseRightDown)
	{
		m_renderWindow->GetCamera()->Translate( Point3r( -m_deltaX / 20.0f, m_deltaY / 20.0f, 0.0f));
	}
}

void CSRenderPanel :: _onMouseWheel( wxMouseEvent & event)
{
	int l_wheelRotation = event.GetWheelRotation();
	const Point3r & l_cameraPos = m_renderWindow->GetCamera()->GetPosition();

	if (l_wheelRotation < 0)
	{
		m_renderWindow->GetCamera()->Translate( Point3r( 0.0f, 0.0f, (l_cameraPos[2] - 1.0f) / 10));
	}
	else if (l_wheelRotation > 0)
	{
		m_renderWindow->GetCamera()->Translate( Point3r( 0.0f, 0.0f, (1.0f - l_cameraPos[2]) / 10));
	}
}

void CSRenderPanel :: _onMenuClose( wxCommandEvent & event)
{
	Close( true);
}

void CSRenderPanel :: _selectGeometry( int p_x, int p_y)
{
	Geometry l_geo( MeshPtr(), NULL, C3DEmptyString);
	m_listener->PostEvent( new CSSelectObjectFrameEvent( m_mainScene, & l_geo, NULL, NULL, NULL, m_renderWindow->GetCamera(), this, p_x, p_y));
}

void CSRenderPanel :: _selectVertex( int p_x, int p_y)
{
	Point3r l_vertex;
	m_listener->PostEvent( new CSSelectObjectFrameEvent( m_mainScene, NULL, NULL, NULL, & l_vertex, m_renderWindow->GetCamera(), this, p_x, p_y));
}

void CSRenderPanel :: _selectAll()
{
}

void CSRenderPanel :: _cloneSelected()
{
}