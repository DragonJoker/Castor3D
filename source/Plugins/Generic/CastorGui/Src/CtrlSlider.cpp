#include "CtrlSlider.hpp"

#include "ControlsManager.hpp"
#include "CtrlStatic.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Overlay/Overlay.hpp>

#include <Overlay/BorderPanelOverlay.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	SliderCtrl::SliderCtrl( String const & p_name
		, Engine & engine
		, ControlRPtr p_parent
		, uint32_t p_id )
		: SliderCtrl{ p_name
			, engine
			, p_parent
			, p_id
			, makeRangedValue( 0, 0, 100 )
			, Position()
			, Size()
			, 0
			, true }
	{
	}

	SliderCtrl::SliderCtrl( String const & p_name
		, Engine & engine
		, ControlRPtr p_parent
		, uint32_t p_id
		, RangedValue< int32_t > const & p_value
		, Position const & p_position
		, Size const & p_size
		, uint32_t p_style
		, bool p_visible )
		: Control{ ControlType::eSlider
			, p_name
			, engine
			, p_parent
			, p_id
			, p_position
			, p_size
			, p_style
			, p_visible }
		, m_value( p_value )
		, m_scrolling( false )
	{
		setBackgroundBorders( Rectangle() );

		EventHandler::connect( MouseEventType::eMove, [this]( MouseEvent const & p_event )
		{
			onMouseMove( p_event );
		} );
		EventHandler::connect( MouseEventType::eLeave, [this]( MouseEvent const & p_event )
		{
			onMouseLeave( p_event );
		} );
		EventHandler::connect( MouseEventType::eReleased, [this]( MouseEvent const & p_event )
		{
			onMouseLButtonUp( p_event );
		} );
		EventHandler::connect( KeyboardEventType::ePushed, [this]( KeyboardEvent const & p_event )
		{
			onKeyDown( p_event );
		} );

		StaticCtrlSPtr line = std::make_shared< StaticCtrl >( p_name + cuT( "_Line" )
			, engine
			, this
			, cuT( "" )
			, Position()
			, Size() );
		line->setBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );
		line->setVisible( p_visible );
		line->connectNC( KeyboardEventType::ePushed, [this]( ControlSPtr p_control, KeyboardEvent const & p_event )
		{
			onNcKeyDown( p_control, p_event );
		} );
		m_line = line;

		StaticCtrlSPtr tick = std::make_shared< StaticCtrl >( p_name + cuT( "_Tick" )
			, engine
			, this
			, cuT( "" )
			, Position()
			, Size() );
		tick->setBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );
		tick->setVisible( p_visible );
		tick->setCatchesMouseEvents( true );
		tick->connectNC( MouseEventType::eMove, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			onTickMouseMove( p_control, p_event );
		} );
		tick->connectNC( MouseEventType::ePushed, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			onTickMouseLButtonDown( p_control, p_event );
		} );
		tick->connectNC( MouseEventType::eReleased, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			onTickMouseLButtonUp( p_control, p_event );
		} );
		tick->connectNC( KeyboardEventType::ePushed, [this]( ControlSPtr p_control, KeyboardEvent const & p_event )
		{
			onNcKeyDown( p_control, p_event );
		} );
		m_tick = tick;
	}

	SliderCtrl::~SliderCtrl()
	{
	}

	void SliderCtrl::setRange( Range< int32_t > const & p_value )
	{
		m_value.updateRange( p_value );
		doUpdateLineAndTick();
	}

	void SliderCtrl::setValue( int32_t p_value )
	{
		m_value =  p_value;
		doUpdateLineAndTick();
	}

	void SliderCtrl::doUpdateLineAndTick()
	{
		Size lineSize( getSize() );
		Position linePosition;
		Size tickSize( getSize() );
		Position tickPosition;

		if ( checkFlag( getStyle(), SliderStyle::eVertical ) )
		{
			lineSize.getWidth() = 3;
			lineSize.getHeight() -= 4;
			linePosition.x() = ( getSize().getWidth() - 3 ) / 2;
			linePosition.y() += 2;
			tickSize.getWidth() = ( getSize().getWidth() / 2 ) + ( getSize().getWidth() % 2 );
			tickSize.getHeight() = 5;
			tickPosition.x() = tickSize.getWidth() / 2;
			tickPosition.y() = int32_t( lineSize.getHeight() * m_value.percent() );
		}
		else
		{
			lineSize.getWidth() -= 4;
			lineSize.getHeight() = 3;
			linePosition.x() += 2;
			linePosition.y() = ( getSize().getHeight() - 3 ) / 2;
			tickSize.getWidth() = 5;
			tickSize.getHeight() = ( getSize().getHeight() / 2 ) + ( getSize().getHeight() % 2 );
			tickPosition.x() = int32_t( lineSize.getWidth() * m_value.percent() );
			tickPosition.y() = tickSize.getHeight() / 2;
		}

		StaticCtrlSPtr line = m_line.lock();

		if ( line )
		{
			line->setPosition( linePosition );
			line->setSize( lineSize );
			line->setVisible( doIsVisible() );
		}

		StaticCtrlSPtr tick = m_tick.lock();

		if ( tick )
		{
			tick->setPosition( tickPosition );
			tick->setSize( tickSize );
			tick->setVisible( doIsVisible() );
		}
	}

	void SliderCtrl::doCreate()
	{
		REQUIRE( getControlsManager() );
		auto & manager = *getControlsManager();
		StaticCtrlSPtr line = m_line.lock();
		line->setBackgroundMaterial( getEngine().getMaterialCache().find( cuT( "Gray" ) ) );
		line->setForegroundMaterial( getForegroundMaterial() );
		manager.create( line );

		StaticCtrlSPtr tick = m_tick.lock();
		tick->setBackgroundMaterial( getEngine().getMaterialCache().find( cuT( "White" ) ) );
		tick->setForegroundMaterial( getForegroundMaterial() );
		manager.create( tick );
		doUpdateLineAndTick();
		
		manager.connectEvents( *this );
	}

	void SliderCtrl::doDestroy()
	{
		REQUIRE( getControlsManager() );
		auto & manager = *getControlsManager();
		manager.disconnectEvents( *this );
		StaticCtrlSPtr line = m_line.lock();

		if ( line )
		{
			manager.destroy( line );
			m_line.reset();
		}

		StaticCtrlSPtr tick = m_tick.lock();

		if ( tick )
		{
			manager.destroy( tick );
			m_tick.reset();
		}
	}

	void SliderCtrl::doSetPosition( Position const & p_value )
	{
		doUpdateLineAndTick();
	}

	void SliderCtrl::doSetSize( Size const & p_value )
	{
		doUpdateLineAndTick();
	}

	void SliderCtrl::doSetBackgroundMaterial( MaterialSPtr p_material )
	{
	}

	void SliderCtrl::doSetForegroundMaterial( MaterialSPtr p_material )
	{
		StaticCtrlSPtr line = m_line.lock();

		if ( line )
		{
			line->setForegroundMaterial( p_material );
			line.reset();
		}

		StaticCtrlSPtr tick = m_tick.lock();

		if ( tick )
		{
			tick->setForegroundMaterial( p_material );
			tick.reset();
		}
	}

	void SliderCtrl::doSetVisible( bool p_value )
	{
		StaticCtrlSPtr line = m_line.lock();

		if ( line )
		{
			line->setVisible( p_value );
			line.reset();
		}

		StaticCtrlSPtr tick = m_tick.lock();

		if ( tick )
		{
			tick->setVisible( p_value );
			tick.reset();
		}
	}

	void SliderCtrl::onMouseMove( MouseEvent const & p_event )
	{
		if ( m_scrolling )
		{
			doMoveMouse( p_event.getPosition() );
			m_signals[size_t( SliderEvent::eThumbTrack )]( m_value.value() );
		}
	}

	void SliderCtrl::onMouseLeave( MouseEvent const & p_event )
	{
		if ( m_scrolling
				&& getControlsManager()->getFocusedControl() != shared_from_this()
				&& getControlsManager()->getFocusedControl() != m_tick.lock()
				&& getControlsManager()->getFocusedControl() != m_line.lock()
		   )
		{
			doMoveMouse( p_event.getPosition() );
			m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
			m_scrolling = false;
		}
	}

	void SliderCtrl::onMouseLButtonUp( MouseEvent const & p_event )
	{
		if ( p_event.getButton() == MouseButton::eLeft )
		{
			if ( !m_scrolling )
			{
				m_mouse = m_tick.lock()->getPosition();
			}

			doMoveMouse( p_event.getPosition() );
			m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
			m_scrolling = false;
		}
	}

	void SliderCtrl::onTickMouseMove( ControlSPtr p_control, MouseEvent const & p_event )
	{
		onMouseMove( p_event );
	}

	void SliderCtrl::onTickMouseLButtonDown( ControlSPtr p_control, MouseEvent const & p_event )
	{
		if ( p_event.getButton() == MouseButton::eLeft )
		{
			m_scrolling = true;
			Point2i relativePosition = p_event.getPosition() - getAbsolutePosition();
			m_mouse = Position( relativePosition[0], relativePosition[1] );
		}
	}

	void SliderCtrl::onTickMouseLButtonUp( ControlSPtr p_control, MouseEvent const & p_event )
	{
		onMouseLButtonUp( p_event );
	}

	void SliderCtrl::onKeyDown( KeyboardEvent const & p_event )
	{
		if ( !m_scrolling )
		{
			if ( checkFlag( getStyle(), SliderStyle::eVertical ) )
			{
				if ( p_event.getKey() == KeyboardKey::eUp )
				{
					doUpdateTick( Position( 0, -1 ) );
					m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
				}
				else if ( p_event.getKey() == KeyboardKey::edown )
				{
					doUpdateTick( Position( 0, 1 ) );
					m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
				}
			}
			else
			{
				if ( p_event.getKey() == KeyboardKey::eLeft )
				{
					doUpdateTick( Position( -1, 0 ) );
					m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
				}
				else if ( p_event.getKey() == KeyboardKey::eRight )
				{
					doUpdateTick( Position( 1, 0 ) );
					m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
				}
			}
		}
	}

	void SliderCtrl::onNcKeyDown( ControlSPtr p_control, KeyboardEvent const & p_event )
	{
		onKeyDown( p_event );
	}

	void SliderCtrl::doUpdateTick( Position const & p_delta )
	{
		Position delta = p_delta;

		if ( checkFlag( getStyle(), SliderStyle::eVertical ) )
		{
			delta.x() = 0;
		}
		else
		{
			delta.y() = 0;
		}

		StaticCtrlSPtr tick = m_tick.lock();

		if ( tick )
		{
			Point2i position = tick->getPosition() + delta;
			double tickValue = 0;
			StaticCtrlSPtr line = m_line.lock();
			Size size = getSize();

			if ( line )
			{
				size = line->getSize();
			}

			if ( checkFlag( getStyle(), SliderStyle::eVertical ) )
			{
				position[1] = std::min( int32_t( size.getHeight() ), std::max( 0, position[1] ) );
				tickValue = ( position[1] - line->getPosition().y() ) / double( size.getHeight() );
			}
			else
			{
				position[0] = std::min( int32_t( size.getWidth() ), std::max( 0, position[0] ) );
				tickValue = ( position[0] - line->getPosition().x() ) / double( size.getWidth() );
			}

			tickValue = std::max( 0.0, std::min( 1.0, tickValue ) );
			tick->setPosition( Position( position[0], position[1] ) );
			m_value = m_value.range().value( float( tickValue ) );
		}
	}

	void SliderCtrl::doMoveMouse( Position const & p_mouse )
	{
		Point2i relativePosition = p_mouse - getAbsolutePosition();
		Point2i delta = relativePosition - m_mouse;
		m_mouse = Position( relativePosition[0], relativePosition[1] );
		doUpdateTick( Position( delta[0], delta[1] ) );
	}

	void SliderCtrl::doUpdateStyle()
	{
		doUpdateLineAndTick();
	}
}
