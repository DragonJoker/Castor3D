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

using namespace Castor3D;
using namespace Castor;

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

		TextureUnitSPtr DoCloneUnit( PassSPtr p_clone, TextureUnit const & p_source )
		{
			TextureUnitSPtr clone = std::make_shared< TextureUnit >( *p_clone->GetOwner()->GetEngine() );

			clone->SetAutoMipmaps( p_source.GetAutoMipmaps() );
			clone->SetChannel( p_source.GetChannel() );
			clone->SetIndex( p_source.GetIndex() );
			clone->SetRenderTarget( p_source.GetRenderTarget() );
			clone->SetSampler( p_source.GetSampler() );
			clone->SetTexture( p_source.GetTexture() );

			return clone;
		}

		void DoClonePass( MaterialSPtr p_clone, Pass const & p_source )
		{
			PassSPtr clone = p_clone->CreatePass();

			switch ( p_clone->GetType() )
			{
			case MaterialType::eLegacy:
				{
					auto & source = static_cast< LegacyPass const & >( p_source );
					auto pass = std::static_pointer_cast< LegacyPass >( clone );
					pass->SetDiffuse( source.GetDiffuse() );
					pass->SetSpecular( source.GetSpecular() );
					pass->SetEmissive( source.GetEmissive() );
					pass->SetShininess( source.GetShininess() );
				}
				break;

			case MaterialType::ePbrMetallicRoughness:
				{
					auto & source = static_cast< MetallicRoughnessPbrPass const & >( p_source );
					auto pass = std::static_pointer_cast< MetallicRoughnessPbrPass >( clone );
					pass->SetAlbedo( source.GetAlbedo() );
					pass->SetRoughness( source.GetRoughness() );
					pass->SetMetallic( source.GetMetallic() );
				}
				break;

			case MaterialType::ePbrSpecularGlossiness:
				{
					auto & source = static_cast< SpecularGlossinessPbrPass const & >( p_source );
					auto pass = std::static_pointer_cast< SpecularGlossinessPbrPass >( clone );
					pass->SetDiffuse( source.GetDiffuse() );
					pass->SetGlossiness( source.GetGlossiness() );
					pass->SetSpecular( source.GetSpecular() );
				}
				break;
			}
			
			clone->SetOpacity( p_source.GetOpacity() );
			clone->SetRefractionRatio( p_source.GetRefractionRatio() );
			clone->SetTwoSided( p_source.IsTwoSided() );
			clone->SetAlphaBlendMode( p_source.GetAlphaBlendMode() );
			clone->SetColourBlendMode( p_source.GetColourBlendMode() );
			clone->SetAlphaFunc( p_source.GetAlphaFunc() );
			clone->SetAlphaValue( p_source.GetAlphaValue() );

			for ( auto const & unit : p_source )
			{
				clone->AddTextureUnit( DoCloneUnit( clone, *unit ) );
			}
		}

		MaterialSPtr DoCloneMaterial( Material const & p_source )
		{
			MaterialSPtr clone = std::make_shared< Material >( p_source.GetName() + cuT( "_Clone" ), *p_source.GetEngine(), p_source.GetType() );

			for ( auto const & pass : p_source )
			{
				DoClonePass( clone, *pass );
			}

			return clone;
		}

		void Restore( RenderPanel::SelectedSubmesh & p_selected
			, GeometrySPtr p_geometry )
		{
			p_geometry->GetScene()->GetListener().PostEvent( MakeFunctorEvent( EventType::ePostRender
				, [p_selected, p_geometry]()
				{
					p_geometry->SetMaterial( *p_selected.m_submesh, p_selected.m_originalMaterial );
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
			p_selected.m_submeshes.reserve(( p_geometry->GetMesh()->GetSubmeshCount() ) );

			for ( auto & submesh : *p_geometry->GetMesh() )
			{
				p_selected.m_submeshes.emplace_back( submesh, p_geometry->GetMaterial( *submesh ) );
			}

			p_selected.m_geometry = p_geometry;
		}

		void Select( RenderPanel::SelectedSubmesh & p_selected
			, GeometrySPtr p_geometry )
		{
			p_selected.m_selectedMaterial = DoCloneMaterial( *p_selected.m_originalMaterial );

			switch ( p_selected.m_selectedMaterial->GetType() )
			{
			case MaterialType::eLegacy:
				{
					auto pass = p_selected.m_selectedMaterial->GetTypedPass< MaterialType::eLegacy >( 0u );
					pass->SetDiffuse( Colour::from_predef( PredefinedColour::eMedAlphaRed ) );
					pass->SetSpecular( Colour::from_predef( PredefinedColour::eMedAlphaRed ) );
				}
				break;

			case MaterialType::ePbrMetallicRoughness:
				{
					auto pass = p_selected.m_selectedMaterial->GetTypedPass< MaterialType::ePbrMetallicRoughness >( 0u );
					pass->SetAlbedo( Colour::from_predef( PredefinedColour::eMedAlphaRed ) );
				}
				break;

			case MaterialType::ePbrSpecularGlossiness:
				{
					auto pass = p_selected.m_selectedMaterial->GetTypedPass< MaterialType::ePbrSpecularGlossiness >( 0u );
					pass->SetDiffuse( Colour::from_predef( PredefinedColour::eMedAlphaRed ) );
				}
				break;
			}

			p_geometry->GetScene()->GetListener().PostEvent( MakeFunctorEvent( EventType::ePostRender
				, [p_selected, p_geometry]()
				{
					p_geometry->SetMaterial( *p_selected.m_submesh, p_selected.m_selectedMaterial );
				} ) );
		}
	}

	RenderPanel::RenderPanel( wxWindow * parent, wxWindowID p_id, wxPoint const & pos, wxSize const & size, long style )
		: wxPanel( parent, p_id, pos, size, style )
		, m_camSpeed( DEF_CAM_SPEED )
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

	void RenderPanel::SetRenderWindow( RenderWindowSPtr p_window )
	{
		m_renderWindow.reset();
		DoStopMovement();
		Castor::Size sizeWnd = GuiCommon::make_Size( GetClientSize() );

		if ( p_window && p_window->Initialise( sizeWnd, GuiCommon::make_WindowHandle( this ) ) )
		{
			Castor::Size sizeScreen;
			Castor::System::GetScreenSize( 0, sizeScreen );
			GetParent()->SetClientSize( sizeWnd.width(), sizeWnd.height() );
			sizeWnd = GuiCommon::make_Size( GetParent()->GetClientSize() );
			GetParent()->SetPosition( wxPoint( std::abs( int( sizeScreen.width() ) - int( sizeWnd.width() ) ) / 2, std::abs( int( sizeScreen.height() ) - int( sizeWnd.height() ) ) / 2 ) );
			m_listener = p_window->GetListener();
			SceneSPtr scene = p_window->GetScene();

			if ( scene )
			{
				auto camera = p_window->GetCamera();

				if ( camera )
				{
					if ( scene->GetSceneNodeCache().Has( cuT( "PointLightsNode" ) ) )
					{
						m_lightsNode = scene->GetSceneNodeCache().Find( cuT( "PointLightsNode" ) );
					}
					else if ( scene->GetSceneNodeCache().Has( cuT( "LightNode" ) ) )
					{
						m_lightsNode = scene->GetSceneNodeCache().Find( cuT( "LightNode" ) );
					}

					auto cameraNode = camera->GetParent();

					if ( cameraNode )
					{
						m_currentNode = cameraNode;
						m_currentState = &DoAddNodeState( m_currentNode );
					}

					m_renderWindow = p_window;
					m_keyboardEvent = std::make_unique< KeyboardEvent >( p_window );

					{
						auto lock = make_unique_lock( scene->GetCameraCache() );
						p_window->GetPickingPass().AddScene( *scene, *( scene->GetCameraCache().begin()->second ) );
					}

					m_camera = camera;
					DoStartMovement();
				}

				m_scene = scene;
			}
		}
		else if ( m_listener )
		{
			m_listener.reset();
		}
	}

	void RenderPanel::DoResetTimers()
	{
		DoStopTimer( eTIMER_ID_COUNT );
		m_camSpeed = DEF_CAM_SPEED;
		m_x = 0.0_r;
		m_y = 0.0_r;
		m_oldX = 0.0_r;
		m_oldY = 0.0_r;
	}

	void RenderPanel::DoStartMovement()
	{
		if ( !m_movementStarted )
		{
			m_pTimer[eTIMER_ID_MOVEMENT]->Start( 30 );
			m_movementStarted = true;
		}
	}

	void RenderPanel::DoStopMovement()
	{
		if ( m_movementStarted )
		{
			m_movementStarted = false;
			m_pTimer[eTIMER_ID_MOVEMENT]->Stop();
		}
	}

	void RenderPanel::DoStartTimer( int p_id )
	{
		m_pTimer[p_id]->Start( 10 );
	}

	void RenderPanel::DoStopTimer( int p_id )
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

	void RenderPanel::DoResetNode()
	{
		DoResetTimers();
		m_currentState->Reset( DEF_CAM_SPEED );
	}

	void RenderPanel::DoTurnCameraHoriz()
	{
		DoResetTimers();
		auto camera = m_camera.lock();

		if ( camera )
		{
			auto cameraNode = camera->GetParent();
			camera->GetScene()->GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, cameraNode]()
			{
				Quaternion orientation{ cameraNode->GetOrientation() };
				orientation *= Quaternion::from_axis_angle( Point3r{ 0.0_r, 1.0_r, 0.0_r }, Angle::from_degrees( 90.0_r ) );
				cameraNode->SetOrientation( orientation );
			} ) );
		}
	}

	void RenderPanel::DoTurnCameraVertic()
	{
		DoResetTimers();
		auto camera = m_camera.lock();

		if ( camera )
		{
			auto cameraNode = camera->GetParent();
			camera->GetScene()->GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, cameraNode]()
			{
				Quaternion orientation{ cameraNode->GetOrientation() };
				orientation *= Quaternion::from_axis_angle( Point3r{ 1.0_r, 0.0_r, 0.0_r }, Angle::from_degrees( 90.0_r ) );
				cameraNode->SetOrientation( orientation );
			} ) );
		}
	}

	void RenderPanel::DoChangeCamera()
	{
		auto camera = m_camera.lock();
		auto window = m_renderWindow.lock();
		auto scene = m_scene.lock();

		if ( camera && window && scene )
		{
			//auto & shadowMaps = window->GetRenderTarget()->GetTechnique()->GetShadowMaps();

			//if ( !shadowMaps.empty() )
			//{
			//	auto it = shadowMaps.begin();
			//	m_currentNode = it->first->GetParent();
			//}
		}
	}

	void RenderPanel::DoReloadScene()
	{
		DoResetTimers();
		wxGetApp().GetMainFrame()->LoadScene();
	}

	real RenderPanel::DoTransformX( int x )
	{
		real result = real( x );
		auto camera = m_camera.lock();

		if ( camera )
		{
			result *= real( camera->GetWidth() ) / GetClientSize().x;
		}

		return result;
	}

	real RenderPanel::DoTransformY( int y )
	{
		real result = real( y );
		auto camera = m_camera.lock();

		if ( camera )
		{
			result *= real( camera->GetHeight() ) / GetClientSize().y;
		}

		return result;
	}

	int RenderPanel::DoTransformX( real x )
	{
		int result = int( x );
		auto camera = m_camera.lock();

		if ( camera )
		{
			result = int( x * GetClientSize().x / real( camera->GetWidth() ) );
		}

		return result;
	}

	int RenderPanel::DoTransformY( real y )
	{
		int result = int( y );
		auto camera = m_camera.lock();

		if ( camera )
		{
			result = int( y * GetClientSize().y / real( camera->GetHeight() ) );
		}

		return result;
	}

	void RenderPanel::DoUpdateSelectedGeometry( Castor3D::GeometrySPtr p_geometry
		, Castor3D::SubmeshSPtr p_submesh )
	{
		SelectedGeometry selected;
		auto submesh = m_selectedSubmesh ? m_selectedSubmesh->m_submesh : nullptr;
		auto oldGeometry = m_selectedGeometry.m_geometry;
		bool changed = false;
		SceneRPtr scene = p_geometry ? p_geometry->GetScene() : nullptr;

		if ( oldGeometry != p_geometry )
		{
			changed = true;
			m_selectedSubmesh = nullptr;

			if ( oldGeometry )
			{
				scene = oldGeometry->GetScene();
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
			}
			changed = true;
		}

		if ( changed )
		{
			scene->GetListener().PostEvent( MakeFunctorEvent( EventType::ePostRender
				, [scene]()
				{
					scene->SetChanged();
				} ) );
		}

		{
			if ( p_geometry )
			{
				m_currentNode = m_selectedGeometry.m_geometry->GetParent();
			}
			else
			{
				m_currentNode = m_camera.lock()->GetParent();
			}

			m_currentState = &DoAddNodeState( m_currentNode );
		}
	}

	GuiCommon::NodeState & RenderPanel::DoAddNodeState( SceneNodeSPtr p_node )
	{
		auto it = m_nodesStates.find( p_node->GetName() );

		if ( it == m_nodesStates.end() )
		{
			it = m_nodesStates.emplace( p_node->GetName()
				, std::make_unique< GuiCommon::NodeState >( *m_listener, p_node ) ).first;
		}

		it->second->SetMaxSpeed( m_camSpeed );
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
		EVT_SET_FOCUS( RenderPanel::OnSetFocus )
		EVT_KILL_FOCUS( RenderPanel::OnKillFocus )
		EVT_KEY_DOWN( RenderPanel::OnKeyDown )
		EVT_KEY_UP( RenderPanel::OnKeyUp )
		EVT_CHAR( RenderPanel::OnChar )
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
		m_currentState->AddScalarVelocity( Point3r{ 0.0_r, 0.0_r, m_camSpeed } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerBck( wxTimerEvent & p_event )
	{
		m_currentState->AddScalarVelocity( Point3r{ 0.0_r, 0.0_r, -m_camSpeed } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerLft( wxTimerEvent & p_event )
	{
		m_currentState->AddScalarVelocity( Point3r{ m_camSpeed, 0.0_r, 0.0_r } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerRgt( wxTimerEvent & p_event )
	{
		m_currentState->AddScalarVelocity( Point3r{ -m_camSpeed, 0.0_r, 0.0_r } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerUp( wxTimerEvent & p_event )
	{
		m_currentState->AddScalarVelocity( Point3r{ 0.0_r, m_camSpeed, 0.0_r } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerDwn( wxTimerEvent & p_event )
	{
		m_currentState->AddScalarVelocity( Point3r{ 0.0_r, -m_camSpeed, 0.0_r } );
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
			m_currentState->Update();
		}

		p_event.Skip();
	}

	void RenderPanel::OnSize( wxSizeEvent & p_event )
	{
		auto window = m_renderWindow.lock();

		if ( window )
		{
			window->Resize( p_event.GetSize().x, p_event.GetSize().y );
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
		auto inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !inputListener || !inputListener->FireKeyDown( ConvertKeyCode( p_event.GetKeyCode() ), p_event.ControlDown(), p_event.AltDown(), p_event.ShiftDown() ) )
		{
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

			case WXK_ALT:
				m_altDown = true;
				break;

			case 'L':
				m_currentNode = m_lightsNode;
				m_currentState = &DoAddNodeState( m_currentNode );
				break;
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnKeyUp( wxKeyEvent & p_event )
	{
		auto inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !inputListener || !inputListener->FireKeyUp( ConvertKeyCode( p_event.GetKeyCode() ), p_event.ControlDown(), p_event.AltDown(), p_event.ShiftDown() ) )
		{
			switch ( p_event.GetKeyCode() )
			{
			case 'R':
				DoResetNode();
				break;

			case 'T':
				DoTurnCameraHoriz();
				break;

			case 'Y':
				DoTurnCameraVertic();
				break;

			case 'C':
				DoChangeCamera();
				break;

			case WXK_F5:
				DoReloadScene();
				break;

			case WXK_LEFT:
			case 'Q':
				DoStopTimer( eTIMER_ID_LEFT );
				break;

			case WXK_RIGHT:
			case 'D':
				DoStopTimer( eTIMER_ID_RIGHT );
				break;

			case WXK_UP:
			case 'Z':
				DoStopTimer( eTIMER_ID_FORWARD );
				break;

			case WXK_DOWN:
			case 'S':
				DoStopTimer( eTIMER_ID_BACK );
				break;

			case WXK_PAGEUP:
				DoStopTimer( eTIMER_ID_UP );
				break;

			case WXK_PAGEDOWN:
				DoStopTimer( eTIMER_ID_DOWN );
				break;

			case WXK_ALT:
				m_altDown = false;
				break;

			case WXK_ESCAPE:
				DoUpdateSelectedGeometry( nullptr, nullptr );
				break;

			case 'L':
				m_currentNode = m_camera.lock()->GetParent();
				m_currentState = &DoAddNodeState( m_currentNode );
				break;
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnChar( wxKeyEvent & p_event )
	{
		auto inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( inputListener )
		{
			wxChar key = p_event.GetUnicodeKey();
			wxString tmp;
			tmp << key;
			inputListener->FireChar( ConvertKeyCode( p_event.GetKeyCode() ), String( tmp.mb_str( wxConvUTF8 ) ) );
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseLDClick( wxMouseEvent & p_event )
	{
		m_x = DoTransformX( p_event.GetX() );
		m_y = DoTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		if ( m_listener )
		{
			m_listener->PostEvent( std::make_unique< KeyboardEvent >( *m_keyboardEvent ) );
			auto window = m_renderWindow.lock();

			if ( window )
			{
				wxGetApp().GetMainFrame()->ToggleFullScreen( !window->IsFullscreen() );
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseLDown( wxMouseEvent & p_event )
	{
		m_mouseLeftDown = true;
		m_x = DoTransformX( p_event.GetX() );
		m_y = DoTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !inputListener || !inputListener->FireMouseButtonPushed( MouseButton::eLeft ) )
		{
			auto window = GetRenderWindow();

			if ( window )
			{
				if ( m_altDown )
				{
					auto x = m_oldX;
					auto y = m_oldY;
					m_listener->PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, window, x, y]()
					{
						Camera & camera = *window->GetCamera();
						camera.Update();
						auto type = window->GetPickingPass().Pick( Position{ int( x ), int( y ) }, camera );

						if ( type != PickingPass::NodeType::eNone
							&& type != PickingPass::NodeType::eBillboard )
						{
							DoUpdateSelectedGeometry( window->GetPickingPass().GetPickedGeometry(), window->GetPickingPass().GetPickedSubmesh() );
						}
						else
						{
							DoUpdateSelectedGeometry( nullptr, nullptr );
						}
					} ) );
				}
				else if ( m_currentState )
				{
					DoStartTimer( eTIMER_ID_MOUSE );
				}
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseLUp( wxMouseEvent & p_event )
	{
		m_mouseLeftDown = false;
		m_x = DoTransformX( p_event.GetX() );
		m_y = DoTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !inputListener || !inputListener->FireMouseButtonReleased( MouseButton::eLeft ) )
		{
			DoStopTimer( eTIMER_ID_MOUSE );
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseMDown( wxMouseEvent & p_event )
	{
		m_mouseMiddleDown = true;
		m_x = DoTransformX( p_event.GetX() );
		m_y = DoTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !inputListener || !inputListener->FireMouseButtonPushed( MouseButton::eMiddle ) )
		{
			auto window = GetRenderWindow();

			if ( window )
			{
				auto x = m_oldX;
				auto y = m_oldY;
				m_listener->PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, window, x, y]()
				{
					Camera & camera = *window->GetCamera();
					camera.Update();
					auto type = window->GetPickingPass().Pick( Position{ int( x ), int( y ) }, camera );

					if ( type != PickingPass::NodeType::eNone
						&& type != PickingPass::NodeType::eBillboard )
					{
						DoUpdateSelectedGeometry( window->GetPickingPass().GetPickedGeometry(), window->GetPickingPass().GetPickedSubmesh() );
					}
					else
					{
						DoUpdateSelectedGeometry( nullptr, nullptr );
					}
				} ) );
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseMUp( wxMouseEvent & p_event )
	{
		m_mouseMiddleDown = false;
		m_x = DoTransformX( p_event.GetX() );
		m_y = DoTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( inputListener  )
		{
			inputListener->FireMouseButtonReleased( MouseButton::eMiddle );
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseRDown( wxMouseEvent & p_event )
	{
		m_mouseRightDown = true;
		m_x = DoTransformX( p_event.GetX() );
		m_y = DoTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( inputListener  )
		{
			inputListener->FireMouseButtonPushed( MouseButton::eRight );
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseRUp( wxMouseEvent & p_event )
	{
		m_mouseRightDown = false;
		m_x = DoTransformX( p_event.GetX() );
		m_y = DoTransformY( p_event.GetY() );
		m_oldX = m_x;
		m_oldY = m_y;

		auto inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( inputListener  )
		{
			inputListener->FireMouseButtonReleased( MouseButton::eRight );
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseMove( wxMouseEvent & p_event )
	{
		m_x = DoTransformX( p_event.GetX() );
		m_y = DoTransformY( p_event.GetY() );

		auto inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !inputListener || !inputListener->FireMouseMove( Position( int32_t( m_x ), int32_t( m_y ) ) ) )
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
					m_currentState->AddAngularVelocity( Point2r{ -deltaY, deltaX } );
				}
				else if ( m_mouseRightDown )
				{
					m_currentState->AddScalarVelocity( Point3r{ deltaX, deltaY, 0.0_r } );
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

		auto inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !inputListener || !inputListener->FireMouseWheel( Position( 0, wheelRotation ) ) )
		{
			if ( wheelRotation < 0 )
			{
				m_camSpeed *= CAM_SPEED_INC;
			}
			else
			{
				m_camSpeed /= CAM_SPEED_INC;
			}

			clamp( m_camSpeed, MIN_CAM_SPEED, MAX_CAM_SPEED );
			m_currentState->SetMaxSpeed( m_camSpeed );
		}

		p_event.Skip();
	}

	void RenderPanel::OnMenuClose( wxCommandEvent & p_event )
	{
		Close( true );
		p_event.Skip();
	}
}
