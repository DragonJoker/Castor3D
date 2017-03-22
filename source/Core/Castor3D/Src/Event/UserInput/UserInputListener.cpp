#include "UserInputListener.hpp"

#include "Engine.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Event/Frame/FunctorEvent.hpp"

using namespace Castor;

namespace Castor3D
{
	UserInputListener::UserInputListener( Engine & p_engine, String const & p_name )
		: OwnedBy< Engine >{ p_engine }
		, m_frameListener{ p_engine.GetFrameListenerCache().Add( p_name ) }
	{
		m_mouse.m_buttons[size_t( MouseButton::eLeft )] = false;
		m_mouse.m_buttons[size_t( MouseButton::eMiddle )] = false;
		m_mouse.m_buttons[size_t( MouseButton::eRight )] = false;
		m_mouse.m_changed = MouseButton::eCount;
		m_keyboard.m_ctrl = false;
		m_keyboard.m_alt = false;
		m_keyboard.m_shift = false;
		m_frameListener->PostEvent( MakeInitialiseEvent( *this ) );
	}

	UserInputListener::~UserInputListener()
	{
	}

	bool UserInputListener::Initialise()
	{
		m_frameListener->PostEvent( MakeFunctorEvent( EventType::ePostRender, std::bind( &UserInputListener::ProcessEvents, this ) ) );
		m_enabled = DoInitialise();
		return m_enabled;
	}

	void UserInputListener::Cleanup()
	{
		m_enabled = false;

		DoCleanup();

		auto l_lock = make_unique_lock( m_mutexHandlers );
		auto l_it = m_handlers.begin();

		while ( l_it != m_handlers.end() )
		{
			auto l_handler = *l_it;
			DoRemoveHandler( l_handler );
			l_it = m_handlers.begin();
		}
	}

	void UserInputListener::ProcessEvents()
	{
		auto l_handlers = DoGetHandlers();

		for ( auto l_handler : l_handlers )
		{
			l_handler->ProcessEvents();
		}

		if ( m_enabled )
		{
			// Push this method again, for next frame.
			m_frameListener->PostEvent( MakeFunctorEvent( EventType::ePostRender, std::bind( &UserInputListener::ProcessEvents, this ) ) );
		}
	}

	bool UserInputListener::FireMouseMove( Position const & p_position )
	{
		bool l_return = false;
		m_mouse.m_position = p_position;
		auto l_current = DoGetMouseTargetableHandler( p_position );
		auto l_last = m_lastMouseTarget.lock();

		if ( l_current != l_last )
		{
			if ( l_last )
			{
				Castor::Logger::LogDebug( Castor::StringStream() << p_position.x() << "x" << p_position.y() );
				l_last->PushEvent( MouseEvent( MouseEventType::eLeave, p_position ) );
				l_last.reset();
				m_lastMouseTarget.reset();
			}
		}

		if ( l_current )
		{
			if ( l_current != l_last )
			{
				l_current->PushEvent( MouseEvent( MouseEventType::eEnter, p_position ) );
			}

			l_current->PushEvent( MouseEvent( MouseEventType::eMove, p_position ) );
			l_return = true;
			m_lastMouseTarget = l_current;
		}

		return l_return;
	}

	bool UserInputListener::FireMouseButtonPushed( MouseButton p_button )
	{
		bool l_return = false;
		m_mouse.m_buttons[size_t( p_button )] = true;
		m_mouse.m_changed = p_button;
		auto l_current = DoGetMouseTargetableHandler( m_mouse.m_position );

		if ( l_current )
		{
			auto l_active = m_activeHandler.lock();

			if ( l_current != l_active )
			{
				if ( l_active )
				{
					l_active->PushEvent( HandlerEvent( HandlerEventType::eDeactivate, l_current ) );
				}

				l_current->PushEvent( HandlerEvent( HandlerEventType::eActivate, l_active ) );
			}

			l_current->PushEvent( MouseEvent( MouseEventType::ePushed, m_mouse.m_position, p_button ) );
			l_return = true;
			m_activeHandler = l_current;
		}

		return l_return;
	}

	bool UserInputListener::FireMouseButtonReleased( MouseButton p_button )
	{
		bool l_return = false;
		m_mouse.m_buttons[size_t( p_button )] = false;
		m_mouse.m_changed = p_button;
		auto l_current = DoGetMouseTargetableHandler( m_mouse.m_position );

		if ( l_current )
		{
			l_current->PushEvent( MouseEvent( MouseEventType::eReleased, m_mouse.m_position, p_button ) );
			l_return = true;
			m_activeHandler = l_current;
		}
		else
		{
			auto l_active = m_activeHandler.lock();

			if ( l_active )
			{
				l_active->PushEvent( HandlerEvent( HandlerEventType::eDeactivate, l_current ) );
			}

			m_activeHandler.reset();
		}

		return l_return;
	}

	bool UserInputListener::FireMouseWheel( Position const & p_offsets )
	{
		bool l_return = false;
		m_mouse.m_wheel += p_offsets;
		auto l_current = DoGetMouseTargetableHandler( m_mouse.m_position );

		if ( l_current )
		{
			l_current->PushEvent( MouseEvent( MouseEventType::eWheel, p_offsets ) );
			l_return = true;
		}

		return l_return;
	}

	bool UserInputListener::FireKeyDown( KeyboardKey p_key, bool p_ctrl, bool p_alt, bool p_shift )
	{
		bool l_return = false;
		auto l_active = m_activeHandler.lock();

		if ( l_active )
		{
			if ( p_key == KeyboardKey::eControl )
			{
				m_keyboard.m_ctrl = true;
			}

			if ( p_key == KeyboardKey::eAlt )
			{
				m_keyboard.m_alt = true;
			}

			if ( p_key == KeyboardKey::eShift )
			{
				m_keyboard.m_shift = true;
			}

			l_active->PushEvent( KeyboardEvent( KeyboardEventType::ePushed, p_key, m_keyboard.m_ctrl, m_keyboard.m_alt, m_keyboard.m_shift ) );
			l_return = true;
		}

		return l_return;
	}

	bool UserInputListener::FireKeyUp( KeyboardKey p_key, bool p_ctrl, bool p_alt, bool p_shift )
	{
		bool l_return = false;
		auto l_active = m_activeHandler.lock();

		if ( l_active )
		{
			if ( p_key == KeyboardKey::eControl )
			{
				m_keyboard.m_ctrl = false;
			}

			if ( p_key == KeyboardKey::eAlt )
			{
				m_keyboard.m_alt = false;
			}

			if ( p_key == KeyboardKey::eShift )
			{
				m_keyboard.m_shift = false;
			}

			l_active->PushEvent( KeyboardEvent( KeyboardEventType::eReleased, p_key, m_keyboard.m_ctrl, m_keyboard.m_alt, m_keyboard.m_shift ) );
			l_return = true;
		}

		return l_return;
	}

	bool UserInputListener::FireChar( KeyboardKey p_key, String const & p_char )
	{
		bool l_return = false;
		auto l_active = m_activeHandler.lock();

		if ( l_active )
		{
			l_active->PushEvent( KeyboardEvent( KeyboardEventType::eChar, p_key, p_char, m_keyboard.m_ctrl, m_keyboard.m_alt, m_keyboard.m_shift ) );
			l_return = true;
		}

		return l_return;
	}
}
