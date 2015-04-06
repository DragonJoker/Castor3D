#include "RenderPanel.hpp"
#include "CastorViewer.hpp"
#include "MainFrame.hpp"
#include "CameraRotateEvent.hpp"
#include "CameraTranslateEvent.hpp"
#include "KeyboardEvent.hpp"

#include <algorithm>

#include <wx/display.h>

#include <Camera.hpp>
#include <RenderWindow.hpp>
#include <Scene.hpp>
#include <SceneNode.hpp>
#include <WindowHandle.hpp>
#include <Utils.hpp>

#define ID_NEW_WINDOW 10000

using namespace Castor3D;
using namespace Castor;

DECLARE_APP( CastorViewer::CastorViewerApp )

namespace CastorViewer
{
	RenderPanel::RenderPanel( wxWindow * parent, wxWindowID p_id, wxPoint const & pos, wxSize const & size, long style )
		: wxPanel( parent, p_id, pos, size, style )
		, m_mouseLeftDown( false )
		, m_mouseRightDown( false )
		, m_mouseMiddleDown( false )
		, m_rZoom( 1.0 )
		, m_deltaX( 0.0 )
		, m_deltaY( 0.0 )
		, m_x( 0.0 )
		, m_y( 0.0 )
		, m_oldX( 0.0 )
		, m_oldY( 0.0 )
		, m_eCameraMode( eCAMERA_MODE_FIXED )
		, m_rFpCamSpeed( 2.0 )
	{
		for ( int i = 0; i < eTIMER_ID_COUNT; i++ )
		{
			m_pTimer[i] = new wxTimer( this, i );
		}

		m_pCursorArrow = new wxCursor( wxCURSOR_ARROW );
		m_pCursorHand = new wxCursor( wxCURSOR_HAND );
		m_pCursorNone = new wxCursor( wxCURSOR_BLANK );
	}

	RenderPanel::~RenderPanel()
	{
		delete m_pCursorArrow;
		delete m_pCursorHand;
		delete m_pCursorNone;

		for ( int i = 0; i < eTIMER_ID_COUNT; i++ )
		{
			delete m_pTimer[i];
			m_pTimer[i] = NULL;
		}
	}

	void RenderPanel::Focus()
	{
	}

	void RenderPanel::UnFocus()
	{
	}

	void RenderPanel::DrawOneFrame()
	{
		wxClientDC l_dc( this );
	}

	void RenderPanel::SetRenderWindow( Castor3D::RenderWindowSPtr p_pWindow )
	{
		m_pRenderWindow.reset();

		if ( p_pWindow )
		{
			if ( p_pWindow->Initialise( GuiCommon::wxMakeWindowHandle( this ) ) )
			{
				Castor::Size l_sizeScreen;
				Castor::Size l_sizeWnd = p_pWindow->GetSize();
				Castor::System::GetScreenSize( 0, l_sizeScreen );
				GetParent()->SetClientSize( l_sizeWnd.width(), l_sizeWnd.height() );
				GetParent()->SetPosition( wxPoint( std::max< int >( 0, ( l_sizeScreen.width() - l_sizeWnd.width() ) / 2 ), std::max< int >( 0, ( l_sizeScreen.height() - l_sizeWnd.height() ) / 2 ) ) );
				m_pListener = p_pWindow->GetListener();
				SceneSPtr l_pScene = p_pWindow->GetScene();
				wxDisplay l_wxDisplay;
				wxRect l_rect( 0, 0, l_sizeScreen.width(), l_sizeScreen.height() );

				if ( l_pScene )
				{
					SceneNodeSPtr l_cameraNode = l_pScene->GetCameraRootNode();
					m_ptOriginalPosition = l_cameraNode->GetPosition();
					m_qOriginalOrientation = l_cameraNode->GetOrientation();
					m_pRotateCamEvent = std::make_shared< CameraRotateEvent >( l_cameraNode, real( 0 ), real( 0 ), real( 0 ) );
					m_pTranslateCamEvent = std::make_shared< CameraTranslateEvent >( l_cameraNode, real( 0 ), real( 0 ), real( 0 ) );
					m_pRenderWindow = p_pWindow;
					m_pKeyboardEvent = std::make_shared< KeyboardEvent >( p_pWindow, wxGetApp().GetMainFrame() );
				}
			}
		}
	}

	void RenderPanel::DoStartTimer( int p_iId )
	{
		m_pTimer[p_iId]->Start( 10 );
	}

	void RenderPanel::DoStopTimer( int p_iId )
	{
		if ( p_iId != eTIMER_ID_COUNT )
		{
			m_pTimer[p_iId]->Stop();
		}
		else
		{
			for ( int i = 0; i < eTIMER_ID_COUNT; i++ )
			{
				m_pTimer[i]->Stop();
			}
		}
	}

	BEGIN_EVENT_TABLE( RenderPanel, wxPanel )
		EVT_TIMER( eTIMER_ID_FORWARD, RenderPanel::OnTimerFwd )
		EVT_TIMER( eTIMER_ID_BACK, RenderPanel::OnTimerBck )
		EVT_TIMER( eTIMER_ID_LEFT, RenderPanel::OnTimerLft )
		EVT_TIMER( eTIMER_ID_RIGHT, RenderPanel::OnTimerRgt )
		EVT_TIMER( eTIMER_ID_UP, RenderPanel::OnTimerUp )
		EVT_TIMER( eTIMER_ID_DOWN, RenderPanel::OnTimerDwn )
		EVT_SIZE( RenderPanel::OnSize )
		EVT_MOVE( RenderPanel::OnMove )
		EVT_PAINT( RenderPanel::OnPaint )
		EVT_ENTER_WINDOW( RenderPanel::OnEnterWindow )
		EVT_LEAVE_WINDOW( RenderPanel::OnLeaveWindow )
		EVT_ERASE_BACKGROUND( RenderPanel::OnEraseBackground )
		EVT_SET_FOCUS( RenderPanel::OnSetFocus )
		EVT_KILL_FOCUS( RenderPanel::OnKillFocus )
		EVT_KEY_DOWN( RenderPanel::OnKeyDown )
		EVT_KEY_UP( RenderPanel::OnKeyUp )
		EVT_LEFT_DCLICK( RenderPanel::OnMouseLDClick )
		EVT_LEFT_DOWN( RenderPanel::OnMouseLDown )
		EVT_LEFT_UP( RenderPanel::OnMouseLUp )
		EVT_MIDDLE_DOWN( RenderPanel::OnMouseMDown )
		EVT_MIDDLE_UP( RenderPanel::OnMouseMUp )
		EVT_RIGHT_DOWN( RenderPanel::OnMouseRDown )
		EVT_RIGHT_UP( RenderPanel::OnMouseRUp )
		EVT_MOTION( RenderPanel::OnMouseMove )
		EVT_MOUSEWHEEL( RenderPanel::OnMouseWheel )
		EVT_MENU( wxID_EXIT, RenderPanel::OnMenuClose )
	END_EVENT_TABLE()

	void RenderPanel::OnTimerFwd( wxTimerEvent & p_event )
	{
		MouseCameraEvent::Add( m_pTranslateCamEvent, m_pListener, real( 0 ), real( 0 ), m_rFpCamSpeed );
		p_event.Skip();
	}

	void RenderPanel::OnTimerBck( wxTimerEvent & p_event )
	{
		MouseCameraEvent::Add( m_pTranslateCamEvent, m_pListener, real( 0 ), real( 0 ), -m_rFpCamSpeed );
		p_event.Skip();
	}

	void RenderPanel::OnTimerLft( wxTimerEvent & p_event )
	{
		MouseCameraEvent::Add( m_pTranslateCamEvent, m_pListener, m_rFpCamSpeed, real( 0 ), real( 0 ) );
		p_event.Skip();
	}

	void RenderPanel::OnTimerRgt( wxTimerEvent & p_event )
	{
		MouseCameraEvent::Add( m_pTranslateCamEvent, m_pListener, -m_rFpCamSpeed, real( 0 ), real( 0 ) );
		p_event.Skip();
	}

	void RenderPanel::OnTimerUp( wxTimerEvent & p_event )
	{
		MouseCameraEvent::Add( m_pTranslateCamEvent, m_pListener, real( 0 ), -m_rFpCamSpeed, real( 0 ) );
		p_event.Skip();
	}

	void RenderPanel::OnTimerDwn( wxTimerEvent & p_event )
	{
		MouseCameraEvent::Add( m_pTranslateCamEvent, m_pListener, real( 0 ), m_rFpCamSpeed, real( 0 ) );
		p_event.Skip();
	}

	void RenderPanel::OnSize( wxSizeEvent & p_event )
	{
		wxClientDC l_dc( this );

		if ( m_pRenderWindow.lock() )
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

	void RenderPanel::OnMove( wxMoveEvent & p_event )
	{
		wxClientDC l_dc( this );

		if ( !m_pRenderWindow.lock() )
		{
			l_dc.SetBrush( wxBrush( *wxWHITE ) );
			l_dc.SetPen( wxPen( *wxWHITE ) );
			l_dc.DrawRectangle( 0, 0, GetClientSize().x, GetClientSize().y );
		}

		p_event.Skip();
	}

	void RenderPanel::OnPaint( wxPaintEvent & p_event )
	{
		wxPaintDC l_dc( this );

		if ( !m_pRenderWindow.lock() )
		{
			l_dc.SetBrush( wxBrush( *wxWHITE ) );
			l_dc.SetPen( wxPen( *wxWHITE ) );
			l_dc.DrawRectangle( 0, 0, GetClientSize().x, GetClientSize().y );
		}

		p_event.Skip();
	}

	void RenderPanel::OnEnterWindow( wxMouseEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::OnLeaveWindow( wxMouseEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::OnEraseBackground( wxEraseEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::OnSetFocus( wxFocusEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::OnKillFocus( wxFocusEvent & p_event )
	{
		DoStopTimer( eTIMER_ID_COUNT );
		p_event.Skip();
	}

	void RenderPanel::OnKeyDown( wxKeyEvent & p_event )
	{
		RenderWindowSPtr l_pWindow = m_pRenderWindow.lock();

		switch ( p_event.GetKeyCode() )
		{
		case WXK_LEFT:
		case 'Q':
			DoStartTimer( eTIMER_ID_LEFT );
			break;

		case WXK_RIGHT:
		case 'D':
			DoStartTimer( eTIMER_ID_RIGHT );
			break;

		case WXK_UP:
		case 'Z':
			DoStartTimer( eTIMER_ID_FORWARD );
			break;

		case WXK_DOWN:
		case 'S':
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

	void RenderPanel::OnKeyUp( wxKeyEvent & p_event )
	{
		RenderWindowSPtr l_pWindow = m_pRenderWindow.lock();
		SceneSPtr l_pScene = l_pWindow->GetScene();

		switch ( p_event.GetKeyCode() )
		{
		case 'R':
			DoStopTimer( eTIMER_ID_COUNT );
			m_rZoom = 1.0;
			m_deltaX = 0.0;
			m_deltaY = 0.0;
			m_x = 0.0;
			m_y = 0.0;
			m_oldX = 0.0;
			m_oldY = 0.0;
			if ( l_pScene )
			{
				SceneNodeSPtr l_cameraNode = l_pScene->GetCameraRootNode();
				l_cameraNode->SetOrientation( m_qOriginalOrientation );
				l_cameraNode->SetPosition( m_ptOriginalPosition );
			}
			break;

		case 'W':
			DoStopTimer( eTIMER_ID_COUNT );

			switch ( l_pWindow->GetPrimitiveType() )
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
			DoStopTimer( eTIMER_ID_LEFT );
			break;

		case WXK_RIGHT:
			DoStopTimer( eTIMER_ID_RIGHT );
			break;

		case WXK_UP:
			DoStopTimer( eTIMER_ID_FORWARD );
			break;

		case WXK_DOWN:
			DoStopTimer( eTIMER_ID_BACK );
			break;

		case WXK_PAGEUP:
			DoStopTimer( eTIMER_ID_UP );
			break;

		case WXK_PAGEDOWN:
			DoStopTimer( eTIMER_ID_DOWN );
			break;
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseLDClick( wxMouseEvent & p_event )
	{
		m_pListener->PostEvent( m_pKeyboardEvent );
		p_event.Skip();
	}

	void RenderPanel::OnMouseLDown( wxMouseEvent & p_event )
	{
		m_mouseLeftDown = true;
		SetCursor( *m_pCursorNone );
		m_oldX = real( p_event.GetX() );
		m_oldY = real( p_event.GetY() );
		p_event.Skip();
	}

	void RenderPanel::OnMouseLUp( wxMouseEvent & p_event )
	{
		m_mouseLeftDown = false;
		SetCursor( *m_pCursorArrow );
		p_event.Skip();
	}

	void RenderPanel::OnMouseMDown( wxMouseEvent & p_event )
	{
		m_mouseMiddleDown = true;
		m_oldX = real( p_event.GetX() );
		m_oldY = real( p_event.GetY() );
		p_event.Skip();
	}

	void RenderPanel::OnMouseMUp( wxMouseEvent & p_event )
	{
		m_mouseMiddleDown = false;
		p_event.Skip();
	}

	void RenderPanel::OnMouseRDown( wxMouseEvent & p_event )
	{
		m_mouseRightDown = true;
		SetCursor( *m_pCursorNone );
		m_oldX = real( p_event.GetX() );
		m_oldY = real( p_event.GetY() );
		p_event.Skip();
	}

	void RenderPanel::OnMouseRUp( wxMouseEvent & p_event )
	{
		m_mouseRightDown = false;
		SetCursor( *m_pCursorArrow );
		p_event.Skip();
	}

	void RenderPanel::OnMouseMove( wxMouseEvent & p_event )
	{
		RenderWindowSPtr l_pWindow = m_pRenderWindow.lock();
		m_x = real( p_event.GetX() );
		m_y = real( p_event.GetY() );
		m_deltaX = ( m_oldX - m_x ) / 2.0f;
		m_deltaY = ( m_oldY - m_y ) / 2.0f;

		if ( l_pWindow )
		{
			if ( m_mouseLeftDown )
			{
				MouseCameraEvent::Add( m_pRotateCamEvent, m_pListener, m_deltaX, 0, m_deltaY );
			}
			else if ( m_mouseRightDown )
			{
				MouseCameraEvent::Add( m_pTranslateCamEvent, m_pListener, -m_deltaX, m_deltaY, 0 );
			}

			if ( m_mouseLeftDown || m_mouseRightDown )
			{
				WarpPointer( int( m_oldX ), int( m_oldY ) );
				m_x = m_oldX;
				m_y = m_oldY;
			}

			p_event.Skip( false );
		}
		else
		{
			p_event.Skip();
		}

		m_oldX = m_x;
		m_oldY = m_y;
	}

	void RenderPanel::OnMouseWheel( wxMouseEvent & p_event )
	{
		RenderWindowSPtr l_pWindow = m_pRenderWindow.lock();

		int l_wheelRotation = p_event.GetWheelRotation();

		if ( l_wheelRotation < 0 )
		{
			m_rFpCamSpeed *= real( 0.9 );
		}
		else
		{
			m_rFpCamSpeed /= real( 0.9 );
		}

		p_event.Skip();
	}

	void RenderPanel::OnMenuClose( wxCommandEvent & p_event )
	{
		Close( true );
		p_event.Skip();
	}
}
