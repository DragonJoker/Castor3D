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
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>
#include <Texture/TextureUnit.hpp>

using namespace castor;
using namespace castor3d;

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

	void RenderPanel::setRenderWindow( castor3d::RenderWindowSPtr p_window )
	{
		m_renderWindow.reset();
		castor::Size sizeWnd = GuiCommon::makeSize( GetClientSize() );

		if ( p_window && p_window->initialise( sizeWnd, GuiCommon::makeWindowHandle( this ) ) )
		{
			castor::Size sizeScreen;
			castor::System::getScreenSize( 0, sizeScreen );
			GetParent()->SetClientSize( sizeWnd.getWidth(), sizeWnd.getHeight() );
			sizeWnd = GuiCommon::makeSize( GetParent()->GetClientSize() );
			GetParent()->SetPosition( wxPoint( std::abs( int( sizeScreen.getWidth() ) - int( sizeWnd.getWidth() ) ) / 2, std::abs( int( sizeScreen.getHeight() ) - int( sizeWnd.getHeight() ) ) / 2 ) );
			SceneSPtr scene = p_window->getScene();

			if ( scene )
			{
				m_marker = scene->getSceneNodeCache().find( cuT( "MapMouse" ) );
				m_marker->setVisible( false );

				m_listener = p_window->getListener();
				m_renderWindow = p_window;

				if ( p_window )
				{
					auto lock = makeUniqueLock( scene->getCameraCache() );
					auto camera = scene->getCameraCache().begin()->second;
					p_window->addPickingScene( *scene );
					m_cameraState = std::make_unique< GuiCommon::NodeState >( scene->getListener(), camera->getParent(), true );
					m_timers[size_t( TimerID::eMouse )]->Start( 30 );
				}
			}
		}
		else if ( m_listener )
		{
			m_listener.reset();
		}
	}

	real RenderPanel::doTransformX( int x )
	{
		real result = real( x );
		RenderWindowSPtr window = m_renderWindow.lock();

		if ( window )
		{
			result *= real( window->getCamera()->getWidth() ) / GetClientSize().x;
		}

		return result;
	}

	real RenderPanel::doTransformY( int y )
	{
		real result = real( y );
		RenderWindowSPtr window = m_renderWindow.lock();

		if ( window )
		{
			result *= real( window->getCamera()->getHeight() ) / GetClientSize().y;
		}

		return result;
	}

	int RenderPanel::doTransformX( real x )
	{
		int result = int( x );
		RenderWindowSPtr window = m_renderWindow.lock();

		if ( window )
		{
			result = int( x * GetClientSize().x / real( window->getCamera()->getWidth() ) );
		}

		return result;
	}

	int RenderPanel::doTransformY( real y )
	{
		int result = int( y );
		RenderWindowSPtr window = m_renderWindow.lock();

		if ( window )
		{
			result = int( y * GetClientSize().y / real( window->getCamera()->getHeight() ) );
		}

		return result;
	}

	void RenderPanel::doUpdateSelectedGeometry( castor3d::GeometrySPtr p_geometry )
	{
		auto geometry = m_selectedGeometry.lock();

		if ( p_geometry != geometry )
		{
			bool freeCell = false;

			if ( !p_geometry || p_geometry->getName() == cuT( "MapBase" ) )
			{
				m_selectedGeometry.reset();
			}
			else
			{
				Cell cell{ 0u, 0u, Cell::State::Invalid };

				if ( p_geometry->getName().find( cuT( "Tower" ) ) == String::npos )
				{
					cell = m_game.getCell( p_geometry->getParent()->getPosition() );
				}
				else
				{
					cell = m_game.getCell( p_geometry->getParent()->getParent()->getPosition() );
				}

				if ( cell.m_state != Cell::State::Invalid )
				{
					switch ( cell.m_state )
					{
					case Cell::State::Empty:
						freeCell = true;
						m_listener->postEvent( makeFunctorEvent( EventType::ePostRender, [this, p_geometry]()
						{
							Point3r position = p_geometry->getParent()->getPosition();
							auto height = p_geometry->getMesh()->getBoundingBox().getMax()[1] - p_geometry->getMesh()->getBoundingBox().getMin()[1];
							m_marker->setPosition( Point3r{ position[0], height + 1, position[2] } );
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

			m_listener->postEvent( makeFunctorEvent( EventType::ePreRender, [this, freeCell]()
			{
				m_marker->setVisible( freeCell );
			} ) );
		}
	}

	void RenderPanel::doUpgradeTowerDamage()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_selectedTower->CanUpgradeDamage() && m_game.CanAfford( m_selectedTower->getDamageUpgradeCost() ) )
		{
			m_listener->postEvent( makeFunctorEvent( EventType::ePostRender, [this]()
			{
				m_game.UpgradeTowerDamage( *m_selectedTower );
			} ) );
		}
	}

	void RenderPanel::doUpgradeTowerSpeed()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_selectedTower->CanUpgradeSpeed() && m_game.CanAfford( m_selectedTower->getSpeedUpgradeCost() ) )
		{
			m_listener->postEvent( makeFunctorEvent( EventType::ePostRender, [this]()
			{
				m_game.UpgradeTowerSpeed( *m_selectedTower );
			} ) );
		}
	}

	void RenderPanel::doUpgradeTowerRange()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_selectedTower->CanUpgradeRange() && m_game.CanAfford( m_selectedTower->getRangeUpgradeCost() ) )
		{
			m_listener->postEvent( makeFunctorEvent( EventType::ePostRender, [this]()
			{
				m_game.UpgradeTowerRange( *m_selectedTower );
			} ) );
		}
	}

	void RenderPanel::doStartTimer( TimerID p_id )
	{
		m_timers[size_t( p_id )]->Start( 10 );
	}

	void RenderPanel::doStopTimer( TimerID p_id )
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
		EVT_KEY_DOWN( RenderPanel::onKeyDown )
		EVT_KEY_UP( RenderPanel::onKeyUp )
		EVT_LEFT_DOWN( RenderPanel::OnMouseLdown )
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

	void RenderPanel::OnsetFocus( wxFocusEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::OnKillFocus( wxFocusEvent & p_event )
	{
		doStopTimer( TimerID::eCount );
		p_event.Skip();
	}

	void RenderPanel::onKeyDown( wxKeyEvent & p_event )
	{
		switch ( p_event.GetKeyCode() )
		{
		case WXK_LEFT:
		case 'Q':
			doStartTimer( TimerID::eLeft );
			break;

		case WXK_RIGHT:
		case 'D':
			doStartTimer( TimerID::eRight );
			break;

		case WXK_UP:
		case 'Z':
			doStartTimer( TimerID::eUp );
			break;

		case WXK_DOWN:
		case 'S':
			doStartTimer( TimerID::eDown );
			break;
		}

		p_event.Skip();
	}

	void RenderPanel::onKeyUp( wxKeyEvent & p_event )
	{
		switch ( p_event.GetKeyCode() )
		{
		case WXK_NUMPAD1:
		case '1':
			doUpgradeTowerDamage();
			break;

		case WXK_NUMPAD2:
		case '2':
			doUpgradeTowerRange();
			break;

		case WXK_NUMPAD3:
		case '3':
			doUpgradeTowerSpeed();
			break;

		case WXK_F1:
			m_listener->postEvent( makeFunctorEvent( EventType::ePostRender, [this]()
			{
				if ( m_game.IsRunning() )
				{
					m_game.Help();
				}
			} ) );
			break;

		case WXK_RETURN:
		case WXK_NUMPAD_ENTER:
			m_listener->postEvent( makeFunctorEvent( EventType::ePostRender, [this]()
			{
				if ( m_game.isEnded() )
				{
					m_game.Reset();
					m_game.start();
				}
				else if ( !m_game.IsStarted() )
				{
					m_game.start();
				}
			} ) );
			break;

		case WXK_SPACE:
			m_listener->postEvent( makeFunctorEvent( EventType::ePostRender, [this]()
			{
				if ( m_game.IsStarted() )
				{
					if ( m_game.isPaused() )
					{
						m_game.resume();
					}
					else
					{
						m_game.pause();
					}
				}
			} ) );
			break;

		case WXK_LEFT:
		case 'Q':
			doStopTimer( TimerID::eLeft );
			break;

		case WXK_RIGHT:
		case 'D':
			doStopTimer( TimerID::eRight );
			break;

		case WXK_UP:
		case 'Z':
			doStopTimer( TimerID::eUp );
			break;

		case WXK_DOWN:
		case 'S':
			doStopTimer( TimerID::eDown );
			break;
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseLdown( wxMouseEvent & p_event )
	{
		m_mouseLeftDown = true;
		auto window = getRenderWindow();

		if ( window )
		{
			if ( m_game.IsRunning() )
			{
				m_x = doTransformX( p_event.GetX() );
				m_y = doTransformY( p_event.GetY() );
				m_oldX = m_x;
				m_oldY = m_y;
			}
		}
	}

	void RenderPanel::OnMouseLUp( wxMouseEvent & p_event )
	{
		m_mouseLeftDown = false;
		auto window = getRenderWindow();

		if ( window )
		{
			if ( m_game.IsRunning() )
			{
				m_x = doTransformX( p_event.GetX() );
				m_y = doTransformY( p_event.GetY() );
				m_oldX = m_x;
				m_oldY = m_y;
				m_listener->postEvent( makeFunctorEvent( EventType::ePreRender, [this, window]()
				{
					Camera & camera = *window->getCamera();
					camera.update();
					auto type = window->pick( Position{ int( m_x ), int( m_y ) } );

					if ( type != PickNodeType::eNone
						&& type != PickNodeType::eBillboard )
					{
						doUpdateSelectedGeometry( window->getPickedGeometry() );
					}
					else
					{
						doUpdateSelectedGeometry( nullptr );
					}
				} ) );
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseRUp( wxMouseEvent & p_event )
	{
		auto window = getRenderWindow();

		if ( window )
		{
			if ( m_game.IsRunning() && m_selectedTower )
			{
				wxMenu menu;

				if ( m_selectedTower->CanUpgradeSpeed() )
				{
					menu.Append( int( MenuID::eUpgradeSpeed ), wxString{ wxT( "Augmenter vitesse (" ) } << m_selectedTower->getSpeedUpgradeCost() << wxT( ")" ) );
					menu.Enable( int( MenuID::eUpgradeSpeed ), m_game.CanAfford( m_selectedTower->getSpeedUpgradeCost() ) );
				}
				else
				{
					menu.Append( int( MenuID::eUpgradeSpeed ), wxString{ wxT( "Augmenter vitesse (Max)" ) } );
					menu.Enable( int( MenuID::eUpgradeSpeed ), false );
				}

				if ( m_selectedTower->CanUpgradeRange() )
				{
					menu.Append( int( MenuID::eUpgradeRange ), wxString{ wxT( "Augmenter portee (" ) } << m_selectedTower->getRangeUpgradeCost() << wxT( ")" ) );
					menu.Enable( int( MenuID::eUpgradeRange ), m_game.CanAfford( m_selectedTower->getRangeUpgradeCost() ) );
				}
				else
				{
					menu.Append( int( MenuID::eUpgradeRange ), wxString{ wxT( "Augmenter portee (Max)" ) } );
					menu.Enable( int( MenuID::eUpgradeRange ), false );
				}

				if ( m_selectedTower->CanUpgradeDamage() )
				{
					menu.Append( int( MenuID::eUpgradeDamage ), wxString{ wxT( "Augmenter degats (" ) } << m_selectedTower->getDamageUpgradeCost() << wxT( ")" ) );
					menu.Enable( int( MenuID::eUpgradeDamage ), m_game.CanAfford( m_selectedTower->getDamageUpgradeCost() ) );
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
				menu.Append( int( MenuID::eNewLRTower ), wxString( "Nouvelle Tour Longue Distance (" ) << m_longRange.getTowerCost() << wxT( ")" ) );
				menu.Append( int( MenuID::eNewSRTower ), wxString( "Nouvelle Tour Courte Distance (" ) << m_shortRange.getTowerCost() << wxT( ")" ) );
				menu.Enable( int( MenuID::eNewLRTower ), m_game.CanAfford( m_longRange.getTowerCost() ) );
				menu.Enable( int( MenuID::eNewSRTower ), m_game.CanAfford( m_shortRange.getTowerCost() ) );
				PopupMenu( &menu, p_event.GetPosition() );
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseMove( wxMouseEvent & p_event )
	{
		m_x = doTransformX( p_event.GetX() );
		m_y = doTransformY( p_event.GetY() );
		auto window = getRenderWindow();

		if ( window )
		{
			if ( m_game.IsRunning() )
			{
				static real constexpr mult = 4.0_r;
				real deltaX = 0.0_r;
				real deltaY = std::min( 1.0_r / mult, 1.0_r ) * ( m_oldY - m_y ) / mult;

				if ( m_mouseLeftDown )
				{
					m_cameraState->addAngularVelocity( Point2r{ -deltaY, deltaX } );
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
				m_cameraState->addScalarVelocity( Point3r{ 0.0_r, 0.0_r, -g_camSpeed } );
			}
			else
			{
				m_cameraState->addScalarVelocity( Point3r{ 0.0_r, 0.0_r, g_camSpeed } );
			}
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseTimer( wxTimerEvent & p_event )
	{
		if ( m_game.IsRunning() && m_cameraState )
		{
			m_cameraState->update();
		}

		p_event.Skip();
	}

	void RenderPanel::OnTimerUp( wxTimerEvent & p_event )
	{
		m_cameraState->addScalarVelocity( Point3r{ 0.0_r, g_camSpeed, 0.0_r } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerDown( wxTimerEvent & p_event )
	{
		m_cameraState->addScalarVelocity( Point3r{ 0.0_r, -g_camSpeed, 0.0_r } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerLeft( wxTimerEvent & p_event )
	{
		m_cameraState->addScalarVelocity( Point3r{ g_camSpeed, 0.0_r, 0.0_r } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerRight( wxTimerEvent & p_event )
	{
		m_cameraState->addScalarVelocity( Point3r{ -g_camSpeed, 0.0_r, 0.0_r } );
		p_event.Skip();
	}

	void RenderPanel::OnNewLongRangeTower( wxCommandEvent & p_event )
	{
		if ( m_game.IsRunning() )
		{
			m_listener->postEvent( makeFunctorEvent( EventType::ePostRender, [this]()
			{
				if ( m_game.BuildTower( m_marker->getPosition(), std::make_unique< LongRangeTower >( m_longRange ) ) )
				{
					doUpdateSelectedGeometry( nullptr );
				}
			} ) );
		}
	}

	void RenderPanel::OnNewShortRangeTower( wxCommandEvent & p_event )
	{
		if ( m_game.IsRunning() )
		{
			m_listener->postEvent( makeFunctorEvent( EventType::ePostRender, [this]()
			{
				if ( m_game.BuildTower( m_marker->getPosition(), std::make_unique< ShortRangeTower >( m_shortRange ) ) )
				{
					doUpdateSelectedGeometry( nullptr );
				}
			} ) );
		}
	}

	void RenderPanel::OnUpgradeTowerSpeed( wxCommandEvent & p_event )
	{
		doUpgradeTowerSpeed();
	}

	void RenderPanel::OnUpgradeTowerRange( wxCommandEvent & p_event )
	{
		doUpgradeTowerRange();
	}

	void RenderPanel::OnUpgradeTowerDamage( wxCommandEvent & p_event )
	{
		doUpgradeTowerDamage();
	}
}
