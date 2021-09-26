#include "RenderPanel.hpp"

#include "CastorDvpTD.hpp"
#include "Game.hpp"

#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Event/Frame/CpuFunctorEvent.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Event/Frame/GpuFunctorEvent.hpp>
#include <Castor3D/Event/UserInput/UserInputListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Render/Ray.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

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

		static float const g_camSpeed = 10.0;
	}

	RenderPanel::RenderPanel( wxWindow * parent, wxSize const & size, Game & game )
		: wxPanel{ parent, wxID_ANY, wxDefaultPosition, size }
		, m_timers
		{
			new wxTimer( this, int( TimerID::eUp ) ),
			new wxTimer( this, int( TimerID::eDown ) ),
			new wxTimer( this, int( TimerID::eLeft ) ),
			new wxTimer( this, int( TimerID::eRight ) ),
			new wxTimer( this, int( TimerID::eMouse ) ),
		}
		, m_game{ game }
	{
		auto & engine = *wxGetApp().getCastor();
		castor::Size sizeWnd = GuiCommon::makeSize( GetClientSize() );
		m_renderWindow = std::make_unique< castor3d::RenderWindow >( "CastorTD"
			, engine
			, sizeWnd
			, GuiCommon::makeWindowHandle( this ) );
	}

	RenderPanel::~RenderPanel()
	{
		for ( auto & timer : m_timers )
		{
			delete timer;
		}
	}

	void RenderPanel::reset()
	{
		m_renderWindow->cleanup();
	}

	void RenderPanel::setRenderTarget( castor3d::RenderTargetSPtr target )
	{
		m_renderWindow->initialise( target );

		if ( target )
		{
			castor::Size sizeWnd = GuiCommon::makeSize( GetClientSize() );
			castor::Size sizeScreen;
			castor::System::getScreenSize( 0, sizeScreen );
			GetParent()->SetClientSize( int( sizeWnd.getWidth() ), int( sizeWnd.getHeight() ) );
			sizeWnd = GuiCommon::makeSize( GetParent()->GetClientSize() );
			GetParent()->SetPosition( wxPoint( std::abs( int( sizeScreen.getWidth() ) - int( sizeWnd.getWidth() ) ) / 2
				, std::abs( int( sizeScreen.getHeight() ) - int( sizeWnd.getHeight() ) ) / 2 ) );
			SceneSPtr scene = target->getScene();

			if ( scene )
			{
				m_marker = scene->getSceneNodeCache().find( cuT( "MapMouse" ) ).lock();
				m_marker->setVisible( false );

				m_listener = m_renderWindow->getListener();

				using LockType = std::unique_lock< CameraCache >;
				LockType lock{ castor::makeUniqueLock( scene->getCameraCache() ) };
				auto camera = scene->getCameraCache().begin()->second;
				m_renderWindow->addPickingScene( *scene );
				m_cameraState = std::make_unique< GuiCommon::NodeState >( scene->getListener(), camera->getParent(), true );
				m_timers[size_t( TimerID::eMouse )]->Start( 30 );
			}
		}
		else if ( m_listener )
		{
			m_listener.reset();
		}
	}

	float RenderPanel::doTransformX( int x )
	{
		float result = float( x );
		result *= float( m_renderWindow->getCamera()->getWidth() ) / float( GetClientSize().x );
		return result;
	}

	float RenderPanel::doTransformY( int y )
	{
		float result = float( y );
		result *= float( m_renderWindow->getCamera()->getHeight() ) / float( GetClientSize().y );
		return result;
	}

	int RenderPanel::doTransformX( float x )
	{
		int result = int( x );
		result = int( x * float( GetClientSize().x ) / float( m_renderWindow->getCamera()->getWidth() ) );
		return result;
	}

	int RenderPanel::doTransformY( float y )
	{
		int result = int( y );
		result = int( y * float( GetClientSize().y ) / float( m_renderWindow->getCamera()->getHeight() ) );
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
						m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
							, [this, p_geometry]()
							{
								castor::Point3f position = p_geometry->getParent()->getPosition();
								auto height = p_geometry->getMesh().lock()->getBoundingBox().getMax()[1] - p_geometry->getMesh().lock()->getBoundingBox().getMin()[1];
								m_marker->setPosition( castor::Point3f{ position[0], height + 1, position[2] } );
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

			m_listener->postEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [this, freeCell]( RenderDevice const & device
					, QueueData const & queueData )
				{
					m_marker->setVisible( freeCell );
				} ) );
		}
	}

	void RenderPanel::doUpgradeTowerDamage()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_selectedTower->CanUpgradeDamage() && m_game.CanAfford( m_selectedTower->getDamageUpgradeCost() ) )
		{
			m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this]()
				{
					m_game.UpgradeTowerDamage( *m_selectedTower );
				} ) );
		}
	}

	void RenderPanel::doUpgradeTowerSpeed()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_selectedTower->CanUpgradeSpeed() && m_game.CanAfford( m_selectedTower->getSpeedUpgradeCost() ) )
		{
			m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this]()
				{
					m_game.UpgradeTowerSpeed( *m_selectedTower );
				} ) );
		}
	}

	void RenderPanel::doUpgradeTowerRange()
	{
		if ( m_game.IsRunning() && m_selectedTower && m_selectedTower->CanUpgradeRange() && m_game.CanAfford( m_selectedTower->getRangeUpgradeCost() ) )
		{
			m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this]()
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
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
#pragma GCC diagnostic pop

	void RenderPanel::OnSize( wxSizeEvent & p_event )
	{
		m_renderWindow->resize( uint32_t( p_event.GetSize().x )
			, uint32_t( p_event.GetSize().y ) );
		p_event.Skip();
	}

	void RenderPanel::OnMove( wxMoveEvent & p_event )
	{
		p_event.Skip();
	}

	void RenderPanel::OnPaint( wxPaintEvent & p_event )
	{
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
			m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this]()
				{
					if ( m_game.IsRunning() )
					{
						m_game.Help();
					}
				} ) );
			break;

		case WXK_RETURN:
		case WXK_NUMPAD_ENTER:
			m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this]()
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
			m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this]()
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

		if ( m_game.IsRunning() )
		{
			m_x = doTransformX( p_event.GetX() );
			m_y = doTransformY( p_event.GetY() );
			m_oldX = m_x;
			m_oldY = m_y;
		}
	}

	void RenderPanel::OnMouseLUp( wxMouseEvent & p_event )
	{
		m_mouseLeftDown = false;

		if ( m_game.IsRunning() )
		{
			m_x = doTransformX( p_event.GetX() );
			m_y = doTransformY( p_event.GetY() );
			m_oldX = m_x;
			m_oldY = m_y;
			m_listener->postEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [this]( RenderDevice const & device
					, QueueData const & queueData )
				{
					Camera & camera = *m_renderWindow->getCamera();
					camera.update();
					auto type = m_renderWindow->pick( Position{ int( m_x ), int( m_y ) } );

					if ( type != PickNodeType::eNone
						&& type != PickNodeType::eBillboard )
					{
						doUpdateSelectedGeometry( m_renderWindow->getPickedGeometry() );
					}
					else
					{
						doUpdateSelectedGeometry( nullptr );
					}
				} ) );
		}

		p_event.Skip();
	}

	void RenderPanel::OnMouseRUp( wxMouseEvent & p_event )
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

		p_event.Skip();
	}

	void RenderPanel::OnMouseMove( wxMouseEvent & p_event )
	{
		m_x = doTransformX( p_event.GetX() );
		m_y = doTransformY( p_event.GetY() );

		if ( m_game.IsRunning() )
		{
			static float constexpr mult = 4.0f;
			float deltaX = 0.0f;
			float deltaY = std::min( 1.0f / mult, 1.0f ) * ( m_oldY - m_y ) / mult;

			if ( m_mouseLeftDown )
			{
				m_cameraState->addAngularVelocity( castor::Point2f{ -deltaY, deltaX } );
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
				m_cameraState->addScalarVelocity( castor::Point3f{ 0.0f, 0.0f, -g_camSpeed } );
			}
			else
			{
				m_cameraState->addScalarVelocity( castor::Point3f{ 0.0f, 0.0f, g_camSpeed } );
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
		m_cameraState->addScalarVelocity( castor::Point3f{ 0.0f, g_camSpeed, 0.0f } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerDown( wxTimerEvent & p_event )
	{
		m_cameraState->addScalarVelocity( castor::Point3f{ 0.0f, -g_camSpeed, 0.0f } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerLeft( wxTimerEvent & p_event )
	{
		m_cameraState->addScalarVelocity( castor::Point3f{ g_camSpeed, 0.0f, 0.0f } );
		p_event.Skip();
	}

	void RenderPanel::OnTimerRight( wxTimerEvent & p_event )
	{
		m_cameraState->addScalarVelocity( castor::Point3f{ -g_camSpeed, 0.0f, 0.0f } );
		p_event.Skip();
	}

	void RenderPanel::OnNewLongRangeTower( wxCommandEvent & p_event )
	{
		if ( m_game.IsRunning() )
		{
			m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this]()
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
			m_listener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this]()
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
