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
	SliderCtrl::SliderCtrl( Engine * p_engine
		, ControlRPtr p_parent
		, uint32_t p_id )
		: SliderCtrl{ p_engine
			, p_parent
			, p_id
			, makeRangedValue( 0, 0, 100 )
			, Position()
			, Size()
			, 0
			, true }
	{
	}

	SliderCtrl::SliderCtrl( Engine * p_engine
		, ControlRPtr p_parent
		, uint32_t p_id
		, RangedValue< int32_t > const & p_value
		, Position const & p_position
		, Size const & p_size
		, uint32_t p_style
		, bool p_visible )
		: Control{ ControlType::eSlider
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

		StaticCtrlSPtr l_line = std::make_shared< StaticCtrl >( p_engine, this, cuT( "" ), Position(), Size() );
		l_line->SetBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );
		l_line->SetVisible( DoIsVisible() );
		l_line->ConnectNC( KeyboardEventType::ePushed, [this]( ControlSPtr p_control, KeyboardEvent const & p_event )
		{
			OnNcKeyDown( p_control, p_event );
		} );
		m_line = l_line;

		StaticCtrlSPtr l_tick = std::make_shared< StaticCtrl >( p_engine, this, cuT( "" ), Position(), Size() );
		l_tick->SetBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );
		l_tick->SetVisible( DoIsVisible() );
		l_tick->SetCatchesMouseEvents( true );
		l_tick->ConnectNC( MouseEventType::eMove, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			OnTickMouseMove( p_control, p_event );
		} );
		l_tick->ConnectNC( MouseEventType::ePushed, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			OnTickMouseLButtonDown( p_control, p_event );
		} );
		l_tick->ConnectNC( MouseEventType::eReleased, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			OnTickMouseLButtonUp( p_control, p_event );
		} );
		l_tick->ConnectNC( KeyboardEventType::ePushed, [this]( ControlSPtr p_control, KeyboardEvent const & p_event )
		{
			OnNcKeyDown( p_control, p_event );
		} );
		m_tick = l_tick;
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
		Size l_lineSize( GetSize() );
		Position l_linePosition;
		Size l_tickSize( GetSize() );
		Position l_tickPosition;

		if ( CheckFlag( GetStyle(), SliderStyle::eVertical ) )
		{
			l_lineSize.width() = 3;
			l_lineSize.height() -= 4;
			l_linePosition.x() = ( GetSize().width() - 3 ) / 2;
			l_linePosition.y() += 2;
			l_tickSize.width() = ( GetSize().width() / 2 ) + ( GetSize().width() % 2 );
			l_tickSize.height() = 5;
			l_tickPosition.x() = l_tickSize.width() / 2;
			l_tickPosition.y() = int32_t( l_lineSize.height() * m_value.percent() );
		}
		else
		{
			l_lineSize.width() -= 4;
			l_lineSize.height() = 3;
			l_linePosition.x() += 2;
			l_linePosition.y() = ( GetSize().height() - 3 ) / 2;
			l_tickSize.width() = 5;
			l_tickSize.height() = ( GetSize().height() / 2 ) + ( GetSize().height() % 2 );
			l_tickPosition.x() = int32_t( l_lineSize.width() * m_value.percent() );
			l_tickPosition.y() = l_tickSize.height() / 2;
		}

		StaticCtrlSPtr l_line = m_line.lock();

		if ( l_line )
		{
			l_line->SetPosition( l_linePosition );
			l_line->SetSize( l_lineSize );
			l_line->SetVisible( DoIsVisible() );
		}

		StaticCtrlSPtr l_tick = m_tick.lock();

		if ( l_tick )
		{
			l_tick->SetPosition( l_tickPosition );
			l_tick->SetSize( l_tickSize );
			l_tick->SetVisible( DoIsVisible() );
		}
	}

	void SliderCtrl::DoCreate()
	{
		StaticCtrlSPtr l_line = m_line.lock();
		l_line->SetBackgroundMaterial( GetEngine()->GetMaterialCache().Find( cuT( "Gray" ) ) );
		l_line->SetForegroundMaterial( GetForegroundMaterial() );
		GetControlsManager()->Create( l_line );

		StaticCtrlSPtr l_tick = m_tick.lock();
		l_tick->SetBackgroundMaterial( GetEngine()->GetMaterialCache().Find( cuT( "White" ) ) );
		l_tick->SetForegroundMaterial( GetForegroundMaterial() );
		GetControlsManager()->Create( l_tick );
		DoUpdateLineAndTick();
	}

	void SliderCtrl::DoDestroy()
	{
		StaticCtrlSPtr l_line = m_line.lock();

		if ( l_line )
		{
			l_line->Destroy();
			m_line.reset();
		}

		StaticCtrlSPtr l_tick = m_tick.lock();

		if ( l_tick )
		{
			l_tick->Destroy();
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
		StaticCtrlSPtr l_line = m_line.lock();

		if ( l_line )
		{
			l_line->SetForegroundMaterial( p_material );
			l_line.reset();
		}

		StaticCtrlSPtr l_tick = m_tick.lock();

		if ( l_tick )
		{
			l_tick->SetForegroundMaterial( p_material );
			l_tick.reset();
		}
	}

	void SliderCtrl::DoSetVisible( bool p_value )
	{
		StaticCtrlSPtr l_line = m_line.lock();

		if ( l_line )
		{
			l_line->SetVisible( p_value );
			l_line.reset();
		}

		StaticCtrlSPtr l_tick = m_tick.lock();

		if ( l_tick )
		{
			l_tick->SetVisible( p_value );
			l_tick.reset();
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
			Point2i l_relativePosition = p_event.GetPosition() - GetAbsolutePosition();
			m_mouse = Position( l_relativePosition[0], l_relativePosition[1] );
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
		Position l_delta = p_delta;

		if ( CheckFlag( GetStyle(), SliderStyle::eVertical ) )
		{
			l_delta.x() = 0;
		}
		else
		{
			l_delta.y() = 0;
		}

		StaticCtrlSPtr l_tick = m_tick.lock();

		if ( l_tick )
		{
			Point2i l_position = l_tick->GetPosition() + l_delta;
			double l_tickValue = 0;
			StaticCtrlSPtr l_line = m_line.lock();
			Size l_size = GetSize();

			if ( l_line )
			{
				l_size = l_line->GetSize();
			}

			if ( CheckFlag( GetStyle(), SliderStyle::eVertical ) )
			{
				l_position[1] = std::min( int32_t( l_size.height() ), std::max( 0, l_position[1] ) );
				l_tickValue = ( l_position[1] - l_line->GetPosition().y() ) / double( l_size.height() );
			}
			else
			{
				l_position[0] = std::min( int32_t( l_size.width() ), std::max( 0, l_position[0] ) );
				l_tickValue = ( l_position[0] - l_line->GetPosition().x() ) / double( l_size.width() );
			}

			l_tickValue = std::max( 0.0, std::min( 1.0, l_tickValue ) );
			l_tick->SetPosition( Position( l_position[0], l_position[1] ) );
			m_value = m_value.range().value( float( l_tickValue ) );
		}
	}

	void SliderCtrl::DoMoveMouse( Position const & p_mouse )
	{
		Point2i l_relativePosition = p_mouse - GetAbsolutePosition();
		Point2i l_delta = l_relativePosition - m_mouse;
		m_mouse = Position( l_relativePosition[0], l_relativePosition[1] );
		DoUpdateTick( Position( l_delta[0], l_delta[1] ) );
	}

	void SliderCtrl::DoUpdateStyle()
	{
		DoUpdateLineAndTick();
	}
}
