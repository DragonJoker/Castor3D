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
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

namespace castortd
{
	namespace panel
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
			c3d::makeRawUnique< wxTimer >( this, int( TimerID::eUp ) ),
			c3d::makeRawUnique< wxTimer >( this, int( TimerID::eDown ) ),
			c3d::makeRawUnique< wxTimer >( this, int( TimerID::eLeft ) ),
			c3d::makeRawUnique< wxTimer >( this, int( TimerID::eRight ) ),
		}
		, m_game{ game }
	{
		auto & engine = *wxGetApp().getCastor();
		c3d::Size sizeWnd = GuiCommon::makeSize( wxPanel::GetClientSize() );
		m_renderWindow = c3d::makeUnique< c3d::RenderWindow >( cuT( "CastorTD" )
			, engine
			, sizeWnd
			, GuiCommon::makeWindowHandle( this ) );
	}

	RenderPanel::~RenderPanel()
	{
		for ( auto & timer : m_timers )
		{
			timer = {};
		}
	}

	void RenderPanel::reset()
	{
		m_renderWindow->cleanup();
	}

	void RenderPanel::updateRenderWindow( c3d::RenderWindowDesc const & window )
	{
		if ( auto target = window.renderTarget )
		{
			m_renderWindow->initialise( window );

			auto sizeWnd = FromDIP( GetClientSize() );
			c3d::Size sizeScreen;
			c3d::system::getScreenSize( 0, sizeScreen );
			GetParent()->SetClientSize( sizeWnd );
			GetParent()->SetPosition( FromDIP( wxPoint( std::max( 0, int( sizeScreen.getWidth() ) - sizeWnd.GetWidth() ) / 2
				, std::max( 0, int( sizeScreen.getHeight() ) - sizeWnd.GetHeight() ) / 2 ) ) );
			c3d::SceneRPtr scene = target->getScene();

			if ( scene )
			{
				m_marker = scene->findSceneNode( cuT( "MapMouse" ) );
				m_marker->setVisible( false );

				m_listener = m_renderWindow->getListener();

				using LockType = c3d::UniqueLock< c3d::CameraCache >;
				LockType lock{ c3d::makeUniqueLock( scene->getCameraCache() ) };
				auto camera = scene->getCameraCache().begin()->second.get();
				m_cameraState = c3d::makeRawUnique< GuiCommon::NodeState >( scene->getListener(), camera->getParent(), true );
				m_cameraState->start();
			}
		}
		else
		{
			if ( m_cameraState )
			{
				m_cameraState->stop();
			}

			if ( m_listener )
			{
				m_listener = {};
			}
		}
	}

	float RenderPanel::doTransformX( int x )const
	{
		auto result = float( x );
		result *= float( m_renderWindow->getRenderTarget()->getDisplaySize().getWidth() ) / float( GetClientSize().x );
		return result;
	}

	float RenderPanel::doTransformY( int y )const
	{
		auto result = float( y );
		result *= float( m_renderWindow->getRenderTarget()->getDisplaySize().getHeight() ) / float( GetClientSize().y );
		return result;
	}

	int RenderPanel::doTransformX( float x )const
	{
		auto result = int( x );
		result = int( x * float( GetClientSize().x ) / float( m_renderWindow->getRenderTarget()->getDisplaySize().getWidth() ) );
		return result;
	}

	int RenderPanel::doTransformY( float y )const
	{
		auto result = int( y );
		result = int( y * float( GetClientSize().y ) / float( m_renderWindow->getRenderTarget()->getDisplaySize().getHeight() ) );
		return result;
	}

	void RenderPanel::doUpdateSelectedGeometry( c3d::GeometryRPtr geometry )
	{
		auto curGeometry = m_selectedGeometry;

		if ( geometry != curGeometry )
		{
			bool freeCell = false;

			if ( !geometry || geometry->getName() == cuT( "MapBase" ) )
			{
				m_selectedGeometry = {};
			}
			else
			{
				Cell cell{ 0u, 0u, Cell::State::Invalid };

				if ( geometry->getName().find( cuT( "Tower" ) ) == c3d::String::npos )
				{
					cell = m_game.getCell( geometry->getParent()->getPosition() );
				}
				else
				{
					cell = m_game.getCell( geometry->getParent()->getParent()->getPosition() );
				}

				if ( cell.m_state != Cell::State::Invalid )
				{
					switch ( cell.m_state )
					{
					case Cell::State::Empty:
						freeCell = true;
						m_listener->postEvent( c3d::makeCpuFunctorEvent( c3d::CpuEventType::ePostCpuStep
							, [this, geometry]()
							{
								if ( geometry && m_marker )
								{
									if ( auto node = geometry->getParent() )
									{
										c3d::Point3f position = node->getPosition();

										if ( auto mesh = geometry->getMesh() )
										{
											auto height = mesh->getBoundingBox().getMax()[1] - mesh->getBoundingBox().getMin()[1];
											m_marker->setPosition( c3d::Point3f{ position[0], height + 1, position[2] } );
										}
									}
								}
							} ) );
						m_selectedTower = nullptr;
						break;

					case Cell::State::Tower:
						m_selectedTower = m_game.selectTower( cell );
						break;

					default:
						m_selectedTower = nullptr;
						break;
					}

					m_selectedGeometry = geometry;
				}
			}

			m_listener->postEvent( c3d::makeGpuFunctorEvent( c3d::GpuEventType::ePreUpload
				, [this, freeCell]( c3d::RenderDevice const &
					, c3d::QueueData const & )
				{
					if ( m_marker )
						m_marker->setVisible( freeCell );
				} ) );
		}
	}

	void RenderPanel::doUpgradeTowerDamage()
	{
		if ( m_game.isRunning() && m_selectedTower && m_selectedTower->canUpgradeDamage() && m_game.canAfford( m_selectedTower->getDamageUpgradeCost() ) )
		{
			m_listener->postEvent( c3d::makeCpuFunctorEvent( c3d::CpuEventType::ePostCpuStep
				, [this]()
				{
					m_game.upgradeTowerDamage( *m_selectedTower );
				} ) );
		}
	}

	void RenderPanel::doUpgradeTowerSpeed()
	{
		if ( m_game.isRunning() && m_selectedTower && m_selectedTower->canUpgradeSpeed() && m_game.canAfford( m_selectedTower->getSpeedUpgradeCost() ) )
		{
			m_listener->postEvent( c3d::makeCpuFunctorEvent( c3d::CpuEventType::ePostCpuStep
				, [this]()
				{
					m_game.upgradeTowerSpeed( *m_selectedTower );
				} ) );
		}
	}

	void RenderPanel::doUpgradeTowerRange()
	{
		if ( m_game.isRunning() && m_selectedTower && m_selectedTower->canUpgradeRange() && m_game.canAfford( m_selectedTower->getRangeUpgradeCost() ) )
		{
			m_listener->postEvent( c3d::makeCpuFunctorEvent( c3d::CpuEventType::ePostCpuStep
				, [this]()
				{
					m_game.upgradeTowerRange( *m_selectedTower );
				} ) );
		}
	}

	void RenderPanel::doStartTimer( TimerID id )
	{
		m_timers[size_t( id )]->Start( 10 );
	}

	void RenderPanel::doStopTimer( TimerID id )
	{
		if ( id != TimerID::eCount )
		{
			m_timers[size_t( id )]->Stop();
		}
		else
		{
			for ( size_t i = 0; i < size_t( TimerID::eCount ); i++ )
			{
				m_timers[i]->Stop();
			}
		}
	}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wambiguous-reversed-operator"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( RenderPanel, wxPanel )
		EVT_SIZE( RenderPanel::onSize )
		EVT_MOVE( RenderPanel::onMove )
		EVT_PAINT( RenderPanel::onPaint )
		EVT_KEY_DOWN( RenderPanel::onKeyDown )
		EVT_KEY_UP( RenderPanel::onKeyUp )
		EVT_LEFT_DOWN( RenderPanel::onMouseLDown )
		EVT_LEFT_UP( RenderPanel::onMouseLUp )
		EVT_RIGHT_UP( RenderPanel::onMouseRUp )
		EVT_MOTION( RenderPanel::onMouseMove )
		EVT_MOUSEWHEEL( RenderPanel::onMouseWheel )
		EVT_TIMER( int( TimerID::eUp ), RenderPanel::onTimerUp )
		EVT_TIMER( int( TimerID::eDown ), RenderPanel::onTimerDown )
		EVT_TIMER( int( TimerID::eLeft ), RenderPanel::onTimerLeft )
		EVT_TIMER( int( TimerID::eRight ), RenderPanel::onTimerRight )
		EVT_MENU( int( panel::MenuID::eNewLRTower ), RenderPanel::onNewLongRangeTower )
		EVT_MENU( int( panel::MenuID::eNewSRTower ), RenderPanel::onNewShortRangeTower )
		EVT_MENU( int( panel::MenuID::eUpgradeSpeed ), RenderPanel::onUpgradeTowerSpeed )
		EVT_MENU( int( panel::MenuID::eUpgradeRange ), RenderPanel::onUpgradeTowerRange )
		EVT_MENU( int( panel::MenuID::eUpgradeDamage ), RenderPanel::onUpgradeTowerDamage )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop
#pragma clang diagnostic pop

	void RenderPanel::onSize( wxSizeEvent & event )
	{
		m_renderWindow->resize( uint32_t( event.GetSize().x )
			, uint32_t( event.GetSize().y ) );
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

	void RenderPanel::onSetFocus( wxFocusEvent & event )
	{
		event.Skip();
	}

	void RenderPanel::onKillFocus( wxFocusEvent & event )
	{
		doStopTimer( TimerID::eCount );
		event.Skip();
	}

	void RenderPanel::onKeyDown( wxKeyEvent & event )
	{
		switch ( event.GetKeyCode() )
		{
		case WXK_LEFT:
		case 'Q':
			doStartTimer( TimerID::eLeft );
			event.Skip( false );
			break;

		case WXK_RIGHT:
		case 'D':
			doStartTimer( TimerID::eRight );
			event.Skip( false );
			break;

		case WXK_UP:
		case 'Z':
			doStartTimer( TimerID::eUp );
			event.Skip( false );
			break;

		case WXK_DOWN:
		case 'S':
			doStartTimer( TimerID::eDown );
			event.Skip( false );
			break;

		default:
			event.Skip();
			break;
		}

	}

	void RenderPanel::onKeyUp( wxKeyEvent & event )
	{
		switch ( event.GetKeyCode() )
		{
		case WXK_NUMPAD1:
		case '1':
			doUpgradeTowerDamage();
			event.Skip( false );
			break;

		case WXK_NUMPAD2:
		case '2':
			doUpgradeTowerRange();
			event.Skip( false );
			break;

		case WXK_NUMPAD3:
		case '3':
			doUpgradeTowerSpeed();
			event.Skip( false );
			break;

		case WXK_F1:
			m_listener->postEvent( c3d::makeCpuFunctorEvent( c3d::CpuEventType::ePostCpuStep
				, [this]()
				{
					if ( m_game.isRunning() )
					{
						m_game.help();
					}
				} ) );
			event.Skip( false );
			break;

		case WXK_RETURN:
		case WXK_NUMPAD_ENTER:
			m_listener->postEvent( c3d::makeCpuFunctorEvent( c3d::CpuEventType::ePostCpuStep
				, [this]()
				{
					if ( m_game.isEnded() )
					{
						m_game.reset();
						m_game.start();
					}
					else if ( !m_game.isStarted() )
					{
						m_game.start();
					}
				} ) );
			event.Skip( false );
			break;

		case WXK_SPACE:
			m_listener->postEvent( c3d::makeCpuFunctorEvent( c3d::CpuEventType::ePostCpuStep
				, [this]()
				{
					if ( m_game.isStarted() )
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
			event.Skip( false );
			break;

		case WXK_LEFT:
		case 'Q':
			doStopTimer( TimerID::eLeft );
			event.Skip( false );
			break;

		case WXK_RIGHT:
		case 'D':
			doStopTimer( TimerID::eRight );
			event.Skip( false );
			break;

		case WXK_UP:
		case 'Z':
			doStopTimer( TimerID::eUp );
			event.Skip( false );
			break;

		case WXK_DOWN:
		case 'S':
			doStopTimer( TimerID::eDown );
			event.Skip( false );
			break;

		default:
			event.Skip();
			break;
		}
	}

	void RenderPanel::onMouseLDown( wxMouseEvent & event )
	{
		m_mouseLeftDown = true;

		if ( m_game.isRunning() )
		{
			m_x = doTransformX( event.GetX() );
			m_y = doTransformY( event.GetY() );
			m_oldX = m_x;
			m_oldY = m_y;
		}
		event.Skip( false );
	}

	void RenderPanel::onMouseLUp( wxMouseEvent & event )
	{
		m_mouseLeftDown = false;

		if ( m_game.isRunning() )
		{
			m_x = doTransformX( event.GetX() );
			m_y = doTransformY( event.GetY() );
			m_oldX = m_x;
			m_oldY = m_y;
			m_listener->postEvent( c3d::makeGpuFunctorEvent( c3d::GpuEventType::ePreUpload
				, [this]( c3d::RenderDevice const &
					, c3d::QueueData const & )
				{
					c3d::Camera & camera = *m_renderWindow->getCamera();
					camera.update();
					auto type = m_renderWindow->pick( c3d::Position{ int( m_x ), int( m_y ) } );

					if ( type != c3d::PickNodeType::eNone
						&& type != c3d::PickNodeType::eBillboard )
					{
						doUpdateSelectedGeometry( m_renderWindow->getPickedGeometry() );
					}
					else
					{
						doUpdateSelectedGeometry( nullptr );
					}
				} ) );
		}

		event.Skip( false );
	}

	void RenderPanel::onMouseRUp( wxMouseEvent & event )
	{
		if ( m_game.isRunning() && m_selectedTower )
		{
			wxMenu menu;

			if ( m_selectedTower->canUpgradeSpeed() )
			{
				menu.Append( int( panel::MenuID::eUpgradeSpeed ), wxString{ wxT( "Augmenter vitesse (" ) } << m_selectedTower->getSpeedUpgradeCost() << wxT( ")" ) );
				menu.Enable( int( panel::MenuID::eUpgradeSpeed ), m_game.canAfford( m_selectedTower->getSpeedUpgradeCost() ) );
			}
			else
			{
				menu.Append( int( panel::MenuID::eUpgradeSpeed ), wxString{ wxT( "Augmenter vitesse (Max)" ) } );
				menu.Enable( int( panel::MenuID::eUpgradeSpeed ), false );
			}

			if ( m_selectedTower->canUpgradeRange() )
			{
				menu.Append( int( panel::MenuID::eUpgradeRange ), wxString{ wxT( "Augmenter portee (" ) } << m_selectedTower->getRangeUpgradeCost() << wxT( ")" ) );
				menu.Enable( int( panel::MenuID::eUpgradeRange ), m_game.canAfford( m_selectedTower->getRangeUpgradeCost() ) );
			}
			else
			{
				menu.Append( int( panel::MenuID::eUpgradeRange ), wxString{ wxT( "Augmenter portee (Max)" ) } );
				menu.Enable( int( panel::MenuID::eUpgradeRange ), false );
			}

			if ( m_selectedTower->canUpgradeDamage() )
			{
				menu.Append( int( panel::MenuID::eUpgradeDamage ), wxString{ wxT( "Augmenter degats (" ) } << m_selectedTower->getDamageUpgradeCost() << wxT( ")" ) );
				menu.Enable( int( panel::MenuID::eUpgradeDamage ), m_game.canAfford( m_selectedTower->getDamageUpgradeCost() ) );
			}
			else
			{
				menu.Append( int( panel::MenuID::eUpgradeDamage ), wxString{ wxT( "Augmenter degats (Max)" ) } );
				menu.Enable( int( panel::MenuID::eUpgradeDamage ), false );
			}

			PopupMenu( &menu, event.GetPosition() );
		}
		else if ( m_selectedGeometry )
		{
			wxMenu menu;
			menu.Append( int( panel::MenuID::eNewLRTower ), wxString( "Nouvelle Tour Longue Distance (" ) << m_longRange.getTowerCost() << wxT( ")" ) );
			menu.Append( int( panel::MenuID::eNewSRTower ), wxString( "Nouvelle Tour Courte Distance (" ) << m_shortRange.getTowerCost() << wxT( ")" ) );
			menu.Enable( int( panel::MenuID::eNewLRTower ), m_game.canAfford( m_longRange.getTowerCost() ) );
			menu.Enable( int( panel::MenuID::eNewSRTower ), m_game.canAfford( m_shortRange.getTowerCost() ) );
			PopupMenu( &menu, event.GetPosition() );
		}

		event.Skip( false );
	}

	void RenderPanel::onMouseMove( wxMouseEvent & event )
	{
		m_x = doTransformX( event.GetX() );
		m_y = doTransformY( event.GetY() );

		if ( m_game.isRunning() )
		{
			static float constexpr mult = 0.1f;
			float deltaX = 0.0f;
			float deltaY = std::min( 1.0f * mult, 1.0f ) * ( m_oldY - m_y ) * mult;

			if ( m_mouseLeftDown )
			{
				m_cameraState->addAngularVelocity( c3d::Point2f{ -deltaY, deltaX } );
			}
		}

		m_oldX = m_x;
		m_oldY = m_y;
		event.Skip( false );
	}

	void RenderPanel::onMouseWheel( wxMouseEvent & event )
	{
		int wheelRotation = event.GetWheelRotation();

		if ( auto inputListener = wxGetApp().getCastor()->getUserInputListener();
			!inputListener || !inputListener->fireMouseWheel( c3d::Position( 0, wheelRotation )
				, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			if ( wheelRotation < 0 )
			{
				m_cameraState->addScalarVelocity( c3d::Point3f{ 0.0f, 0.0f, -panel::g_camSpeed } );
			}
			else
			{
				m_cameraState->addScalarVelocity( c3d::Point3f{ 0.0f, 0.0f, panel::g_camSpeed } );
			}
		}

		event.Skip( false );
	}

	void RenderPanel::onTimerUp( wxTimerEvent & event )
	{
		m_cameraState->addScalarVelocity( c3d::Point3f{ 0.0f, panel::g_camSpeed, 0.0f } );
		event.Skip( false );
	}

	void RenderPanel::onTimerDown( wxTimerEvent & event )
	{
		m_cameraState->addScalarVelocity( c3d::Point3f{ 0.0f, -panel::g_camSpeed, 0.0f } );
		event.Skip( false );
	}

	void RenderPanel::onTimerLeft( wxTimerEvent & event )
	{
		m_cameraState->addScalarVelocity( c3d::Point3f{ panel::g_camSpeed, 0.0f, 0.0f } );
		event.Skip( false );
	}

	void RenderPanel::onTimerRight( wxTimerEvent & event )
	{
		m_cameraState->addScalarVelocity( c3d::Point3f{ -panel::g_camSpeed, 0.0f, 0.0f } );
		event.Skip( false );
	}

	void RenderPanel::onNewLongRangeTower( wxCommandEvent & event )
	{
		if ( m_game.isRunning() )
		{
			m_listener->postEvent( c3d::makeCpuFunctorEvent( c3d::CpuEventType::ePostCpuStep
				, [this]()
				{
					if ( m_game.buildTower( m_marker->getPosition(), c3d::makeRawUnique< LongRangeTower >( m_longRange ) ) )
					{
						doUpdateSelectedGeometry( nullptr );
					}
				} ) );
		}
		event.Skip( false );
	}

	void RenderPanel::onNewShortRangeTower( wxCommandEvent & event )
	{
		if ( m_game.isRunning() )
		{
			m_listener->postEvent( c3d::makeCpuFunctorEvent( c3d::CpuEventType::ePostCpuStep
				, [this]()
				{
					if ( m_game.buildTower( m_marker->getPosition(), c3d::makeRawUnique< ShortRangeTower >( m_shortRange ) ) )
					{
						doUpdateSelectedGeometry( nullptr );
					}
				} ) );
		}
		event.Skip( false );
	}

	void RenderPanel::onUpgradeTowerSpeed( wxCommandEvent & event )
	{
		doUpgradeTowerSpeed();
		event.Skip( false );
	}

	void RenderPanel::onUpgradeTowerRange( wxCommandEvent & event )
	{
		doUpgradeTowerRange();
		event.Skip( false );
	}

	void RenderPanel::onUpgradeTowerDamage( wxCommandEvent & event )
	{
		doUpgradeTowerDamage();
		event.Skip( false );
	}
}
