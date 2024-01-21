#include "Castor3D/Gui/Controls/CtrlSlider.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlStatic.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/Overlay.hpp"

CU_ImplementSmartPtr( castor3d, SliderCtrl )

namespace castor3d
{
	SliderCtrl::SliderCtrl( SceneRPtr scene
		, castor::String const & name
		, SliderStyleRPtr style
		, ControlRPtr parent )
		: SliderCtrl{ scene
			, name
			, style
			, parent
			, castor::makeRangedValue( 0, 0, 100 )
			, castor::Position{}
			, castor::Size{}
			, 0
			, true }
	{
	}

	SliderCtrl::SliderCtrl( SceneRPtr scene
		, castor::String const & name
		, SliderStyleRPtr style
		, ControlRPtr parent
		, castor::RangedValue< int32_t > const & value
		, castor::Position const & position
		, castor::Size const & size
		, ControlFlagType flags
		, bool visible )
		: Control{ Type
			, scene
			, name
			, style
			, parent
			, position
			, size
			, flags
			, visible }
		, m_value{ value }
		, m_scrolling{ false }
	{
		auto & manager = *getEngine().getControlsManager();
		setBorderSize( castor::Point4ui{} );
		EventHandler::connect( KeyboardEventType::ePushed
			, [this]( KeyboardEvent const & event )
			{
				onKeyDown( event );
			} );

		m_line = manager.registerControlT( castor::makeUnique< StaticCtrl >( m_scene
			, cuT( "Line" )
			, &style->getLineStyle()
			, this
			, castor::U32String{}
			, castor::Position{}
			, castor::Size{} ) );
		m_line->setVisible( visible );
		m_line->connectNC( KeyboardEventType::ePushed
			, [this]( ControlRPtr, KeyboardEvent const & event )
			{
				onNcKeyDown( event );
			} );

		m_tick = manager.registerControlT( castor::makeUnique< StaticCtrl >( m_scene
			, cuT( "Tick" )
			, &style->getTickStyle()
			, this
			, castor::U32String{}
			, castor::Position{}
			, castor::Size{} ) );
		m_tick->setVisible( visible );
		m_tick->setCatchesMouseEvents( true );
		m_tick->connectNC( MouseEventType::eMove
			, [this]( ControlRPtr, MouseEvent const & event )
			{
				onTickMouseMove( event );
			} );
		m_tick->connectNC( MouseEventType::ePushed
			, [this]( ControlRPtr, MouseEvent const & event )
			{
				onTickMouseButtonDown( event );
			} );
		m_tick->connectNC( MouseEventType::eReleased
			, [this]( ControlRPtr, MouseEvent const & event )
			{
				onTickMouseButtonUp( event );
			} );
		m_tick->connectNC( KeyboardEventType::ePushed
			, [this]( ControlRPtr, KeyboardEvent const & event )
			{
				onNcKeyDown( event );
			} );

		setStyle( style );
	}

	SliderCtrl::~SliderCtrl()noexcept
	{
		auto & manager = *getEngine().getControlsManager();
		manager.unregisterControl( *m_tick );
		manager.unregisterControl( *m_line );
	}

	void SliderCtrl::setRange( castor::Range< int32_t > const & value )
	{
		m_value.updateRange( value );
		doUpdateLineAndTick();
	}

	void SliderCtrl::setValue( int32_t value )
	{
		m_value =  value;
		doUpdateLineAndTick();
	}

	void SliderCtrl::doUpdateLineAndTick()
	{
		castor::Size lineSize( getSize() );
		castor::Position linePosition;
		castor::Size tickSize( getSize() );
		castor::Position tickPosition;

		if ( castor::checkFlag( getFlags(), SliderFlag::eVertical ) )
		{
			lineSize.getWidth() = 3;
			lineSize.getHeight() -= 4;
			linePosition.x() = int32_t( ( getSize().getWidth() - 3 ) / 2 );
			linePosition.y() += 2;
			tickSize.getWidth() = ( getSize().getWidth() / 2 ) + ( getSize().getWidth() % 2 );
			tickSize.getHeight() = 5;
			tickPosition.x() = int32_t( tickSize.getWidth() / 2 );
			tickPosition.y() = int32_t( float( lineSize.getHeight() ) * m_value.percent() );
		}
		else
		{
			lineSize.getWidth() -= 4;
			lineSize.getHeight() = 3;
			linePosition.x() += 2;
			linePosition.y() = int32_t( ( getSize().getHeight() - 3 ) / 2 );
			tickSize.getWidth() = 5;
			tickSize.getHeight() = ( getSize().getHeight() / 2 ) + ( getSize().getHeight() % 2 );
			tickPosition.x() = int32_t( float( lineSize.getWidth() ) * m_value.percent() );
			tickPosition.y() = int32_t( tickSize.getHeight() / 2 );
		}

		if ( auto line = m_line )
		{
			line->setPosition( linePosition );
			line->setSize( lineSize );
			line->setVisible( isBackgroundVisible() );
		}

		if ( auto tick = m_tick )
		{
			tick->setPosition( tickPosition );
			tick->setSize( tickSize );
			tick->setVisible( isBackgroundVisible() );
		}
	}

	void SliderCtrl::doUpdateStyle()
	{
		auto & style = getStyle();

		if ( auto line = m_line )
		{
			line->setStyle( &style.getLineStyle() );
		}

		if ( auto tick = m_tick )
		{
			tick->setStyle( &style.getTickStyle() );
		}
	}

	void SliderCtrl::doCreate()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		manager.create( m_line );
		manager.create( m_tick );
		doUpdateLineAndTick();
		
		manager.connectEvents( *this );
	}

	void SliderCtrl::doDestroy()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		manager.disconnectEvents( *this );
		manager.destroy( m_line );
		manager.destroy( m_tick );
	}

	void SliderCtrl::doSetPosition( castor::Position const & value )
	{
		doUpdateLineAndTick();
	}

	void SliderCtrl::doSetSize( castor::Size const & value )
	{
		doUpdateLineAndTick();
	}

	void SliderCtrl::doSetVisible( bool value )
	{
		if ( auto line = m_line )
		{
			line->setVisible( value );
		}

		if ( auto tick = m_tick )
		{
			tick->setVisible( value );
		}
	}

	void SliderCtrl::doOnMouseMove( MouseEvent const & event )
	{
		if ( m_scrolling )
		{
			doMoveMouse( event.getPosition() );
			m_signals[size_t( SliderEvent::eThumbTrack )]( m_value.value() );
		}
	}

	void SliderCtrl::doOnMouseLeave( MouseEvent const & event )
	{
		auto controls = getControlsManager();

		if ( !controls )
		{
			return;
		}

		auto focusedControl = controls->getFocusedControl();

		if ( m_scrolling
				&& focusedControl != this
				&& focusedControl != m_tick
				&& focusedControl != m_line
		   )
		{
			doMoveMouse( event.getPosition() );
			m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
			m_scrolling = false;
		}
	}

	void SliderCtrl::doOnMouseButtonUp( MouseEvent const & event )
	{
		if ( event.getButton() == MouseButton::eLeft )
		{
			if ( !m_scrolling )
			{
				m_mouse = m_tick->getPosition();
			}

			doMoveMouse( event.getPosition() );
			m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
			m_scrolling = false;
		}
	}

	void SliderCtrl::onTickMouseMove( MouseEvent const & event )
	{
		doOnMouseMove( event );
	}

	void SliderCtrl::onTickMouseButtonDown( MouseEvent const & event )
	{
		if ( event.getButton() == MouseButton::eLeft )
		{
			m_scrolling = true;
			castor::Point2i relativePosition = event.getPosition() - getAbsolutePosition();
			m_mouse = castor::Position( relativePosition[0], relativePosition[1] );
		}
	}

	void SliderCtrl::onTickMouseButtonUp( MouseEvent const & event )
	{
		doOnMouseButtonUp( event );
	}

	void SliderCtrl::onKeyDown( KeyboardEvent const & event )
	{
		if ( !m_scrolling )
		{
			if ( castor::checkFlag( getFlags(), SliderFlag::eVertical ) )
			{
				if ( event.getKey() == KeyboardKey::eUp )
				{
					doUpdateTick( castor::Position( 0, -1 ) );
					m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
				}
				else if ( event.getKey() == KeyboardKey::eDown )
				{
					doUpdateTick( castor::Position( 0, 1 ) );
					m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
				}
			}
			else
			{
				if ( event.getKey() == KeyboardKey::eLeft )
				{
					doUpdateTick( castor::Position( -1, 0 ) );
					m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
				}
				else if ( event.getKey() == KeyboardKey::eRight )
				{
					doUpdateTick( castor::Position( 1, 0 ) );
					m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
				}
			}
		}
	}

	void SliderCtrl::onNcKeyDown( KeyboardEvent const & event )
	{
		onKeyDown( event );
	}

	void SliderCtrl::doUpdateTick( castor::Position const & delta )
	{
		castor::Position realDelta = delta;

		if ( castor::checkFlag( getFlags(), SliderFlag::eVertical ) )
		{
			realDelta.x() = 0;
		}
		else
		{
			realDelta.y() = 0;
		}

		if ( auto tick = m_tick )
		{
			castor::Point2i position = tick->getPosition() + realDelta;
			double tickValue = 0;

			if ( auto line = m_line )
			{
				auto size = line->getSize();

				if ( castor::checkFlag( getFlags(), SliderFlag::eVertical ) )
				{
					position[1] = std::min( int32_t( size.getHeight() ), std::max( 0, position[1] ) );
					tickValue = ( position[1] - line->getPosition().y() ) / double( size.getHeight() );
				}
				else
				{
					position[0] = std::min( int32_t( size.getWidth() ), std::max( 0, position[0] ) );
					tickValue = ( position[0] - line->getPosition().x() ) / double( size.getWidth() );
				}
			}

			tickValue = std::max( 0.0, std::min( 1.0, tickValue ) );
			tick->setPosition( castor::Position( position[0], position[1] ) );
			m_value = m_value.range().value( float( tickValue ) );
		}
	}

	void SliderCtrl::doMoveMouse( castor::Position const & mouse )
	{
		castor::Point2i relativePosition = mouse - getAbsolutePosition();
		castor::Point2i delta = relativePosition - m_mouse;
		m_mouse = castor::Position( relativePosition[0], relativePosition[1] );
		doUpdateTick( castor::Position( delta[0], delta[1] ) );
	}

	void SliderCtrl::doUpdateFlags()
	{
		doUpdateLineAndTick();
	}
}
