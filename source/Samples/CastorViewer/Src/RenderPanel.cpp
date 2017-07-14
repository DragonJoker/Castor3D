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
#include <Material/PbrPass.hpp>
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
			KeyboardKey l_return = KeyboardKey::eNone;

			if ( p_code < 0x20 )
			{
				switch ( p_code )
				{
				case WXK_BACK:
				case WXK_TAB:
				case WXK_RETURN:
				case WXK_ESCAPE:
					l_return = KeyboardKey( p_code );
					break;
				}
			}
			else if ( p_code == 0x7F )
			{
				l_return = KeyboardKey::eDelete;
			}
			else if ( p_code > 0xFF )
			{
				l_return = KeyboardKey( p_code + int( KeyboardKey::eStart ) - WXK_START );
			}
			else
			{
				// ASCII or extended ASCII character
				l_return = KeyboardKey( p_code );
			}

			return l_return;
		}

		TextureUnitSPtr DoCloneUnit( PassSPtr p_clone, TextureUnit const & p_source )
		{
			TextureUnitSPtr l_clone = std::make_shared< TextureUnit >( *p_clone->GetOwner()->GetEngine() );

			l_clone->SetAutoMipmaps( p_source.GetAutoMipmaps() );
			l_clone->SetChannel( p_source.GetChannel() );
			l_clone->SetIndex( p_source.GetIndex() );
			l_clone->SetRenderTarget( p_source.GetRenderTarget() );
			l_clone->SetSampler( p_source.GetSampler() );
			l_clone->SetTexture( p_source.GetTexture() );

			return l_clone;
		}

		void DoClonePass( MaterialSPtr p_clone, Pass const & p_source )
		{
			PassSPtr l_clone = p_clone->CreatePass();

			switch ( p_clone->GetType() )
			{
			case MaterialType::eLegacy:
				{
					auto & l_source = static_cast< LegacyPass const & >( p_source );
					auto l_pass = std::static_pointer_cast< LegacyPass >( l_clone );
					l_pass->SetDiffuse( l_source.GetDiffuse() );
					l_pass->SetSpecular( l_source.GetSpecular() );
					l_pass->SetEmissive( l_source.GetEmissive() );
					l_pass->SetShininess( l_source.GetShininess() );
				}
				break;

			case MaterialType::ePbr:
				{
					auto & l_source = static_cast< PbrPass const & >( p_source );
					auto l_pass = std::static_pointer_cast< PbrPass >( l_clone );
					l_pass->SetAlbedo( l_source.GetAlbedo() );
					l_pass->SetRoughness( l_source.GetRoughness() );
					l_pass->SetMetallic( l_source.GetMetallic() );
				}
				break;
			}
			
			l_clone->SetOpacity( p_source.GetOpacity() );
			l_clone->SetRefractionRatio( p_source.GetRefractionRatio() );
			l_clone->SetTwoSided( p_source.IsTwoSided() );
			l_clone->SetAlphaBlendMode( p_source.GetAlphaBlendMode() );
			l_clone->SetColourBlendMode( p_source.GetColourBlendMode() );
			l_clone->SetAlphaFunc( p_source.GetAlphaFunc() );
			l_clone->SetAlphaValue( p_source.GetAlphaValue() );

			for ( auto const & l_unit : p_source )
			{
				l_clone->AddTextureUnit( DoCloneUnit( l_clone, *l_unit ) );
			}
		}

		MaterialSPtr DoCloneMaterial( Material const & p_source )
		{
			MaterialSPtr l_clone = std::make_shared< Material >( p_source.GetName() + cuT( "_Clone" ), *p_source.GetEngine(), p_source.GetType() );

			for ( auto const & l_pass : p_source )
			{
				DoClonePass( l_clone, *l_pass );
			}

			return l_clone;
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
			auto l_geometry = p_selected.m_geometry;

			for ( auto & l_submesh : p_selected.m_submeshes )
			{
				Restore( l_submesh, l_geometry );
			}

			p_selected.m_geometry.reset();
			p_selected.m_submeshes.clear();
		}

		void Save( RenderPanel::SelectedGeometry & p_selected
			, GeometrySPtr p_geometry )
		{
			auto l_geometry = p_selected.m_geometry;
			p_selected.m_submeshes.reserve(( p_geometry->GetMesh()->GetSubmeshCount() ) );

			for ( auto & l_submesh : *p_geometry->GetMesh() )
			{
				p_selected.m_submeshes.emplace_back( l_submesh, p_geometry->GetMaterial( *l_submesh ) );
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
					auto l_pass = p_selected.m_selectedMaterial->GetTypedPass< MaterialType::eLegacy >( 0u );
					l_pass->SetDiffuse( Colour::from_predef( PredefinedColour::eMedAlphaRed ) );
					l_pass->SetSpecular( Colour::from_predef( PredefinedColour::eMedAlphaRed ) );
				}
				break;

			case MaterialType::ePbr:
				{
					auto l_pass = p_selected.m_selectedMaterial->GetTypedPass< MaterialType::ePbr >( 0u );
					l_pass->SetAlbedo( Colour::from_predef( PredefinedColour::eMedAlphaRed ) );
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
		wxClientDC l_dc( this );
	}

	void RenderPanel::SetRenderWindow( RenderWindowSPtr p_window )
	{
		m_renderWindow.reset();
		DoStopMovement();
		Castor::Size l_sizeWnd = GuiCommon::make_Size( GetClientSize() );

		if ( p_window && p_window->Initialise( l_sizeWnd, GuiCommon::make_WindowHandle( this ) ) )
		{
			Castor::Size l_sizeScreen;
			Castor::System::GetScreenSize( 0, l_sizeScreen );
			GetParent()->SetClientSize( l_sizeWnd.width(), l_sizeWnd.height() );
			l_sizeWnd = GuiCommon::make_Size( GetParent()->GetClientSize() );
			GetParent()->SetPosition( wxPoint( std::abs( int( l_sizeScreen.width() ) - int( l_sizeWnd.width() ) ) / 2, std::abs( int( l_sizeScreen.height() ) - int( l_sizeWnd.height() ) ) / 2 ) );
			m_listener = p_window->GetListener();
			SceneSPtr l_scene = p_window->GetScene();

			if ( l_scene )
			{
				auto l_camera = p_window->GetCamera();

				if ( l_camera )
				{
					if ( l_scene->GetSceneNodeCache().Has( cuT( "PointLightsNode" ) ) )
					{
						m_lightsNode = l_scene->GetSceneNodeCache().Find( cuT( "PointLightsNode" ) );
					}
					else if ( l_scene->GetSceneNodeCache().Has( cuT( "LightNode" ) ) )
					{
						m_lightsNode = l_scene->GetSceneNodeCache().Find( cuT( "LightNode" ) );
					}

					auto l_cameraNode = l_camera->GetParent();

					if ( l_cameraNode )
					{
						m_currentNode = l_cameraNode;
						m_currentState = &DoAddNodeState( m_currentNode );
					}

					m_renderWindow = p_window;
					m_keyboardEvent = std::make_unique< KeyboardEvent >( p_window );

					{
						auto l_lock = make_unique_lock( l_scene->GetCameraCache() );
						p_window->GetPickingPass().AddScene( *l_scene, *( l_scene->GetCameraCache().begin()->second ) );
					}

					m_camera = l_camera;
					DoStartMovement();
				}

				m_scene = l_scene;
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
		auto l_camera = m_camera.lock();

		if ( l_camera )
		{
			auto l_cameraNode = l_camera->GetParent();
			l_camera->GetScene()->GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, l_cameraNode]()
			{
				Quaternion l_orientation{ l_cameraNode->GetOrientation() };
				l_orientation *= Quaternion::from_axis_angle( Point3r{ 0.0_r, 1.0_r, 0.0_r }, Angle::from_degrees( 90.0_r ) );
				l_cameraNode->SetOrientation( l_orientation );
			} ) );
		}
	}

	void RenderPanel::DoTurnCameraVertic()
	{
		DoResetTimers();
		auto l_camera = m_camera.lock();

		if ( l_camera )
		{
			auto l_cameraNode = l_camera->GetParent();
			l_camera->GetScene()->GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, l_cameraNode]()
			{
				Quaternion l_orientation{ l_cameraNode->GetOrientation() };
				l_orientation *= Quaternion::from_axis_angle( Point3r{ 1.0_r, 0.0_r, 0.0_r }, Angle::from_degrees( 90.0_r ) );
				l_cameraNode->SetOrientation( l_orientation );
			} ) );
		}
	}

	void RenderPanel::DoChangeCamera()
	{
		auto l_camera = m_camera.lock();
		auto l_window = m_renderWindow.lock();
		auto l_scene = m_scene.lock();

		if ( l_camera && l_window && l_scene )
		{
			//auto & l_shadowMaps = l_window->GetRenderTarget()->GetTechnique()->GetShadowMaps();

			//if ( !l_shadowMaps.empty() )
			//{
			//	auto l_it = l_shadowMaps.begin();
			//	m_currentNode = l_it->first->GetParent();
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
		real l_result = real( x );
		auto l_camera = m_camera.lock();

		if ( l_camera )
		{
			l_result *= real( l_camera->GetWidth() ) / GetClientSize().x;
		}

		return l_result;
	}

	real RenderPanel::DoTransformY( int y )
	{
		real l_result = real( y );
		auto l_camera = m_camera.lock();

		if ( l_camera )
		{
			l_result *= real( l_camera->GetHeight() ) / GetClientSize().y;
		}

		return l_result;
	}

	int RenderPanel::DoTransformX( real x )
	{
		int l_result = int( x );
		auto l_camera = m_camera.lock();

		if ( l_camera )
		{
			l_result = int( x * GetClientSize().x / real( l_camera->GetWidth() ) );
		}

		return l_result;
	}

	int RenderPanel::DoTransformY( real y )
	{
		int l_result = int( y );
		auto l_camera = m_camera.lock();

		if ( l_camera )
		{
			l_result = int( y * GetClientSize().y / real( l_camera->GetHeight() ) );
		}

		return l_result;
	}

	void RenderPanel::DoUpdateSelectedGeometry( Castor3D::GeometrySPtr p_geometry
		, Castor3D::SubmeshSPtr p_submesh )
	{
		SelectedGeometry l_selected;
		auto l_submesh = m_selectedSubmesh ? m_selectedSubmesh->m_submesh : nullptr;
		auto l_oldGeometry = m_selectedGeometry.m_geometry;
		bool l_changed = false;
		SceneRPtr l_scene = p_geometry ? p_geometry->GetScene() : nullptr;

		if ( l_oldGeometry != p_geometry )
		{
			l_changed = true;
			m_selectedSubmesh = nullptr;

			if ( l_oldGeometry )
			{
				l_scene = l_oldGeometry->GetScene();
				Restore( m_selectedGeometry );
			}

			if ( p_geometry )
			{
				Save( m_selectedGeometry, p_geometry );
			}
		}

		if ( l_submesh != p_submesh )
		{
			if ( m_selectedSubmesh )
			{
				Restore( *m_selectedSubmesh, l_oldGeometry );
			}

			if ( p_submesh )
			{
				auto l_it = std::find_if( m_selectedGeometry.m_submeshes.begin()
					, m_selectedGeometry.m_submeshes.end()
					, [&p_submesh]( auto & l_selectedSubmesh )
					{
						return l_selectedSubmesh.m_submesh == p_submesh;
					} );
				REQUIRE( l_it != m_selectedGeometry.m_submeshes.end());
				m_selectedSubmesh = &( *l_it );
				Select( *m_selectedSubmesh
					, p_geometry );
			}
			l_changed = true;
		}

		if ( l_changed )
		{
			l_scene->GetListener().PostEvent( MakeFunctorEvent( EventType::ePostRender
				, [l_scene]()
				{
					l_scene->SetChanged();
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
		auto l_it = m_nodesStates.find( p_node->GetName() );

		if ( l_it == m_nodesStates.end() )
		{
			l_it = m_nodesStates.emplace( p_node->GetName()
				, std::make_unique< GuiCommon::NodeState >( *m_listener, p_node ) ).first;
		}

		l_it->second->SetMaxSpeed( m_camSpeed );
		return *l_it->second;
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
		auto l_window = m_renderWindow.lock();

		if ( l_window )
		{
			l_window->Resize( p_event.GetSize().x, p_event.GetSize().y );
		}
		else
		{
			wxClientDC l_dc( this );
			l_dc.SetBrush( wxBrush( GuiCommon::INACTIVE_TAB_COLOUR ) );
			l_dc.SetPen( wxPen( GuiCommon::INACTIVE_TAB_COLOUR ) );
			l_dc.DrawRectangle( 0, 0, p_event.GetSize().x, p_event.GetSize().y );
		}

		p_event.Skip();
	}

	void RenderPanel::OnMove( wxMoveEvent & p_event )
	{
		auto l_window = m_renderWindow.lock();

		if ( !l_window )
		{
			wxClientDC l_dc( this );
			l_dc.SetBrush( wxBrush( GuiCommon::INACTIVE_TAB_COLOUR ) );
			l_dc.SetPen( wxPen( GuiCommon::INACTIVE_TAB_COLOUR ) );
			l_dc.DrawRectangle( 0, 0, GetClientSize().x, GetClientSize().y );
		}

		p_event.Skip();
	}

	void RenderPanel::OnPaint( wxPaintEvent & p_event )
	{
		auto l_window = m_renderWindow.lock();

		if ( !l_window )
		{
			wxPaintDC l_dc( this );
			l_dc.SetBrush( wxBrush( GuiCommon::INACTIVE_TAB_COLOUR ) );
			l_dc.SetPen( wxPen( GuiCommon::INACTIVE_TAB_COLOUR ) );
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
		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !l_inputListener || !l_inputListener->FireKeyDown( ConvertKeyCode( p_event.GetKeyCode() ), p_event.ControlDown(), p_event.AltDown(), p_event.ShiftDown() ) )
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
		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !l_inputListener || !l_inputListener->FireKeyUp( ConvertKeyCode( p_event.GetKeyCode() ), p_event.ControlDown(), p_event.AltDown(), p_event.ShiftDown() ) )
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
		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( l_inputListener )
		{
			wxChar l_key = p_event.GetUnicodeKey();
			wxString l_tmp;
			l_tmp << l_key;
			l_inputListener->FireChar( ConvertKeyCode( p_event.GetKeyCode() ), String( l_tmp.mb_str( wxConvUTF8 ) ) );
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
			auto l_window = m_renderWindow.lock();

			if ( l_window )
			{
				wxGetApp().GetMainFrame()->ToggleFullScreen( !l_window->IsFullscreen() );
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

		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !l_inputListener || !l_inputListener->FireMouseButtonPushed( MouseButton::eLeft ) )
		{
			auto l_window = GetRenderWindow();

			if ( l_window )
			{
				if ( m_altDown )
				{
					auto l_x = m_oldX;
					auto l_y = m_oldY;
					m_listener->PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, l_window, l_x, l_y]()
					{
						Camera & l_camera = *l_window->GetCamera();
						l_camera.Update();
						auto l_type = l_window->GetPickingPass().Pick( Position{ int( l_x ), int( l_y ) }, l_camera );

						if ( l_type != PickingPass::NodeType::eNone
							&& l_type != PickingPass::NodeType::eBillboard )
						{
							DoUpdateSelectedGeometry( l_window->GetPickingPass().GetPickedGeometry(), l_window->GetPickingPass().GetPickedSubmesh() );
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

		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !l_inputListener || !l_inputListener->FireMouseButtonReleased( MouseButton::eLeft ) )
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

		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !l_inputListener || !l_inputListener->FireMouseButtonPushed( MouseButton::eMiddle ) )
		{
			auto l_window = GetRenderWindow();

			if ( l_window )
			{
				auto l_x = m_oldX;
				auto l_y = m_oldY;
				m_listener->PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, l_window, l_x, l_y]()
				{
					Camera & l_camera = *l_window->GetCamera();
					l_camera.Update();
					auto l_type = l_window->GetPickingPass().Pick( Position{ int( l_x ), int( l_y ) }, l_camera );

					if ( l_type != PickingPass::NodeType::eNone
						&& l_type != PickingPass::NodeType::eBillboard )
					{
						DoUpdateSelectedGeometry( l_window->GetPickingPass().GetPickedGeometry(), l_window->GetPickingPass().GetPickedSubmesh() );
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

		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( l_inputListener  )
		{
			l_inputListener->FireMouseButtonReleased( MouseButton::eMiddle );
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

		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( l_inputListener  )
		{
			l_inputListener->FireMouseButtonPushed( MouseButton::eRight );
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

		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( l_inputListener  )
		{
			l_inputListener->FireMouseButtonReleased( MouseButton::eRight );
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseMove( wxMouseEvent & p_event )
	{
		m_x = DoTransformX( p_event.GetX() );
		m_y = DoTransformY( p_event.GetY() );

		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !l_inputListener || !l_inputListener->FireMouseMove( Position( int32_t( m_x ), int32_t( m_y ) ) ) )
		{
			if ( m_currentState )
			{
				static real constexpr l_mult = 4.0_r;
				real l_deltaX = std::min( m_camSpeed / l_mult, 1.0_r ) * ( m_oldX - m_x ) / l_mult;
				real l_deltaY = std::min( m_camSpeed / l_mult, 1.0_r ) * ( m_oldY - m_y ) / l_mult;

				if ( p_event.ControlDown() )
				{
					l_deltaX = 0;
				}
				else if ( p_event.ShiftDown() )
				{
					l_deltaY = 0;
				}

				if ( m_mouseLeftDown )
				{
					m_currentState->AddAngularVelocity( Point2r{ -l_deltaY, l_deltaX } );
				}
				else if ( m_mouseRightDown )
				{
					m_currentState->AddScalarVelocity( Point3r{ l_deltaX, l_deltaY, 0.0_r } );
				}
			}
		}

		m_oldX = m_x;
		m_oldY = m_y;
		p_event.Skip();
	}

	void RenderPanel::OnMouseWheel( wxMouseEvent & p_event )
	{
		int l_wheelRotation = p_event.GetWheelRotation();

		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !l_inputListener || !l_inputListener->FireMouseWheel( Position( 0, l_wheelRotation ) ) )
		{
			if ( l_wheelRotation < 0 )
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
