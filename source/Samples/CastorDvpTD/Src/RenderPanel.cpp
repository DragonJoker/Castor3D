#include "RenderPanel.hpp"

#include "CastorDvpTD.hpp"
#include "Game.hpp"

#include <Event/Frame/FrameListener.hpp>
#include <Event/UserInput/UserInputListener.hpp>
#include <Material/Material.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Mesh.hpp>
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
		enum class MenuID
		{
			eNewLRTower = 1,
			eNewSRTower,
			eUpgradeSpeed,
			eUpgradeRange,
			eUpgradeDamage,
		};

		static real const g_camSpeed = 10.0_r;
	}

	RenderPanel::RenderPanel( wxWindow * p_parent, wxSize const & p_size, Game & p_game )
		: wxPanel{ p_parent, wxID_ANY, wxDefaultPosition, p_size }
		, m_game{ p_game }
		, m_timers
		{
			new wxTimer( this, int( TimerID::eUp ) ),
			new wxTimer( this, int( TimerID::eDown ) ),
			new wxTimer( this, int( TimerID::eLeft ) ),
			new wxTimer( this, int( TimerID::eRight ) ),
			new wxTimer( this, int( TimerID::eMouse ) ),
		}
	{
	}

	RenderPanel::~RenderPanel()
	{
		for ( auto & timer : m_timers )
		{
			delete timer;
		}
	}

	void RenderPanel::SetRenderWindow( Castor3D::RenderWindowSPtr p_window )
	{
		m_renderWindow.reset();
		Castor::Size l_sizeWnd = GuiCommon::make_Size( GetClientSize() );

		if ( p_window && p_window->Initialise( l_sizeWnd, GuiCommon::make_WindowHandle( this ) ) )
		{
			Castor::Size l_sizeScreen;
			Castor::System::GetScreenSize( 0, l_sizeScreen );
			GetParent()->SetClientSize( l_sizeWnd.width(), l_sizeWnd.height() );
			l_sizeWnd = GuiCommon::make_Size( GetParent()->GetClientSize() );
			GetParent()->SetPosition( wxPoint( std::abs( int( l_sizeScreen.width() ) - int( l_sizeWnd.width() ) ) / 2, std::abs( int( l_sizeScreen.height() ) - int( l_sizeWnd.height() ) ) / 2 ) );
			SceneSPtr l_scene = p_window->GetScene();

			if ( l_scene )
			{
				m_marker = l_scene->GetSceneNodeCache().Find( cuT( "MapMouse" ) );
				m_marker->SetVisible( false );

				m_listener = p_window->GetListener();
				m_renderWindow = p_window;

				if ( p_window )
				{
					auto l_lock = make_unique_lock( l_scene->GetCameraCache() );
					auto l_camera = l_scene->GetCameraCache().begin()->second;
					p_window->GetPickingPass().AddScene( *l_scene, *l_camera );
					m_cameraState = std::make_unique< GuiCommon::NodeState >( l_scene->GetListener(), l_camera->GetParent() );
					m_timers[size_t( TimerID::eMouse )]->Start( 30 );
				}
			}
		}
		else if ( m_listener )
		{
			m_listener.reset();
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
			bool l_freeCell = false;

			if ( !p_geometry || p_geometry->GetName() == cuT( "MapBase" ) )
			{
				m_selectedGeometry.reset();
			}
			else
			{
				Cell l_cell{ 0u, 0u, Cell::State::Invalid };

				if ( p_geometry->GetName().find( cuT( "Tower" ) ) == String::npos )
				{
					l_cell = m_game.GetCell( p_geometry->GetParent()->GetPosition() );
				}
				else
				{
					l_cell = m_game.GetCell( p_geometry->GetParent()->GetParent()->GetPosition() );
				}

				if ( l_cell.m_state != Cell::State::Invalid )
				{
					switch ( l_cell.m_state )
					{
					case Cell::State::Empty:
						l_freeCell = true;
						m_listener->PostEvent( MakeFunctorEvent( EventType::ePostRender, [this, p_geometry]()
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

					m_selectedGeometry = p_geometry;
				}
			}

			m_listener->PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, l_freeCell]()
			{
				m_marker->SetVisible( l_freeCell );
			} ) );
		}
	}

	void RenderPanel::DoUpgradeTowerDamage()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_selectedTower->CanUpgradeDamage() && m_game.CanAfford( m_selectedTower->GetDamageUpgradeCost() ) )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::ePostRender, [this]()
			{
				m_game.UpgradeTowerDamage( *m_selectedTower );
			} ) );
		}
	}

	void RenderPanel::DoUpgradeTowerSpeed()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_selectedTower->CanUpgradeSpeed() && m_game.CanAfford( m_selectedTower->GetSpeedUpgradeCost() ) )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::ePostRender, [this]()
			{
				m_game.UpgradeTowerSpeed( *m_selectedTower );
			} ) );
		}
	}

	void RenderPanel::DoUpgradeTowerRange()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_selectedTower->CanUpgradeRange() && m_game.CanAfford( m_selectedTower->GetRangeUpgradeCost() ) )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::ePostRender, [this]()
			{
				m_game.UpgradeTowerRange( *m_selectedTower );
			} ) );
		}
	}

	void RenderPanel::DoStartTimer( TimerID p_id )
	{
		m_timers[size_t( p_id )]->Start( 10 );
	}

	void RenderPanel::DoStopTimer( TimerID p_id )
	{
		if ( p_id != TimerID::eCount )
		{
			m_timers[size_t( p_id )]->Stop();
		}
		else
		{
			for ( size_t i = 0; i < size_t( TimerID::eMouse ); i++ )
			{
				m_timers[i]->Stop();
			}
		}
	}

	BEGIN_EVENT_TABLE( RenderPanel, wxPanel )
		EVT_SIZE( RenderPanel::OnSize )
		EVT_MOVE( RenderPanel::OnMove )
		EVT_PAINT( RenderPanel::OnPaint )
		EVT_KEY_DOWN( RenderPanel::OnKeyDown )
		EVT_KEY_UP( RenderPanel::OnKeyUp )
		EVT_LEFT_DOWN( RenderPanel::OnMouseLDown )
		EVT_LEFT_UP( RenderPanel::OnMouseLUp )
		EVT_RIGHT_UP( RenderPanel::OnMouseRUp )
		EVT_MOTION( RenderPanel::OnMouseMove )
		EVT_MOUSEWHEEL( RenderPanel::OnMouseWheel )
		EVT_TIMER( int( TimerID::eUp ), RenderPanel::OnTimerUp )
		EVT_TIMER( int( TimerID::eDown ), RenderPanel::OnTimerDown )
		EVT_TIMER( int( TimerID::eLeft ), RenderPanel::OnTimerLeft )
		EVT_TIMER( int( TimerID::eRight ), RenderPanel::OnTimerRight )
		EVT_TIMER( int( TimerID::eMouse ), RenderPanel::OnMouseTimer )
		EVT_MENU( int( MenuID::eNewLRTower ), RenderPanel::OnNewLongRangeTower )
		EVT_MENU( int( MenuID::eNewSRTower ), RenderPanel::OnNewShortRangeTower )
		EVT_MENU( int( MenuID::eUpgradeSpeed ), RenderPanel::OnUpgradeTowerSpeed )
		EVT_MENU( int( MenuID::eUpgradeRange ), RenderPanel::OnUpgradeTowerRange )
		EVT_MENU( int( MenuID::eUpgradeDamage ), RenderPanel::OnUpgradeTowerDamage )
	END_EVENT_TABLE()

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

	void RenderPanel::OnSetFocus( wxFocusEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::OnKillFocus( wxFocusEvent & p_event )
	{
		DoStopTimer( TimerID::eCount );
		p_event.Skip();
	}

	void RenderPanel::OnKeyDown( wxKeyEvent & p_event )
	{
		switch ( p_event.GetKeyCode() )
		{
		case WXK_LEFT:
		case 'Q':
			DoStartTimer( TimerID::eLeft );
			break;

		case WXK_RIGHT:
		case 'D':
			DoStartTimer( TimerID::eRight );
			break;

		case WXK_UP:
		case 'Z':
			DoStartTimer( TimerID::eUp );
			break;

		case WXK_DOWN:
		case 'S':
			DoStartTimer( TimerID::eDown );
			break;
		}

		p_event.Skip();
	}

	void RenderPanel::OnKeyUp( wxKeyEvent & p_event )
	{
		switch ( p_event.GetKeyCode() )
		{
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
			m_listener->PostEvent( MakeFunctorEvent( EventType::ePostRender, [this]()
			{
				if ( m_game.IsRunning() )
				{
					m_game.Help();
				}
			} ) );
			break;

		case WXK_RETURN:
		case WXK_NUMPAD_ENTER:
			m_listener->PostEvent( MakeFunctorEvent( EventType::ePostRender, [this]()
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
			m_listener->PostEvent( MakeFunctorEvent( EventType::ePostRender, [this]()
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

		case WXK_LEFT:
		case 'Q':
			DoStopTimer( TimerID::eLeft );
			break;

		case WXK_RIGHT:
		case 'D':
			DoStopTimer( TimerID::eRight );
			break;

		case WXK_UP:
		case 'Z':
			DoStopTimer( TimerID::eUp );
			break;

		case WXK_DOWN:
		case 'S':
			DoStopTimer( TimerID::eDown );
			break;
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseLDown( wxMouseEvent & p_event )
	{
		m_mouseLeftDown = true;
		auto l_window = GetRenderWindow();

		if ( l_window )
		{
			if ( m_game.IsRunning() )
			{
				m_x = DoTransformX( p_event.GetX() );
				m_y = DoTransformY( p_event.GetY() );
				m_oldX = m_x;
				m_oldY = m_y;
			}
		}
	}

	void RenderPanel::OnMouseLUp( wxMouseEvent & p_event )
	{
		m_mouseLeftDown = false;
		auto l_window = GetRenderWindow();

		if ( l_window )
		{
			if ( m_game.IsRunning() )
			{
				m_x = DoTransformX( p_event.GetX() );
				m_y = DoTransformY( p_event.GetY() );
				m_oldX = m_x;
				m_oldY = m_y;
				m_listener->PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, l_window]()
				{
					Camera & l_camera = *l_window->GetCamera();
					l_camera.Update();
					auto l_type = l_window->GetPickingPass().Pick( Position{ int( m_x ), int( m_y ) }, l_camera );

					if ( l_type != PickingPass::NodeType::eNone
						&& l_type != PickingPass::NodeType::eBillboard )
					{
						DoUpdateSelectedGeometry( l_window->GetPickingPass().GetPickedGeometry() );
					}
					else
					{
						DoUpdateSelectedGeometry( nullptr );
					}
				} ) );
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseRUp( wxMouseEvent & p_event )
	{
		auto l_window = GetRenderWindow();

		if ( l_window )
		{
			if ( m_game.IsRunning() && m_selectedTower )
			{
				wxMenu l_menu;

				if ( m_selectedTower->CanUpgradeSpeed() )
				{
					l_menu.Append( int( MenuID::eUpgradeSpeed ), wxString{ wxT( "Augmenter vitesse (" ) } << m_selectedTower->GetSpeedUpgradeCost() << wxT( ")" ) );
					l_menu.Enable( int( MenuID::eUpgradeSpeed ), m_game.CanAfford( m_selectedTower->GetSpeedUpgradeCost() ) );
				}
				else
				{
					l_menu.Append( int( MenuID::eUpgradeSpeed ), wxString{ wxT( "Augmenter vitesse (Max)" ) } );
					l_menu.Enable( int( MenuID::eUpgradeSpeed ), false );
				}

				if ( m_selectedTower->CanUpgradeRange() )
				{
					l_menu.Append( int( MenuID::eUpgradeRange ), wxString{ wxT( "Augmenter portee (" ) } << m_selectedTower->GetRangeUpgradeCost() << wxT( ")" ) );
					l_menu.Enable( int( MenuID::eUpgradeRange ), m_game.CanAfford( m_selectedTower->GetRangeUpgradeCost() ) );
				}
				else
				{
					l_menu.Append( int( MenuID::eUpgradeRange ), wxString{ wxT( "Augmenter portee (Max)" ) } );
					l_menu.Enable( int( MenuID::eUpgradeRange ), false );
				}

				if ( m_selectedTower->CanUpgradeDamage() )
				{
					l_menu.Append( int( MenuID::eUpgradeDamage ), wxString{ wxT( "Augmenter degats (" ) } << m_selectedTower->GetDamageUpgradeCost() << wxT( ")" ) );
					l_menu.Enable( int( MenuID::eUpgradeDamage ), m_game.CanAfford( m_selectedTower->GetDamageUpgradeCost() ) );
				}
				else
				{
					l_menu.Append( int( MenuID::eUpgradeDamage ), wxString{ wxT( "Augmenter degats (Max)" ) } );
					l_menu.Enable( int( MenuID::eUpgradeDamage ), false );
				}

				PopupMenu( &l_menu, p_event.GetPosition() );
			}
			else if ( !m_selectedGeometry.expired() )
			{
				wxMenu l_menu;
				l_menu.Append( int( MenuID::eNewLRTower ), wxString( "Nouvelle Tour Longue Distance (" ) << m_longRange.GetTowerCost() << wxT( ")" ) );
				l_menu.Append( int( MenuID::eNewSRTower ), wxString( "Nouvelle Tour Courte Distance (" ) << m_shortRange.GetTowerCost() << wxT( ")" ) );
				l_menu.Enable( int( MenuID::eNewLRTower ), m_game.CanAfford( m_longRange.GetTowerCost() ) );
				l_menu.Enable( int( MenuID::eNewSRTower ), m_game.CanAfford( m_shortRange.GetTowerCost() ) );
				PopupMenu( &l_menu, p_event.GetPosition() );
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseMove( wxMouseEvent & p_event )
	{
		m_x = DoTransformX( p_event.GetX() );
		m_y = DoTransformY( p_event.GetY() );
		auto l_window = GetRenderWindow();

		if ( l_window )
		{
			if ( m_game.IsRunning() )
			{
				static real constexpr l_mult = 4.0_r;
				real l_deltaX = 0.0_r;
				real l_deltaY = std::min( 1.0_r / l_mult, 1.0_r ) * ( m_oldY - m_y ) / l_mult;

				if ( m_mouseLeftDown )
				{
					m_cameraState->AddAngularVelocity( Point2r{ -l_deltaY, l_deltaX } );
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
				m_cameraState->AddScalarVelocity( Point3r{ 0.0_r, 0.0_r, -g_camSpeed } );
			}
			else
			{
				m_cameraState->AddScalarVelocity( Point3r{ 0.0_r, 0.0_r, g_camSpeed } );
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseTimer( wxTimerEvent & p_event )
	{
		if ( m_game.IsRunning() && m_cameraState )
		{
			m_cameraState->Update();
		}

		p_event.Skip();
	}

	void RenderPanel::OnTimerUp( wxTimerEvent & p_event )
	{
		m_cameraState->AddScalarVelocity( Point3r{ 0.0_r, g_camSpeed, 0.0_r } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerDown( wxTimerEvent & p_event )
	{
		m_cameraState->AddScalarVelocity( Point3r{ 0.0_r, -g_camSpeed, 0.0_r } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerLeft( wxTimerEvent & p_event )
	{
		m_cameraState->AddScalarVelocity( Point3r{ g_camSpeed, 0.0_r, 0.0_r } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerRight( wxTimerEvent & p_event )
	{
		m_cameraState->AddScalarVelocity( Point3r{ -g_camSpeed, 0.0_r, 0.0_r } );
		p_event.Skip();
	}

	void RenderPanel::OnNewLongRangeTower( wxCommandEvent & p_event )
	{
		if ( m_game.IsRunning() )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::ePostRender, [this]()
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
			m_listener->PostEvent( MakeFunctorEvent( EventType::ePostRender, [this]()
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
