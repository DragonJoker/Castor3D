#include "CastorViewer/RenderPanel.hpp"
#include "CastorViewer/CastorViewer.hpp"
#include "CastorViewer/MainFrame.hpp"
#include "CastorViewer/RotateNodeEvent.hpp"
#include "CastorViewer/TranslateNodeEvent.hpp"
#include "CastorViewer/KeyboardEvent.hpp"

#include <wx/display.h>

#include <Castor3D/Cache/SceneNodeCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Event/Frame/FunctorEvent.hpp>
#include <Castor3D/Event/UserInput/UserInputListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp>
#include <Castor3D/Material/Pass/SpecularGlossinessPbrPass.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Render/ShadowMap/ShadowMapPass.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <ashespp/Core/WindowHandle.hpp>

using namespace castor3d;
using namespace castor;

namespace CastorViewer
{
	namespace
	{
		static const float MAX_CAM_SPEED = 10.0f;
		static const float DEF_CAM_SPEED = 0.5f;
		static const float MIN_CAM_SPEED = 0.05f;
		static const float CAM_SPEED_INC = 0.9f;
	}

	namespace
	{
		KeyboardKey doConvertKeyCode( int code )
		{
			KeyboardKey result = KeyboardKey::eNone;

			if ( code < 0x20 )
			{
				switch ( code )
				{
				case WXK_BACK:
				case WXK_TAB:
				case WXK_RETURN:
				case WXK_ESCAPE:
					result = KeyboardKey( code );
					break;
				}
			}
			else if ( code == 0x7F )
			{
				result = KeyboardKey::eDelete;
			}
			else if ( code > 0xFF )
			{
				result = KeyboardKey( code + int( KeyboardKey::eStart ) - WXK_START );
			}
			else
			{
				// ASCII or extended ASCII character
				result = KeyboardKey( code );
			}

			return result;
		}
	}

	RenderPanel::RenderPanel( wxWindow * parent
		, wxWindowID id
		, wxPoint const & pos
		, wxSize const & size
		, long style )
		: wxPanel( parent, id, pos, size, style )
		, m_camSpeed( DEF_CAM_SPEED, Range< float >{ MIN_CAM_SPEED, MAX_CAM_SPEED } )
	{
		m_timers[0] = nullptr;

		for ( int i = 1; i < eTIMER_ID_COUNT; i++ )
		{
			m_timers[i] = new wxTimer( this, i );
		}

		m_cursorArrow = new wxCursor( wxCURSOR_ARROW );
		m_cursorHand = new wxCursor( wxCURSOR_HAND );
		m_cursorNone = new wxCursor( wxCURSOR_BLANK );
	}

	RenderPanel::~RenderPanel()
	{
		delete m_cursorArrow;
		delete m_cursorHand;
		delete m_cursorNone;

		for ( int i = 1; i <= eTIMER_ID_MOVEMENT; i++ )
		{
			delete m_timers[i];
			m_timers[i] = nullptr;
		}
	}

	void RenderPanel::resetRenderWindow()
	{
		doStopMovement();
		m_selectedSubmesh.reset();
		m_selectedGeometry.reset();
		m_currentState = nullptr;
		m_nodesStates.clear();
		m_camera.reset();
		m_scene.reset();
		m_keyboardEvent.reset();
		m_currentNode = nullptr;
		m_lightsNode = nullptr;
		m_listener.reset();
		m_cubeManager.reset();
		m_renderWindow.reset();
	}

	void RenderPanel::setRenderWindow( RenderWindowSPtr window )
	{
		CU_Require( window );
		castor::Size sizeWnd = GuiCommon::makeSize( GetClientSize() );

		if ( window->initialise( sizeWnd, GuiCommon::makeWindowHandle( this ) ) )
		{
			castor::Size sizeScreen;
			castor::System::getScreenSize( 0, sizeScreen );
			GetParent()->SetClientSize( sizeWnd.getWidth(), sizeWnd.getHeight() );
			GetParent()->SetMinClientSize( GuiCommon::make_wxSize( window->getRenderTarget()->getSize() ) );
			sizeWnd = GuiCommon::makeSize( GetParent()->GetClientSize() );
			GetParent()->SetPosition( wxPoint( std::abs( int( sizeScreen.getWidth() ) - int( sizeWnd.getWidth() ) ) / 2
				, std::abs( int( sizeScreen.getHeight() ) - int( sizeWnd.getHeight() ) ) / 2 ) );
			m_listener = window->getListener();
			SceneSPtr scene = window->getScene();

			if ( scene )
			{
				m_cubeManager = std::make_unique< GuiCommon::CubeBoxManager >( *scene );
				auto camera = window->getCamera();

				if ( camera )
				{
					if ( scene->getSceneNodeCache().has( cuT( "PointLightsNode" ) ) )
					{
						m_lightsNode = scene->getSceneNodeCache().find( cuT( "PointLightsNode" ) ).get();
					}
					else if ( scene->getSceneNodeCache().has( cuT( "LightNode" ) ) )
					{
						m_lightsNode = scene->getSceneNodeCache().find( cuT( "LightNode" ) ).get();
					}

					auto cameraNode = camera->getParent();

					if ( cameraNode )
					{
						m_currentNode = cameraNode;
						m_currentState = &doAddNodeState( m_currentNode, true );
					}

					m_renderWindow = window;
					m_keyboardEvent = std::make_unique< KeyboardEvent >( window );

					{
						window->addPickingScene( *scene );
					}

					m_camera = camera;
					doStartMovement();
				}

				m_scene = scene;
			}
		}
	}

	void RenderPanel::doResetTimers()
	{
		doStopTimer( eTIMER_ID_COUNT );
		m_camSpeed = DEF_CAM_SPEED;
		m_x = 0.0f;
		m_y = 0.0f;
		m_oldX = 0.0f;
		m_oldY = 0.0f;
	}

	void RenderPanel::doStartMovement()
	{
		if ( !m_movementStarted )
		{
			m_timers[eTIMER_ID_MOVEMENT]->Start( 30 );
			m_movementStarted = true;
		}
	}

	void RenderPanel::doStopMovement()
	{
		if ( m_movementStarted )
		{
			m_movementStarted = false;
			m_timers[eTIMER_ID_MOVEMENT]->Stop();
		}
	}

	void RenderPanel::doStartTimer( int p_id )
	{
		m_timers[p_id]->Start( 10 );
	}

	void RenderPanel::doStopTimer( int p_id )
	{
		if ( p_id != eTIMER_ID_COUNT )
		{
			m_timers[p_id]->Stop();
		}
		else
		{
			for ( int i = 1; i < eTIMER_ID_MOVEMENT; i++ )
			{
				m_timers[i]->Stop();
			}
		}
	}

	void RenderPanel::doResetNode()
	{
		doResetTimers();

		if ( m_currentState )
		{
			m_currentState->reset( DEF_CAM_SPEED );
		}
	}

	void RenderPanel::doTurnCameraHoriz()
	{
		doResetTimers();

		if ( m_currentState )
		{
			m_currentState->yaw( 90.0_degrees );
		}
	}

	void RenderPanel::doTurnCameraVertic()
	{
		doResetTimers();
		auto camera = m_camera.lock();

		if ( camera )
		{
			auto cameraNode = camera->getParent();
			camera->getScene()->getListener().postEvent( makeFunctorEvent( EventType::ePostRender
				, [this, cameraNode]()
				{
					Quaternion orientation{ cameraNode->getOrientation() };
					orientation *= Quaternion::fromAxisAngle( castor::Point3f{ 1.0f, 0.0f, 0.0f }, 90.0_degrees );
					cameraNode->setOrientation( orientation );
				} ) );
		}
	}

	void RenderPanel::doChangeCamera()
	{
		auto camera = m_camera.lock();
		auto window = m_renderWindow.lock();
		auto scene = m_scene.lock();

		if ( camera && window && scene )
		{
			//auto & shadowMaps = window->getRenderTarget()->getTechnique()->getShadowMaps();

			//if ( !shadowMaps.empty() )
			//{
			//	auto it = shadowMaps.begin();
			//	m_currentNode = it->first->getParent();
			//}
		}
	}

	void RenderPanel::doReloadScene()
	{
		doResetTimers();
		wxGetApp().getMainFrame()->loadScene();
	}

	float RenderPanel::doTransformX( int x )
	{
		float result = float( x );
		auto camera = m_camera.lock();

		if ( camera )
		{
			result *= float( camera->getWidth() ) / GetClientSize().x;
		}

		return result;
	}

	float RenderPanel::doTransformY( int y )
	{
		float result = float( y );
		auto camera = m_camera.lock();

		if ( camera )
		{
			result *= float( camera->getHeight() ) / GetClientSize().y;
		}

		return result;
	}

	int RenderPanel::doTransformX( float x )
	{
		int result = int( x );
		auto camera = m_camera.lock();

		if ( camera )
		{
			result = int( x * GetClientSize().x / float( camera->getWidth() ) );
		}

		return result;
	}

	int RenderPanel::doTransformY( float y )
	{
		int result = int( y );
		auto camera = m_camera.lock();

		if ( camera )
		{
			result = int( y * GetClientSize().y / float( camera->getHeight() ) );
		}

		return result;
	}

	void RenderPanel::doUpdateSelectedGeometry( castor3d::GeometrySPtr geometry
		, castor3d::SubmeshSPtr submesh )
	{
		auto oldSubmesh = m_selectedSubmesh;
		auto oldGeometry = m_selectedGeometry;
		bool changed = false;
		SceneRPtr scene = geometry ? geometry->getScene() : nullptr;

		if ( oldGeometry != geometry )
		{
			changed = true;
			m_selectedSubmesh = nullptr;

			if ( oldGeometry )
			{
				m_cubeManager->hideObject( *oldGeometry );
				scene = oldGeometry->getScene();
			}

			if ( geometry )
			{
				m_cubeManager->displayObject( *geometry, *submesh );
			}

			m_selectedGeometry = geometry;
		}

		if ( oldSubmesh != submesh )
		{
			changed = true;

			if ( submesh )
			{
				m_selectedSubmesh = submesh;
				wxGetApp().getMainFrame()->select( m_selectedGeometry, m_selectedSubmesh );
			}
		}

		if ( changed )
		{
			scene->getListener().postEvent( makeFunctorEvent( EventType::ePostRender
				, [scene]()
				{
					scene->setChanged();
				} ) );
		}

		if ( geometry )
		{
			m_currentNode = m_selectedGeometry->getParent();
		}
		else
		{
			m_currentNode = m_camera.lock()->getParent();
		}

		m_currentState = &doAddNodeState( m_currentNode, false );
	}

	GuiCommon::NodeState & RenderPanel::doAddNodeState( SceneNodeRPtr node
		, bool camera )
	{
		auto it = m_nodesStates.find( node->getName() );

		if ( it == m_nodesStates.end() )
		{
			it = m_nodesStates.emplace( node->getName()
				, std::make_unique< GuiCommon::NodeState >( *m_listener, node, camera ) ).first;
		}

		it->second->setMaxSpeed( m_camSpeed.value() );
		return *it->second;
	}

	BEGIN_EVENT_TABLE( RenderPanel, wxPanel )
		EVT_TIMER( eTIMER_ID_FORWARD, RenderPanel::onTimerFwd )
		EVT_TIMER( eTIMER_ID_BACK, RenderPanel::onTimerBck )
		EVT_TIMER( eTIMER_ID_LEFT, RenderPanel::onTimerLft )
		EVT_TIMER( eTIMER_ID_RIGHT, RenderPanel::onTimerRgt )
		EVT_TIMER( eTIMER_ID_UP, RenderPanel::onTimerUp )
		EVT_TIMER( eTIMER_ID_DOWN, RenderPanel::onTimerDwn )
		EVT_TIMER( eTIMER_ID_MOUSE, RenderPanel::onTimerMouse )
		EVT_TIMER( eTIMER_ID_MOVEMENT, RenderPanel::onTimerMovement )
		EVT_SIZE( RenderPanel::onSize )
		EVT_MOVE( RenderPanel::onMove )
		EVT_PAINT( RenderPanel::onPaint )
		EVT_ENTER_WINDOW( RenderPanel::onEnterWindow )
		EVT_LEAVE_WINDOW( RenderPanel::onLeaveWindow )
		EVT_ERASE_BACKGROUND( RenderPanel::onEraseBackground )
		EVT_SET_FOCUS( RenderPanel::onsetFocus )
		EVT_KILL_FOCUS( RenderPanel::onKillFocus )
		EVT_KEY_DOWN( RenderPanel::onKeydown )
		EVT_KEY_UP( RenderPanel::onKeyUp )
		EVT_CHAR( RenderPanel::onChar )
		EVT_LEFT_DCLICK( RenderPanel::onMouseLDClick )
		EVT_LEFT_DOWN( RenderPanel::onMouseLdown )
		EVT_LEFT_UP( RenderPanel::onMouseLUp )
		EVT_MIDDLE_DOWN( RenderPanel::onMouseMdown )
		EVT_MIDDLE_UP( RenderPanel::onMouseMUp )
		EVT_RIGHT_DOWN( RenderPanel::onMouseRdown )
		EVT_RIGHT_UP( RenderPanel::onMouseRUp )
		EVT_MOTION( RenderPanel::onMouseMove )
		EVT_MOUSEWHEEL( RenderPanel::onMouseWheel )
		EVT_MENU( wxID_EXIT, RenderPanel::onMenuClose )
	END_EVENT_TABLE()

	void RenderPanel::onTimerFwd( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->addScalarVelocity( castor::Point3f{ 0.0f, 0.0f, m_camSpeed.value() } );
		}

		p_event.Skip();
	}

	void RenderPanel::onTimerBck( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->addScalarVelocity( castor::Point3f{ 0.0f, 0.0f, -m_camSpeed.value() } );
		}

		p_event.Skip();
	}

	void RenderPanel::onTimerLft( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->addScalarVelocity( castor::Point3f{ m_camSpeed.value(), 0.0f, 0.0f } );
		}

		p_event.Skip();
	}

	void RenderPanel::onTimerRgt( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->addScalarVelocity( castor::Point3f{ -m_camSpeed.value(), 0.0f, 0.0f } );
		}

		p_event.Skip();
	}

	void RenderPanel::onTimerUp( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->addScalarVelocity( castor::Point3f{ 0.0f, m_camSpeed.value(), 0.0f } );
		}

		p_event.Skip();
	}

	void RenderPanel::onTimerDwn( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->addScalarVelocity( castor::Point3f{ 0.0f, -m_camSpeed.value(), 0.0f } );
		}

		p_event.Skip();
	}

	void RenderPanel::onTimerMouse( wxTimerEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::onTimerMovement( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->update();
		}

		p_event.Skip();
	}

	void RenderPanel::onSize( wxSizeEvent & p_event )
	{
		auto window = m_renderWindow.lock();

		if ( window )
		{
			if ( m_resizeWindow )
			{
				window->resize( p_event.GetSize().x, p_event.GetSize().y );
			}
		}
		else
		{
			wxClientDC dc( this );
			dc.SetBrush( wxBrush( GuiCommon::INACTIVE_TAB_COLOUR ) );
			dc.SetPen( wxPen( GuiCommon::INACTIVE_TAB_COLOUR ) );
			dc.DrawRectangle( 0, 0, p_event.GetSize().x, p_event.GetSize().y );
		}

		p_event.Skip();
	}

	void RenderPanel::onMove( wxMoveEvent & p_event )
	{
		auto window = m_renderWindow.lock();

		if ( !window )
		{
			wxClientDC dc( this );
			dc.SetBrush( wxBrush( GuiCommon::INACTIVE_TAB_COLOUR ) );
			dc.SetPen( wxPen( GuiCommon::INACTIVE_TAB_COLOUR ) );
			dc.DrawRectangle( 0, 0, GetClientSize().x, GetClientSize().y );
		}

		p_event.Skip();
	}

	void RenderPanel::onPaint( wxPaintEvent & p_event )
	{
		auto window = m_renderWindow.lock();

		if ( !window )
		{
			wxPaintDC dc( this );
			dc.SetBrush( wxBrush( GuiCommon::INACTIVE_TAB_COLOUR ) );
			dc.SetPen( wxPen( GuiCommon::INACTIVE_TAB_COLOUR ) );
			dc.DrawRectangle( 0, 0, GetClientSize().x, GetClientSize().y );
		}

		p_event.Skip();
	}

	void RenderPanel::onEnterWindow( wxMouseEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::onLeaveWindow( wxMouseEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::onEraseBackground( wxEraseEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::onsetFocus( wxFocusEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::onKillFocus( wxFocusEvent & p_event )
	{
		doStopTimer( eTIMER_ID_COUNT );
		p_event.Skip();
	}

	void RenderPanel::onKeydown( wxKeyEvent & p_event )
	{
		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireKeydown( doConvertKeyCode( p_event.GetKeyCode() ), p_event.ControlDown(), p_event.AltDown(), p_event.ShiftDown() ) )
		{
			switch ( p_event.GetKeyCode() )
			{
			case WXK_LEFT:
			case 'Q':
				doStartTimer( eTIMER_ID_LEFT );
				break;

			case WXK_RIGHT:
			case 'D':
				doStartTimer( eTIMER_ID_RIGHT );
				break;

			case WXK_UP:
			case 'Z':
				doStartTimer( eTIMER_ID_FORWARD );
				break;

			case WXK_DOWN:
			case 'S':
				doStartTimer( eTIMER_ID_BACK );
				break;

			case WXK_PAGEUP:
				doStartTimer( eTIMER_ID_UP );
				break;

			case WXK_PAGEDOWN:
				doStartTimer( eTIMER_ID_DOWN );
				break;

			case WXK_ALT:
				m_altdown = true;
				break;

			case 'L':
				if ( m_lightsNode )
				{
					m_currentNode = m_lightsNode;
					m_currentState = &doAddNodeState( m_currentNode, false );
				}
				break;
			}
		}

		p_event.Skip();
	}

	void RenderPanel::onKeyUp( wxKeyEvent & p_event )
	{
		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireKeyUp( doConvertKeyCode( p_event.GetKeyCode() ), p_event.ControlDown(), p_event.AltDown(), p_event.ShiftDown() ) )
		{
			switch ( p_event.GetKeyCode() )
			{
			case 'R':
				doResetNode();
				break;

			case 'T':
				doTurnCameraHoriz();
				break;

			case 'Y':
				doTurnCameraVertic();
				break;

			case 'C':
				doChangeCamera();
				break;

			case WXK_F5:
				doReloadScene();
				break;

			case WXK_LEFT:
			case 'Q':
				doStopTimer( eTIMER_ID_LEFT );
				break;

			case WXK_RIGHT:
			case 'D':
				doStopTimer( eTIMER_ID_RIGHT );
				break;

			case WXK_UP:
			case 'Z':
				doStopTimer( eTIMER_ID_FORWARD );
				break;

			case WXK_DOWN:
			case 'S':
				doStopTimer( eTIMER_ID_BACK );
				break;

			case WXK_PAGEUP:
				doStopTimer( eTIMER_ID_UP );
				break;

			case WXK_PAGEDOWN:
				doStopTimer( eTIMER_ID_DOWN );
				break;

			case WXK_ALT:
				m_altdown = false;
				break;

			case WXK_ESCAPE:
				if ( m_selectedGeometry )
				{
					doUpdateSelectedGeometry( nullptr, nullptr );
				}
				else
				{
					m_parent->Close();
				}
				break;

			case 'L':
				m_currentNode = m_camera.lock()->getParent();
				m_currentState = &doAddNodeState( m_currentNode, false );
				break;
			}
		}

		p_event.Skip();
	}

	void RenderPanel::onChar( wxKeyEvent & p_event )
	{
		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( inputListener )
		{
			wxChar key = p_event.GetUnicodeKey();
			wxString tmp;
			tmp << key;
			inputListener->fireChar( doConvertKeyCode( p_event.GetKeyCode() ), String( tmp.mb_str( wxConvUTF8 ) ) );
		}

		p_event.Skip();
	}

	void RenderPanel::onMouseLDClick( wxMouseEvent & p_event )
	{
		m_x = doTransformX( p_event.GetX() );
		m_y = doTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		if ( m_listener )
		{
			m_listener->postEvent( std::make_unique< KeyboardEvent >( *m_keyboardEvent ) );
			auto window = m_renderWindow.lock();

			if ( window )
			{
				wxGetApp().getMainFrame()->toggleFullScreen( !window->isFullscreen() );
			}
		}

		p_event.Skip();
	}

	void RenderPanel::onMouseLdown( wxMouseEvent & p_event )
	{
		m_mouseLeftDown = true;
		m_x = doTransformX( p_event.GetX() );
		m_y = doTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireMouseButtonPushed( MouseButton::eLeft ) )
		{
			auto window = getRenderWindow();

			if ( window )
			{
				if ( m_altdown )
				{
					auto x = m_oldX;
					auto y = m_oldY;
					m_listener->postEvent( makeFunctorEvent( EventType::ePreRender, [this, window, x, y]()
					{
						Camera & camera = *window->getCamera();
						camera.update();
						auto type = window->pick( Position{ int( x ), int( y ) } );

						if ( type != PickNodeType::eNone
							&& type != PickNodeType::eBillboard )
						{
							doUpdateSelectedGeometry( window->getPickedGeometry(), window->getPickedSubmesh() );
						}
						else
						{
							doUpdateSelectedGeometry( nullptr, nullptr );
						}
					} ) );
				}
				else if ( m_currentState )
				{
					doStartTimer( eTIMER_ID_MOUSE );
				}
			}
		}

		p_event.Skip();
	}

	void RenderPanel::onMouseLUp( wxMouseEvent & p_event )
	{
		m_mouseLeftDown = false;
		m_x = doTransformX( p_event.GetX() );
		m_y = doTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireMouseButtonReleased( MouseButton::eLeft ) )
		{
			doStopTimer( eTIMER_ID_MOUSE );
		}

		p_event.Skip();
	}

	void RenderPanel::onMouseMdown( wxMouseEvent & p_event )
	{
		m_mouseMiddleDown = true;
		m_x = doTransformX( p_event.GetX() );
		m_y = doTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireMouseButtonPushed( MouseButton::eMiddle ) )
		{
			auto window = getRenderWindow();

			if ( window )
			{
				auto x = m_oldX;
				auto y = m_oldY;
				m_listener->postEvent( makeFunctorEvent( EventType::ePreRender, [this, window, x, y]()
				{
					auto type = window->pick( Position{ int( x ), int( y ) } );

					if ( type != PickNodeType::eNone
						&& type != PickNodeType::eBillboard )
					{
						doUpdateSelectedGeometry( window->getPickedGeometry(), window->getPickedSubmesh() );
					}
					else
					{
						doUpdateSelectedGeometry( nullptr, nullptr );
					}
				} ) );
			}
		}

		p_event.Skip();
	}

	void RenderPanel::onMouseMUp( wxMouseEvent & p_event )
	{
		m_mouseMiddleDown = false;
		m_x = doTransformX( p_event.GetX() );
		m_y = doTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( inputListener  )
		{
			inputListener->fireMouseButtonReleased( MouseButton::eMiddle );
		}

		p_event.Skip();
	}

	void RenderPanel::onMouseRdown( wxMouseEvent & p_event )
	{
		m_mouseRightDown = true;
		m_x = doTransformX( p_event.GetX() );
		m_y = doTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( inputListener  )
		{
			inputListener->fireMouseButtonPushed( MouseButton::eRight );
		}

		p_event.Skip();
	}

	void RenderPanel::onMouseRUp( wxMouseEvent & p_event )
	{
		m_mouseRightDown = false;
		m_x = doTransformX( p_event.GetX() );
		m_y = doTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( inputListener  )
		{
			inputListener->fireMouseButtonReleased( MouseButton::eRight );
		}

		p_event.Skip();
	}

	void RenderPanel::onMouseMove( wxMouseEvent & p_event )
	{
		m_x = doTransformX( p_event.GetX() );
		m_y = doTransformY( p_event.GetY() );

		if ( !wxGetApp().getCastor()->fireMouseMove( Position{ int32_t( m_x ), int32_t( m_y ) } ) )
		{
			if ( m_currentState )
			{
				static float constexpr mult = 2.0f;
				float deltaX = std::min( m_camSpeed / mult, 1.0f ) * ( m_oldX - m_x ) / mult;
				float deltaY = std::min( m_camSpeed / mult, 1.0f ) * ( m_oldY - m_y ) / mult;

				if ( p_event.ControlDown() )
				{
					deltaX = 0;
				}
				else if ( p_event.ShiftDown() )
				{
					deltaY = 0;
				}

				if ( m_mouseLeftDown )
				{
					m_currentState->addAngularVelocity( castor::Point2f{ -deltaY, deltaX } );
				}
				else if ( m_mouseRightDown )
				{
					m_currentState->addScalarVelocity( castor::Point3f{ deltaX, -deltaY, 0.0f } );
				}
			}
		}

		m_oldX = m_x;
		m_oldY = m_y;
		p_event.Skip();
	}

	void RenderPanel::onMouseWheel( wxMouseEvent & p_event )
	{
		int wheelRotation = p_event.GetWheelRotation();

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireMouseWheel( Position( 0, wheelRotation ) ) )
		{
			if ( wheelRotation < 0 )
			{
				m_camSpeed *= CAM_SPEED_INC;
			}
			else
			{
				m_camSpeed /= CAM_SPEED_INC;
			}

			if ( m_currentState )
			{
				m_currentState->setMaxSpeed( m_camSpeed.value() );
			}
		}

		p_event.Skip();
	}

	void RenderPanel::onMenuClose( wxCommandEvent & p_event )
	{
		Close( true );
		p_event.Skip();
	}
}
