#include "CastorViewer/RenderPanel.hpp"
#include "CastorViewer/CastorViewer.hpp"
#include "CastorViewer/MainFrame.hpp"
#include "CastorViewer/RotateNodeEvent.hpp"
#include "CastorViewer/TranslateNodeEvent.hpp"

#include <wx/display.h>

#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Event/UserInput/UserInputListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Render/ShadowMap/ShadowMapPass.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

#include <ashespp/Core/WindowHandle.hpp>

CU_ImplementSmartPtr( CastorViewer, MouseNodeEvent )
CU_ImplementSmartPtr( CastorViewer, TranslateNodeEvent )

namespace CastorViewer
{
	namespace panel
	{
		static const float MAX_CAM_SPEED = 10.0f;
		static const float DEF_CAM_SPEED = 0.5f;
		static const float MIN_CAM_SPEED = 0.05f;
		static const float CAM_SPEED_INC = 0.9f;

		static castor3d::KeyboardKey doConvertKeyCode( int code )
		{
			castor3d::KeyboardKey result = castor3d::KeyboardKey::eNone;

			if ( code < 0x20 )
			{
				switch ( code )
				{
				case WXK_BACK:
				case WXK_TAB:
				case WXK_RETURN:
				case WXK_ESCAPE:
					result = castor3d::KeyboardKey( code );
					break;
				}
			}
			else if ( code == 0x7F )
			{
				result = castor3d::KeyboardKey::eDelete;
			}
			else if ( code > 0xFF )
			{
				result = castor3d::KeyboardKey( code + int( castor3d::KeyboardKey::eStart ) - WXK_START );
			}
			else
			{
				// ASCII or extended ASCII character
				result = castor3d::KeyboardKey( code );
			}

			return result;
		}

		static std::array< wxTimer *, eTIMER_ID_COUNT > createTimers( wxWindow * window )
		{
			std::array< wxTimer *, eTIMER_ID_COUNT > result;
			result[0] = nullptr;

			for ( int i = 1; i < eTIMER_ID_COUNT; i++ )
			{
				result[size_t( i )] = new wxTimer( window, i );
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
		, m_timers{ panel::createTimers( this ) }
		, m_camSpeed( panel::DEF_CAM_SPEED, castor::Range< float >{ panel::MIN_CAM_SPEED, panel::MAX_CAM_SPEED } )
	{
		m_renderWindow = std::make_unique< castor3d::RenderWindow >( cuT( "RenderPanel" )
			, *wxGetApp().getCastor()
			, GuiCommon::makeSize( GetClientSize() )
			, GuiCommon::makeWindowHandle( this ) );
		auto listener = wxGetApp().getCastor()->getUserInputListener();
		listener->registerClipboardTextAction( [this]( bool set
			, castor::U32String text )
			{
				m_setClipboardText = set;
				wxMenuEvent event{ wxEVT_MENU, eCLIPBOARD_CHANGE };
				event.SetEventObject( this );

				if ( set )
				{
					{
						auto lock( castor::makeUniqueLock( m_mtxClipSet ) );
						m_clipSet = text;
					}
					ProcessThreadEvent( event );
				}
				else
				{
					m_clipGet = {};
					auto f = m_clipGet.get_future();
					ProcessThreadEvent( event );
					f.wait();
					text = f.get();
				}

				return text;
			} );
		listener->registerCursorAction( [this]( castor3d::MouseCursor cursor )
			{
				if ( m_cursor != cursor )
				{
					m_cursor = cursor;

					switch ( cursor )
					{
					case castor3d::MouseCursor::eArrow:
						SetCursor( wxCursor{ wxCURSOR_ARROW } );
						break;
					case castor3d::MouseCursor::eHand:
						SetCursor( wxCursor{ wxCURSOR_HAND } );
						break;
					case castor3d::MouseCursor::eText:
						SetCursor( wxCursor{ wxCURSOR_IBEAM } );
						break;
					case castor3d::MouseCursor::eSizeWE:
						SetCursor( wxCursor{ wxCURSOR_SIZEWE } );
						break;
					case castor3d::MouseCursor::eSizeNS:
						SetCursor( wxCursor{ wxCURSOR_SIZENS } );
						break;
					case castor3d::MouseCursor::eSizeNWSE:
						SetCursor( wxCursor{ wxCURSOR_SIZENWSE } );
						break;
					case castor3d::MouseCursor::eSizeNESW:
						SetCursor( wxCursor{ wxCURSOR_SIZENESW } );
						break;
					default:
						castor3d::log::error << "Unsupported MouseCursor." << std::endl;
						SetCursor( wxCursor{ wxCURSOR_ARROW } );
						break;
					}
				}
			} );
	}

	RenderPanel::~RenderPanel()
	{
		for ( size_t i = 1; i <= size_t( eTIMER_ID_MOVEMENT ); i++ )
		{
			delete m_timers[i];
			m_timers[i] = nullptr;
		}

		m_renderWindow.reset();
	}

	void RenderPanel::reset()
	{
		castor::Logger::logInfo( cuT( "Cleaning up RenderPanel." ) );
		doStopMovement();
		m_selectedSubmesh = {};
		m_selectedGeometry = {};
		m_currentState = nullptr;
		m_nodesStates.clear();
		m_camera = {};
		m_scene = {};
		m_currentNode = nullptr;
		m_lightsNode = nullptr;
		m_listener = {};
		m_cubeManager.reset();
		wxGetApp().getCastor()->postEvent( makeCpuCleanupEvent( *m_renderWindow ) );
		castor::Logger::logInfo( cuT( "RenderPanel cleaned up." ) );
	}

	void RenderPanel::setTarget( castor3d::RenderTargetRPtr target )
	{
		m_listener = m_renderWindow->getListener();
		m_renderWindow->initialise( *target );
		auto scene = target->getScene();

		if ( scene )
		{
			m_cubeManager = std::make_unique< GuiCommon::CubeBoxManager >( *scene );
			auto camera = target->getCamera();

			if ( camera )
			{
				if ( scene->hasSceneNode( cuT( "PointLightsNode" ) ) )
				{
					m_lightsNode = scene->findSceneNode( cuT( "PointLightsNode" ) );
				}
				else if ( scene->hasSceneNode( cuT( "LightNode" ) ) )
				{
					m_lightsNode = scene->findSceneNode( cuT( "LightNode" ) );
				}

				auto cameraNode = camera->getParent();

				if ( cameraNode )
				{
					m_currentNode = cameraNode;
					m_currentState = &doAddNodeState( m_currentNode, true );
				}

				m_renderWindow->addPickingScene( *scene );
				m_camera = camera;
				doStartMovement();
			}

			m_scene = scene;
		}
	}

	void RenderPanel::doResetTimers()
	{
		doStopTimer( eTIMER_ID_COUNT );
		m_camSpeed = panel::DEF_CAM_SPEED;
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

	void RenderPanel::doStartTimer( int id )
	{
		m_timers[size_t( id )]->Start( 10 );
	}

	void RenderPanel::doStopTimer( int id )
	{
		if ( id != eTIMER_ID_COUNT )
		{
			m_timers[size_t( id )]->Stop();
		}
		else
		{
			for ( size_t i = 1; i < size_t( eTIMER_ID_MOVEMENT ); i++ )
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
			m_currentState->reset( panel::DEF_CAM_SPEED );
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
		auto camera = m_camera;

		if ( camera )
		{
			auto cameraNode = camera->getParent();
			camera->getScene()->getListener().postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
				, [cameraNode]()
				{
					castor::Quaternion orientation{ cameraNode->getOrientation() };
					orientation *= castor::Quaternion::fromAxisAngle( castor::Point3f{ 1.0f, 0.0f, 0.0f }, 90.0_degrees );
					cameraNode->setOrientation( orientation );
				} ) );
		}
	}

	void RenderPanel::doChangeCamera()
	{
		auto camera = m_camera;
		auto scene = m_scene;

		if ( camera && scene )
		{
			auto & cache = scene->getCameraCache();
			auto lock = castor::makeUniqueLock( cache );
			auto it = std::find_if( cache.begin()
				, cache.end()
				, [camera]( auto const & lookup )
				{
					return camera->getName() == lookup.first;
				} );

			if ( it != cache.end() )
			{
				++it;
			}

			if ( it == cache.end() )
			{
				it = cache.begin();
			}

			m_camera = it->second.get();
			m_currentNode = camera->getParent();
			m_currentState = &doAddNodeState( m_currentNode, true );
			m_renderWindow->setCamera( *m_camera );
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
		auto camera = m_camera;

		if ( camera )
		{
			result *= float( camera->getWidth() ) / float( GetClientSize().x );
		}

		return result;
	}

	float RenderPanel::doTransformY( int y )
	{
		float result = float( y );
		auto camera = m_camera;

		if ( camera )
		{
			result *= float( camera->getHeight() ) / float( GetClientSize().y );
		}

		return result;
	}

	int RenderPanel::doTransformX( float x )
	{
		int result = int( x );
		auto camera = m_camera;

		if ( camera )
		{
			result = int( x * float( GetClientSize().x ) / float( camera->getWidth() ) );
		}

		return result;
	}

	int RenderPanel::doTransformY( float y )
	{
		int result = int( y );
		auto camera = m_camera;

		if ( camera )
		{
			result = int( y * float( GetClientSize().y ) / float( camera->getHeight() ) );
		}

		return result;
	}

	void RenderPanel::doUpdateSelectedGeometry( castor3d::GeometryRPtr geometry
		, castor3d::Submesh const * submesh )
	{
		auto oldSubmesh = m_selectedSubmesh;
		auto oldGeometry = m_selectedGeometry;

		if ( oldGeometry != geometry )
		{
			m_selectedSubmesh = nullptr;

			if ( oldGeometry )
			{
				m_cubeManager->hideObject( *oldGeometry );
			}

			if ( geometry )
			{
				m_cubeManager->displayObject( *geometry, *submesh );
			}

			m_selectedGeometry = geometry;
		}

		if ( oldSubmesh != submesh )
		{
			if ( submesh )
			{
				m_selectedSubmesh = submesh;
				wxGetApp().getMainFrame()->select( m_selectedGeometry, m_selectedSubmesh );
			}
		}

		if ( m_selectedGeometry )
		{
			m_currentNode = m_selectedGeometry->getParent();
		}
		else if ( auto camera = m_camera )
		{
			m_currentNode = camera->getParent();
		}

		m_currentState = &doAddNodeState( m_currentNode, false );
	}

	GuiCommon::NodeState & RenderPanel::doAddNodeState( castor3d::SceneNodeRPtr node
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

	void RenderPanel::doUpdateMaxSpeed( float factor )
	{
		for ( auto & nodeState : m_nodesStates )
		{
			nodeState.second->multMaxSpeed( factor );
		}
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
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
		EVT_SET_FOCUS( RenderPanel::onSetFocus )
		EVT_KILL_FOCUS( RenderPanel::onKillFocus )
		EVT_KEY_DOWN( RenderPanel::onKeyDown )
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
		EVT_MENU( eCLIPBOARD_CHANGE, RenderPanel::onClipboardText )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void RenderPanel::onTimerFwd( wxTimerEvent & event )
	{
		if ( m_currentState )
		{
			auto speed = m_camSpeed.value() * ( m_extraSpeed ? 10.0f : 1.0f );
			m_currentState->addScalarVelocity( castor::Point3f{ 0.0f, 0.0f, speed } );
		}

		event.Skip();
	}

	void RenderPanel::onTimerBck( wxTimerEvent & event )
	{
		if ( m_currentState )
		{
			auto speed = m_camSpeed.value() * ( m_extraSpeed ? 10.0f : 1.0f );
			m_currentState->addScalarVelocity( castor::Point3f{ 0.0f, 0.0f, -speed } );
		}

		event.Skip();
	}

	void RenderPanel::onTimerLft( wxTimerEvent & event )
	{
		if ( m_currentState )
		{
			auto speed = m_camSpeed.value() * ( m_extraSpeed ? 10.0f : 1.0f );
			m_currentState->addScalarVelocity( castor::Point3f{ speed, 0.0f, 0.0f } );
		}

		event.Skip();
	}

	void RenderPanel::onTimerRgt( wxTimerEvent & event )
	{
		if ( m_currentState )
		{
			auto speed = m_camSpeed.value() * ( m_extraSpeed ? 10.0f : 1.0f );
			m_currentState->addScalarVelocity( castor::Point3f{ -speed, 0.0f, 0.0f } );
		}

		event.Skip();
	}

	void RenderPanel::onTimerUp( wxTimerEvent & event )
	{
		if ( m_currentState )
		{
			auto speed = m_camSpeed.value() * ( m_extraSpeed ? 10.0f : 1.0f );
			m_currentState->addScalarVelocity( castor::Point3f{ 0.0f, speed, 0.0f } );
		}

		event.Skip();
	}

	void RenderPanel::onTimerDwn( wxTimerEvent & event )
	{
		if ( m_currentState )
		{
			auto speed = m_camSpeed.value() * ( m_extraSpeed ? 10.0f : 1.0f );
			m_currentState->addScalarVelocity( castor::Point3f{ 0.0f, -speed, 0.0f } );
		}

		event.Skip();
	}

	void RenderPanel::onTimerMouse( wxTimerEvent & event )
	{
		event.Skip();
	}

	void RenderPanel::onTimerMovement( wxTimerEvent & event )
	{
		if ( m_currentState )
		{
			m_currentState->update();
		}

		event.Skip();
	}

	void RenderPanel::onSize( wxSizeEvent & event )
	{
		if ( m_resizeWindow && m_renderWindow )
		{
			m_renderWindow->resize( uint32_t( event.GetSize().x )
				, uint32_t( event.GetSize().y ) );
		}

		event.Skip();
	}

	void RenderPanel::onMove( wxMoveEvent & event )
	{
		event.Skip();
	}

	void RenderPanel::onPaint( wxPaintEvent & event )
	{
		event.Skip();
	}

	void RenderPanel::onEnterWindow( wxMouseEvent & event )
	{
		event.Skip();
	}

	void RenderPanel::onLeaveWindow( wxMouseEvent & event )
	{
		event.Skip();
	}

	void RenderPanel::onEraseBackground( wxEraseEvent & event )
	{
		event.Skip();
	}

	void RenderPanel::onSetFocus( wxFocusEvent & event )
	{
		event.Skip();
	}

	void RenderPanel::onKillFocus( wxFocusEvent & event )
	{
		doStopTimer( eTIMER_ID_COUNT );
		event.Skip();
	}

	void RenderPanel::onKeyDown( wxKeyEvent & event )
	{
		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireKeydown( panel::doConvertKeyCode( event.GetKeyCode() )
			, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			switch ( event.GetKeyCode() )
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

			case 'E':
				m_extraSpeed = true;
				doUpdateMaxSpeed( 10.0f );
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

		event.Skip();
	}

	void RenderPanel::onKeyUp( wxKeyEvent & event )
	{
		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireKeyUp( panel::doConvertKeyCode( event.GetKeyCode() )
			, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			switch ( event.GetKeyCode() )
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

			case 'E':
				m_extraSpeed = false;
				doUpdateMaxSpeed( 0.1f );
				break;

			case WXK_ESCAPE:
				if ( m_selectedGeometry )
				{
					doUpdateSelectedGeometry( nullptr, nullptr );
				}
				else if ( m_parent )
				{
					m_parent->Close();
				}
				break;

			case 'L':
				if ( auto camera = m_camera )
				{
					m_currentNode = camera->getParent();
					m_currentState = &doAddNodeState( m_currentNode, false );
				}
				break;
			}
		}

		event.Skip();
	}

	void RenderPanel::onChar( wxKeyEvent & event )
	{
		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( inputListener )
		{
			wxChar key = event.GetUnicodeKey();
			wxString tmp;
			tmp << key;
			inputListener->fireChar( panel::doConvertKeyCode( event.GetKeyCode() )
				, castor::String( tmp.mb_str( wxConvUTF8 ) ) );
		}

		event.Skip();
	}

	void RenderPanel::onMouseLDClick( wxMouseEvent & event )
	{
		m_x = doTransformX( event.GetX() );
		m_y = doTransformY( event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		if ( m_listener )
		{
			m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
				, [this]()
				{
					if ( m_renderWindow )
					{
						m_renderWindow->enableFullScreen( !m_renderWindow->isFullscreen() );
					}
				} ) );
			wxGetApp().getMainFrame()->toggleFullScreen( !m_renderWindow->isFullscreen() );
		}

		event.Skip();
	}

	void RenderPanel::onMouseLdown( wxMouseEvent & event )
	{
		m_mouseLeftDown = true;
		m_x = doTransformX( event.GetX() );
		m_y = doTransformY( event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireMouseButtonPushed( castor3d::MouseButton::eLeft
			, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			if ( m_currentState )
			{
				doStartTimer( eTIMER_ID_MOUSE );
			}
		}

		event.Skip();
	}

	void RenderPanel::onMouseLUp( wxMouseEvent & event )
	{
		m_mouseLeftDown = false;
		m_x = doTransformX( event.GetX() );
		m_y = doTransformY( event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireMouseButtonReleased( castor3d::MouseButton::eLeft
			, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			doStopTimer( eTIMER_ID_MOUSE );
		}

		event.Skip();
	}

	void RenderPanel::onMouseMdown( wxMouseEvent & event )
	{
		m_mouseMiddleDown = true;
		m_x = doTransformX( event.GetX() );
		m_y = doTransformY( event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireMouseButtonPushed( castor3d::MouseButton::eMiddle
			, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			auto x = m_oldX;
			auto y = m_oldY;
			m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePreRender
				, [this, x, y]()
				{
					auto type = m_renderWindow->pick( castor::Position{ int( x ), int( y ) } );

					if ( type != castor3d::PickNodeType::eNone
						&& type != castor3d::PickNodeType::eBillboard )
					{
						doUpdateSelectedGeometry( m_renderWindow->getPickedGeometry(), m_renderWindow->getPickedSubmesh() );
					}
					else
					{
						doUpdateSelectedGeometry( nullptr, nullptr );
					}
				} ) );
		}

		event.Skip();
	}

	void RenderPanel::onMouseMUp( wxMouseEvent & event )
	{
		m_mouseMiddleDown = false;
		m_x = doTransformX( event.GetX() );
		m_y = doTransformY( event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( inputListener  )
		{
			inputListener->fireMouseButtonReleased( castor3d::MouseButton::eMiddle
				, event.ControlDown(), event.AltDown(), event.ShiftDown() );
		}

		event.Skip();
	}

	void RenderPanel::onMouseRdown( wxMouseEvent & event )
	{
		m_mouseRightDown = true;
		m_x = doTransformX( event.GetX() );
		m_y = doTransformY( event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( inputListener  )
		{
			inputListener->fireMouseButtonPushed( castor3d::MouseButton::eRight
				, event.ControlDown(), event.AltDown(), event.ShiftDown() );
		}

		event.Skip();
	}

	void RenderPanel::onMouseRUp( wxMouseEvent & event )
	{
		m_mouseRightDown = false;
		m_x = doTransformX( event.GetX() );
		m_y = doTransformY( event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( inputListener  )
		{
			inputListener->fireMouseButtonReleased( castor3d::MouseButton::eRight
				, event.ControlDown(), event.AltDown(), event.ShiftDown() );
		}

		event.Skip();
	}

	void RenderPanel::onMouseMove( wxMouseEvent & event )
	{
		m_x = doTransformX( event.GetX() );
		m_y = doTransformY( event.GetY() );

		if ( !wxGetApp().getCastor()->fireMouseMove( castor::Position{ int32_t( m_x ), int32_t( m_y ) }
			, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			if ( m_currentState )
			{
				static float constexpr mult = 8.0f;
				float deltaX = ( m_oldX - m_x ) / mult;
				float deltaY = ( m_oldY - m_y ) / mult;

				if ( event.ControlDown() )
				{
					deltaX = 0;
				}
				else if ( event.ShiftDown() )
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
		event.Skip();
	}

	void RenderPanel::onMouseWheel( wxMouseEvent & event )
	{
		int wheelRotation = event.GetWheelRotation();

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireMouseWheel( castor::Position( 0, wheelRotation )
			, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			if ( wheelRotation < 0 )
			{
				m_camSpeed *= panel::CAM_SPEED_INC;
			}
			else
			{
				m_camSpeed /= panel::CAM_SPEED_INC;
			}

			if ( m_currentState )
			{
				m_currentState->setMaxSpeed( m_camSpeed.value() );
			}
		}

		event.Skip();
	}

	void RenderPanel::onMenuClose( wxCommandEvent & event )
	{
		Close( true );
		event.Skip();
	}

	void RenderPanel::onClipboardText( wxCommandEvent & event )
	{
		if ( wxTheClipboard->Open() )
		{
			if ( m_setClipboardText )
			{
				castor::U32String text;
				{
					auto lock( castor::makeUniqueLock( m_mtxClipSet ) );
					text = m_clipSet;
				}
				wxTheClipboard->SetData( new wxTextDataObject{ GuiCommon::make_wxString( text ) } );
			}
			else
			{
				wxTextDataObject data;
				wxTheClipboard->GetData( data );
				m_clipGet.set_value( GuiCommon::make_U32String( data.GetText() ) );
			}

			wxTheClipboard->Close();
		}
	}
}
