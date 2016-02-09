#include "RenderPanel.hpp"

#include "MainFrame.hpp"
#include "CastorShape.hpp"
#include "FrameListener.hpp"
#include "MouseEvent.hpp"

#if defined( __linux )
#	include <GL/glx.h>
#	include <gtk/gtk.h>
#	include <gdk/gdkx.h>
#elif defined( _WIN32 )
#	include <Windows.h>
#endif

#include <CameraManager.hpp>
#include <GeometryManager.hpp>
#include <SceneManager.hpp>
#include <SceneNodeManager.hpp>
#include <TargetManager.hpp>
#include <WindowHandle.hpp>
#include <WindowManager.hpp>

using namespace Castor3D;
using namespace CastorShape;
using namespace Castor;

#define ID_NEW_WINDOW 10000

RenderPanel::RenderPanel( eVIEWPORT_TYPE p_renderType, SceneSPtr p_scene, ePROJECTION_DIRECTION p_look, wxWindow * parent, wxWindowID p_id, wxPoint const & pos, wxSize const & size, long style )
	: wxPanel( parent, p_id, pos, size, style )
	, m_renderType( p_renderType )
	, m_lookAt( p_look )
	, m_mainScene( p_scene )
	, m_mouseLeftDown( false )
	, m_mouseRightDown( false )
	, m_mouseMiddleDown( false )
	, m_actionType( atNone )
	, m_selectionType( stNone )
	, m_rZoom( 1.0 )
{
}

RenderPanel::~RenderPanel()
{
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

void RenderPanel::SelectGeometry( GeometrySPtr p_geometry )
{
	wxGetApp().GetMainFrame()->SelectGeometry( p_geometry );
}

void RenderPanel::SelectVertex( Vertex * p_vertex )
{
	wxGetApp().GetMainFrame()->SelectVertex( p_vertex );
}

void RenderPanel::DestroyRenderWindow()
{
	m_pRenderWindow.reset();
}

void RenderPanel::InitialiseRenderWindow()
{
	Logger::LogInfo( cuT( "Initialising RenderWindow" ) );
	StringStream l_streamName;
	l_streamName << cuT( "RenderPanel_" ) << GetId();
	SceneNodeSPtr l_node;
	RenderWindowSPtr l_pRenderWindow = wxGetApp().GetCastor()->GetWindowManager().Create();
	RenderTargetSPtr l_pRenderTarget = wxGetApp().GetCastor()->GetTargetManager().Create( eTARGET_TYPE_WINDOW );
	SceneNodeSPtr l_pCamBaseNode = m_mainScene->GetSceneNodeManager().Create( l_streamName.str() + cuT( "_CamNode" ), m_mainScene->GetCameraRootNode()	);
	l_pCamBaseNode->SetPosition( Point3r( 0, 0, -100 ) );

	if ( m_renderType == eVIEWPORT_TYPE_PERSPECTIVE )
	{
		SceneNodeSPtr l_pCamYawNode = m_mainScene->GetSceneNodeManager().Create( l_streamName.str() + cuT( "_CamYawNode" ), l_pCamBaseNode );
		SceneNodeSPtr l_pCamPitchNode = m_mainScene->GetSceneNodeManager().Create( l_streamName.str() + cuT( "_CamPitchNode" ), l_pCamYawNode );
		SceneNodeSPtr l_pCamRollNode = m_mainScene->GetSceneNodeManager().Create( l_streamName.str() + cuT( "_CamRollNode" ), l_pCamPitchNode );
		l_node = l_pCamRollNode;
	}

	CameraSPtr l_pCamera = m_mainScene->GetCameraManager().Create( l_streamName.str() + cuT( "_Camera" ), l_node );
	l_pCamera->GetViewport().SetSize( Size( GetClientSize().x, GetClientSize().y ) );
	l_pRenderTarget->SetScene( m_mainScene );
	l_pRenderTarget->SetCamera( l_pCamera );
	l_pRenderTarget->SetSize( Size( GetClientSize().x, GetClientSize().y ) );
	l_pRenderTarget->SetPixelFormat( ePIXEL_FORMAT_A8R8G8B8 );
	l_pRenderWindow->SetRenderTarget( l_pRenderTarget );
	Castor::Size l_sizeWnd = GuiCommon::make_Size( GetClientSize() );

	if ( l_pRenderWindow->Initialise( l_sizeWnd, GuiCommon::make_WindowHandle( this ) ) )
	{
		m_listener = l_pRenderWindow->GetListener();
		m_pRotateCamEvent = std::make_shared< CameraRotateEvent >( l_pRenderWindow->GetScene()->GetObjectRootNode(), real( 0 ), real( 0 ), real( 0 ) );
		m_pTranslateCamEvent = std::make_shared< CameraTranslateEvent >( l_pRenderWindow->GetCamera()->GetParent(), real( 0 ), real( 0 ), real( 0 ) );
		m_pRenderWindow = l_pRenderWindow;
		//m_pRotateCamEvent = std::make_shared< CameraRotateEvent >( l_pRenderWindow->GetCamera(), real( 0 ), real( 0 ), real( 0 ) );
		//m_pTranslateCamEvent = std::make_shared< CameraTranslateEvent >( l_pRenderWindow->GetCamera(), real( 0 ), real( 0 ), real( 0 ) );
		Logger::LogInfo( cuT( "RenderWindow Initialised" ) );
	}
}

BEGIN_EVENT_TABLE( RenderPanel, wxPanel )
	EVT_SIZE( RenderPanel::OnSize )
	EVT_MOVE( RenderPanel::OnMove )
	EVT_CLOSE( RenderPanel::OnClose )
	EVT_PAINT( RenderPanel::OnPaint )
	EVT_ENTER_WINDOW( RenderPanel::OnEnterWindow )
	EVT_LEAVE_WINDOW( RenderPanel::OnLeaveWindow )
	EVT_ERASE_BACKGROUND( RenderPanel::OnEraseBackground )

	EVT_SET_FOCUS( RenderPanel::OnSetFocus )
	EVT_KILL_FOCUS( RenderPanel::OnKillFocus )

	EVT_KEY_DOWN( RenderPanel::OnKeyDown )
	EVT_KEY_UP( RenderPanel::OnKeyUp )
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

void RenderPanel::OnSize( wxSizeEvent & WXUNUSED( p_event ) )
{
	if ( !m_pRenderWindow.expired() )
	{
		m_pRenderWindow.lock()->Resize( GetClientSize().x, GetClientSize().y );
	}
}

void RenderPanel::OnMove( wxMoveEvent & WXUNUSED( p_event ) )
{
}

void RenderPanel::OnPaint( wxPaintEvent & WXUNUSED( event ) )
{
}

void RenderPanel::OnClose( wxCloseEvent & WXUNUSED( p_event ) )
{
	m_pRenderWindow.reset();
	Destroy();
}

void RenderPanel::OnEnterWindow( wxMouseEvent & WXUNUSED( event ) )
{
	if ( wxGetApp().GetMainFrame() != NULL )
	{
		wxGetApp().GetMainFrame()->SetCurrentPanel( this, this );
	}
}

void RenderPanel::OnLeaveWindow( wxMouseEvent & WXUNUSED( event ) )
{
	if ( wxGetApp().GetMainFrame() != NULL )
	{
		wxGetApp().GetMainFrame()->SetCurrentPanel( this, NULL );
	}
}

void RenderPanel::OnEraseBackground( wxEraseEvent & WXUNUSED( p_event ) )
{
}

void RenderPanel::OnSetFocus( wxFocusEvent & WXUNUSED( p_event ) )
{
	if ( wxGetApp().GetMainFrame() != NULL )
	{
		wxGetApp().GetMainFrame()->SetCurrentPanel( this, this );
	}
}

void RenderPanel::OnKillFocus( wxFocusEvent & WXUNUSED( p_event ) )
{
	if ( wxGetApp().GetMainFrame() != NULL )
	{
		wxGetApp().GetMainFrame()->SetCurrentPanel( this, NULL );
	}
}

void RenderPanel::OnKeyDown( wxKeyEvent & event )
{
	int l_keyCode = event.GetKeyCode();

	if ( ! m_pRenderWindow.expired() )
	{
		RenderWindowSPtr l_pWindow = m_pRenderWindow.lock();

		if ( l_keyCode == 82 ) // r
		{
			l_pWindow->GetCamera()->ResetPosition();
			l_pWindow->GetCamera()->ResetOrientation();
			l_pWindow->GetCamera()->GetParent()->Translate( Point3r( 0.0f, 0.0f, -5.0f ) );
		}
	}
}

void RenderPanel::OnKeyUp( wxKeyEvent & WXUNUSED( p_event ) )
{
}

void RenderPanel::OnMouseLDown( wxMouseEvent & event )
{
	m_mouseLeftDown = true;
	m_oldX = real( event.GetX() );
	m_oldY = real( event.GetY() );
}

void RenderPanel::OnMouseLUp( wxMouseEvent & event )
{
	m_mouseLeftDown = false;

	if ( m_actionType == atSelect )
	{
		if ( m_selectionType == stGeometry )
		{
			DoSelectGeometry( event.GetX(), event.GetY() );
		}
		else if ( m_selectionType == stVertex )
		{
			DoSelectVertex( event.GetX(), event.GetY() );
		}
		else if ( m_selectionType == stAll )
		{
			DoSelectAll();
		}
		else if ( m_selectionType == stClone )
		{
			DoCloneSelected();
		}
	}
	else if ( m_actionType == atModify )
	{
	}
}

void RenderPanel::OnMouseMDown( wxMouseEvent & event )
{
	m_mouseMiddleDown = true;
	m_oldX = real( event.GetX() );
	m_oldY = real( event.GetY() );
}

void RenderPanel::OnMouseMUp( wxMouseEvent & WXUNUSED( p_event ) )
{
	m_mouseMiddleDown = false;
}

void RenderPanel::OnMouseRDown( wxMouseEvent & event )
{
	m_mouseRightDown = true;
	m_oldX = real( event.GetX() );
	m_oldY = real( event.GetY() );
}

void RenderPanel::OnMouseRUp( wxMouseEvent & WXUNUSED( p_event ) )
{
	m_mouseRightDown = false;
}

void RenderPanel::OnMouseMove( wxMouseEvent & event )
{
	m_x = real( event.GetX() );
	m_y = real( event.GetY() );
	m_deltaX = ( m_oldX - m_x ) / 2.0f;
	m_deltaY = ( m_oldY - m_y ) / 2.0f;
	m_oldX = m_x;
	m_oldY = m_y;

	if ( !m_pRenderWindow.expired() )
	{
		RenderWindowSPtr l_pRenderWindow = m_pRenderWindow.lock();

		if ( m_mouseLeftDown )
		{
			if ( l_pRenderWindow->GetViewportType() == eVIEWPORT_TYPE_PERSPECTIVE )
			{
				MouseCameraEvent::Add( m_pRotateCamEvent, m_listener, m_deltaX, m_deltaY, 0 );
			}
			else
			{
				MouseCameraEvent::Add( m_pRotateCamEvent, m_listener, 0, 0, m_deltaX );
			}
		}
		else if ( m_mouseRightDown )
		{
			MouseCameraEvent::Add( m_pTranslateCamEvent, m_listener, -m_rZoom * m_deltaX / real( 40 ), m_rZoom * m_deltaY / real( 40 ), 0 );
		}
	}
}

void RenderPanel::OnMouseWheel( wxMouseEvent & event )
{
	if ( !m_pRenderWindow.expired() )
	{
		RenderWindowSPtr l_pRenderWindow = m_pRenderWindow.lock();
		int l_wheelRotation = event.GetWheelRotation();
		Point3r const & l_cameraPos = l_pRenderWindow->GetCamera()->GetParent()->GetPosition();

		if ( l_wheelRotation < 0 )
		{
			MouseCameraEvent::Add( m_pTranslateCamEvent, m_listener, 0, 0, ( l_cameraPos[2] - real( 1 ) ) / 10 );
			m_rZoom /= real( 0.9 );
		}
		else if ( l_wheelRotation > 0 )
		{
			MouseCameraEvent::Add( m_pTranslateCamEvent, m_listener, 0, 0, ( real( 1 ) - l_cameraPos[2] ) / 10 );
			m_rZoom *= real( 0.9 );
		}

		if ( m_rZoom <= 1.0 )
		{
			m_rZoom = real( 1.0 );
		}
	}
}

void RenderPanel::OnMenuClose( wxCommandEvent & WXUNUSED( p_event ) )
{
	Close( true );
}

void RenderPanel::DoSelectGeometry( int p_x, int p_y )
{
	//GeometrySPtr l_geo = std::make_shared< Geometry >( m_mainScene );

	//if ( ! m_pRenderWindow.expired() )
	//{
	//	FrameEventSPtr l_pEvent( new SelectObjectFrameEvent( m_mainScene, l_geo, SubmeshSPtr(), FaceSPtr(), VertexSPtr(), m_pRenderWindow.lock()->GetCamera(), this, p_x, p_y ) );
	//	m_listener->PostEvent( l_pEvent );
	//}
}

void RenderPanel::DoSelectVertex( int CU_PARAM_UNUSED( p_x ), int CU_PARAM_UNUSED( p_y ) )
{
	//VertexSPtr l_vertex = std::make_shared< Vertex >(  );

	//if ( ! m_pRenderWindow.expired())
	//{
	//	m_listener->PostEvent( new SelectObjectFrameEvent( m_mainScene, GeometrySPtr(), SubmeshSPtr(), FaceSPtr(), l_vertex, m_pRenderWindow.lock()->GetCamera(), this, p_x, p_y)));
	//}
}

void RenderPanel::DoSelectAll()
{
}

void RenderPanel::DoCloneSelected()
{
}
