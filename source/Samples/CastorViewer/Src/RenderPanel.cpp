#include "RenderPanel.hpp"
#include "CastorViewer.hpp"
#include "MainFrame.hpp"
#include "RotateNodeEvent.hpp"
#include "TranslateNodeEvent.hpp"
#include "KeyboardEvent.hpp"

#include <wx/display.h>

#include <Event/Frame/FrameListener.hpp>
#include <Event/Frame/FunctorEvent.hpp>
#include <Event/UserInput/UserInputListener.hpp>
#include <Material/Material.hpp>
#include <Material/LegacyPass.hpp>
#include <Material/MetallicRoughnessPbrPass.hpp>
#include <Material/SpecularGlossinessPbrPass.hpp>
#include <Mesh/Submesh.hpp>
#include <Miscellaneous/PickingPass.hpp>
#include <ShadowMap/ShadowMapPass.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>

#include <Core/WindowHandle.hpp>

using namespace castor3d;
using namespace castor;

namespace CastorViewer
{
	namespace
	{
		static const real MAX_CAM_SPEED = 10.0_r;
		static const real DEF_CAM_SPEED = 0.5_r;
		static const real MIN_CAM_SPEED = 0.05_r;
		static const real CAM_SPEED_INC = 0.9_r;
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
		, m_camSpeed( DEF_CAM_SPEED, Range< real >{ MIN_CAM_SPEED, MAX_CAM_SPEED } )
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

	void RenderPanel::Focus()
	{
	}

	void RenderPanel::UnFocus()
	{
	}

	void RenderPanel::DrawOneFrame()
	{
		wxClientDC dc( this );
	}

	void RenderPanel::setRenderWindow( RenderWindowSPtr window )
	{
		m_cubeManager.reset();
		m_renderWindow.reset();
		doStopMovement();
		castor::Size sizeWnd = GuiCommon::makeSize( GetClientSize() );

		if ( window && window->initialise( sizeWnd, GuiCommon::makeWindowHandle( this ) ) )
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
						m_lightsNode = scene->getSceneNodeCache().find( cuT( "PointLightsNode" ) );
					}
					else if ( scene->getSceneNodeCache().has( cuT( "LightNode" ) ) )
					{
						m_lightsNode = scene->getSceneNodeCache().find( cuT( "LightNode" ) );
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
						auto lock = makeUniqueLock( scene->getCameraCache() );
						window->getPickingPass().addScene( *scene, *( scene->getCameraCache().begin()->second ) );
					}

					m_camera = camera;
					doStartMovement();
				}

				m_scene = scene;
			}
		}
		else if ( m_listener )
		{
			m_listener.reset();
		}
	}

	void RenderPanel::doResetTimers()
	{
		doStopTimer( eTIMER_ID_COUNT );
		m_camSpeed = DEF_CAM_SPEED;
		m_x = 0.0_r;
		m_y = 0.0_r;
		m_oldX = 0.0_r;
		m_oldY = 0.0_r;
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
					orientation *= Quaternion::fromAxisAngle( Point3r{ 1.0_r, 0.0_r, 0.0_r }, 90.0_degrees );
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

	real RenderPanel::doTransformX( int x )
	{
		real result = real( x );
		auto camera = m_camera.lock();

		if ( camera )
		{
			result *= real( camera->getWidth() ) / GetClientSize().x;
		}

		return result;
	}

	real RenderPanel::doTransformY( int y )
	{
		real result = real( y );
		auto camera = m_camera.lock();

		if ( camera )
		{
			result *= real( camera->getHeight() ) / GetClientSize().y;
		}

		return result;
	}

	int RenderPanel::doTransformX( real x )
	{
		int result = int( x );
		auto camera = m_camera.lock();

		if ( camera )
		{
			result = int( x * GetClientSize().x / real( camera->getWidth() ) );
		}

		return result;
	}

	int RenderPanel::doTransformY( real y )
	{
		int result = int( y );
		auto camera = m_camera.lock();

		if ( camera )
		{
			result = int( y * GetClientSize().y / real( camera->getHeight() ) );
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
				m_cubeManager->displayObject( *geometry );
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

	GuiCommon::NodeState & RenderPanel::doAddNodeState( SceneNodeSPtr node
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
		EVT_TIMER( eTIMER_ID_FORWARD, RenderPanel::OnTimerFwd )
		EVT_TIMER( eTIMER_ID_BACK, RenderPanel::OnTimerBck )
		EVT_TIMER( eTIMER_ID_LEFT, RenderPanel::OnTimerLft )
		EVT_TIMER( eTIMER_ID_RIGHT, RenderPanel::OnTimerRgt )
		EVT_TIMER( eTIMER_ID_UP, RenderPanel::OnTimerUp )
		EVT_TIMER( eTIMER_ID_DOWN, RenderPanel::OnTimerDwn )
		EVT_TIMER( eTIMER_ID_MOUSE, RenderPanel::OnTimerMouse )
		EVT_TIMER( eTIMER_ID_MOVEMENT, RenderPanel::OnTimerMovement )
		EVT_SIZE( RenderPanel::OnSize )
		EVT_MOVE( RenderPanel::OnMove )
		EVT_PAINT( RenderPanel::OnPaint )
		EVT_ENTER_WINDOW( RenderPanel::OnEnterWindow )
		EVT_LEAVE_WINDOW( RenderPanel::OnLeaveWindow )
		EVT_ERASE_BACKGROUND( RenderPanel::OnEraseBackground )
		EVT_SET_FOCUS( RenderPanel::OnsetFocus )
		EVT_KILL_FOCUS( RenderPanel::OnKillFocus )
		EVT_KEY_DOWN( RenderPanel::onKeydown )
		EVT_KEY_UP( RenderPanel::OnKeyUp )
		EVT_CHAR( RenderPanel::OnChar )
		EVT_LEFT_DCLICK( RenderPanel::OnMouseLDClick )
		EVT_LEFT_DOWN( RenderPanel::OnMouseLdown )
		EVT_LEFT_UP( RenderPanel::OnMouseLUp )
		EVT_MIDDLE_DOWN( RenderPanel::OnMouseMdown )
		EVT_MIDDLE_UP( RenderPanel::OnMouseMUp )
		EVT_RIGHT_DOWN( RenderPanel::OnMouseRdown )
		EVT_RIGHT_UP( RenderPanel::OnMouseRUp )
		EVT_MOTION( RenderPanel::OnMouseMove )
		EVT_MOUSEWHEEL( RenderPanel::OnMouseWheel )
		EVT_MENU( wxID_EXIT, RenderPanel::OnMenuClose )
	END_EVENT_TABLE()

	void RenderPanel::OnTimerFwd( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->addScalarVelocity( Point3r{ 0.0_r, 0.0_r, m_camSpeed.value() } );
		}

		p_event.Skip();
	}

	void RenderPanel::OnTimerBck( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->addScalarVelocity( Point3r{ 0.0_r, 0.0_r, -m_camSpeed.value() } );
		}

		p_event.Skip();
	}

	void RenderPanel::OnTimerLft( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->addScalarVelocity( Point3r{ m_camSpeed.value(), 0.0_r, 0.0_r } );
		}

		p_event.Skip();
	}

	void RenderPanel::OnTimerRgt( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->addScalarVelocity( Point3r{ -m_camSpeed.value(), 0.0_r, 0.0_r } );
		}

		p_event.Skip();
	}

	void RenderPanel::OnTimerUp( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->addScalarVelocity( Point3r{ 0.0_r, m_camSpeed.value(), 0.0_r } );
		}

		p_event.Skip();
	}

	void RenderPanel::OnTimerDwn( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->addScalarVelocity( Point3r{ 0.0_r, -m_camSpeed.value(), 0.0_r } );
		}

		p_event.Skip();
	}

	void RenderPanel::OnTimerMouse( wxTimerEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::OnTimerMovement( wxTimerEvent & p_event )
	{
		if ( m_currentState )
		{
			m_currentState->update();
		}

		p_event.Skip();
	}

	void RenderPanel::OnSize( wxSizeEvent & p_event )
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

	void RenderPanel::OnMove( wxMoveEvent & p_event )
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

	void RenderPanel::OnPaint( wxPaintEvent & p_event )
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

	void RenderPanel::OnsetFocus( wxFocusEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::OnKillFocus( wxFocusEvent & p_event )
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

	void RenderPanel::OnKeyUp( wxKeyEvent & p_event )
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
				doUpdateSelectedGeometry( nullptr, nullptr );
				break;

			case 'L':
				m_currentNode = m_camera.lock()->getParent();
				m_currentState = &doAddNodeState( m_currentNode, false );
				break;
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnChar( wxKeyEvent & p_event )
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

	void RenderPanel::OnMouseLDClick( wxMouseEvent & p_event )
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

	void RenderPanel::OnMouseLdown( wxMouseEvent & p_event )
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
						auto type = window->getPickingPass().pick( Position{ int( x ), int( y ) }, camera );

						if ( type != PickingPass::NodeType::eNone
							&& type != PickingPass::NodeType::eBillboard )
						{
							doUpdateSelectedGeometry( window->getPickingPass().getPickedGeometry(), window->getPickingPass().getPickedSubmesh() );
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

	void RenderPanel::OnMouseLUp( wxMouseEvent & p_event )
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

	void RenderPanel::OnMouseMdown( wxMouseEvent & p_event )
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
					Camera & camera = *window->getCamera();
					camera.update();
					auto type = window->getPickingPass().pick( Position{ int( x ), int( y ) }, camera );

					if ( type != PickingPass::NodeType::eNone
						&& type != PickingPass::NodeType::eBillboard )
					{
						doUpdateSelectedGeometry( window->getPickingPass().getPickedGeometry(), window->getPickingPass().getPickedSubmesh() );
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

	void RenderPanel::OnMouseMUp( wxMouseEvent & p_event )
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

	void RenderPanel::OnMouseRdown( wxMouseEvent & p_event )
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

	void RenderPanel::OnMouseRUp( wxMouseEvent & p_event )
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

	void RenderPanel::OnMouseMove( wxMouseEvent & p_event )
	{
		m_x = doTransformX( p_event.GetX() );
		m_y = doTransformY( p_event.GetY() );

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireMouseMove( Position( int32_t( m_x ), int32_t( m_y ) ) ) )
		{
			if ( m_currentState )
			{
				static real constexpr mult = 2.0_r;
				real deltaX = std::min( m_camSpeed / mult, 1.0_r ) * ( m_oldX - m_x ) / mult;
				real deltaY = std::min( m_camSpeed / mult, 1.0_r ) * ( m_oldY - m_y ) / mult;

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
					m_currentState->addAngularVelocity( Point2r{ -deltaY, deltaX } );
				}
				else if ( m_mouseRightDown )
				{
					m_currentState->addScalarVelocity( Point3r{ deltaX, -deltaY, 0.0_r } );
				}
			}
		}

		m_oldX = m_x;
		m_oldY = m_y;
		p_event.Skip();
	}

	void RenderPanel::OnMouseWheel( wxMouseEvent & p_event )
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

	void RenderPanel::OnMenuClose( wxCommandEvent & p_event )
	{
		Close( true );
		p_event.Skip();
	}
}
