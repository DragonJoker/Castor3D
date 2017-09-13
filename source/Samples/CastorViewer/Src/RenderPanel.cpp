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
#include <ShadowMap/ShadowMapPass.hpp>
#include <Miscellaneous/WindowHandle.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>

using namespace castor3d;
using namespace castor;

namespace CastorViewer
{
	namespace
	{
		static const real MAX_CAM_SPEED = 10.0_r;
		static const real DEF_CAM_SPEED = 1.0_r;
		static const real MIN_CAM_SPEED = 0.1_r;
		static const real CAM_SPEED_INC = 0.9_r;
	}

	namespace
	{
		KeyboardKey ConvertKeyCode( int p_code )
		{
			KeyboardKey result = KeyboardKey::eNone;

			if ( p_code < 0x20 )
			{
				switch ( p_code )
				{
				case WXK_BACK:
				case WXK_TAB:
				case WXK_RETURN:
				case WXK_ESCAPE:
					result = KeyboardKey( p_code );
					break;
				}
			}
			else if ( p_code == 0x7F )
			{
				result = KeyboardKey::eDelete;
			}
			else if ( p_code > 0xFF )
			{
				result = KeyboardKey( p_code + int( KeyboardKey::eStart ) - WXK_START );
			}
			else
			{
				// ASCII or extended ASCII character
				result = KeyboardKey( p_code );
			}

			return result;
		}

		TextureUnitSPtr doCloneUnit( PassSPtr p_clone, TextureUnit const & p_source )
		{
			TextureUnitSPtr clone = std::make_shared< TextureUnit >( *p_clone->getOwner()->getEngine() );

			clone->setAutoMipmaps( p_source.getAutoMipmaps() );
			clone->setChannel( p_source.getChannel() );
			clone->setIndex( p_source.getIndex() );
			clone->setRenderTarget( p_source.getRenderTarget() );
			clone->setSampler( p_source.getSampler() );
			clone->setTexture( p_source.getTexture() );

			return clone;
		}

		void doClonePass( MaterialSPtr p_clone, Pass const & p_source )
		{
			PassSPtr clone = p_clone->createPass();

			switch ( p_clone->getType() )
			{
			case MaterialType::eLegacy:
				{
					auto & source = static_cast< LegacyPass const & >( p_source );
					auto pass = std::static_pointer_cast< LegacyPass >( clone );
					pass->setDiffuse( source.getDiffuse() );
					pass->setSpecular( source.getSpecular() );
					pass->setEmissive( source.getEmissive() );
					pass->setShininess( source.getShininess() );
				}
				break;

			case MaterialType::ePbrMetallicRoughness:
				{
					auto & source = static_cast< MetallicRoughnessPbrPass const & >( p_source );
					auto pass = std::static_pointer_cast< MetallicRoughnessPbrPass >( clone );
					pass->setAlbedo( source.getAlbedo() );
					pass->setRoughness( source.getRoughness() );
					pass->setMetallic( source.getMetallic() );
				}
				break;

			case MaterialType::ePbrSpecularGlossiness:
				{
					auto & source = static_cast< SpecularGlossinessPbrPass const & >( p_source );
					auto pass = std::static_pointer_cast< SpecularGlossinessPbrPass >( clone );
					pass->setDiffuse( source.getDiffuse() );
					pass->setGlossiness( source.getGlossiness() );
					pass->setSpecular( source.getSpecular() );
				}
				break;
			}
			
			clone->setOpacity( p_source.getOpacity() );
			clone->setRefractionRatio( p_source.getRefractionRatio() );
			clone->setTwoSided( p_source.IsTwoSided() );
			clone->setAlphaBlendMode( p_source.getAlphaBlendMode() );
			clone->setColourBlendMode( p_source.getColourBlendMode() );
			clone->setAlphaFunc( p_source.getAlphaFunc() );
			clone->setAlphaValue( p_source.getAlphaValue() );

			for ( auto const & unit : p_source )
			{
				clone->addTextureUnit( doCloneUnit( clone, *unit ) );
			}
		}

		MaterialSPtr doCloneMaterial( Material const & p_source )
		{
			MaterialSPtr clone = std::make_shared< Material >( p_source.getName() + cuT( "_Clone" ), *p_source.getEngine(), p_source.getType() );

			for ( auto const & pass : p_source )
			{
				doClonePass( clone, *pass );
			}

			return clone;
		}

		void Restore( RenderPanel::SelectedSubmesh & p_selected
			, GeometrySPtr p_geometry )
		{
			p_geometry->getScene()->getListener().postEvent( makeFunctorEvent( EventType::ePostRender
				, [p_selected, p_geometry]()
				{
					p_geometry->setMaterial( *p_selected.m_submesh, p_selected.m_originalMaterial );
				} ) );
		}

		void Restore( RenderPanel::SelectedGeometry & p_selected )
		{
			auto geometry = p_selected.m_geometry;

			for ( auto & submesh : p_selected.m_submeshes )
			{
				Restore( submesh, geometry );
			}

			p_selected.m_geometry.reset();
			p_selected.m_submeshes.clear();
		}

		void Save( RenderPanel::SelectedGeometry & p_selected
			, GeometrySPtr p_geometry )
		{
			auto geometry = p_selected.m_geometry;
			p_selected.m_submeshes.reserve(( p_geometry->getMesh()->getSubmeshCount() ) );

			for ( auto & submesh : *p_geometry->getMesh() )
			{
				p_selected.m_submeshes.emplace_back( submesh, p_geometry->getMaterial( *submesh ) );
			}

			p_selected.m_geometry = p_geometry;
		}

		void Select( RenderPanel::SelectedSubmesh & p_selected
			, GeometrySPtr p_geometry )
		{
			p_selected.m_selectedMaterial = doCloneMaterial( *p_selected.m_originalMaterial );

			switch ( p_selected.m_selectedMaterial->getType() )
			{
			case MaterialType::eLegacy:
				{
					auto pass = p_selected.m_selectedMaterial->getTypedPass< MaterialType::eLegacy >( 0u );
					pass->setDiffuse( Colour::fromPredefined( PredefinedColour::eMedAlphaRed ) );
					pass->setSpecular( Colour::fromPredefined( PredefinedColour::eMedAlphaRed ) );
				}
				break;

			case MaterialType::ePbrMetallicRoughness:
				{
					auto pass = p_selected.m_selectedMaterial->getTypedPass< MaterialType::ePbrMetallicRoughness >( 0u );
					pass->setAlbedo( Colour::fromPredefined( PredefinedColour::eMedAlphaRed ) );
				}
				break;

			case MaterialType::ePbrSpecularGlossiness:
				{
					auto pass = p_selected.m_selectedMaterial->getTypedPass< MaterialType::ePbrSpecularGlossiness >( 0u );
					pass->setDiffuse( Colour::fromPredefined( PredefinedColour::eMedAlphaRed ) );
				}
				break;
			}

			p_geometry->getScene()->getListener().postEvent( makeFunctorEvent( EventType::ePostRender
				, [p_selected, p_geometry]()
				{
					p_geometry->setMaterial( *p_selected.m_submesh, p_selected.m_selectedMaterial );
				} ) );
		}
	}

	RenderPanel::RenderPanel( wxWindow * parent, wxWindowID p_id, wxPoint const & pos, wxSize const & size, long style )
		: wxPanel( parent, p_id, pos, size, style )
		, m_camSpeed( DEF_CAM_SPEED, Range< real >{ MIN_CAM_SPEED, MAX_CAM_SPEED } )
	{
		m_pTimer[0] = nullptr;

		for ( int i = 1; i < eTIMER_ID_COUNT; i++ )
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

		for ( int i = 1; i <= eTIMER_ID_MOVEMENT; i++ )
		{
			delete m_pTimer[i];
			m_pTimer[i] = nullptr;
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

	void RenderPanel::setRenderWindow( RenderWindowSPtr p_window )
	{
		m_renderWindow.reset();
		doStopMovement();
		castor::Size sizeWnd = GuiCommon::make_Size( GetClientSize() );

		if ( p_window && p_window->initialise( sizeWnd, GuiCommon::make_WindowHandle( this ) ) )
		{
			castor::Size sizeScreen;
			castor::System::getScreenSize( 0, sizeScreen );
			GetParent()->SetClientSize( sizeWnd.getWidth(), sizeWnd.getHeight() );
			sizeWnd = GuiCommon::make_Size( GetParent()->GetClientSize() );
			GetParent()->SetPosition( wxPoint( std::abs( int( sizeScreen.getWidth() ) - int( sizeWnd.getWidth() ) ) / 2, std::abs( int( sizeScreen.getHeight() ) - int( sizeWnd.getHeight() ) ) / 2 ) );
			m_listener = p_window->getListener();
			SceneSPtr scene = p_window->getScene();

			if ( scene )
			{
				auto camera = p_window->getCamera();

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
						m_currentState = &doAddNodeState( m_currentNode );
					}

					m_renderWindow = p_window;
					m_keyboardEvent = std::make_unique< KeyboardEvent >( p_window );

					{
						auto lock = makeUniqueLock( scene->getCameraCache() );
						p_window->getPickingPass().addScene( *scene, *( scene->getCameraCache().begin()->second ) );
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
			m_pTimer[eTIMER_ID_MOVEMENT]->Start( 30 );
			m_movementStarted = true;
		}
	}

	void RenderPanel::doStopMovement()
	{
		if ( m_movementStarted )
		{
			m_movementStarted = false;
			m_pTimer[eTIMER_ID_MOVEMENT]->Stop();
		}
	}

	void RenderPanel::doStartTimer( int p_id )
	{
		m_pTimer[p_id]->Start( 10 );
	}

	void RenderPanel::doStopTimer( int p_id )
	{
		if ( p_id != eTIMER_ID_COUNT )
		{
			m_pTimer[p_id]->Stop();
		}
		else
		{
			for ( int i = 1; i < eTIMER_ID_MOVEMENT; i++ )
			{
				m_pTimer[i]->Stop();
			}
		}
	}

	void RenderPanel::doResetNode()
	{
		doResetTimers();

		if ( m_currentState )
		{
			m_currentState->Reset( DEF_CAM_SPEED );
		}
	}

	void RenderPanel::doTurnCameraHoriz()
	{
		doResetTimers();
		auto camera = m_camera.lock();

		if ( camera )
		{
			auto cameraNode = camera->getParent();
			camera->getScene()->getListener().postEvent( makeFunctorEvent( EventType::ePreRender, [this, cameraNode]()
			{
				Quaternion orientation{ cameraNode->getOrientation() };
				orientation *= Quaternion::fromAxisAngle( Point3r{ 0.0_r, 1.0_r, 0.0_r }, Angle::fromDegrees( 90.0_r ) );
				cameraNode->setOrientation( orientation );
			} ) );
		}
	}

	void RenderPanel::doTurnCameraVertic()
	{
		doResetTimers();
		auto camera = m_camera.lock();

		if ( camera )
		{
			auto cameraNode = camera->getParent();
			camera->getScene()->getListener().postEvent( makeFunctorEvent( EventType::ePreRender, [this, cameraNode]()
			{
				Quaternion orientation{ cameraNode->getOrientation() };
				orientation *= Quaternion::fromAxisAngle( Point3r{ 1.0_r, 0.0_r, 0.0_r }, Angle::fromDegrees( 90.0_r ) );
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

	void RenderPanel::doUpdateSelectedGeometry( castor3d::GeometrySPtr p_geometry
		, castor3d::SubmeshSPtr p_submesh )
	{
		SelectedGeometry selected;
		auto submesh = m_selectedSubmesh ? m_selectedSubmesh->m_submesh : nullptr;
		auto oldGeometry = m_selectedGeometry.m_geometry;
		bool changed = false;
		SceneRPtr scene = p_geometry ? p_geometry->getScene() : nullptr;

		if ( oldGeometry != p_geometry )
		{
			changed = true;
			m_selectedSubmesh = nullptr;

			if ( oldGeometry )
			{
				scene = oldGeometry->getScene();
				Restore( m_selectedGeometry );
			}

			if ( p_geometry )
			{
				Save( m_selectedGeometry, p_geometry );
			}
		}

		if ( submesh != p_submesh )
		{
			if ( m_selectedSubmesh )
			{
				Restore( *m_selectedSubmesh, oldGeometry );
			}

			if ( p_submesh )
			{
				auto it = std::find_if( m_selectedGeometry.m_submeshes.begin()
					, m_selectedGeometry.m_submeshes.end()
					, [&p_submesh]( auto & selectedSubmesh )
					{
						return selectedSubmesh.m_submesh == p_submesh;
					} );
				REQUIRE( it != m_selectedGeometry.m_submeshes.end());
				m_selectedSubmesh = &( *it );
				Select( *m_selectedSubmesh
					, p_geometry );
				wxGetApp().getMainFrame()->select( p_geometry, m_selectedSubmesh->m_submesh );
			}
			changed = true;
		}

		if ( changed )
		{
			scene->getListener().postEvent( makeFunctorEvent( EventType::ePostRender
				, [scene]()
				{
					scene->setChanged();
				} ) );
		}

		{
			if ( p_geometry )
			{
				m_currentNode = m_selectedGeometry.m_geometry->getParent();
			}
			else
			{
				m_currentNode = m_camera.lock()->getParent();
			}

			m_currentState = &doAddNodeState( m_currentNode );
		}
	}

	GuiCommon::NodeState & RenderPanel::doAddNodeState( SceneNodeSPtr p_node )
	{
		auto it = m_nodesStates.find( p_node->getName() );

		if ( it == m_nodesStates.end() )
		{
			it = m_nodesStates.emplace( p_node->getName()
				, std::make_unique< GuiCommon::NodeState >( *m_listener, p_node ) ).first;
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
			window->resize( p_event.GetSize().x, p_event.GetSize().y );
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

		if ( !inputListener || !inputListener->fireKeydown( ConvertKeyCode( p_event.GetKeyCode() ), p_event.ControlDown(), p_event.AltDown(), p_event.ShiftDown() ) )
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
					m_currentState = &doAddNodeState( m_currentNode );
				}
				break;
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnKeyUp( wxKeyEvent & p_event )
	{
		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireKeyUp( ConvertKeyCode( p_event.GetKeyCode() ), p_event.ControlDown(), p_event.AltDown(), p_event.ShiftDown() ) )
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
				m_currentState = &doAddNodeState( m_currentNode );
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
			inputListener->fireChar( ConvertKeyCode( p_event.GetKeyCode() ), String( tmp.mb_str( wxConvUTF8 ) ) );
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
				static real constexpr mult = 4.0_r;
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
					m_currentState->addScalarVelocity( Point3r{ deltaX, deltaY, 0.0_r } );
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
