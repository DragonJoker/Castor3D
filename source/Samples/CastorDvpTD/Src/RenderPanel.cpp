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
		typedef enum eMENU_ID
		{
			eMENU_ID_NEW_LR_TOWER = 1,
			eMENU_ID_NEW_SR_TOWER,
			eMENU_ID_UPGRADE_SPEED,
			eMENU_ID_UPGRADE_RANGE,
			eMENU_ID_UPGRADE_DAMAGE,
		}	eMENU_ID;
	}

	RenderPanel::RenderPanel( wxWindow * p_parent, wxSize const & p_size, Game & p_game )
		: wxPanel{ p_parent, wxID_ANY, wxDefaultPosition, p_size }
		, m_game{ p_game }
		, m_picking{ *wxGetApp().GetCastor() }
	{
	}

	RenderPanel::~RenderPanel()
	{
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
		else if ( m_listener )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::PreRender, [this]()
			{
				m_picking.Cleanup();
			} ) );
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
			if ( !p_geometry
				 || p_geometry->GetName() != cuT( "MapBase" ) )
			{
				m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this, l_geometry]()
				{
					m_marker->SetVisible( false );
				} ) );
			}
			else
			{
				Cell l_cell;

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
		if ( m_game.IsRunning() && m_selectedTower && m_selectedTower->CanUpgradeDamage() && m_game.CanAfford( m_selectedTower->GetDamageUpgradeCost() ) )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this]()
			{
				m_game.UpgradeTowerDamage( *m_selectedTower );
			} ) );
		}
	}

	void RenderPanel::DoUpgradeTowerSpeed()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_selectedTower->CanUpgradeSpeed() && m_game.CanAfford( m_selectedTower->GetSpeedUpgradeCost() ) )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this]()
			{
				m_game.UpgradeTowerSpeed( *m_selectedTower );
			} ) );
		}
	}

	void RenderPanel::DoUpgradeTowerRange()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_selectedTower->CanUpgradeRange() && m_game.CanAfford( m_selectedTower->GetRangeUpgradeCost() ) )
		{
			m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this]()
			{
				m_game.UpgradeTowerRange( *m_selectedTower );
			} ) );
		}
	}

	BEGIN_EVENT_TABLE( RenderPanel, wxPanel )
		EVT_SIZE( RenderPanel::OnSize )
		EVT_MOVE( RenderPanel::OnMove )
		EVT_PAINT( RenderPanel::OnPaint )
		EVT_KEY_UP( RenderPanel::OnKeyUp )
		EVT_LEFT_UP( RenderPanel::OnMouseLUp )
		EVT_RIGHT_UP( RenderPanel::OnMouseRUp )
		EVT_MENU( eMENU_ID_NEW_LR_TOWER, RenderPanel::OnNewLongRangeTower )
		EVT_MENU( eMENU_ID_NEW_SR_TOWER, RenderPanel::OnNewShortRangeTower )
		EVT_MENU( eMENU_ID_UPGRADE_SPEED, RenderPanel::OnUpgradeTowerSpeed )
		EVT_MENU( eMENU_ID_UPGRADE_RANGE, RenderPanel::OnUpgradeTowerRange )
		EVT_MENU( eMENU_ID_UPGRADE_DAMAGE, RenderPanel::OnUpgradeTowerDamage )
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
			m_listener->PostEvent( MakeFunctorEvent( EventType::PostRender, [this]()
			{
				if ( m_game.IsRunning() )
				{
					m_game.Help();
				}
			} ) );
			break;

		case WXK_RETURN:
		case WXK_NUMPAD_ENTER:
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

		p_event.Skip();
	}

	void RenderPanel::OnMouseLUp( wxMouseEvent & p_event )
	{
		auto l_window = GetRenderWindow();

		if ( l_window )
		{
			if ( m_game.IsRunning() )
			{
				auto l_x = DoTransformX( p_event.GetX() );
				auto l_y = DoTransformY( p_event.GetY() );
				m_listener->PostEvent( MakeFunctorEvent( EventType::PreRender, [this, l_window, l_x, l_y]()
				{
					Camera & l_camera = *l_window->GetCamera();
					l_camera.Update();

					if ( m_picking.Pick( Position{ int( l_x ), int( l_y ) }, l_camera ) )
					{
						DoUpdateSelectedGeometry( m_picking.GetPickedGeometry() );
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
