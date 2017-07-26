#include "UserInputListener.hpp"

#include "Engine.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Overlay/Overlay.hpp"
#include "Overlay/TextOverlay.hpp"

using namespace Castor;

namespace Castor3D
{
	UserInputListener::UserInputListener( Engine & engine, String const & p_name )
		: OwnedBy< Engine >{ engine }
		, m_frameListener{ engine.GetFrameListenerCache().Add( p_name ) }
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

		auto lock = make_unique_lock( m_mutexHandlers );
		auto it = m_handlers.begin();

		while ( it != m_handlers.end() )
		{
			auto handler = *it;
			DoRemoveHandler( handler );
			it = m_handlers.begin();
		}
	}

	void UserInputListener::ProcessEvents()
	{
		auto handlers = DoGetHandlers();

		for ( auto handler : handlers )
		{
			handler->ProcessEvents();
		}

		if ( m_enabled )
		{
			// Push this method again, for next frame.
			m_frameListener->PostEvent( MakeFunctorEvent( EventType::ePostRender, std::bind( &UserInputListener::ProcessEvents, this ) ) );
		}
	}

	void UserInputListener::RegisterClickAction( String const & p_handler
		, OnClickActionFunction p_function )
	{
		auto it = m_onClickActions.find( p_handler );

		if ( it == m_onClickActions.end() )
		{
			m_onClickActions.emplace( p_handler, p_function );
		}
	}

	void UserInputListener::RegisterSelectAction( String const & p_handler
		, OnSelectActionFunction p_function )
	{
		auto it = m_onSelectActions.find( p_handler );

		if ( it == m_onSelectActions.end() )
		{
			m_onSelectActions.emplace( p_handler, p_function );
		}
	}

	void UserInputListener::RegisterTextAction( String const & p_handler
		, OnTextActionFunction p_function )
	{
		auto it = m_onTextActions.find( p_handler );

		if ( it == m_onTextActions.end() )
		{
			m_onTextActions.emplace( p_handler, p_function );
		}
	}

	void UserInputListener::UnregisterClickAction( String const & p_handler )
	{
		auto it = m_onClickActions.find( p_handler );

		if ( it != m_onClickActions.end() )
		{
			m_onClickActions.erase( it );
		}
	}

	void UserInputListener::UnregisterSelectAction( String const & p_handler )
	{
		auto it = m_onSelectActions.find( p_handler );

		if ( it != m_onSelectActions.end() )
		{
			m_onSelectActions.erase( it );
		}
	}

	void UserInputListener::UnregisterTextAction( String const & p_handler )
	{
		auto it = m_onTextActions.find( p_handler );

		if ( it != m_onTextActions.end() )
		{
			m_onTextActions.erase( it );
		}
	}

	void UserInputListener::OnClickAction( String const & p_handler )
	{
		auto it = m_onClickActions.find( p_handler );

		if ( it != m_onClickActions.end() )
		{
			it->second();
		}
	}

	void UserInputListener::OnSelectAction( String const & p_handler, int p_index )
	{
		auto it = m_onSelectActions.find( p_handler );

		if ( it != m_onSelectActions.end() )
		{
			it->second( p_index );
		}
	}

	void UserInputListener::OnTextAction( String const & p_handler, Castor::String const & p_text )
	{
		auto it = m_onTextActions.find( p_handler );

		if ( it != m_onTextActions.end() )
		{
			it->second( p_text );
		}
	}

	bool UserInputListener::FireMouseMove( Position const & p_position )
	{
		bool result = false;
		m_mouse.m_position = p_position;
		auto current = DoGetMouseTargetableHandler( p_position );
		auto last = m_lastMouseTarget.lock();

		if ( current != last )
		{
			if ( last )
			{
				Castor::Logger::LogDebug( Castor::StringStream() << p_position.x() << "x" << p_position.y() );
				last->PushEvent( MouseEvent( MouseEventType::eLeave, p_position ) );
				last.reset();
				m_lastMouseTarget.reset();
			}
		}

		if ( current )
		{
			if ( current != last )
			{
				current->PushEvent( MouseEvent( MouseEventType::eEnter, p_position ) );
			}

			current->PushEvent( MouseEvent( MouseEventType::eMove, p_position ) );
			result = true;
			m_lastMouseTarget = current;
		}

		return result;
	}

	bool UserInputListener::FireMouseButtonPushed( MouseButton p_button )
	{
		bool result = false;
		m_mouse.m_buttons[size_t( p_button )] = true;
		m_mouse.m_changed = p_button;
		auto current = DoGetMouseTargetableHandler( m_mouse.m_position );

		if ( current )
		{
			auto active = m_activeHandler.lock();

			if ( current != active )
			{
				if ( active )
				{
					active->PushEvent( HandlerEvent( HandlerEventType::eDeactivate, current ) );
				}

				current->PushEvent( HandlerEvent( HandlerEventType::eActivate, active ) );
			}

			current->PushEvent( MouseEvent( MouseEventType::ePushed, m_mouse.m_position, p_button ) );
			result = true;
			m_activeHandler = current;
		}

		return result;
	}

	bool UserInputListener::FireMouseButtonReleased( MouseButton p_button )
	{
		bool result = false;
		m_mouse.m_buttons[size_t( p_button )] = false;
		m_mouse.m_changed = p_button;
		auto current = DoGetMouseTargetableHandler( m_mouse.m_position );

		if ( current )
		{
			current->PushEvent( MouseEvent( MouseEventType::eReleased, m_mouse.m_position, p_button ) );
			result = true;
			m_activeHandler = current;
		}
		else
		{
			auto active = m_activeHandler.lock();

			if ( active )
			{
				active->PushEvent( HandlerEvent( HandlerEventType::eDeactivate, current ) );
			}

			m_activeHandler.reset();
		}

		return result;
	}

	bool UserInputListener::FireMouseWheel( Position const & p_offsets )
	{
		bool result = false;
		m_mouse.m_wheel += p_offsets;
		auto current = DoGetMouseTargetableHandler( m_mouse.m_position );

		if ( current )
		{
			current->PushEvent( MouseEvent( MouseEventType::eWheel, p_offsets ) );
			result = true;
		}

		return result;
	}

	bool UserInputListener::FireKeyDown( KeyboardKey p_key, bool p_ctrl, bool p_alt, bool p_shift )
	{
		bool result = false;
		auto active = m_activeHandler.lock();

		if ( active )
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

			active->PushEvent( KeyboardEvent( KeyboardEventType::ePushed, p_key, m_keyboard.m_ctrl, m_keyboard.m_alt, m_keyboard.m_shift ) );
			result = true;
		}

		return result;
	}

	bool UserInputListener::FireKeyUp( KeyboardKey p_key, bool p_ctrl, bool p_alt, bool p_shift )
	{
		bool result = false;
		auto active = m_activeHandler.lock();

		if ( active )
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

			active->PushEvent( KeyboardEvent( KeyboardEventType::eReleased, p_key, m_keyboard.m_ctrl, m_keyboard.m_alt, m_keyboard.m_shift ) );
			result = true;
		}

		return result;
	}

	bool UserInputListener::FireChar( KeyboardKey p_key, String const & p_char )
	{
		bool result = false;
		auto active = m_activeHandler.lock();

		if ( active )
		{
			active->PushEvent( KeyboardEvent( KeyboardEventType::eChar, p_key, p_char, m_keyboard.m_ctrl, m_keyboard.m_alt, m_keyboard.m_shift ) );
			result = true;
		}

		return result;
	}

	bool UserInputListener::FireMaterialEvent( Castor::String const & p_overlay, Castor::String const & p_material )
	{
		bool result = false;
		auto & cache = GetEngine()->GetOverlayCache();
		auto overlay = cache.Find( p_overlay );

		if ( overlay )
		{
			auto material = GetEngine()->GetMaterialCache().Find( p_material );

			if ( material )
			{
				this->m_frameListener->PostEvent( MakeFunctorEvent( EventType::ePreRender
					, [overlay, material]()
					{
						overlay->SetMaterial( material );
					} ) );
				result = true;
			}
		}
		
		return result;
	}

	bool UserInputListener::FireTextEvent( Castor::String const & p_overlay, Castor::String const & p_caption )
	{
		bool result = false;
		auto & cache = GetEngine()->GetOverlayCache();
		auto overlay = cache.Find( p_overlay );

		if ( overlay && overlay->GetType() == OverlayType::eText )
		{
			overlay->GetTextOverlay()->SetCaption( p_caption );
			result = true;
		}

		return result;
	}
}
