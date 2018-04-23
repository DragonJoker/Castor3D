#include "UserInputListener.hpp"

#include "Engine.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Overlay/Overlay.hpp"
#include "Overlay/TextOverlay.hpp"

using namespace castor;

namespace castor3d
{
	UserInputListener::UserInputListener( Engine & engine, String const & name )
		: OwnedBy< Engine >{ engine }
		, m_frameListener{ engine.getFrameListenerCache().add( name ) }
	{
		m_mouse.m_buttons[size_t( MouseButton::eLeft )] = false;
		m_mouse.m_buttons[size_t( MouseButton::eMiddle )] = false;
		m_mouse.m_buttons[size_t( MouseButton::eRight )] = false;
		m_mouse.m_changed = MouseButton::eCount;
		m_keyboard.m_ctrl = false;
		m_keyboard.m_alt = false;
		m_keyboard.m_shift = false;
		m_frameListener->postEvent( makeInitialiseEvent( *this ) );
	}

	UserInputListener::~UserInputListener()
	{
	}

	bool UserInputListener::initialise()
	{
		m_frameListener->postEvent( makeFunctorEvent( EventType::ePostRender, std::bind( &UserInputListener::processEvents, this ) ) );
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
			m_frameListener->postEvent( makeFunctorEvent( EventType::ePostRender, std::bind( &UserInputListener::processEvents, this ) ) );
		}
	}

	void UserInputListener::registerClickAction( String const & handler
		, OnClickActionFunction function )
	{
		auto it = m_onClickActions.find( handler );

		if ( it == m_onClickActions.end() )
		{
			m_onClickActions.emplace( handler, function );
		}
	}

	void UserInputListener::registerSelectAction( String const & handler
		, OnSelectActionFunction function )
	{
		auto it = m_onSelectActions.find( handler );

		if ( it == m_onSelectActions.end() )
		{
			m_onSelectActions.emplace( handler, function );
		}
	}

	void UserInputListener::registerTextAction( String const & handler
		, OnTextActionFunction function )
	{
		auto it = m_onTextActions.find( handler );

		if ( it == m_onTextActions.end() )
		{
			m_onTextActions.emplace( handler, function );
		}
	}

	void UserInputListener::unregisterClickAction( String const & handler )
	{
		auto it = m_onClickActions.find( handler );

		if ( it != m_onClickActions.end() )
		{
			m_onClickActions.erase( it );
		}
	}

	void UserInputListener::unregisterSelectAction( String const & handler )
	{
		auto it = m_onSelectActions.find( handler );

		if ( it != m_onSelectActions.end() )
		{
			m_onSelectActions.erase( it );
		}
	}

	void UserInputListener::unregisterTextAction( String const & handler )
	{
		auto it = m_onTextActions.find( handler );

		if ( it != m_onTextActions.end() )
		{
			m_onTextActions.erase( it );
		}
	}

	void UserInputListener::onClickAction( String const & handler )
	{
		auto it = m_onClickActions.find( handler );

		if ( it != m_onClickActions.end() )
		{
			it->second();
		}
	}

	void UserInputListener::onSelectAction( String const & handler, int index )
	{
		auto it = m_onSelectActions.find( handler );

		if ( it != m_onSelectActions.end() )
		{
			it->second( index );
		}
	}

	void UserInputListener::onTextAction( String const & handler, castor::String const & text )
	{
		auto it = m_onTextActions.find( handler );

		if ( it != m_onTextActions.end() )
		{
			it->second( text );
		}
	}

	bool UserInputListener::fireMouseMove( Position const & position )
	{
		bool result = false;

		if ( doHasHandlers() )
		{
			m_mouse.m_position = position;
			auto current = doGetMouseTargetableHandler( position );
			auto last = m_lastMouseTarget.lock();

			if ( current != last )
			{
				if ( last )
				{
					castor::Logger::logDebug( castor::makeStringStream() << position.x() << "x" << position.y() );
					last->pushEvent( MouseEvent( MouseEventType::eLeave, position ) );
					last.reset();
					m_lastMouseTarget.reset();
				}
			}

			if ( current )
			{
				if ( current != last )
				{
					current->pushEvent( MouseEvent( MouseEventType::eEnter, position ) );
				}

				current->pushEvent( MouseEvent( MouseEventType::eMove, position ) );
				result = true;
				m_lastMouseTarget = current;
			}
		}

		return result;
	}

	bool UserInputListener::fireMouseButtonPushed( MouseButton button )
	{
		bool result = false;

		if ( doHasHandlers() )
		{
			m_mouse.m_buttons[size_t( button )] = true;
			m_mouse.m_changed = button;
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

				current->pushEvent( MouseEvent( MouseEventType::ePushed, m_mouse.m_position, button ) );
				result = true;
				m_activeHandler = current;
			}
		}

		return result;
	}

	bool UserInputListener::fireMouseButtonReleased( MouseButton button )
	{
		bool result = false;

		if ( doHasHandlers() )
		{
			m_mouse.m_buttons[size_t( button )] = false;
			m_mouse.m_changed = button;
			auto current = doGetMouseTargetableHandler( m_mouse.m_position );

			if ( current )
			{
				current->pushEvent( MouseEvent( MouseEventType::eReleased, m_mouse.m_position, button ) );
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
		}

		return result;
	}

	bool UserInputListener::fireMouseWheel( Position const & offsets )
	{
		bool result = false;

		if ( doHasHandlers() )
		{
			m_mouse.m_wheel += offsets;
			auto current = doGetMouseTargetableHandler( m_mouse.m_position );

			if ( current )
			{
				current->pushEvent( MouseEvent( MouseEventType::eWheel, offsets ) );
				result = true;
			}
		}

		return result;
	}

	bool UserInputListener::fireKeydown( KeyboardKey key, bool ctrl, bool alt, bool shift )
	{
		bool result = false;

		if ( doHasHandlers() )
		{
			auto active = m_activeHandler.lock();

			if ( active )
			{
				if ( key == KeyboardKey::eControl )
				{
					m_keyboard.m_ctrl = true;
				}

				if ( key == KeyboardKey::eAlt )
				{
					m_keyboard.m_alt = true;
				}

				if ( key == KeyboardKey::eShift )
				{
					m_keyboard.m_shift = true;
				}

				active->pushEvent( KeyboardEvent( KeyboardEventType::ePushed, key, m_keyboard.m_ctrl, m_keyboard.m_alt, m_keyboard.m_shift ) );
				result = true;
			}
		}

		return result;
	}

	bool UserInputListener::fireKeyUp( KeyboardKey key, bool ctrl, bool alt, bool shift )
	{
		bool result = false;

		if ( doHasHandlers() )
		{
			auto active = m_activeHandler.lock();

			if ( active )
			{
				if ( key == KeyboardKey::eControl )
				{
					m_keyboard.m_ctrl = false;
				}

				if ( key == KeyboardKey::eAlt )
				{
					m_keyboard.m_alt = false;
				}

				if ( key == KeyboardKey::eShift )
				{
					m_keyboard.m_shift = false;
				}

				active->pushEvent( KeyboardEvent( KeyboardEventType::eReleased, key, m_keyboard.m_ctrl, m_keyboard.m_alt, m_keyboard.m_shift ) );
				result = true;
			}
		}

		return result;
	}

	bool UserInputListener::fireChar( KeyboardKey key, String const & value )
	{
		bool result = false;

		if ( doHasHandlers() )
		{
			auto active = m_activeHandler.lock();

			if ( active )
			{
				active->pushEvent( KeyboardEvent( KeyboardEventType::eChar, key, value, m_keyboard.m_ctrl, m_keyboard.m_alt, m_keyboard.m_shift ) );
				result = true;
			}
		}

		return result;
	}

	bool UserInputListener::fireMaterialEvent( castor::String const & overlayName, castor::String const & materialName )
	{
		bool result = false;

		if ( doHasHandlers() )
		{
			auto & cache = getEngine()->getOverlayCache();
			auto overlay = cache.find( overlayName );

			if ( overlay )
			{
				auto material = getEngine()->getMaterialCache().find( materialName );

				if ( material )
				{
					this->m_frameListener->postEvent( makeFunctorEvent( EventType::ePreRender
						, [overlay, material]()
						{
							overlay->setMaterial( material );
						} ) );
					result = true;
				}
			}
		}
		
		return result;
	}

	bool UserInputListener::fireTextEvent( castor::String const & overlayName, castor::String const & caption )
	{
		bool result = false;

		if ( doHasHandlers() )
		{
			auto & cache = getEngine()->getOverlayCache();
			auto overlay = cache.find( overlayName );

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				overlay->getTextOverlay()->setCaption( caption );
				result = true;
			}
		}

		return result;
	}
}
