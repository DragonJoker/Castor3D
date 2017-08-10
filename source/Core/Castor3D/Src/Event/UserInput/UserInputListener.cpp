#include "UserInputListener.hpp"

#include "Engine.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Overlay/Overlay.hpp"
#include "Overlay/TextOverlay.hpp"

using namespace castor;

namespace castor3d
{
	UserInputListener::UserInputListener( Engine & engine, String const & p_name )
		: OwnedBy< Engine >{ engine }
		, m_frameListener{ engine.getFrameListenerCache().add( p_name ) }
	{
		m_mouse.m_buttons[size_t( MouseButton::eLeft )] = false;
		m_mouse.m_buttons[size_t( MouseButton::eMiddle )] = false;
		m_mouse.m_buttons[size_t( MouseButton::eRight )] = false;
		m_mouse.m_changed = MouseButton::eCount;
		m_keyboard.m_ctrl = false;
		m_keyboard.m_alt = false;
		m_keyboard.m_shift = false;
		m_frameListener->postEvent( MakeInitialiseEvent( *this ) );
	}

	UserInputListener::~UserInputListener()
	{
	}

	bool UserInputListener::initialise()
	{
		m_frameListener->postEvent( MakeFunctorEvent( EventType::ePostRender, std::bind( &UserInputListener::processEvents, this ) ) );
		m_enabled = doInitialise();
		return m_enabled;
	}

	void UserInputListener::cleanup()
	{
		m_enabled = false;

		doCleanup();

		auto lock = makeUniqueLock( m_mutexHandlers );
		auto it = m_handlers.begin();

		while ( it != m_handlers.end() )
		{
			auto handler = *it;
			doRemoveHandler( handler );
			it = m_handlers.begin();
		}
	}

	void UserInputListener::processEvents()
	{
		auto handlers = doGetHandlers();

		for ( auto handler : handlers )
		{
			handler->processEvents();
		}

		if ( m_enabled )
		{
			// Push this method again, for next frame.
			m_frameListener->postEvent( MakeFunctorEvent( EventType::ePostRender, std::bind( &UserInputListener::processEvents, this ) ) );
		}
	}

	void UserInputListener::registerClickAction( String const & p_handler
		, OnClickActionFunction p_function )
	{
		auto it = m_onClickActions.find( p_handler );

		if ( it == m_onClickActions.end() )
		{
			m_onClickActions.emplace( p_handler, p_function );
		}
	}

	void UserInputListener::registerSelectAction( String const & p_handler
		, OnSelectActionFunction p_function )
	{
		auto it = m_onSelectActions.find( p_handler );

		if ( it == m_onSelectActions.end() )
		{
			m_onSelectActions.emplace( p_handler, p_function );
		}
	}

	void UserInputListener::registerTextAction( String const & p_handler
		, OnTextActionFunction p_function )
	{
		auto it = m_onTextActions.find( p_handler );

		if ( it == m_onTextActions.end() )
		{
			m_onTextActions.emplace( p_handler, p_function );
		}
	}

	void UserInputListener::unregisterClickAction( String const & p_handler )
	{
		auto it = m_onClickActions.find( p_handler );

		if ( it != m_onClickActions.end() )
		{
			m_onClickActions.erase( it );
		}
	}

	void UserInputListener::unregisterSelectAction( String const & p_handler )
	{
		auto it = m_onSelectActions.find( p_handler );

		if ( it != m_onSelectActions.end() )
		{
			m_onSelectActions.erase( it );
		}
	}

	void UserInputListener::unregisterTextAction( String const & p_handler )
	{
		auto it = m_onTextActions.find( p_handler );

		if ( it != m_onTextActions.end() )
		{
			m_onTextActions.erase( it );
		}
	}

	void UserInputListener::onClickAction( String const & p_handler )
	{
		auto it = m_onClickActions.find( p_handler );

		if ( it != m_onClickActions.end() )
		{
			it->second();
		}
	}

	void UserInputListener::onSelectAction( String const & p_handler, int p_index )
	{
		auto it = m_onSelectActions.find( p_handler );

		if ( it != m_onSelectActions.end() )
		{
			it->second( p_index );
		}
	}

	void UserInputListener::onTextAction( String const & p_handler, castor::String const & p_text )
	{
		auto it = m_onTextActions.find( p_handler );

		if ( it != m_onTextActions.end() )
		{
			it->second( p_text );
		}
	}

	bool UserInputListener::fireMouseMove( Position const & p_position )
	{
		bool result = false;
		m_mouse.m_position = p_position;
		auto current = doGetMouseTargetableHandler( p_position );
		auto last = m_lastMouseTarget.lock();

		if ( current != last )
		{
			if ( last )
			{
				castor::Logger::logDebug( castor::StringStream() << p_position.x() << "x" << p_position.y() );
				last->pushEvent( MouseEvent( MouseEventType::eLeave, p_position ) );
				last.reset();
				m_lastMouseTarget.reset();
			}
		}

		if ( current )
		{
			if ( current != last )
			{
				current->pushEvent( MouseEvent( MouseEventType::eEnter, p_position ) );
			}

			current->pushEvent( MouseEvent( MouseEventType::eMove, p_position ) );
			result = true;
			m_lastMouseTarget = current;
		}

		return result;
	}

	bool UserInputListener::fireMouseButtonPushed( MouseButton p_button )
	{
		bool result = false;
		m_mouse.m_buttons[size_t( p_button )] = true;
		m_mouse.m_changed = p_button;
		auto current = doGetMouseTargetableHandler( m_mouse.m_position );

		if ( current )
		{
			auto active = m_activeHandler.lock();

			if ( current != active )
			{
				if ( active )
				{
					active->pushEvent( HandlerEvent( HandlerEventType::eDeactivate, current ) );
				}

				current->pushEvent( HandlerEvent( HandlerEventType::eActivate, active ) );
			}

			current->pushEvent( MouseEvent( MouseEventType::ePushed, m_mouse.m_position, p_button ) );
			result = true;
			m_activeHandler = current;
		}

		return result;
	}

	bool UserInputListener::fireMouseButtonReleased( MouseButton p_button )
	{
		bool result = false;
		m_mouse.m_buttons[size_t( p_button )] = false;
		m_mouse.m_changed = p_button;
		auto current = doGetMouseTargetableHandler( m_mouse.m_position );

		if ( current )
		{
			current->pushEvent( MouseEvent( MouseEventType::eReleased, m_mouse.m_position, p_button ) );
			result = true;
			m_activeHandler = current;
		}
		else
		{
			auto active = m_activeHandler.lock();

			if ( active )
			{
				active->pushEvent( HandlerEvent( HandlerEventType::eDeactivate, current ) );
			}

			m_activeHandler.reset();
		}

		return result;
	}

	bool UserInputListener::fireMouseWheel( Position const & p_offsets )
	{
		bool result = false;
		m_mouse.m_wheel += p_offsets;
		auto current = doGetMouseTargetableHandler( m_mouse.m_position );

		if ( current )
		{
			current->pushEvent( MouseEvent( MouseEventType::eWheel, p_offsets ) );
			result = true;
		}

		return result;
	}

	bool UserInputListener::fireKeydown( KeyboardKey p_key, bool p_ctrl, bool p_alt, bool p_shift )
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

			active->pushEvent( KeyboardEvent( KeyboardEventType::ePushed, p_key, m_keyboard.m_ctrl, m_keyboard.m_alt, m_keyboard.m_shift ) );
			result = true;
		}

		return result;
	}

	bool UserInputListener::fireKeyUp( KeyboardKey p_key, bool p_ctrl, bool p_alt, bool p_shift )
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

			active->pushEvent( KeyboardEvent( KeyboardEventType::eReleased, p_key, m_keyboard.m_ctrl, m_keyboard.m_alt, m_keyboard.m_shift ) );
			result = true;
		}

		return result;
	}

	bool UserInputListener::fireChar( KeyboardKey p_key, String const & p_char )
	{
		bool result = false;
		auto active = m_activeHandler.lock();

		if ( active )
		{
			active->pushEvent( KeyboardEvent( KeyboardEventType::eChar, p_key, p_char, m_keyboard.m_ctrl, m_keyboard.m_alt, m_keyboard.m_shift ) );
			result = true;
		}

		return result;
	}

	bool UserInputListener::fireMaterialEvent( castor::String const & p_overlay, castor::String const & p_material )
	{
		bool result = false;
		auto & cache = getEngine()->getOverlayCache();
		auto overlay = cache.find( p_overlay );

		if ( overlay )
		{
			auto material = getEngine()->getMaterialCache().find( p_material );

			if ( material )
			{
				this->m_frameListener->postEvent( MakeFunctorEvent( EventType::ePreRender
					, [overlay, material]()
					{
						overlay->setMaterial( material );
					} ) );
				result = true;
			}
		}
		
		return result;
	}

	bool UserInputListener::fireTextEvent( castor::String const & p_overlay, castor::String const & p_caption )
	{
		bool result = false;
		auto & cache = getEngine()->getOverlayCache();
		auto overlay = cache.find( p_overlay );

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			overlay->getTextOverlay()->setCaption( p_caption );
			result = true;
		}

		return result;
	}
}
