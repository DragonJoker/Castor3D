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

	void RenderPanel::updateRenderWindow( castor3d::RenderWindowDesc const & window )
	{
		if ( auto target = window.renderTarget )
		{
			m_renderWindow->initialise( window );

			auto sizeWnd = FromDIP( GetClientSize() );
			castor::Size sizeScreen;
			castor::System::getScreenSize( 0, sizeScreen );
			GetParent()->SetClientSize( sizeWnd );
			GetParent()->SetPosition( FromDIP( wxPoint( std::max( 0, int( sizeScreen.getWidth() ) - sizeWnd.GetWidth() ) / 2
				, std::max( 0, int( sizeScreen.getHeight() ) - sizeWnd.GetHeight() ) / 2 ) ) );
			castor3d::SceneRPtr scene = target->getScene();

			if ( scene )
			{
				m_marker = scene->findSceneNode( cuT( "MapMouse" ) );
				m_marker->setVisible( false );

				m_listener = m_renderWindow->getListener();

				using LockType = std::unique_lock< castor3d::CameraCache >;
				LockType lock{ castor::makeUniqueLock( scene->getCameraCache() ) };
				auto camera = scene->getCameraCache().begin()->second.get();
				m_renderWindow->addPickingScene( *scene );
				m_cameraState = std::make_unique< GuiCommon::NodeState >( scene->getListener(), camera->getParent(), true );
				m_timers[size_t( TimerID::eMouse )]->Start( 30 );
			}
		}
		else if ( m_listener )
		{
			m_listener = {};
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

	void RenderPanel::doUpdateSelectedGeometry( castor3d::GeometryRPtr geometry )
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

				if ( geometry->getName().find( cuT( "Tower" ) ) == castor::String::npos )
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
						m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
							, [this, geometry]()
							{
								if ( geometry && m_marker )
								{
									if ( auto node = geometry->getParent() )
									{
										castor::Point3f position = node->getPosition();

										if ( auto mesh = geometry->getMesh() )
										{
											auto height = mesh->getBoundingBox().getMax()[1] - mesh->getBoundingBox().getMin()[1];
											m_marker->setPosition( castor::Point3f{ position[0], height + 1, position[2] } );
										}
									}
								}
							} ) );
						m_selectedTower = nullptr;
						break;

					case Cell::State::Tower:
						m_selectedTower = m_game.selectTower( cell );
						break;

					case Cell::State::Target:
					case Cell::State::Path:
					default:
						m_selectedTower = nullptr;
						break;
					}

					m_selectedGeometry = geometry;
				}
			}

			m_listener->postEvent( castor3d::makeGpuFunctorEvent( castor3d::EventType::ePreRender
				, [this, freeCell]( castor3d::RenderDevice const & device
					, castor3d::QueueData const & queueData )
				{
					if ( m_marker )
					{
						m_marker->setVisible( freeCell );
					}
				} ) );
		}
	}

	void RenderPanel::doUpgradeTowerDamage()
	{
		if ( m_game.isRunning() && m_selectedTower && m_selectedTower->canUpgradeDamage() && m_game.canAfford( m_selectedTower->getDamageUpgradeCost() ) )
		{
			m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
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
			m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
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
			m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
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
			for ( size_t i = 0; i < size_t( TimerID::eMouse ); i++ )
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
		EVT_MENU( int( panel::MenuID::eNewLRTower ), RenderPanel::OnNewLongRangeTower )
		EVT_MENU( int( panel::MenuID::eNewSRTower ), RenderPanel::OnNewShortRangeTower )
		EVT_MENU( int( panel::MenuID::eUpgradeSpeed ), RenderPanel::OnUpgradeTowerSpeed )
		EVT_MENU( int( panel::MenuID::eUpgradeRange ), RenderPanel::OnUpgradeTowerRange )
		EVT_MENU( int( panel::MenuID::eUpgradeDamage ), RenderPanel::OnUpgradeTowerDamage )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop
#pragma clang diagnostic pop

	void RenderPanel::OnSize( wxSizeEvent & event )
	{
		m_renderWindow->resize( uint32_t( event.GetSize().x )
			, uint32_t( event.GetSize().y ) );
		event.Skip();
	}

	void RenderPanel::OnMove( wxMoveEvent & event )
	{
		event.Skip();
	}

	void RenderPanel::OnPaint( wxPaintEvent & event )
	{
		event.Skip();
	}

	void RenderPanel::OnsetFocus( wxFocusEvent & event )
	{
		event.Skip();
	}

	void RenderPanel::OnKillFocus( wxFocusEvent & event )
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

		event.Skip();
	}

	void RenderPanel::onKeyUp( wxKeyEvent & event )
	{
		switch ( event.GetKeyCode() )
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
			m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
				, [this]()
				{
					if ( m_game.isRunning() )
					{
						m_game.help();
					}
				} ) );
			break;

		case WXK_RETURN:
		case WXK_NUMPAD_ENTER:
			m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
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
			break;

		case WXK_SPACE:
			m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
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

		event.Skip();
	}

	void RenderPanel::OnMouseLdown( wxMouseEvent & event )
	{
		m_mouseLeftDown = true;

		if ( m_game.isRunning() )
		{
			m_x = doTransformX( event.GetX() );
			m_y = doTransformY( event.GetY() );
			m_oldX = m_x;
			m_oldY = m_y;
		}
	}

	void RenderPanel::OnMouseLUp( wxMouseEvent & event )
	{
		m_mouseLeftDown = false;

		if ( m_game.isRunning() )
		{
			m_x = doTransformX( event.GetX() );
			m_y = doTransformY( event.GetY() );
			m_oldX = m_x;
			m_oldY = m_y;
			m_listener->postEvent( castor3d::makeGpuFunctorEvent( castor3d::EventType::ePreRender
				, [this]( castor3d::RenderDevice const & device
					, castor3d::QueueData const & queueData )
				{
					castor3d::Camera & camera = *m_renderWindow->getCamera();
					camera.update();
					auto type = m_renderWindow->pick( castor::Position{ int( m_x ), int( m_y ) } );

					if ( type != castor3d::PickNodeType::eNone
						&& type != castor3d::PickNodeType::eBillboard )
					{
						doUpdateSelectedGeometry( m_renderWindow->getPickedGeometry() );
					}
					else
					{
						doUpdateSelectedGeometry( nullptr );
					}
				} ) );
		}

		event.Skip();
	}

	void RenderPanel::OnMouseRUp( wxMouseEvent & event )
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

		event.Skip();
	}

	void RenderPanel::OnMouseMove( wxMouseEvent & event )
	{
		m_x = doTransformX( event.GetX() );
		m_y = doTransformY( event.GetY() );

		if ( m_game.isRunning() )
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
		event.Skip();
	}

	void RenderPanel::OnMouseWheel( wxMouseEvent & event )
	{
		int wheelRotation = event.GetWheelRotation();

		auto inputListener = wxGetApp().getCastor()->getUserInputListener();

		if ( !inputListener || !inputListener->fireMouseWheel( castor::Position( 0, wheelRotation )
			, event.ControlDown(), event.AltDown(), event.ShiftDown() ) )
		{
			if ( wheelRotation < 0 )
			{
				m_cameraState->addScalarVelocity( castor::Point3f{ 0.0f, 0.0f, -panel::g_camSpeed } );
			}
			else
			{
				m_cameraState->addScalarVelocity( castor::Point3f{ 0.0f, 0.0f, panel::g_camSpeed } );
			}
		}

		event.Skip();
	}

	void RenderPanel::OnMouseTimer( wxTimerEvent & event )
	{
		if ( m_game.isRunning() && m_cameraState )
		{
			m_cameraState->update();
		}

		event.Skip();
	}

	void RenderPanel::OnTimerUp( wxTimerEvent & event )
	{
		m_cameraState->addScalarVelocity( castor::Point3f{ 0.0f, panel::g_camSpeed, 0.0f } );
		event.Skip();
	}

	void RenderPanel::OnTimerDown( wxTimerEvent & event )
	{
		m_cameraState->addScalarVelocity( castor::Point3f{ 0.0f, -panel::g_camSpeed, 0.0f } );
		event.Skip();
	}

	void RenderPanel::OnTimerLeft( wxTimerEvent & event )
	{
		m_cameraState->addScalarVelocity( castor::Point3f{ panel::g_camSpeed, 0.0f, 0.0f } );
		event.Skip();
	}

	void RenderPanel::OnTimerRight( wxTimerEvent & event )
	{
		m_cameraState->addScalarVelocity( castor::Point3f{ -panel::g_camSpeed, 0.0f, 0.0f } );
		event.Skip();
	}

	void RenderPanel::OnNewLongRangeTower( wxCommandEvent & event )
	{
		if ( m_game.isRunning() )
		{
			m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
				, [this]()
				{
					if ( m_game.buildTower( m_marker->getPosition(), std::make_unique< LongRangeTower >( m_longRange ) ) )
					{
						doUpdateSelectedGeometry( nullptr );
					}
				} ) );
		}
	}

	void RenderPanel::OnNewShortRangeTower( wxCommandEvent & event )
	{
		if ( m_game.isRunning() )
		{
			m_listener->postEvent( castor3d::makeCpuFunctorEvent( castor3d::EventType::ePostRender
				, [this]()
				{
					if ( m_game.buildTower( m_marker->getPosition(), std::make_unique< ShortRangeTower >( m_shortRange ) ) )
					{
						doUpdateSelectedGeometry( nullptr );
					}
				} ) );
		}
	}

	void RenderPanel::OnUpgradeTowerSpeed( wxCommandEvent & event )
	{
		doUpgradeTowerSpeed();
	}

	void RenderPanel::OnUpgradeTowerRange( wxCommandEvent & event )
	{
		doUpgradeTowerRange();
	}

	void RenderPanel::OnUpgradeTowerDamage( wxCommandEvent & event )
	{
		doUpgradeTowerDamage();
	}
}
