#include "CastorViewer/PrecompiledHeader.hpp"

#include "CastorViewer/RenderPanel.hpp"
#include "CastorViewer/CastorViewer.hpp"
#include "CastorViewer/MainFrame.hpp"
#include "CastorViewer/MouseEvent.hpp"

#if defined( __linux )
#	include <GL/glx.h>
#	include <gtk/gtk.h>
#	include <gdk/gdkx.h>
#elif defined( _WIN32 )
#	include <Windows.h>
#endif

#include <Castor3D/WindowHandle.hpp>
#include <algorithm>

#define ID_NEW_WINDOW 10000

using namespace CastorViewer;
using namespace Castor3D;
using namespace Castor;

DECLARE_APP( CastorViewerApp )

RenderPanel :: RenderPanel( wxWindow * parent, wxWindowID p_id, wxPoint const & pos, wxSize const & size, long style )
	:	wxPanel				( parent, p_id, pos, size, style	)
	,	m_mouseLeftDown		( false								)
	,	m_mouseRightDown	( false								)
	,	m_mouseMiddleDown	( false								)
	,	m_rZoom				( 1.0								)
	,	m_deltaX			( 0.0								)
	,	m_deltaY			( 0.0								)
	,	m_x					( 0.0								)
	,	m_y					( 0.0								)
	,	m_oldX				( 0.0								)
	,	m_oldY				( 0.0								)
	,	m_eCameraMode		( eCAMERA_MODE_FIXED				)
	,	m_rFpCamSpeed		( 2.0								)
{
	for( int i = 0; i < eTIMER_ID_COUNT; i++ )
	{
		m_pTimer[i] = new wxTimer( this, i );
	}
	
	m_pCursorArrow = new wxCursor( wxCURSOR_ARROW );
	m_pCursorHand = new wxCursor( wxCURSOR_HAND );
	m_pCursorNone = new wxCursor( wxCURSOR_BLANK );
}

RenderPanel :: ~RenderPanel()
{
	delete m_pCursorArrow;
	delete m_pCursorHand;
	delete m_pCursorNone;

	for( int i = 0; i < eTIMER_ID_COUNT; i++ )
	{
		delete m_pTimer[i];
		m_pTimer[i] = NULL;
	}
}

void RenderPanel :: Focus()
{
}

void RenderPanel :: UnFocus()
{
}

void RenderPanel :: DrawOneFrame()
{
	wxClientDC l_dc( this );
}

void RenderPanel :: SetRenderWindow( Castor3D::RenderWindowSPtr p_pWindow )
{
	m_pRenderWindow.reset();

	if( p_pWindow )
	{
#if defined( _WIN32 )
		WindowHandle l_handle( std::make_shared< IMswWindowHandle >( this->GetHandle() ) );
#elif defined( __linux__ )
		GtkWidget * l_pGtkWidget = static_cast< GtkWidget * >( this->GetHandle() );
		GLXDrawable l_drawable = None;
		Display * l_pDisplay = NULL;

		if( l_pGtkWidget && l_pGtkWidget->window )
		{
			l_drawable = GDK_WINDOW_XID( l_pGtkWidget->window );

			GdkDisplay * l_pGtkDisplay = gtk_widget_get_display( l_pGtkWidget );

			if( l_pGtkDisplay )
			{
				l_pDisplay = gdk_x11_display_get_xdisplay( l_pGtkDisplay );
			}
		}

		WindowHandle l_handle( std::make_shared< IXWindowHandle >( l_drawable, l_pDisplay ) );
#else
#	error "Yet unsupported OS"
#endif
		if( p_pWindow->Initialise( l_handle ) )
		{
			Castor::Size l_sizeScreen;
			Castor::Size l_sizeWnd = p_pWindow->GetSize();
			Castor::GetScreenSize( l_sizeScreen );

			GetParent()->SetClientSize( l_sizeWnd.width(), l_sizeWnd.height() );
			GetParent()->SetPosition( wxPoint( std::max< int >( 0, (l_sizeScreen.width() - l_sizeWnd.width()) / 2 ), std::max< int >( 0, (l_sizeScreen.height() - l_sizeWnd.height()) / 2 ) ) );
			m_pListener = p_pWindow->GetListener();
			SceneSPtr l_pScene = p_pWindow->GetScene();
			wxDisplay l_wxDisplay;
			wxRect l_rect( 0, 0, l_sizeScreen.width(), l_sizeScreen.height() );

			if( l_pScene )
			{
				m_pRotateCamEvent		= std::make_shared< CameraRotateEvent		>( l_pScene->GetObjectRootNode(),	real( 0 ), real( 0 ), real( 0 ) );
				m_pTranslateCamEvent	= std::make_shared< CameraTranslateEvent	>( l_pScene->GetCameraRootNode(),	real( 0 ), real( 0 ), real( 0 ) );

				SceneNodeSPtr l_pCamBaseNode		= l_pScene->CreateSceneNode( cuT( "CastorViewer_CamNode"	), l_pScene->GetCameraRootNode()	);
				l_pCamBaseNode->SetPosition( Point3r( 0, 0, -100 ) );
				SceneNodeSPtr l_pCamYawNode		= l_pScene->CreateSceneNode( cuT( "CastorViewer_CamYawNode"		), l_pCamBaseNode					);
				SceneNodeSPtr l_pCamPitchNode	= l_pScene->CreateSceneNode( cuT( "CastorViewer_CamPitchNode"	), l_pCamYawNode					);
				SceneNodeSPtr l_pCamRollNode	= l_pScene->CreateSceneNode( cuT( "CastorViewer_CamRollNode"	), l_pCamPitchNode					);
				CameraSPtr l_pCamera = l_pScene->CreateCamera( cuT( "CastorViewer_Camera" ), l_rect.width, l_rect.height, l_pCamRollNode, eVIEWPORT_TYPE_3D );

				p_pWindow->SetCamera( l_pCamera );

				if( p_pWindow->GetCamera() )
				{
					m_pFpRotateCamEvent		= std::make_shared< FPCameraRotateEvent		>( l_pCamRollNode,	real( 0 ), real( 0 ) );
					m_pFpTranslateCamEvent	= std::make_shared< FPCameraTranslateEvent	>( l_pCamRollNode,	real( 0 ), real( 0 ), real( 0 ) );
					m_ptOriginalPosition = l_pCamRollNode->GetPosition();
					m_qOriginalOrientation = l_pCamRollNode->GetOrientation();
				}

				m_pRenderWindow = p_pWindow;
				m_pKeyboardEvent = std::make_shared< KeyboardEvent >( p_pWindow, wxGetApp().GetMainFrame() );
			}
		}
	}
}

void RenderPanel :: DoStartTimer( int p_iId )
{
	m_pTimer[p_iId]->Start( 10 );
}

void RenderPanel :: DoStopTimer( int p_iId )
{
	if( p_iId != eTIMER_ID_COUNT )
	{
		m_pTimer[p_iId]->Stop();
	}
	else
	{
		for( int i = 0; i < eTIMER_ID_COUNT; i++ )
		{
			m_pTimer[i]->Stop();
		}
	}
}

BEGIN_EVENT_TABLE( RenderPanel, wxPanel )
	EVT_TIMER( eTIMER_ID_FORWARD,	RenderPanel::OnTimerFwd			)
	EVT_TIMER( eTIMER_ID_BACK,		RenderPanel::OnTimerBck			)
	EVT_TIMER( eTIMER_ID_LEFT,		RenderPanel::OnTimerLft			)
	EVT_TIMER( eTIMER_ID_RIGHT,		RenderPanel::OnTimerRgt			)
	EVT_TIMER( eTIMER_ID_UP,		RenderPanel::OnTimerUp			)
	EVT_TIMER( eTIMER_ID_DOWN,		RenderPanel::OnTimerDwn			)
	EVT_SIZE(						RenderPanel::OnSize				)
	EVT_MOVE(						RenderPanel::OnMove				)
	EVT_PAINT(						RenderPanel::OnPaint			)
	EVT_ENTER_WINDOW(				RenderPanel::OnEnterWindow		)
	EVT_LEAVE_WINDOW(				RenderPanel::OnLeaveWindow		)
	EVT_ERASE_BACKGROUND(			RenderPanel::OnEraseBackground	)
	EVT_SET_FOCUS(					RenderPanel::OnSetFocus			)
	EVT_KILL_FOCUS(					RenderPanel::OnKillFocus		)
	EVT_KEY_DOWN(					RenderPanel::OnKeyDown			)
	EVT_KEY_UP(						RenderPanel::OnKeyUp			)
	EVT_LEFT_DCLICK(				RenderPanel::OnMouseLDClick		)
	EVT_LEFT_DOWN(					RenderPanel::OnMouseLDown		)
	EVT_LEFT_UP(					RenderPanel::OnMouseLUp			)
	EVT_MIDDLE_DOWN(				RenderPanel::OnMouseMDown		)
	EVT_MIDDLE_UP(					RenderPanel::OnMouseMUp			)
	EVT_RIGHT_DOWN(					RenderPanel::OnMouseRDown		)
	EVT_RIGHT_UP(					RenderPanel::OnMouseRUp			)
	EVT_MOTION(						RenderPanel::OnMouseMove		)
	EVT_MOUSEWHEEL(					RenderPanel::OnMouseWheel		)
	EVT_MENU(	wxID_EXIT,			RenderPanel::OnMenuClose		)
END_EVENT_TABLE()

void RenderPanel :: OnTimerFwd( wxTimerEvent & p_event )
{
	MouseCameraEvent::Add( m_pFpTranslateCamEvent, m_pListener, real( 0 ), real( 0 ), m_rFpCamSpeed );
	p_event.Skip();
}

void RenderPanel :: OnTimerBck( wxTimerEvent & p_event )
{
	MouseCameraEvent::Add( m_pFpTranslateCamEvent, m_pListener, real( 0 ), real( 0 ), -m_rFpCamSpeed );
	p_event.Skip();
}

void RenderPanel :: OnTimerLft( wxTimerEvent & p_event )
{
	MouseCameraEvent::Add( m_pFpTranslateCamEvent, m_pListener, m_rFpCamSpeed, real( 0 ), real( 0 ) );
	p_event.Skip();
}

void RenderPanel :: OnTimerRgt( wxTimerEvent & p_event )
{
	MouseCameraEvent::Add( m_pFpTranslateCamEvent, m_pListener, -m_rFpCamSpeed, real( 0 ), real( 0 ) );
	p_event.Skip();
}

void RenderPanel :: OnTimerUp( wxTimerEvent & p_event )
{
	MouseCameraEvent::Add( m_pFpTranslateCamEvent, m_pListener, real( 0 ), -m_rFpCamSpeed, real( 0 ) );
	p_event.Skip();
}

void RenderPanel :: OnTimerDwn( wxTimerEvent & p_event )
{
	MouseCameraEvent::Add( m_pFpTranslateCamEvent, m_pListener, real( 0 ), m_rFpCamSpeed, real( 0 ) );
	p_event.Skip();
}

void RenderPanel :: OnSize( wxSizeEvent & p_event )
{
	wxClientDC l_dc( this );

	if( m_pRenderWindow.lock() )
	{
		m_pRenderWindow.lock()->Resize( p_event.GetSize().x, p_event.GetSize().y );
	}
	else
	{
		l_dc.SetBrush( wxBrush( *wxWHITE ) );
		l_dc.SetPen( wxPen( *wxWHITE ) );
		l_dc.DrawRectangle( 0, 0, p_event.GetSize().x, p_event.GetSize().y );
	}

	p_event.Skip();
}

void RenderPanel :: OnMove( wxMoveEvent & p_event )
{
	wxClientDC l_dc( this );

	if( !m_pRenderWindow.lock() )
	{
		l_dc.SetBrush( wxBrush( *wxWHITE ) );
		l_dc.SetPen( wxPen( *wxWHITE ) );
		l_dc.DrawRectangle( 0, 0, GetClientSize().x, GetClientSize().y );
	}

	p_event.Skip();
}

void RenderPanel :: OnPaint( wxPaintEvent & p_event )
{
	wxPaintDC l_dc( this );

	if( !m_pRenderWindow.lock() )
	{
		l_dc.SetBrush( wxBrush( *wxWHITE ) );
		l_dc.SetPen( wxPen( *wxWHITE ) );
		l_dc.DrawRectangle( 0, 0, GetClientSize().x, GetClientSize().y );
	}

	p_event.Skip();
}

void RenderPanel :: OnEnterWindow( wxMouseEvent & p_event )
{
	p_event.Skip();
}

void RenderPanel :: OnLeaveWindow( wxMouseEvent & p_event )
{
	p_event.Skip();
}

void RenderPanel :: OnEraseBackground(wxEraseEvent& p_event )
{
	p_event.Skip();
}

void RenderPanel :: OnSetFocus( wxFocusEvent & p_event )
{
	p_event.Skip();
}

void RenderPanel :: OnKillFocus( wxFocusEvent & p_event )
{
	DoStopTimer( eTIMER_ID_COUNT );
	p_event.Skip();
}

void RenderPanel :: OnKeyDown( wxKeyEvent & p_event )
{
	RenderWindowSPtr l_pWindow = m_pRenderWindow.lock();

	switch( p_event.GetKeyCode() )
	{
	case WXK_LEFT:
		DoStartTimer( eTIMER_ID_LEFT );
		break;

	case WXK_RIGHT:
		DoStartTimer( eTIMER_ID_RIGHT );
		break;

	case WXK_UP:
		DoStartTimer( eTIMER_ID_FORWARD );
		break;

	case WXK_DOWN:
		DoStartTimer( eTIMER_ID_BACK );
		break;

	case WXK_PAGEUP:
		DoStartTimer( eTIMER_ID_UP );
		break;

	case WXK_PAGEDOWN:
		DoStartTimer( eTIMER_ID_DOWN );
		break;
	}

	p_event.Skip();
}

void RenderPanel :: OnKeyUp( wxKeyEvent & p_event )
{
	RenderWindowSPtr l_pWindow = m_pRenderWindow.lock();

	switch( p_event.GetKeyCode() )
	{
	case 'C':
		DoStopTimer( eTIMER_ID_COUNT );
		m_eCameraMode = eCAMERA_MODE( (eCAMERA_MODE_COUNT-1) - int( m_eCameraMode ) );
		if( m_eCameraMode == eCAMERA_MODE_MOBILE )
		{
			this->SetCursor( *m_pCursorNone );
		}
		else
		{
			this->SetCursor( *m_pCursorArrow );
		}
		break;

	case 'R':
		DoStopTimer( eTIMER_ID_COUNT );
		l_pWindow->GetScene()->GetObjectRootNode()->SetPosition( 0, 0, 0 );
		l_pWindow->GetScene()->GetObjectRootNode()->SetOrientation( m_qOriginalOrientation );
		l_pWindow->GetCamera()->GetParent()->SetPosition( m_ptOriginalPosition );
		m_rZoom		= 1.0;
		m_deltaX	= 0.0;
		m_deltaY	= 0.0;
		m_x			= 0.0;
		m_y			= 0.0;
		m_oldX		= 0.0;
		m_oldY		= 0.0;
		break;

	case 'W':
		DoStopTimer( eTIMER_ID_COUNT );
		switch( l_pWindow->GetPrimitiveType() )
		{
		case eTOPOLOGY_TRIANGLES:
			l_pWindow->SetPrimitiveType( eTOPOLOGY_LINES );
			break;

		case eTOPOLOGY_LINES:
			l_pWindow->SetPrimitiveType( eTOPOLOGY_POINTS );
			break;

		case eTOPOLOGY_POINTS:
			l_pWindow->SetPrimitiveType( eTOPOLOGY_TRIANGLES );
			break;
		}
		break;

	case WXK_F5:
		DoStopTimer( eTIMER_ID_COUNT );
		wxGetApp().GetMainFrame()->LoadScene();
		m_rZoom		= 1.0;
		m_deltaX	= 0.0;
		m_deltaY	= 0.0;
		m_x			= 0.0;
		m_y			= 0.0;
		m_oldX		= 0.0;
		m_oldY		= 0.0;
		break;

	case WXK_LEFT:
		if( m_eCameraMode == eCAMERA_MODE_FIXED )
		{
			MouseCameraEvent::Add( m_pRotateCamEvent, m_pListener, 90.0, 0, 0 );
		}
		else
		{
			DoStopTimer( eTIMER_ID_LEFT );
		}
		break;

	case WXK_RIGHT:
		if( m_eCameraMode == eCAMERA_MODE_FIXED )
		{
			MouseCameraEvent::Add( m_pRotateCamEvent, m_pListener, -90.0, 0, 0 );
		}
		else
		{
			DoStopTimer( eTIMER_ID_RIGHT );
		}
		break;

	case WXK_UP:
		if( m_eCameraMode == eCAMERA_MODE_MOBILE )
		{
			DoStopTimer( eTIMER_ID_FORWARD );
		}
		break;

	case WXK_DOWN:
		if( m_eCameraMode == eCAMERA_MODE_MOBILE )
		{
			DoStopTimer( eTIMER_ID_BACK );
		}
		break;

	case WXK_PAGEUP:
		if( m_eCameraMode == eCAMERA_MODE_MOBILE )
		{
			DoStopTimer( eTIMER_ID_UP );
		}
		break;

	case WXK_PAGEDOWN:
		if( m_eCameraMode == eCAMERA_MODE_MOBILE )
		{
			DoStopTimer( eTIMER_ID_DOWN );
		}
		break;
	}

	p_event.Skip();
}

void RenderPanel :: OnMouseLDClick( wxMouseEvent & p_event )
{
	m_pListener->PostEvent( m_pKeyboardEvent );
	p_event.Skip();
}

void RenderPanel :: OnMouseLDown( wxMouseEvent & p_event )
{
	m_mouseLeftDown = true;
	m_oldX = real( p_event.GetX() );
	m_oldY = real( p_event.GetY() );
	p_event.Skip();
}

void RenderPanel :: OnMouseLUp( wxMouseEvent & p_event )
{
	m_mouseLeftDown = false;
	p_event.Skip();
}

void RenderPanel :: OnMouseMDown( wxMouseEvent & p_event )
{
	m_mouseMiddleDown = true;
	m_oldX = real( p_event.GetX() );
	m_oldY = real( p_event.GetY() );
	p_event.Skip();
}

void RenderPanel :: OnMouseMUp( wxMouseEvent & p_event )
{
	m_mouseMiddleDown = false;
	p_event.Skip();
}

void RenderPanel :: OnMouseRDown( wxMouseEvent & p_event )
{
	m_mouseRightDown = true;
	m_oldX = real( p_event.GetX() );
	m_oldY = real( p_event.GetY() );
	p_event.Skip();
}

void RenderPanel :: OnMouseRUp( wxMouseEvent & p_event )
{
	m_mouseRightDown = false;
	p_event.Skip();
}

void RenderPanel :: OnMouseMove( wxMouseEvent & p_event )
{
	RenderWindowSPtr l_pWindow = m_pRenderWindow.lock();
	m_x = real( p_event.GetX() );
	m_y = real( p_event.GetY() );
	m_deltaX = (m_oldX - m_x) / 2.0f;
	m_deltaY = (m_oldY - m_y) / 2.0f;

	if( l_pWindow )
	{
		if( m_eCameraMode == eCAMERA_MODE_FIXED )
		{
			if( m_mouseLeftDown )
			{
				if( l_pWindow->GetViewportType() == eVIEWPORT_TYPE_3D )
				{
					MouseCameraEvent::Add( m_pRotateCamEvent, m_pListener, m_deltaX, 0, 0 );
				}
				else
				{
					MouseCameraEvent::Add( m_pRotateCamEvent, m_pListener, 0, 0, m_deltaX );
				}
			}
			else if( m_mouseMiddleDown )
			{
				MouseCameraEvent::Add( m_pRotateCamEvent, m_pListener, 0, m_deltaX, 0 );
			}
			else if( m_mouseRightDown )
			{
				MouseCameraEvent::Add( m_pTranslateCamEvent, m_pListener, -m_rZoom * m_deltaX / real( 40 ), m_rZoom * m_deltaY / real( 40 ), 0 );
			}
			
			p_event.Skip();
		}
		else
		{
			Quaternion l_qCamera = l_pWindow->GetCamera()->GetParent()->GetOrientation();
			MouseCameraEvent::Add( m_pFpRotateCamEvent, m_pListener, m_deltaX, m_deltaY, 0 );
			WarpPointer( int( m_oldX ), int( m_oldY ) );
			m_x = m_oldX;
			m_y = m_oldY;
			p_event.Skip( false );
		}
	}
	else
	{
		p_event.Skip();
	}

	m_oldX = m_x;
	m_oldY = m_y;
}

void RenderPanel :: OnMouseWheel( wxMouseEvent & p_event )
{
	RenderWindowSPtr l_pWindow = m_pRenderWindow.lock();
	
	if( m_eCameraMode == eCAMERA_MODE_FIXED )
	{
		if( l_pWindow )
		{
			int l_wheelRotation = p_event.GetWheelRotation();
			Point3r const & l_cameraPos = l_pWindow->GetCamera()->GetParent()->GetPosition();

			if( l_wheelRotation < 0 )
			{
				MouseCameraEvent::Add( m_pTranslateCamEvent, m_pListener, 0, 0, (l_cameraPos[2] - real( 1 )) / 10 );
				m_rZoom /= real( 0.9 );
			}
			else if( l_wheelRotation > 0 )
			{
				MouseCameraEvent::Add( m_pTranslateCamEvent, m_pListener, 0, 0, (real( 1 ) - l_cameraPos[2]) / 10 );
				m_rZoom *= real( 0.9 );
			}

			if( m_rZoom <= 1.0 )
			{
				m_rZoom = real( 1.0 );
			}
		}
	}
	else if( m_eCameraMode == eCAMERA_MODE_MOBILE )
	{
		int l_wheelRotation = p_event.GetWheelRotation();

		if( l_wheelRotation < 0 )
		{
			m_rFpCamSpeed *= real( 0.9 );
		}
		else
		{
			m_rFpCamSpeed /= real( 0.9 );
		}
	}

	p_event.Skip();
}

void RenderPanel :: OnMenuClose( wxCommandEvent & p_event )
{
	Close( true );
	p_event.Skip();
}
