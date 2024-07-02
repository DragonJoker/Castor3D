#include "CastorViewer/RenderPanel.hpp"
#include "CastorViewer/CastorViewer.hpp"
#include "CastorViewer/MainFrame.hpp"

#include <GuiCommon/System/SpaceMouseController.hpp>

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
#include <Castor3D/Scene/Light/Light.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

#include <ashespp/Core/WindowHandle.hpp>

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
				default:
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

		static castor::Array< wxTimer *, size_t( eTIMER_ID::COUNT ) > createTimers( wxWindow * window )
		{
			castor::Array< wxTimer *, size_t( eTIMER_ID::COUNT ) > result;
			result[0] = nullptr;

			for ( int i = 1; i < int( eTIMER_ID::COUNT ); i++ )
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
		m_renderWindow = castor::makeUnique< castor3d::RenderWindow >( cuT( "RenderPanel" )
			, *wxGetApp().getCastor()
			, GuiCommon::makeSize( GetClientSize() )
			, GuiCommon::makeWindowHandle( this ) );
		auto listener = wxGetApp().getCastor()->getUserInputListener();
		listener->registerClipboardTextAction( [this]( bool set
			, castor::U32String text )
			{
				m_setClipboardText = set;
				wxMenuEvent event{ wxEVT_MENU, int( eTIMER_ID::CLIPBOARD_CHANGE ) };
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
		m_3dController = GuiCommon::I3DController::create( wxGetApp().getInternalName()
			, listener->getFrameListener() );
	}

	RenderPanel::~RenderPanel()
	{
		m_3dController.reset();

		for ( size_t i = 1; i <= size_t( eTIMER_ID::MOVEMENT ); i++ )
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

		if ( m_3dController )
		{
			m_3dController->setGeometry( nullptr );
			m_3dController->setCamera( nullptr );
		}

		m_nodesStates.clear();
		m_camera = {};
		m_scene = {};
		m_currentNode = nullptr;
		m_lightsNode = nullptr;
		m_listener = {};
		m_debugMeshManager.reset();
		wxGetApp().getCastor()->postEvent( makeCpuCleanupEvent( *m_renderWindow ) );
		castor::Logger::logInfo( cuT( "RenderPanel cleaned up." ) );
	}

	void RenderPanel::select( castor3d::Geometry * geometry, castor3d::Submesh const * submesh )
	{
		if ( m_debugMeshManager )
		{
			doUpdateSelectedGeometry( geometry, submesh, false );
		}
	}

	void RenderPanel::select( castor3d::Light * light )
	{
		if ( m_debugMeshManager )
		{
			if ( light )
			{
				m_debugMeshManager->select( *light );
				select( light->getParent(), false );
			}
			else
			{
				m_debugMeshManager->unselect();
				select( m_camera->getParent(), true );
			}
		}
	}

	void RenderPanel::select( castor3d::SceneNode * node
		, bool cameraNode )
	{
		m_currentNode = node;

		if ( m_currentNode )
		{
			m_currentState = &doAddNodeState( m_currentNode, cameraNode );
		}
		else if ( m_camera )
		{
			select( m_camera->getParent(), true );
		}
	}

	void RenderPanel::select( castor3d::Camera * camera )
	{
		if ( m_camera )
		{
			doStopMovement();
		}

		if ( auto cameraNode = camera->getParent() )
		{
			select( cameraNode, true );

			if ( m_3dController )
			{
				m_3dController->setCamera( camera );
			}
		}

		m_camera = camera;
		m_renderWindow->setCamera( *m_camera );
		doStartMovement();
	}

	void RenderPanel::updateWindow( castor3d::RenderWindowDesc const & window )
	{
		auto target = window.renderTarget;

		if ( !target )
		{

			CU_Failure( "RenderPanel - No render target" );
			castor::Logger::logError( cuT( "RenderPanel - No render target" ) );
			return;
		}

		m_listener = m_renderWindow->getListener();
		m_renderWindow->initialise( window );

		if ( auto scene = target->getScene() )
		{
			m_debugMeshManager = castor::make_unique< GuiCommon::DebugMeshManager >( *target );

			if ( scene->hasSceneNode( cuT( "PointLightsNode" ) ) )
			{
				m_lightsNode = scene->findSceneNode( cuT( "PointLightsNode" ) );
			}
			else if ( scene->hasSceneNode( cuT( "LightNode" ) ) )
			{
				m_lightsNode = scene->findSceneNode( cuT( "LightNode" ) );
			}

			if ( auto camera = target->getCamera() )
			{
				select( camera );
			}

			m_scene = scene;
		}
	}

	void RenderPanel::doResetTimers()
	{
		doStopTimer( eTIMER_ID::COUNT );
		m_camSpeed = castor::makeRangedValue( panel::DEF_CAM_SPEED
			, panel::MIN_CAM_SPEED
			, panel::MAX_CAM_SPEED );
		m_x = 0.0f;
		m_y = 0.0f;
		m_oldX = 0.0f;
		m_oldY = 0.0f;

		if ( m_3dController )
		{
			m_3dController->setSpeedFactor( doGetRealSpeed() );
		}
	}

	void RenderPanel::doStartMovement()
	{
		if ( !m_movementStarted )
		{
			m_timers[size_t( eTIMER_ID::MOVEMENT )]->Start( 30 );
			m_movementStarted = true;
		}
	}

	void RenderPanel::doStopMovement()
	{
		if ( m_movementStarted )
		{
			m_movementStarted = false;
			m_timers[size_t( eTIMER_ID::MOVEMENT )]->Stop();
		}
	}

	void RenderPanel::doStartTimer( eTIMER_ID id )
	{
		if ( !m_timers[size_t( id )]->Start( 10 ) )
		{
			castor3d::log::error << "Couldn't start timer: " << castor::system::getLastErrorText() << "\n";
		}
	}

	void RenderPanel::doStopTimer( eTIMER_ID id )
	{
		if ( id != eTIMER_ID::COUNT )
		{
			m_timers[size_t( id )]->Stop();
		}
		else
		{
			for ( size_t i = 1; i < size_t( eTIMER_ID::MOVEMENT ); i++ )
			{
				m_timers[i]->Stop();
			}
		}
	}

	float RenderPanel::doGetRealSpeed()const noexcept
	{
		return m_camSpeed.value()
			* ( m_extraSpeed
				? 10.0f
				: 1.0f );
	}

	void RenderPanel::doResetNode()
	{
		doResetTimers();

		if ( m_currentState )
		{
			m_currentState->reset( doGetRealSpeed() );

			if ( m_currentState->isCamera() && m_3dController )
			{
				m_3dController->reset();
				m_3dController->setSpeedFactor( doGetRealSpeed() );
			}
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

		if ( m_currentState )
		{
			m_currentState->pitch( 90.0_degrees );
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

			select( it->second.get() );
		}
	}

	void RenderPanel::doReloadScene()
	{
		doResetTimers();
		wxGetApp().getMainFrame()->loadScene();
	}

	float RenderPanel::doTransformX( int x )
	{
		auto result = float( x );

		if ( auto camera = m_camera )
		{
			result *= float( camera->getWidth() ) / float( GetClientSize().x );
		}

		return result;
	}

	float RenderPanel::doTransformY( int y )
	{
		auto result = float( y );

		if ( auto camera = m_camera )
		{
			result *= float( camera->getHeight() ) / float( GetClientSize().y );
		}

		return result;
	}

	int RenderPanel::doTransformX( float x )
	{
		auto result = int( x );

		if ( auto camera = m_camera )
		{
			result = int( x * float( GetClientSize().x ) / float( camera->getWidth() ) );
		}

		return result;
	}

	int RenderPanel::doTransformY( float y )
	{
		auto result = int( y );

		if ( auto camera = m_camera )
		{
			result = int( y * float( GetClientSize().y ) / float( camera->getHeight() ) );
		}

		return result;
	}

	void RenderPanel::doUpdateSelectedGeometry( castor3d::Geometry const * geometry
		, castor3d::Submesh const * submesh
		, bool forwardToMain )
	{
		auto oldSubmesh = m_selectedSubmesh;

		if ( auto oldGeometry = m_selectedGeometry;
			oldGeometry != geometry || oldSubmesh != submesh )
		{
			m_selectedSubmesh = nullptr;

			if ( oldGeometry )
			{
				m_debugMeshManager->unselect();
			}

			if ( geometry )
			{
				m_debugMeshManager->select( *geometry, *submesh );
			}

			m_selectedGeometry = geometry;
		}

		if ( oldSubmesh != submesh )
		{
			if ( submesh )
			{
				m_selectedSubmesh = submesh;

				if ( forwardToMain )
				{
					wxGetApp().getMainFrame()->select( m_selectedGeometry, m_selectedSubmesh );
				}
			}
		}

		if ( m_selectedGeometry )
		{
			select( m_selectedGeometry->getParent() );

			if ( m_3dController )
			{
				m_3dController->setGeometry( m_selectedGeometry );
			}
		}
		else if ( auto camera = m_camera )
		{
			select( camera->getParent() );

			if ( m_3dController )
			{
				m_3dController->setCamera( camera );
			}
		}
	}

	GuiCommon::NodeState & RenderPanel::doAddNodeState( castor3d::SceneNodeRPtr node
		, bool camera )
	{
		auto it = m_nodesStates.find( node->getName() );

		if ( it == m_nodesStates.end() )
		{
			it = m_nodesStates.try_emplace( node->getName()
				, castor::make_unique< GuiCommon::NodeState >( *m_listener, node, camera ) ).first;
		}

		doUpdateSpeed();
		return *it->second;
	}

	void RenderPanel::doUpdateSpeed( float factor )
	{
		m_camSpeed *= factor;

		if ( m_currentState )
		{
			m_currentState->setMaxSpeed( doGetRealSpeed() );
		}

		if ( m_3dController )
		{
			m_3dController->setSpeedFactor( doGetRealSpeed() );
		}
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( RenderPanel, wxPanel )
		EVT_TIMER( int( eTIMER_ID::FORWARD ), RenderPanel::onTimerFwd )
		EVT_TIMER( int( eTIMER_ID::BACK ), RenderPanel::onTimerBck )
		EVT_TIMER( int( eTIMER_ID::LEFT ), RenderPanel::onTimerLft )
		EVT_TIMER( int( eTIMER_ID::RIGHT ), RenderPanel::onTimerRgt )
		EVT_TIMER( int( eTIMER_ID::UP ), RenderPanel::onTimerUp )
		EVT_TIMER( int( eTIMER_ID::DOWN ), RenderPanel::onTimerDwn )
		EVT_TIMER( int( eTIMER_ID::MOUSE ), RenderPanel::onTimerMouse )
		EVT_TIMER( int( eTIMER_ID::MOVEMENT ), RenderPanel::onTimerMovement )
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
		EVT_LEFT_DOWN( RenderPanel::onMouseLDown )
		EVT_LEFT_UP( RenderPanel::onMouseLUp )
		EVT_MIDDLE_DOWN( RenderPanel::onMouseMDown )
		EVT_MIDDLE_UP( RenderPanel::onMouseMUp )
		EVT_RIGHT_DOWN( RenderPanel::onMouseRDown )
		EVT_RIGHT_UP( RenderPanel::onMouseRUp )
		EVT_MOTION( RenderPanel::onMouseMove )
		EVT_MOUSEWHEEL( RenderPanel::onMouseWheel )
		EVT_MENU( wxID_EXIT, RenderPanel::onMenuClose )
		EVT_MENU( int( eTIMER_ID::CLIPBOARD_CHANGE ), RenderPanel::onClipboardText )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

		void RenderPanel::onTimerFwd( wxTimerEvent & event )
	{
		if ( m_currentState )
		{
			auto speed = doGetRealSpeed();
			m_currentState->addScalarVelocity( castor::Point3f{ 0.0f, 0.0f, speed } );
		}

		event.Skip();
	}

	void RenderPanel::onTimerBck( wxTimerEvent & event )
	{
		if ( m_currentState )
		{
			auto speed = doGetRealSpeed();
			m_currentState->addScalarVelocity( castor::Point3f{ 0.0f, 0.0f, -speed } );
		}

		event.Skip();
	}

	void RenderPanel::onTimerLft( wxTimerEvent & event )
	{
		if ( m_currentState )
		{
			auto speed = doGetRealSpeed();
			m_currentState->addScalarVelocity( castor::Point3f{ speed, 0.0f, 0.0f } );
		}

		event.Skip();
	}

	void RenderPanel::onTimerRgt( wxTimerEvent & event )
	{
		if ( m_currentState )
		{
			auto speed = doGetRealSpeed();
			m_currentState->addScalarVelocity( castor::Point3f{ -speed, 0.0f, 0.0f } );
		}

		event.Skip();
	}

	void RenderPanel::onTimerUp( wxTimerEvent & event )
	{
		if ( m_currentState )
		{
			auto speed = doGetRealSpeed();
			m_currentState->addScalarVelocity( castor::Point3f{ 0.0f, speed, 0.0f } );
		}

		event.Skip();
	}

	void RenderPanel::onTimerDwn( wxTimerEvent & event )
	{
		if ( m_currentState )
		{
			auto speed = doGetRealSpeed();
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
		doStopTimer( eTIMER_ID::COUNT );
		event.Skip();
	}

	void RenderPanel::onKeyDown( wxKeyEvent & event )
	{
		if ( auto inputListener = wxGetApp().getCastor()->getUserInputListener();
			!inputListener || !inputListener->fireKeydown( panel::doConvertKeyCode( event.GetKeyCode() )
				, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			switch ( event.GetKeyCode() )
			{
			case WXK_LEFT:
			case 'Q':
				doStartTimer( eTIMER_ID::LEFT );
				break;

			case WXK_RIGHT:
			case 'D':
				doStartTimer( eTIMER_ID::RIGHT );
				break;

			case WXK_UP:
			case 'Z':
				doStartTimer( eTIMER_ID::FORWARD );
				break;

			case WXK_DOWN:
			case 'S':
				doStartTimer( eTIMER_ID::BACK );
				break;

			case WXK_PAGEUP:
				doStartTimer( eTIMER_ID::UP );
				break;

			case WXK_PAGEDOWN:
				doStartTimer( eTIMER_ID::DOWN );
				break;

			case WXK_ALT:
				m_altdown = true;
				break;

			case 'E':
				if ( !m_extraSpeed )
				{
					m_extraSpeed = true;
					doUpdateSpeed();
				}
				break;

			case 'L':
				if ( m_lightsNode )
				{
					m_currentNode = m_lightsNode;
					m_currentState = &doAddNodeState( m_currentNode, false );
				}
				break;

			default:
				break;
			}
		}

		event.Skip();
	}

	void RenderPanel::onKeyUp( wxKeyEvent & event )
	{
		if ( auto inputListener = wxGetApp().getCastor()->getUserInputListener();
			!inputListener || !inputListener->fireKeyUp( panel::doConvertKeyCode( event.GetKeyCode() )
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
				doStopTimer( eTIMER_ID::LEFT );
				break;

			case WXK_RIGHT:
			case 'D':
				doStopTimer( eTIMER_ID::RIGHT );
				break;

			case WXK_UP:
			case 'Z':
				doStopTimer( eTIMER_ID::FORWARD );
				break;

			case WXK_DOWN:
			case 'S':
				doStopTimer( eTIMER_ID::BACK );
				break;

			case WXK_PAGEUP:
				doStopTimer( eTIMER_ID::UP );
				break;

			case WXK_PAGEDOWN:
				doStopTimer( eTIMER_ID::DOWN );
				break;

			case WXK_ALT:
				m_altdown = false;
				break;

			case 'E':
				if ( m_extraSpeed )
				{
					m_extraSpeed = false;
					doUpdateSpeed();
				}
				break;

			case WXK_ESCAPE:
				if ( m_selectedGeometry )
				{
					doUpdateSelectedGeometry( nullptr, nullptr, true );
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

					if ( m_3dController )
					{
						m_3dController->setCamera( camera );
						m_3dController->setSpeedFactor( m_currentState->getMaxSpeed() );
					}
				}
				break;

			default:
				break;
			}
		}

		event.Skip();
	}

	void RenderPanel::onChar( wxKeyEvent & event )
	{
		if ( auto inputListener = wxGetApp().getCastor()->getUserInputListener() )
		{
			wxChar key = event.GetUnicodeKey();
			wxString tmp;
			tmp << key;
			inputListener->fireChar( panel::doConvertKeyCode( event.GetKeyCode() )
				, GuiCommon::make_String( tmp ) );
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
			m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePreGpuStep
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

	void RenderPanel::onMouseLDown( wxMouseEvent & event )
	{
		m_mouseLeftDown = true;
		m_x = doTransformX( event.GetX() );
		m_y = doTransformY( event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		if ( auto inputListener = wxGetApp().getCastor()->getUserInputListener();
			!inputListener || !inputListener->fireMouseButtonPushed( castor3d::MouseButton::eLeft
				, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			if ( m_currentState )
			{
				doStartTimer( eTIMER_ID::MOUSE );
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

		if ( auto inputListener = wxGetApp().getCastor()->getUserInputListener();
			!inputListener || !inputListener->fireMouseButtonReleased( castor3d::MouseButton::eLeft
				, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			doStopTimer( eTIMER_ID::MOUSE );
		}

		event.Skip();
	}

	void RenderPanel::onMouseMDown( wxMouseEvent & event )
	{
		m_mouseMiddleDown = true;
		m_x = doTransformX( event.GetX() );
		m_y = doTransformY( event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		if ( auto inputListener = wxGetApp().getCastor()->getUserInputListener();
			!inputListener || !inputListener->fireMouseButtonPushed( castor3d::MouseButton::eMiddle
				, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			auto x = m_oldX;
			auto y = m_oldY;
			m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::CpuEventType::ePreGpuStep
				, [this, x, y]()
				{
					auto type = m_renderWindow->pick( castor::Position{ int( x ), int( y ) } );

					if ( type != castor3d::PickNodeType::eNone
						&& type != castor3d::PickNodeType::eBillboard )
					{
						doUpdateSelectedGeometry( m_renderWindow->getPickedGeometry(), m_renderWindow->getPickedSubmesh(), true );
					}
					else
					{
						doUpdateSelectedGeometry( nullptr, nullptr, true );
						select( castor3d::LightRPtr{} );
						select( castor3d::SceneNodeRPtr{} );
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

		if ( auto inputListener = wxGetApp().getCastor()->getUserInputListener() )
		{
			inputListener->fireMouseButtonReleased( castor3d::MouseButton::eMiddle
				, event.ControlDown(), event.AltDown(), event.ShiftDown() );
		}

		event.Skip();
	}

	void RenderPanel::onMouseRDown( wxMouseEvent & event )
	{
		m_mouseRightDown = true;
		m_x = doTransformX( event.GetX() );
		m_y = doTransformY( event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		if ( auto inputListener = wxGetApp().getCastor()->getUserInputListener() )
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

		if ( auto inputListener = wxGetApp().getCastor()->getUserInputListener() )
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

		if ( auto inputListener = wxGetApp().getCastor()->getUserInputListener();
			!inputListener || !inputListener->fireMouseWheel( castor::Position( 0, wheelRotation )
				, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			if ( wheelRotation < 0 )
			{
				doUpdateSpeed( panel::CAM_SPEED_INC );
			}
			else
			{
				doUpdateSpeed( 1.0f / panel::CAM_SPEED_INC );
			}
		}

		event.Skip();
	}

	void RenderPanel::onMenuClose( wxCommandEvent & event )
	{
		Close( true );
		event.Skip();
	}

	void RenderPanel::onClipboardText( wxCommandEvent & )
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
