#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/main/RenderPanel.hpp"
#include "CastorShape/main/MainFrame.hpp"
#include "CastorShape/main/CastorShape.hpp"
#include "CastorShape/main/FrameListener.hpp"
#include "CastorShape/main/MouseEvent.hpp"

#if defined( __linux )
#	include <GL/glx.h>
#	include <gtk/gtk.h>
#	include <gdk/gdkx.h>
#elif defined( _WIN32 )
#	include <Windows.h>
#endif

#include <Castor3D/WindowHandle.hpp>

using namespace Castor3D;
using namespace CastorShape;
using namespace Castor;

DECLARE_APP(CastorShapeApp);

#define ID_NEW_WINDOW 10000

RenderPanel :: RenderPanel( wxWindow * parent, wxWindowID p_id, eVIEWPORT_TYPE pDoRenderType, SceneSPtr p_scene, wxPoint const & pos, wxSize const & size, ePROJECTION_DIRECTION p_look, long style)
	:	wxPanel				( parent, p_id, pos, size, style		)
	,	mDoRenderType		( pDoRenderType							)
	,	m_lookAt			( p_look								)
	,	m_mainScene			( p_scene								)
	,	m_mouseLeftDown		( false									)
	,	m_mouseRightDown	( false									)
	,	m_mouseMiddleDown	( false									)
	,	m_actionType		( atNone								)
	,	m_selectionType		( stNone								)
	,	m_pMainFrame		( static_cast< MainFrame * >( parent )	)
	,	m_rZoom				( 1.0									)
{
}

RenderPanel :: ~RenderPanel()
{
}

void RenderPanel :: Focus()
{
}

void RenderPanel :: UnFocus()
{
}

void RenderPanel :: DrawOneFrame()
{
	wxClientDC l_dc( this);
}

void RenderPanel :: SelectGeometry( GeometrySPtr p_geometry)
{
	m_pMainFrame->SelectGeometry( p_geometry);
}

void RenderPanel :: SelectVertex( Vertex * p_vertex)
{
	m_pMainFrame->SelectVertex( p_vertex);
}

void RenderPanel :: InitialiseRenderWindow()
{
	Logger::LogMessage( cuT( "Initialising RenderWindow"));
#if defined( _WIN32 )
	WindowHandle l_handle( std::make_shared< IMswWindowHandle >( this->GetHandle() ) );
#elif defined( __linux__ )
	GtkWidget * l_pGtkWidget = static_cast< GtkWidget * >( this->GetHandle() );
	GLXDrawable l_drawable = None;
	Display * l_pDisplay = NULL;

	if( l_pGtkWidget && l_pGtkWidget->window )
	{
		l_drawable = GDK_WINDOW_XID( l_pGtkWidget->window );
		GdkDisplay * l_pGtkDisplay = gtk_widget_get_display( l_pGtkWidget);

		if( l_pGtkDisplay )
		{
			l_pDisplay = gdk_x11_display_get_xdisplay( l_pGtkDisplay );
		}
	}

	WindowHandle l_handle( std::make_shared< IXWindowHandle >( l_drawable, l_pDisplay ) );
#else
#	error "Yet unsupported OS"
#endif
	StringStream l_streamName;
	l_streamName << cuT( "RenderPanel_" ) << GetId();
	SceneNodeSPtr l_pNode;

	RenderWindowSPtr l_pRenderWindow = m_pMainFrame->GetEngine()->CreateRenderWindow();
	RenderTargetSPtr l_pRenderTarget = m_pMainFrame->GetEngine()->CreateRenderTarget( eTARGET_TYPE_WINDOW );
	SceneNodeSPtr l_pCamBasNode		= m_mainScene->CreateSceneNode( l_streamName.str() + cuT( "_CamNode"		), m_mainScene->GetCameraRootNode()	);
	l_pCamBasNode->SetPosition( Point3r( 0, 0, -100 ) );

	if( mDoRenderType == eVIEWPORT_TYPE_3D )
	{
		SceneNodeSPtr l_pCamYawNode		= m_mainScene->CreateSceneNode( l_streamName.str() + cuT( "_CamYawNode"		), l_pCamBasNode );
		SceneNodeSPtr l_pCamPitchNode	= m_mainScene->CreateSceneNode( l_streamName.str() + cuT( "_CamPitchNode"	), l_pCamYawNode );
		SceneNodeSPtr l_pCamRollNode	= m_mainScene->CreateSceneNode( l_streamName.str() + cuT( "_CamRollNode"	), l_pCamPitchNode );
		l_pNode = l_pCamRollNode;
	}

	CameraSPtr l_pCamera = m_mainScene->CreateCamera( l_streamName.str() + cuT( "_Camera" ), GetClientSize().x, GetClientSize().y, l_pNode, mDoRenderType );
	l_pRenderTarget->SetScene( m_mainScene );
	l_pRenderTarget->SetCamera( l_pCamera );
	l_pRenderTarget->SetSize( Size( GetClientSize().x, GetClientSize().y ) );
	l_pRenderTarget->SetPixelFormat( ePIXEL_FORMAT_A8R8G8B8 );
	l_pRenderWindow->SetRenderTarget( l_pRenderTarget );

	if( l_pRenderWindow->Initialise( l_handle ) )
	{

		m_listener = l_pRenderWindow->GetListener();
		m_pRotateCamEvent = std::make_shared< CameraRotateEvent >( l_pRenderWindow->GetScene()->GetObjectRootNode(), real( 0 ), real( 0 ), real( 0 ) );
		m_pTranslateCamEvent = std::make_shared< CameraTranslateEvent >( l_pRenderWindow->GetCamera()->GetParent(), real( 0 ), real( 0 ), real( 0 ) );

		m_pRenderWindow = l_pRenderWindow;
//		m_pRotateCamEvent = std::make_shared< CameraRotateEvent >( l_pRenderWindow->GetCamera(), real( 0 ), real( 0 ), real( 0 ) );
//		m_pTranslateCamEvent = std::make_shared< CameraTranslateEvent >( l_pRenderWindow->GetCamera(), real( 0 ), real( 0 ), real( 0 ) );
		Logger::LogMessage( cuT( "RenderWindow Initialised"));
	}
}

BEGIN_EVENT_TABLE( RenderPanel, wxPanel)
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

void RenderPanel :: _onSize( wxSizeEvent & WXUNUSED( p_event ))
{
	if ( !m_pRenderWindow.expired() )
	{
		m_pRenderWindow.lock()->Resize( GetClientSize().x, GetClientSize().y);
	}
}

void RenderPanel :: _onMove( wxMoveEvent & WXUNUSED( p_event ))
{
}

void RenderPanel :: _onPaint( wxPaintEvent & WXUNUSED(event))
{
}

void RenderPanel :: _onClose( wxCloseEvent & WXUNUSED( p_event ))
{
	m_pRenderWindow.reset();
	Destroy();
}

void RenderPanel :: _onEnterWindow( wxMouseEvent & WXUNUSED(event))
{
	if (m_pMainFrame != NULL)
	{
		m_pMainFrame->SetCurrentPanel( this, this);
	}
}

void RenderPanel :: _onLeaveWindow( wxMouseEvent & WXUNUSED(event))
{
	if (m_pMainFrame != NULL)
	{
		m_pMainFrame->SetCurrentPanel( this, NULL);
	}
}

void RenderPanel :: _onEraseBackground(wxEraseEvent& WXUNUSED( p_event ))
{
}

void RenderPanel :: _onSetFocus( wxFocusEvent & WXUNUSED( p_event ))
{
	if (m_pMainFrame != NULL)
	{
		m_pMainFrame->SetCurrentPanel( this, this);
	}
}

void RenderPanel :: _onKillFocus( wxFocusEvent & WXUNUSED( p_event ))
{
	if (m_pMainFrame != NULL)
	{
		m_pMainFrame->SetCurrentPanel( this, NULL);
	}
}

void RenderPanel :: _onKeyDown(wxKeyEvent& event)
{
	int l_keyCode = event.GetKeyCode();

	if ( ! m_pRenderWindow.expired())
	{
		RenderWindowSPtr l_pWindow = m_pRenderWindow.lock();

		if (l_keyCode == 82)// r
		{
			l_pWindow->GetCamera()->ResetPosition();
			l_pWindow->GetCamera()->ResetOrientation();
			l_pWindow->GetCamera()->GetParent()->Translate( 0.0f, 0.0f, -5.0f);
		}
	}
}

void RenderPanel :: _onKeyUp(wxKeyEvent& WXUNUSED( p_event ))
{
}

void RenderPanel :: _onMouseLDown( wxMouseEvent & event)
{
	m_mouseLeftDown = true;
	m_oldX = real( event.GetX() );
	m_oldY = real( event.GetY() );
}

void RenderPanel :: _onMouseLUp( wxMouseEvent & event)
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

void RenderPanel :: _onMouseMDown( wxMouseEvent & event)
{
	m_mouseMiddleDown = true;
	m_oldX = real( event.GetX() );
	m_oldY = real( event.GetY() );
}

void RenderPanel :: _onMouseMUp( wxMouseEvent & WXUNUSED( p_event ))
{
	m_mouseMiddleDown = false;
}

void RenderPanel :: _onMouseRDown( wxMouseEvent & event)
{
	m_mouseRightDown = true;
	m_oldX = real( event.GetX() );
	m_oldY = real( event.GetY() );
}

void RenderPanel :: _onMouseRUp( wxMouseEvent & WXUNUSED( p_event ))
{
	m_mouseRightDown = false;
}

void RenderPanel :: _onMouseMove( wxMouseEvent & event)
{
	m_x = real( event.GetX() );
	m_y = real( event.GetY() );
	m_deltaX = (m_oldX - m_x) / 2.0f;
	m_deltaY = (m_oldY - m_y) / 2.0f;
	m_oldX = m_x;
	m_oldY = m_y;

	if( !m_pRenderWindow.expired() )
	{
		RenderWindowSPtr l_pRenderWindow = m_pRenderWindow.lock();

		if (m_mouseLeftDown)
		{
			if (l_pRenderWindow->GetViewportType() == eVIEWPORT_TYPE_3D)
			{
				MouseCameraEvent::Add( m_pRotateCamEvent, m_listener, m_deltaX, m_deltaY, 0);
			}
			else
			{
				MouseCameraEvent::Add( m_pRotateCamEvent, m_listener, 0, 0, m_deltaX);
			}
		}
		else if (m_mouseRightDown)
		{
			MouseCameraEvent::Add( m_pTranslateCamEvent, m_listener, -m_rZoom * m_deltaX / real( 40 ), m_rZoom * m_deltaY / real( 40 ), 0);
		}
	}
}

void RenderPanel :: _onMouseWheel( wxMouseEvent & event)
{
	if( !m_pRenderWindow.expired() )
	{
		RenderWindowSPtr l_pRenderWindow = m_pRenderWindow.lock();
		int l_wheelRotation = event.GetWheelRotation();
		Point3r const & l_cameraPos = l_pRenderWindow->GetCamera()->GetParent()->GetPosition();

		if (l_wheelRotation < 0)
		{
			MouseCameraEvent::Add( m_pTranslateCamEvent, m_listener, 0, 0, (l_cameraPos[2] - real( 1 )) / 10);
			m_rZoom /= real( 0.9 );
		}
		else if (l_wheelRotation > 0)
		{
			MouseCameraEvent::Add( m_pTranslateCamEvent, m_listener, 0, 0, (real( 1 ) - l_cameraPos[2]) / 10);
			m_rZoom *= real( 0.9 );
		}

		if( m_rZoom <= 1.0 )
		{
			m_rZoom = real( 1.0 );
		}
	}
}

void RenderPanel :: _onMenuClose( wxCommandEvent & WXUNUSED( p_event ))
{
	Close( true);
}

void RenderPanel :: _selectGeometry( int p_x, int p_y)
{
	GeometrySPtr l_geo = std::make_shared< Geometry >( m_mainScene.get() );

	if ( ! m_pRenderWindow.expired())
	{
		FrameEventSPtr l_pEvent( new SelectObjectFrameEvent( m_mainScene, l_geo, SubmeshSPtr(), FaceSPtr(), VertexSPtr(), m_pRenderWindow.lock()->GetCamera(), this, p_x, p_y ) );
		m_listener->PostEvent( l_pEvent );
	}
}

void RenderPanel :: _selectVertex( int CU_PARAM_UNUSED( p_x ), int CU_PARAM_UNUSED( p_y ) )
{
/*
	VertexSPtr l_vertex = std::make_shared< Vertex >(  );

	if ( ! m_pRenderWindow.expired())
	{
		m_listener->PostEvent( new SelectObjectFrameEvent( m_mainScene, GeometrySPtr(), SubmeshSPtr(), FaceSPtr(), l_vertex, m_pRenderWindow.lock()->GetCamera(), this, p_x, p_y)));
	}
*/
}

void RenderPanel :: _selectAll()
{
}

void RenderPanel :: _cloneSelected()
{
}
