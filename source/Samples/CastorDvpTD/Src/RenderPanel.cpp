#include "RenderPanel.hpp"

#include "CastorDvpTD.hpp"
#include "TranslateNodeEvent.hpp"
#include "RotateNodeEvent.hpp"
#include "Game.hpp"

#include <Event/Frame/FrameListener.hpp>
#include <Event/UserInput/UserInputListener.hpp>
#include <Material/Material.hpp>
#include <Material/Pass.hpp>
#include <Miscellaneous/Ray.hpp>
#include <Render/RenderWindow.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>
#include <Texture/TextureUnit.hpp>

using namespace Castor;
using namespace Castor3D;

namespace castortd
{
	namespace
	{
		typedef enum eMENU_ID
		{
			eMENU_ID_NEW_LR_TOWER = 1,
			eMENU_ID_NEW_SR_TOWER,
			eMENU_ID_UPGRADE_SPEED,
			eMENU_ID_UPGRADE_RANGE,
			eMENU_ID_UPGRADE_DAMAGE,
		}	eMENU_ID;

		static const real MAX_CAM_SPEED = 10.0_r;
		static const real DEF_CAM_SPEED = 2.0_r;
		static const real MIN_CAM_SPEED = 0.1_r;
		static const real CAM_SPEED_INC = 0.9_r;

		eKEYBOARD_KEY ConvertKeyCode( int p_code )
		{
			eKEYBOARD_KEY l_return = eKEYBOARD_KEY_NONE;

			if ( p_code < 0x20 )
			{
				switch ( p_code )
				{
				case WXK_BACK:
				case WXK_TAB:
				case WXK_RETURN:
				case WXK_ESCAPE:
					l_return = eKEYBOARD_KEY( p_code );
					break;
				}
			}
			else if ( p_code == 0x7F )
			{
				l_return = eKEY_DELETE;
			}
			else if ( p_code > 0xFF )
			{
				l_return = eKEYBOARD_KEY( p_code + eKEY_START - WXK_START );
			}
			else
			{
				// ASCII or extended ASCII character
				l_return = eKEYBOARD_KEY( p_code );
			}

			return l_return;
		}

		TextureUnitSPtr DoCloneUnit( PassSPtr p_clone, TextureUnit const & p_source )
		{
			TextureUnitSPtr l_clone = std::make_shared< TextureUnit >( *p_clone->GetEngine() );

			l_clone->SetAlpArgument( BlendSrcIndex::Index0, p_source.GetAlpArgument( BlendSrcIndex::Index0 ) );
			l_clone->SetAlpArgument( BlendSrcIndex::Index1, p_source.GetAlpArgument( BlendSrcIndex::Index1 ) );
			l_clone->SetAlpArgument( BlendSrcIndex::Index2, p_source.GetAlpArgument( BlendSrcIndex::Index2 ) );
			l_clone->SetAlpFunction( p_source.GetAlpFunction() );
			l_clone->SetAlphaFunc( p_source.GetAlphaFunc() );
			l_clone->SetAlphaValue( p_source.GetAlphaValue() );
			l_clone->SetAutoMipmaps( p_source.GetAutoMipmaps() );
			l_clone->SetBlendColour( p_source.GetBlendColour() );
			l_clone->SetChannel( p_source.GetChannel() );
			l_clone->SetIndex( p_source.GetIndex() );
			l_clone->SetRenderTarget( p_source.GetRenderTarget() );
			l_clone->SetRgbArgument( BlendSrcIndex::Index0, p_source.GetRgbArgument( BlendSrcIndex::Index0 ) );
			l_clone->SetRgbArgument( BlendSrcIndex::Index1, p_source.GetRgbArgument( BlendSrcIndex::Index1 ) );
			l_clone->SetRgbArgument( BlendSrcIndex::Index2, p_source.GetRgbArgument( BlendSrcIndex::Index2 ) );
			l_clone->SetRgbFunction( p_source.GetRgbFunction() );
			l_clone->SetSampler( p_source.GetSampler() );
			l_clone->SetTexture( p_source.GetTexture() );

			return l_clone;
		}

		PassSPtr DoClonePass( MaterialSPtr p_clone, Pass const & p_source )
		{
			PassSPtr l_clone = std::make_shared< Pass >( *p_clone->GetEngine(), p_clone );
			l_clone->SetAmbient( p_source.GetAmbient() );
			l_clone->SetDiffuse( p_source.GetDiffuse() );
			l_clone->SetSpecular( p_source.GetSpecular() );
			l_clone->SetEmissive( p_source.GetEmissive() );
			l_clone->SetAlpha( p_source.GetAlpha() );
			l_clone->SetAlphaBlendMode( p_source.GetAlphaBlendMode() );
			l_clone->SetColourBlendMode( p_source.GetColourBlendMode() );
			l_clone->SetShininess( p_source.GetShininess() );
			l_clone->SetTwoSided( p_source.IsTwoSided() );

			for ( auto const & l_unit : p_source )
			{
				l_clone->AddTextureUnit( DoCloneUnit( l_clone, *l_unit ) );
			}

			return l_clone;
		}

		MaterialSPtr DoCloneMaterial( Material const & p_source )
		{
			MaterialSPtr l_clone = std::make_shared< Material >( p_source.GetName() + cuT( "_Clone" ), *p_source.GetEngine() );

			for ( auto const & l_pass : p_source )
			{
				l_clone->AddPass( DoClonePass( l_clone, *l_pass ) );
			}

			return l_clone;
		}
	}

	RenderPanel::RenderPanel( wxWindow * p_parent, wxSize const & p_size, Game & p_game )
		: wxPanel{ p_parent, wxID_ANY, wxDefaultPosition, p_size }
		, m_camSpeed{ DEF_CAM_SPEED }
		, m_game{ p_game }
		, m_picking{ *wxGetApp().GetCastor() }
	{
		for ( int i = 0; i < eTIMER_ID_COUNT; i++ )
		{
			m_timer[i] = new wxTimer( this, i );
		}

		m_cursorArrow = new wxCursor( wxCURSOR_ARROW );
		m_cursorNone = new wxCursor( wxCURSOR_BLANK );
	}

	RenderPanel::~RenderPanel()
	{
	}

	void RenderPanel::SetRenderWindow( Castor3D::RenderWindowSPtr p_window )
	{
		m_renderWindow.reset();

		if ( p_window )
		{
			Castor::Size l_sizeWnd = GuiCommon::make_Size( GetClientSize() );

			if ( p_window->Initialise( l_sizeWnd, GuiCommon::make_WindowHandle( this ) ) )
			{
				Castor::Size l_sizeScreen;
				Castor::System::GetScreenSize( 0, l_sizeScreen );
				GetParent()->SetClientSize( l_sizeWnd.width(), l_sizeWnd.height() );
				l_sizeWnd = GuiCommon::make_Size( GetParent()->GetClientSize() );
				GetParent()->SetPosition( wxPoint( std::abs( int( l_sizeScreen.width() ) - int( l_sizeWnd.width() ) ) / 2, std::abs( int( l_sizeScreen.height() ) - int( l_sizeWnd.height() ) ) / 2 ) );
				SceneSPtr l_scene = p_window->GetScene();

				if ( l_scene )
				{
					m_node = l_scene->GetSceneNodeCache().Find( cuT( "MapBase" ) );
					m_marker = l_scene->GetSceneNodeCache().Find( cuT( "MapMouse" ) );
					m_marker->SetVisible( false );

					if ( m_node )
					{
						m_originalPosition = m_node->GetPosition();
						m_originalOrientation = m_node->GetOrientation();
					}

					m_listener = p_window->GetListener();
					m_renderWindow = p_window;

					{
						auto l_lock = make_unique_lock( l_scene->GetCameraCache() );
						m_picking.AddScene( *l_scene, *( l_scene->GetCameraCache().begin()->second ) );
					}

					m_listener->PostEvent( MakeFunctorEvent( EventType::PreRender, [this, l_sizeWnd]()
					{
						m_picking.Initialise( l_sizeWnd );
					} ) );
				}
			}
		}
		else if ( m_listener )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::PreRender, [this]()
			{
				m_picking.Cleanup();
			} ) );
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

	void RenderPanel::DoStartTimer( int p_iId )
	{
		if ( m_game.IsRunning() )
		{
			m_timer[p_iId]->Start( 10 );
		}
	}

	void RenderPanel::DoStopTimer( int p_iId )
	{
		if ( p_iId != eTIMER_ID_COUNT )
		{
			m_timer[p_iId]->Stop();
		}
		else
		{
			for ( int i = 0; i < eTIMER_ID_COUNT; i++ )
			{
				m_timer[i]->Stop();
			}
		}
	}

	void RenderPanel::DoResetCamera()
	{
		DoResetTimers();

		if ( m_node )
		{
			RenderWindowSPtr l_window = m_renderWindow.lock();

			if ( l_window )
			{
				l_window->GetScene()->SetChanged();
			}

			m_listener->PostEvent( MakeFunctorEvent( EventType::PreRender, [this]()
			{
				m_node->SetOrientation( m_originalOrientation );
				m_node->SetPosition( m_originalPosition );
			} ) );
			m_camSpeed = DEF_CAM_SPEED;
		}
	}

	real RenderPanel::DoTransformX( int x )
	{
		real l_result = real( x );
		RenderWindowSPtr l_window = m_renderWindow.lock();

		if ( l_window )
		{
			l_result *= real( l_window->GetCamera()->GetWidth() ) / GetClientSize().x;
		}

		return l_result;
	}

	real RenderPanel::DoTransformY( int y )
	{
		real l_result = real( y );
		RenderWindowSPtr l_window = m_renderWindow.lock();

		if ( l_window )
		{
			l_result *= real( l_window->GetCamera()->GetHeight() ) / GetClientSize().y;
		}

		return l_result;
	}

	int RenderPanel::DoTransformX( real x )
	{
		int l_result = int( x );
		RenderWindowSPtr l_window = m_renderWindow.lock();

		if ( l_window )
		{
			l_result = int( x * GetClientSize().x / real( l_window->GetCamera()->GetWidth() ) );
		}

		return l_result;
	}

	int RenderPanel::DoTransformY( real y )
	{
		int l_result = int( y );
		RenderWindowSPtr l_window = m_renderWindow.lock();

		if ( l_window )
		{
			l_result = int( y * GetClientSize().y / real( l_window->GetCamera()->GetHeight() ) );
		}

		return l_result;
	}

	void RenderPanel::DoUpdateSelectedGeometry( Castor3D::GeometrySPtr p_geometry )
	{
		auto l_geometry = m_selectedGeometry.lock();

		if ( p_geometry != l_geometry )
		{
			if ( !p_geometry )
			{
				l_geometry->GetScene()->SetChanged();

				m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this, l_geometry]()
				{
					m_marker->SetVisible( false );
				} ) );
			}
			else
			{
				auto l_cell = m_game.GetCell( p_geometry->GetParent()->GetPosition() );

				if ( l_cell.m_state != Cell::State::Invalid )
				{
					p_geometry->GetScene()->SetChanged();

					switch( l_cell.m_state )
					{
					case Cell::State::Empty:
						if ( !l_geometry )
						{
							m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this, p_geometry]()
							{
								m_marker->SetVisible( true );
							} ) );
						}
						m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this, p_geometry]()
						{
							Point3r l_position = p_geometry->GetParent()->GetPosition();
							auto l_height = p_geometry->GetMesh()->GetCollisionBox().GetMax()[1] - p_geometry->GetMesh()->GetCollisionBox().GetMin()[1];
							m_marker->SetPosition( Point3r{ l_position[0], l_height + 1, l_position[2] } );
						} ) );
						m_selectedTower = nullptr;
						break;

					case Cell::State::Tower:
						m_selectedTower = m_game.SelectTower( l_cell );
						break;

					case Cell::State::Target:
						m_selectedTower = nullptr;
						break;

					case Cell::State::Path:
						m_selectedTower = nullptr;
						break;

					default:
						m_selectedTower = nullptr;
						break;
					}
				}
			}

			m_selectedGeometry = p_geometry;
		}
	}

	void RenderPanel::DoUpgradeTowerDamage()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_game.CanAfford( m_selectedTower->GetDamageUpgradeCost() ) )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this]()
			{
				m_game.UpgradeTowerDamage( *m_selectedTower );
			} ) );
		}
	}

	void RenderPanel::DoUpgradeTowerSpeed()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_game.CanAfford( m_selectedTower->GetSpeedUpgradeCost() ) )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this]()
			{
				m_game.UpgradeTowerSpeed( *m_selectedTower );
			} ) );
		}
	}

	void RenderPanel::DoUpgradeTowerRange()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_game.CanAfford( m_selectedTower->GetRangeUpgradeCost() ) )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this]()
			{
				m_game.UpgradeTowerRange( *m_selectedTower );
			} ) );
		}
	}

	BEGIN_EVENT_TABLE( RenderPanel, wxPanel )
		EVT_TIMER( eTIMER_ID_FORWARD, RenderPanel::OnTimerFwd )
		EVT_TIMER( eTIMER_ID_BACK, RenderPanel::OnTimerBck )
		EVT_TIMER( eTIMER_ID_LEFT, RenderPanel::OnTimerLft )
		EVT_TIMER( eTIMER_ID_RIGHT, RenderPanel::OnTimerRgt )
		EVT_SIZE( RenderPanel::OnSize )
		EVT_MOVE( RenderPanel::OnMove )
		EVT_PAINT( RenderPanel::OnPaint )
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
		EVT_MENU( eMENU_ID_NEW_LR_TOWER, RenderPanel::OnNewLongRangeTower )
		EVT_MENU( eMENU_ID_NEW_SR_TOWER, RenderPanel::OnNewShortRangeTower )
		EVT_MENU( eMENU_ID_UPGRADE_SPEED, RenderPanel::OnUpgradeTowerSpeed )
		EVT_MENU( eMENU_ID_UPGRADE_RANGE, RenderPanel::OnUpgradeTowerRange )
		EVT_MENU( eMENU_ID_UPGRADE_DAMAGE, RenderPanel::OnUpgradeTowerDamage )
	END_EVENT_TABLE()

	void RenderPanel::OnTimerFwd( wxTimerEvent & p_event )
	{
		m_listener->PostEvent( std::make_unique< TranslateNodeEvent >( m_node, 0.0_r, 0.0_r, -m_camSpeed ) );
		p_event.Skip();
	}

	void RenderPanel::OnTimerBck( wxTimerEvent & p_event )
	{
		m_listener->PostEvent( std::make_unique< TranslateNodeEvent >( m_node, 0.0_r, 0.0_r, m_camSpeed ) );
		p_event.Skip();
	}

	void RenderPanel::OnTimerLft( wxTimerEvent & p_event )
	{
		m_listener->PostEvent( std::make_unique< TranslateNodeEvent >( m_node, -m_camSpeed, 0.0_r, 0.0_r ) );
		p_event.Skip();
	}

	void RenderPanel::OnTimerRgt( wxTimerEvent & p_event )
	{
		m_listener->PostEvent( std::make_unique< TranslateNodeEvent >( m_node, m_camSpeed, 0.0_r, 0.0_r ) );
		p_event.Skip();
	}

	void RenderPanel::OnSize( wxSizeEvent & p_event )
	{
		RenderWindowSPtr l_window = m_renderWindow.lock();

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
		RenderWindowSPtr l_window = m_renderWindow.lock();

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
		RenderWindowSPtr l_window = m_renderWindow.lock();

		if ( !l_window )
		{
			wxPaintDC l_dc( this );
			l_dc.SetBrush( wxBrush( GuiCommon::INACTIVE_TAB_COLOUR ) );
			l_dc.SetPen( wxPen( GuiCommon::INACTIVE_TAB_COLOUR ) );
			l_dc.DrawRectangle( 0, 0, GetClientSize().x, GetClientSize().y );
		}

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
				DoResetCamera();
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

			case WXK_NUMPAD1:
			case '&':
			case '1':
				DoUpgradeTowerDamage();
				break;

			case WXK_NUMPAD2:
			case 'é':
			case '2':
				DoUpgradeTowerRange();
				break;

			case WXK_NUMPAD3:
			case '\"':
			case '3':
				DoUpgradeTowerSpeed();
				break;

			case WXK_F1:
				m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this]()
				{
					if ( m_game.IsRunning() )
					{
						m_game.Help();
					}
				} ) );
				break;

			case WXK_RETURN:
				m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this]()
				{
					if ( m_game.IsEnded() )
					{
						m_game.Reset();
						m_game.Start();
					}
					else if ( !m_game.IsStarted() )
					{
						m_game.Start();
					}
				} ) );
				break;

			case WXK_SPACE:
				m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this]()
				{
					if ( m_game.IsStarted() )
					{
						if ( m_game.IsPaused() )
						{
							m_game.Resume();
						}
						else
						{
							m_game.Pause();
						}
					}
				} ) );
				break;
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseLDClick( wxMouseEvent & p_event )
	{
		m_oldX = DoTransformX( p_event.GetX() );
		m_oldY = DoTransformY( p_event.GetY() );

		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !l_inputListener || !l_inputListener->FireMouseButtonPushed( eMOUSE_BUTTON_MIDDLE ) )
		{
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseLDown( wxMouseEvent & p_event )
	{
		m_mouseLeftDown = true;
		m_oldX = DoTransformX( p_event.GetX() );
		m_oldY = DoTransformY( p_event.GetY() );

		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !l_inputListener || !l_inputListener->FireMouseButtonPushed( eMOUSE_BUTTON_LEFT ) )
		{
			SetCursor( *m_cursorNone );
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseLUp( wxMouseEvent & p_event )
	{
		m_mouseLeftDown = false;
		m_oldX = DoTransformX( p_event.GetX() );
		m_oldY = DoTransformY( p_event.GetY() );

		auto l_window = GetRenderWindow();

		if ( l_window )
		{
#if defined( NDEBUG )
			l_window->GetScene()->SetChanged();
#endif
			auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

			if ( !l_inputListener || !l_inputListener->FireMouseButtonReleased( eMOUSE_BUTTON_LEFT ) )
			{
				SetCursor( *m_cursorArrow );

				if ( m_game.IsRunning() )
				{
					m_listener->PostEvent( MakeFunctorEvent( EventType::PreRender, [this, l_window]()
					{
						SubmeshSPtr l_submesh;
						Face l_face{ 0, 0, 0 };
						real l_distance{ std::numeric_limits< real >::max() };
						Camera const & l_camera = *l_window->GetCamera();
						auto l_geometry = l_window->GetScene()->GetObjectRootNode()->GetNearestGeometry( Ray{ int( m_oldX ), int( m_oldY ), l_camera }
																										 , l_camera
																										 , l_distance
																										 , l_face
																										 , l_submesh );
						DoUpdateSelectedGeometry( l_geometry );
					} ) );

					//m_listener->PostEvent( MakeFunctorEvent( EventType::PreRender, [this, l_window]()
					//{
					//	Camera const & l_camera = *l_window->GetCamera();
					//	m_picking.Update();
					//	auto l_geometry = m_picking.Pick( Position{ int32_t( m_oldX ), int32_t( m_oldY ) }, l_camera );

					//	if ( l_geometry && l_geometry->GetMesh()->GetName() == cuT( "MapBase" ) )
					//	{
					//		l_geometry.reset();
					//	}

					//	DoUpdateSelectedGeometry( l_geometry );
					//} ) );
				}
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseMDown( wxMouseEvent & p_event )
	{
		m_mouseMiddleDown = true;
		m_oldX = DoTransformX( p_event.GetX() );
		m_oldY = DoTransformY( p_event.GetY() );
		p_event.Skip();
	}

	void RenderPanel::OnMouseMUp( wxMouseEvent & p_event )
	{
		m_mouseMiddleDown = false;

		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !l_inputListener || !l_inputListener->FireMouseButtonReleased( eMOUSE_BUTTON_MIDDLE ) )
		{
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseRDown( wxMouseEvent & p_event )
	{
		m_mouseRightDown = true;
		m_oldX = DoTransformX( p_event.GetX() );
		m_oldY = DoTransformY( p_event.GetY() );

		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( !l_inputListener || !l_inputListener->FireMouseButtonPushed( eMOUSE_BUTTON_RIGHT ) )
		{
			SetCursor( *m_cursorNone );
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseRUp( wxMouseEvent & p_event )
	{
		m_mouseRightDown = false;

		auto l_window = GetRenderWindow();

		if ( l_window )
		{
#if defined( NDEBUG )
			l_window->GetScene()->SetChanged();
#endif
			auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

			if ( !l_inputListener || !l_inputListener->FireMouseButtonReleased( eMOUSE_BUTTON_RIGHT ) )
			{
				SetCursor( *m_cursorArrow );

				if ( m_game.IsRunning() && m_selectedTower )
				{
					wxMenu l_menu;

					if ( m_selectedTower->CanUpgradeSpeed() )
					{
						l_menu.Append( eMENU_ID_UPGRADE_SPEED, wxString{ wxT( "Augmenter vitesse (" ) } << m_selectedTower->GetSpeedUpgradeCost() << wxT( ")" ) );
						l_menu.Enable( eMENU_ID_UPGRADE_SPEED, m_game.CanAfford( m_selectedTower->GetSpeedUpgradeCost() ) );
					}
					else
					{
						l_menu.Append( eMENU_ID_UPGRADE_SPEED, wxString{ wxT( "Augmenter vitesse (Max)" ) } );
						l_menu.Enable( eMENU_ID_UPGRADE_SPEED, false );
					}

					if ( m_selectedTower->CanUpgradeRange() )
					{
						l_menu.Append( eMENU_ID_UPGRADE_RANGE, wxString{ wxT( "Augmenter portee (" ) } << m_selectedTower->GetRangeUpgradeCost() << wxT( ")" ) );
						l_menu.Enable( eMENU_ID_UPGRADE_RANGE, m_game.CanAfford( m_selectedTower->GetRangeUpgradeCost() ) );
					}
					else
					{
						l_menu.Append( eMENU_ID_UPGRADE_RANGE, wxString{ wxT( "Augmenter portee (Max)" ) } );
						l_menu.Enable( eMENU_ID_UPGRADE_RANGE, false );
					}

					if ( m_selectedTower->CanUpgradeDamage() )
					{
						l_menu.Append( eMENU_ID_UPGRADE_DAMAGE, wxString{ wxT( "Augmenter degats (" ) } << m_selectedTower->GetDamageUpgradeCost() << wxT( ")" ) );
						l_menu.Enable( eMENU_ID_UPGRADE_DAMAGE, m_game.CanAfford( m_selectedTower->GetDamageUpgradeCost() ) );
					}
					else
					{
						l_menu.Append( eMENU_ID_UPGRADE_DAMAGE, wxString{ wxT( "Augmenter degats (Max)" ) } );
						l_menu.Enable( eMENU_ID_UPGRADE_DAMAGE, false );
					}

					PopupMenu( &l_menu, p_event.GetPosition() );
				}
				else if ( !m_selectedGeometry.expired() )
				{
					wxMenu l_menu;
					l_menu.Append( eMENU_ID_NEW_LR_TOWER, wxString( "Nouvelle Tour Longue Distance (" ) << m_longRange.GetTowerCost() << wxT( ")" ) );
					l_menu.Append( eMENU_ID_NEW_SR_TOWER, wxString( "Nouvelle Tour Courte Distance (" ) << m_shortRange.GetTowerCost() << wxT( ")" ) );
					l_menu.Enable( eMENU_ID_NEW_LR_TOWER, m_game.CanAfford( m_longRange.GetTowerCost() ) );
					l_menu.Enable( eMENU_ID_NEW_SR_TOWER, m_game.CanAfford( m_shortRange.GetTowerCost() ) );
					PopupMenu( &l_menu, p_event.GetPosition() );
				}
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseMove( wxMouseEvent & p_event )
	{
		m_x = DoTransformX( p_event.GetX() );
		m_y = DoTransformY( p_event.GetY() );

		auto l_inputListener = wxGetApp().GetCastor()->GetUserInputListener();

		if ( l_inputListener && l_inputListener->FireMouseMove( Position( int32_t( m_x ), int32_t( m_y ) ) ) )
		{
			p_event.Skip();
		}
		else if ( m_game.IsRunning() )
		{
			RenderWindowSPtr l_window = m_renderWindow.lock();

			if ( l_window )
			{
				real l_deltaX = ( std::min( m_camSpeed, 2.0_r ) / 2.0_r ) * ( m_oldX - m_x ) / 2.0_r;
				real l_deltaY = ( std::min( m_camSpeed, 2.0_r ) / 2.0_r ) * ( m_oldY - m_y ) / 2.0_r;

				if ( m_mouseLeftDown )
				{
					l_deltaY = 0;
					m_listener->PostEvent( std::make_unique< RotateNodeEvent >( m_node, -l_deltaY, l_deltaX, 0.0_r ) );
				}
				else if ( m_mouseRightDown )
				{
					m_listener->PostEvent( std::make_unique< TranslateNodeEvent >( m_node, l_deltaX, 0.0_r, l_deltaY ) );
				}

				if ( m_mouseLeftDown || m_mouseRightDown )
				{
					WarpPointer( DoTransformX( m_oldX ), DoTransformY( m_oldY ) );
					m_x = m_oldX;
					m_y = m_oldY;
				}

				p_event.Skip( false );
			}
			else
			{
				p_event.Skip();
			}
		}

		m_oldX = m_x;
		m_oldY = m_y;
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
		}

		p_event.Skip();
	}

	void RenderPanel::OnNewLongRangeTower( wxCommandEvent & p_event )
	{
		if ( m_game.IsRunning() )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this]()
			{
				if ( m_game.BuildTower( m_marker->GetPosition(), std::make_unique< LongRangeTower >( m_longRange ) ) )
				{
					DoUpdateSelectedGeometry( nullptr );
				}
			} ) );
		}
	}

	void RenderPanel::OnNewShortRangeTower( wxCommandEvent & p_event )
	{
		if ( m_game.IsRunning() )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this]()
			{
				if ( m_game.BuildTower( m_marker->GetPosition(), std::make_unique< ShortRangeTower >( m_shortRange ) ) )
				{
					DoUpdateSelectedGeometry( nullptr );
				}
			} ) );
		}
	}

	void RenderPanel::OnUpgradeTowerSpeed( wxCommandEvent & p_event )
	{
		DoUpgradeTowerSpeed();
	}

	void RenderPanel::OnUpgradeTowerRange( wxCommandEvent & p_event )
	{
		DoUpgradeTowerRange();
	}

	void RenderPanel::OnUpgradeTowerDamage( wxCommandEvent & p_event )
	{
		DoUpgradeTowerDamage();
	}
}
