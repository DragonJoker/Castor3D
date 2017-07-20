#include "CtrlSlider.hpp"

#include "ControlsManager.hpp"
#include "CtrlStatic.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Overlay/Overlay.hpp>

#include <Overlay/BorderPanelOverlay.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	SliderCtrl::SliderCtrl( String const & p_name
		, Engine & p_engine
		, ControlRPtr p_parent
		, uint32_t p_id )
		: SliderCtrl{ p_name
			, p_engine
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
		, Engine & p_engine
		, ControlRPtr p_parent
		, uint32_t p_id
		, RangedValue< int32_t > const & p_value
		, Position const & p_position
		, Size const & p_size
		, uint32_t p_style
		, bool p_visible )
		: Control{ ControlType::eSlider
			, p_name
			, p_engine
			, p_parent
			, p_id
			, p_position
			, p_size
			, p_style
			, p_visible }
		, m_value( p_value )
		, m_scrolling( false )
	{
		SetBackgroundBorders( Rectangle() );

		EventHandler::Connect( MouseEventType::eMove, [this]( MouseEvent const & p_event )
		{
			OnMouseMove( p_event );
		} );
		EventHandler::Connect( MouseEventType::eLeave, [this]( MouseEvent const & p_event )
		{
			OnMouseLeave( p_event );
		} );
		EventHandler::Connect( MouseEventType::eReleased, [this]( MouseEvent const & p_event )
		{
			OnMouseLButtonUp( p_event );
		} );
		EventHandler::Connect( KeyboardEventType::ePushed, [this]( KeyboardEvent const & p_event )
		{
			OnKeyDown( p_event );
		} );

		StaticCtrlSPtr line = std::make_shared< StaticCtrl >( p_name + cuT( "_Line" )
			, p_engine
			, this
			, cuT( "" )
			, Position()
			, Size() );
		line->SetBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );
		line->SetVisible( DoIsVisible() );
		line->ConnectNC( KeyboardEventType::ePushed, [this]( ControlSPtr p_control, KeyboardEvent const & p_event )
		{
			OnNcKeyDown( p_control, p_event );
		} );
		m_line = line;

		StaticCtrlSPtr tick = std::make_shared< StaticCtrl >( p_name + cuT( "_Tick" )
			, p_engine
			, this
			, cuT( "" )
			, Position()
			, Size() );
		tick->SetBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );
		tick->SetVisible( DoIsVisible() );
		tick->SetCatchesMouseEvents( true );
		tick->ConnectNC( MouseEventType::eMove, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			OnTickMouseMove( p_control, p_event );
		} );
		tick->ConnectNC( MouseEventType::ePushed, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			OnTickMouseLButtonDown( p_control, p_event );
		} );
		tick->ConnectNC( MouseEventType::eReleased, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			OnTickMouseLButtonUp( p_control, p_event );
		} );
		tick->ConnectNC( KeyboardEventType::ePushed, [this]( ControlSPtr p_control, KeyboardEvent const & p_event )
		{
			OnNcKeyDown( p_control, p_event );
		} );
		m_tick = tick;
	}

	SliderCtrl::~SliderCtrl()
	{
	}

	void SliderCtrl::SetRange( Range< int32_t > const & p_value )
	{
		m_value.update_range( p_value );
		DoUpdateLineAndTick();
	}

	void SliderCtrl::SetValue( int32_t p_value )
	{
		m_value =  p_value;
		DoUpdateLineAndTick();
	}

	void SliderCtrl::DoUpdateLineAndTick()
	{
		Size lineSize( GetSize() );
		Position linePosition;
		Size tickSize( GetSize() );
		Position tickPosition;

		if ( CheckFlag( GetStyle(), SliderStyle::eVertical ) )
		{
			lineSize.width() = 3;
			lineSize.height() -= 4;
			linePosition.x() = ( GetSize().width() - 3 ) / 2;
			linePosition.y() += 2;
			tickSize.width() = ( GetSize().width() / 2 ) + ( GetSize().width() % 2 );
			tickSize.height() = 5;
			tickPosition.x() = tickSize.width() / 2;
			tickPosition.y() = int32_t( lineSize.height() * m_value.percent() );
		}
		else
		{
			lineSize.width() -= 4;
			lineSize.height() = 3;
			linePosition.x() += 2;
			linePosition.y() = ( GetSize().height() - 3 ) / 2;
			tickSize.width() = 5;
			tickSize.height() = ( GetSize().height() / 2 ) + ( GetSize().height() % 2 );
			tickPosition.x() = int32_t( lineSize.width() * m_value.percent() );
			tickPosition.y() = tickSize.height() / 2;
		}

		StaticCtrlSPtr line = m_line.lock();

		if ( line )
		{
			line->SetPosition( linePosition );
			line->SetSize( lineSize );
			line->SetVisible( DoIsVisible() );
		}

		StaticCtrlSPtr tick = m_tick.lock();

		if ( tick )
		{
			tick->SetPosition( tickPosition );
			tick->SetSize( tickSize );
			tick->SetVisible( DoIsVisible() );
		}
	}

	void SliderCtrl::DoCreate()
	{
		REQUIRE( GetControlsManager() );
		auto & manager = *GetControlsManager();
		StaticCtrlSPtr line = m_line.lock();
		line->SetBackgroundMaterial( GetEngine().GetMaterialCache().Find( cuT( "Gray" ) ) );
		line->SetForegroundMaterial( GetForegroundMaterial() );
		manager.Create( line );

		StaticCtrlSPtr tick = m_tick.lock();
		tick->SetBackgroundMaterial( GetEngine().GetMaterialCache().Find( cuT( "White" ) ) );
		tick->SetForegroundMaterial( GetForegroundMaterial() );
		manager.Create( tick );
		DoUpdateLineAndTick();
		
		manager.ConnectEvents( *this );
	}

	void SliderCtrl::DoDestroy()
	{
		REQUIRE( GetControlsManager() );
		auto & manager = *GetControlsManager();
		manager.DisconnectEvents( *this );
		StaticCtrlSPtr line = m_line.lock();

		if ( line )
		{
			manager.Destroy( line );
			m_line.reset();
		}

		StaticCtrlSPtr tick = m_tick.lock();

		if ( tick )
		{
			manager.Destroy( tick );
			m_tick.reset();
		}
	}

	void SliderCtrl::DoSetPosition( Position const & p_value )
	{
		DoUpdateLineAndTick();
	}

	void SliderCtrl::DoSetSize( Size const & p_value )
	{
		DoUpdateLineAndTick();
	}

	void SliderCtrl::DoSetBackgroundMaterial( MaterialSPtr p_material )
	{
	}

	void SliderCtrl::DoSetForegroundMaterial( MaterialSPtr p_material )
	{
		StaticCtrlSPtr line = m_line.lock();

		if ( line )
		{
			line->SetForegroundMaterial( p_material );
			line.reset();
		}

		StaticCtrlSPtr tick = m_tick.lock();

		if ( tick )
		{
			tick->SetForegroundMaterial( p_material );
			tick.reset();
		}
	}

	void SliderCtrl::DoSetVisible( bool p_value )
	{
		StaticCtrlSPtr line = m_line.lock();

		if ( line )
		{
			line->SetVisible( p_value );
			line.reset();
		}

		StaticCtrlSPtr tick = m_tick.lock();

		if ( tick )
		{
			tick->SetVisible( p_value );
			tick.reset();
		}
	}

	void SliderCtrl::OnMouseMove( MouseEvent const & p_event )
	{
		if ( m_scrolling )
		{
			DoMoveMouse( p_event.GetPosition() );
			m_signals[size_t( SliderEvent::eThumbTrack )]( m_value.value() );
		}
	}

	void SliderCtrl::OnMouseLeave( MouseEvent const & p_event )
	{
		if ( m_scrolling
				&& GetControlsManager()->GetFocusedControl() != shared_from_this()
				&& GetControlsManager()->GetFocusedControl() != m_tick.lock()
				&& GetControlsManager()->GetFocusedControl() != m_line.lock()
		   )
		{
			DoMoveMouse( p_event.GetPosition() );
			m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
			m_scrolling = false;
		}
	}

	void SliderCtrl::OnMouseLButtonUp( MouseEvent const & p_event )
	{
		if ( p_event.GetButton() == MouseButton::eLeft )
		{
			if ( !m_scrolling )
			{
				m_mouse = m_tick.lock()->GetPosition();
			}

			DoMoveMouse( p_event.GetPosition() );
			m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
			m_scrolling = false;
		}
	}

	void SliderCtrl::OnTickMouseMove( ControlSPtr p_control, MouseEvent const & p_event )
	{
		OnMouseMove( p_event );
	}

	void SliderCtrl::OnTickMouseLButtonDown( ControlSPtr p_control, MouseEvent const & p_event )
	{
		if ( p_event.GetButton() == MouseButton::eLeft )
		{
			m_scrolling = true;
			Point2i relativePosition = p_event.GetPosition() - GetAbsolutePosition();
			m_mouse = Position( relativePosition[0], relativePosition[1] );
		}
	}

	void SliderCtrl::OnTickMouseLButtonUp( ControlSPtr p_control, MouseEvent const & p_event )
	{
		OnMouseLButtonUp( p_event );
	}

	void SliderCtrl::OnKeyDown( KeyboardEvent const & p_event )
	{
		if ( !m_scrolling )
		{
			if ( CheckFlag( GetStyle(), SliderStyle::eVertical ) )
			{
				if ( p_event.GetKey() == KeyboardKey::eUp )
				{
					DoUpdateTick( Position( 0, -1 ) );
					m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
				}
				else if ( p_event.GetKey() == KeyboardKey::eDown )
				{
					DoUpdateTick( Position( 0, 1 ) );
					m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
				}
			}
			else
			{
				if ( p_event.GetKey() == KeyboardKey::eLeft )
				{
					DoUpdateTick( Position( -1, 0 ) );
					m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
				}
				else if ( p_event.GetKey() == KeyboardKey::eRight )
				{
					DoUpdateTick( Position( 1, 0 ) );
					m_signals[size_t( SliderEvent::eThumbRelease )]( m_value.value() );
				}
			}
		}
	}

	void SliderCtrl::OnNcKeyDown( ControlSPtr p_control, KeyboardEvent const & p_event )
	{
		OnKeyDown( p_event );
	}

	void SliderCtrl::DoUpdateTick( Position const & p_delta )
	{
		Position delta = p_delta;

		if ( CheckFlag( GetStyle(), SliderStyle::eVertical ) )
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
			Point2i position = tick->GetPosition() + delta;
			double tickValue = 0;
			StaticCtrlSPtr line = m_line.lock();
			Size size = GetSize();

			if ( line )
			{
				size = line->GetSize();
			}

			if ( CheckFlag( GetStyle(), SliderStyle::eVertical ) )
			{
				position[1] = std::min( int32_t( size.height() ), std::max( 0, position[1] ) );
				tickValue = ( position[1] - line->GetPosition().y() ) / double( size.height() );
			}
			else
			{
				position[0] = std::min( int32_t( size.width() ), std::max( 0, position[0] ) );
				tickValue = ( position[0] - line->GetPosition().x() ) / double( size.width() );
			}

			tickValue = std::max( 0.0, std::min( 1.0, tickValue ) );
			tick->SetPosition( Position( position[0], position[1] ) );
			m_value = m_value.range().value( float( tickValue ) );
		}
	}

	void SliderCtrl::DoMoveMouse( Position const & p_mouse )
	{
		Point2i relativePosition = p_mouse - GetAbsolutePosition();
		Point2i delta = relativePosition - m_mouse;
		m_mouse = Position( relativePosition[0], relativePosition[1] );
		DoUpdateTick( Position( delta[0], delta[1] ) );
	}

	void SliderCtrl::DoUpdateStyle()
	{
		DoUpdateLineAndTick();
	}
}
