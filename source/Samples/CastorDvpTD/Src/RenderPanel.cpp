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
		Castor::Size sizeWnd = GuiCommon::make_Size( GetClientSize() );

		if ( p_window && p_window->Initialise( sizeWnd, GuiCommon::make_WindowHandle( this ) ) )
		{
			Castor::Size sizeScreen;
			Castor::System::GetScreenSize( 0, sizeScreen );
			GetParent()->SetClientSize( sizeWnd.width(), sizeWnd.height() );
			sizeWnd = GuiCommon::make_Size( GetParent()->GetClientSize() );
			GetParent()->SetPosition( wxPoint( std::abs( int( sizeScreen.width() ) - int( sizeWnd.width() ) ) / 2, std::abs( int( sizeScreen.height() ) - int( sizeWnd.height() ) ) / 2 ) );
			SceneSPtr scene = p_window->GetScene();

			if ( scene )
			{
				m_marker = scene->GetSceneNodeCache().Find( cuT( "MapMouse" ) );
				m_marker->SetVisible( false );

				m_listener = p_window->GetListener();
				m_renderWindow = p_window;

				if ( p_window )
				{
					auto lock = make_unique_lock( scene->GetCameraCache() );
					auto camera = scene->GetCameraCache().begin()->second;
					p_window->GetPickingPass().AddScene( *scene, *camera );
					m_cameraState = std::make_unique< GuiCommon::NodeState >( scene->GetListener(), camera->GetParent() );
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
		real result = real( x );
		RenderWindowSPtr window = m_renderWindow.lock();

		if ( window )
		{
			result *= real( window->GetCamera()->GetWidth() ) / GetClientSize().x;
		}

		return result;
	}

	real RenderPanel::DoTransformY( int y )
	{
		real result = real( y );
		RenderWindowSPtr window = m_renderWindow.lock();

		if ( window )
		{
			result *= real( window->GetCamera()->GetHeight() ) / GetClientSize().y;
		}

		return result;
	}

	int RenderPanel::DoTransformX( real x )
	{
		int result = int( x );
		RenderWindowSPtr window = m_renderWindow.lock();

		if ( window )
		{
			result = int( x * GetClientSize().x / real( window->GetCamera()->GetWidth() ) );
		}

		return result;
	}

	int RenderPanel::DoTransformY( real y )
	{
		int result = int( y );
		RenderWindowSPtr window = m_renderWindow.lock();

		if ( window )
		{
			result = int( y * GetClientSize().y / real( window->GetCamera()->GetHeight() ) );
		}

		return result;
	}

	void RenderPanel::DoUpdateSelectedGeometry( Castor3D::GeometrySPtr p_geometry )
	{
		auto geometry = m_selectedGeometry.lock();

		if ( p_geometry != geometry )
		{
			bool freeCell = false;

			if ( !p_geometry || p_geometry->GetName() == cuT( "MapBase" ) )
			{
				m_selectedGeometry.reset();
			}
			else
			{
				Cell cell{ 0u, 0u, Cell::State::Invalid };

				if ( p_geometry->GetName().find( cuT( "Tower" ) ) == String::npos )
				{
					cell = m_game.GetCell( p_geometry->GetParent()->GetPosition() );
				}
				else
				{
					cell = m_game.GetCell( p_geometry->GetParent()->GetParent()->GetPosition() );
				}

				if ( cell.m_state != Cell::State::Invalid )
				{
					switch ( cell.m_state )
					{
					case Cell::State::Empty:
						freeCell = true;
						m_listener->PostEvent( MakeFunctorEvent( EventType::ePostRender, [this, p_geometry]()
						{
							Point3r position = p_geometry->GetParent()->GetPosition();
							auto height = p_geometry->GetMesh()->GetCollisionBox().GetMax()[1] - p_geometry->GetMesh()->GetCollisionBox().GetMin()[1];
							m_marker->SetPosition( Point3r{ position[0], height + 1, position[2] } );
						} ) );
						m_selectedTower = nullptr;
						break;

					case Cell::State::Tower:
						m_selectedTower = m_game.SelectTower( cell );
						break;

					case Cell::State::Target:
					case Cell::State::Path:
					default:
						m_selectedTower = nullptr;
						break;
					}

					m_selectedGeometry = p_geometry;
				}
			}

			m_listener->PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, freeCell]()
			{
				m_marker->SetVisible( freeCell );
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
		RenderWindowSPtr window = m_renderWindow.lock();

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
		RenderWindowSPtr window = m_renderWindow.lock();

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
		RenderWindowSPtr window = m_renderWindow.lock();

		if ( !window )
		{
			wxPaintDC dc( this );
			dc.SetBrush( wxBrush( GuiCommon::INACTIVE_TAB_COLOUR ) );
			dc.SetPen( wxPen( GuiCommon::INACTIVE_TAB_COLOUR ) );
			dc.DrawRectangle( 0, 0, GetClientSize().x, GetClientSize().y );
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
		auto window = GetRenderWindow();

		if ( window )
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
		auto window = GetRenderWindow();

		if ( window )
		{
			if ( m_game.IsRunning() )
			{
				m_x = DoTransformX( p_event.GetX() );
				m_y = DoTransformY( p_event.GetY() );
				m_oldX = m_x;
				m_oldY = m_y;
				m_listener->PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, window]()
				{
					Camera & camera = *window->GetCamera();
					camera.Update();
					auto type = window->GetPickingPass().Pick( Position{ int( m_x ), int( m_y ) }, camera );

					if ( type != PickingPass::NodeType::eNone
						&& type != PickingPass::NodeType::eBillboard )
					{
						DoUpdateSelectedGeometry( window->GetPickingPass().GetPickedGeometry() );
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
		auto window = GetRenderWindow();

		if ( window )
		{
			if ( m_game.IsRunning() && m_selectedTower )
			{
				wxMenu menu;

				if ( m_selectedTower->CanUpgradeSpeed() )
				{
					menu.Append( int( MenuID::eUpgradeSpeed ), wxString{ wxT( "Augmenter vitesse (" ) } << m_selectedTower->GetSpeedUpgradeCost() << wxT( ")" ) );
					menu.Enable( int( MenuID::eUpgradeSpeed ), m_game.CanAfford( m_selectedTower->GetSpeedUpgradeCost() ) );
				}
				else
				{
					menu.Append( int( MenuID::eUpgradeSpeed ), wxString{ wxT( "Augmenter vitesse (Max)" ) } );
					menu.Enable( int( MenuID::eUpgradeSpeed ), false );
				}

				if ( m_selectedTower->CanUpgradeRange() )
				{
					menu.Append( int( MenuID::eUpgradeRange ), wxString{ wxT( "Augmenter portee (" ) } << m_selectedTower->GetRangeUpgradeCost() << wxT( ")" ) );
					menu.Enable( int( MenuID::eUpgradeRange ), m_game.CanAfford( m_selectedTower->GetRangeUpgradeCost() ) );
				}
				else
				{
					menu.Append( int( MenuID::eUpgradeRange ), wxString{ wxT( "Augmenter portee (Max)" ) } );
					menu.Enable( int( MenuID::eUpgradeRange ), false );
				}

				if ( m_selectedTower->CanUpgradeDamage() )
				{
					menu.Append( int( MenuID::eUpgradeDamage ), wxString{ wxT( "Augmenter degats (" ) } << m_selectedTower->GetDamageUpgradeCost() << wxT( ")" ) );
					menu.Enable( int( MenuID::eUpgradeDamage ), m_game.CanAfford( m_selectedTower->GetDamageUpgradeCost() ) );
				}
				else
				{
					menu.Append( int( MenuID::eUpgradeDamage ), wxString{ wxT( "Augmenter degats (Max)" ) } );
					menu.Enable( int( MenuID::eUpgradeDamage ), false );
				}

				PopupMenu( &menu, p_event.GetPosition() );
			}
			else if ( !m_selectedGeometry.expired() )
			{
				wxMenu menu;
				menu.Append( int( MenuID::eNewLRTower ), wxString( "Nouvelle Tour Longue Distance (" ) << m_longRange.GetTowerCost() << wxT( ")" ) );
				menu.Append( int( MenuID::eNewSRTower ), wxString( "Nouvelle Tour Courte Distance (" ) << m_shortRange.GetTowerCost() << wxT( ")" ) );
				menu.Enable( int( MenuID::eNewLRTower ), m_game.CanAfford( m_longRange.GetTowerCost() ) );
				menu.Enable( int( MenuID::eNewSRTower ), m_game.CanAfford( m_shortRange.GetTowerCost() ) );
				PopupMenu( &menu, p_event.GetPosition() );
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseMove( wxMouseEvent & p_event )
	{
		m_x = DoTransformX( p_event.GetX() );
		m_y = DoTransformY( p_event.GetY() );
		auto window = GetRenderWindow();

		if ( window )
		{
			if ( m_game.IsRunning() )
			{
				static real constexpr mult = 4.0_r;
				real deltaX = 0.0_r;
				real deltaY = std::min( 1.0_r / mult, 1.0_r ) * ( m_oldY - m_y ) / mult;

				if ( m_mouseLeftDown )
				{
					m_cameraState->AddAngularVelocity( Point2r{ -deltaY, deltaX } );
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
