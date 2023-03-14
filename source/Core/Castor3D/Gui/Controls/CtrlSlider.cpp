#include "Castor3D/Gui/Controls/CtrlSlider.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlStatic.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/Overlay.hpp"

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
		, uint32_t flags
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
		setBackgroundBorders( castor::Point4ui{} );
		EventHandler::connect( MouseEventType::eMove
			, [this]( MouseEvent const & event )
			{
				onMouseMove( event );
			} );
		EventHandler::connect( MouseEventType::eLeave
			, [this]( MouseEvent const & event )
			{
				onMouseLeave( event );
			} );
		EventHandler::connect( MouseEventType::eReleased
			, [this]( MouseEvent const & event )
			{
				onMouseLButtonUp( event );
			} );
		EventHandler::connect( KeyboardEventType::ePushed
			, [this]( KeyboardEvent const & event )
			{
				onKeyDown( event );
			} );

		auto line = std::make_shared< StaticCtrl >( m_scene
			, name + cuT( "_Line" )
			, &style->getLineStyle()
			, this
			, castor::String{}
			, castor::Position{}
			, castor::Size{} );
		line->setVisible( visible );
		line->connectNC( KeyboardEventType::ePushed
			, [this]( ControlSPtr control, KeyboardEvent const & event )
			{
				onNcKeyDown( control, event );
			} );
		m_line = line;

		auto tick = std::make_shared< StaticCtrl >( m_scene
			, name + cuT( "_Tick" )
			, &style->getTickStyle()
			, this
			, castor::String{}
			, castor::Position{}
			, castor::Size{} );
		tick->setVisible( visible );
		tick->setCatchesMouseEvents( true );
		tick->connectNC( MouseEventType::eMove
			, [this]( ControlSPtr control, MouseEvent const & event )
			{
				onTickMouseMove( control, event );
			} );
		tick->connectNC( MouseEventType::ePushed
			, [this]( ControlSPtr control, MouseEvent const & event )
			{
				onTickMouseLButtonDown( control, event );
			} );
		tick->connectNC( MouseEventType::eReleased
			, [this]( ControlSPtr control, MouseEvent const & event )
			{
				onTickMouseLButtonUp( control, event );
			} );
		tick->connectNC( KeyboardEventType::ePushed
			, [this]( ControlSPtr control, KeyboardEvent const & event )
			{
				onNcKeyDown( control, event );
			} );
		m_tick = tick;

		doUpdateStyle();
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

		if ( auto line = m_line.lock() )
		{
			line->setPosition( linePosition );
			line->setSize( lineSize );
			line->setVisible( doIsVisible() );
		}

		if ( auto tick = m_tick.lock() )
		{
			tick->setPosition( tickPosition );
			tick->setSize( tickSize );
			tick->setVisible( doIsVisible() );
		}
	}

	void SliderCtrl::doUpdateStyle()
	{
		auto & style = getStyle();

		if ( auto line = m_line.lock() )
		{
			line->setStyle( &style.getLineStyle() );
		}

		if ( auto tick = m_tick.lock() )
		{
			tick->setStyle( &style.getTickStyle() );
		}
	}

	void SliderCtrl::doCreate()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		StaticCtrlSPtr line = m_line.lock();
		manager.create( line );

		StaticCtrlSPtr tick = m_tick.lock();
		manager.create( tick );
		doUpdateLineAndTick();
		
		manager.connectEvents( *this );
	}

	void SliderCtrl::doDestroy()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		manager.disconnectEvents( *this );

		if ( auto line = m_line.lock() )
		{
			manager.destroy( line );
		}

		if ( auto tick = m_tick.lock() )
		{
			manager.destroy( tick );
		}
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
		if ( auto line = m_line.lock() )
		{
			line->setVisible( value );
		}

		if ( auto tick = m_tick.lock() )
		{
			tick->setVisible( value );
		}
	}

	void SliderCtrl::onMouseMove( MouseEvent const & event )
	{
		if ( m_scrolling )
		{
			doMoveMouse( event.getPosition() );
			m_signals[size_t( SliderEvent::eThumbTrack )]( m_value.value() );
		}
	}

	void SliderCtrl::onMouseLeave( MouseEvent const & event )
	{
		auto controls = getControlsManager();

		if ( !controls )
		{
			return;
		}

		auto focusedControl = controls->getFocusedControl();

		if ( m_scrolling
				&& focusedControl != this
				&& focusedControl != m_tick.lock().get()
				&& focusedControl != m_line.lock().get()
		   )
		{
			doMoveMouse( event.getPosition() );
			m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
			m_scrolling = false;
		}
	}

	void SliderCtrl::onMouseLButtonUp( MouseEvent const & event )
	{
		if ( event.getButton() == MouseButton::eLeft )
		{
			if ( !m_scrolling )
			{
				m_mouse = m_tick.lock()->getPosition();
			}

			doMoveMouse( event.getPosition() );
			m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
			m_scrolling = false;
		}
	}

	void SliderCtrl::onTickMouseMove( ControlSPtr control
		, MouseEvent const & event )
	{
		onMouseMove( event );
	}

	void SliderCtrl::onTickMouseLButtonDown( ControlSPtr control
		, MouseEvent const & event )
	{
		if ( event.getButton() == MouseButton::eLeft )
		{
			m_scrolling = true;
			castor::Point2i relativePosition = event.getPosition() - getAbsolutePosition();
			m_mouse = castor::Position( relativePosition[0], relativePosition[1] );
		}
	}

	void SliderCtrl::onTickMouseLButtonUp( ControlSPtr control, MouseEvent const & event )
	{
		onMouseLButtonUp( event );
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
				else if ( event.getKey() == KeyboardKey::edown )
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

	void SliderCtrl::onNcKeyDown( ControlSPtr control
		, KeyboardEvent const & event )
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

		if ( auto tick = m_tick.lock() )
		{
			castor::Point2i position = tick->getPosition() + realDelta;
			double tickValue = 0;

			if ( auto line = m_line.lock() )
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
