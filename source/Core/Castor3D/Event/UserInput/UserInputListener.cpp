#include "Castor3D/Event/UserInput/UserInputListener.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"

#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

CU_ImplementCUSmartPtr( castor3d, UserInputListener )

namespace castor3d
{
	UserInputListener::UserInputListener( Engine & engine, castor::String const & name )
		: castor::OwnedBy< Engine >{ engine }
		, m_frameListener{ engine.addNewFrameListener( name ) }
	{
		m_mouse.buttons[size_t( MouseButton::eLeft )] = false;
		m_mouse.buttons[size_t( MouseButton::eMiddle )] = false;
		m_mouse.buttons[size_t( MouseButton::eRight )] = false;
		m_mouse.changed = MouseButton::eCount;
		m_keyboard.ctrl = false;
		m_keyboard.alt = false;
		m_keyboard.shift = false;
		m_frameListener->postEvent( makeCpuFunctorEvent( EventType::ePreRender
			, [this]()
			{
				initialise();
			} ) );
	}

	UserInputListener::~UserInputListener()
	{
	}

	bool UserInputListener::initialise()
	{
		m_frameListener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
			, [this]()
			{
				processEvents();
			} ) );
		m_enabled = doInitialise();
		return m_enabled;
	}

	void UserInputListener::cleanup()
	{
		m_enabled = false;

		doCleanup();

		m_activeHandler = {};
		m_lastMouseTarget = {};
		using LockType = std::unique_lock< std::mutex >;
		LockType lock{ castor::makeUniqueLock( m_mutexHandlers ) };
		auto it = m_handlers.begin();

		while ( it != m_handlers.end() )
		{
			auto & handler = **it;
			doRemoveHandlerNL( handler );
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
			m_frameListener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this]()
				{
					processEvents();
				} ) );
		}
	}

	void UserInputListener::registerMouseMoveAction( castor::String const & handler
		, OnMouseMoveActionFunction function )
	{
		auto it = m_onMouseMoveActions.find( handler );

		if ( it == m_onMouseMoveActions.end() )
		{
			m_onMouseMoveActions.emplace( handler, function );
		}
	}

	void UserInputListener::registerClickAction( castor::String const & handler
		, OnClickActionFunction function )
	{
		auto it = m_onClickActions.find( handler );

		if ( it == m_onClickActions.end() )
		{
			m_onClickActions.emplace( handler, function );
		}
	}

	void UserInputListener::registerSelectAction( castor::String const & handler
		, OnSelectActionFunction function )
	{
		auto it = m_onSelectActions.find( handler );

		if ( it == m_onSelectActions.end() )
		{
			m_onSelectActions.emplace( handler, function );
		}
	}

	void UserInputListener::registerTextAction( castor::String const & handler
		, OnTextActionFunction function )
	{
		auto it = m_onTextActions.find( handler );

		if ( it == m_onTextActions.end() )
		{
			m_onTextActions.emplace( handler, function );
		}
	}

	void UserInputListener::registerExpandAction( castor::String const & handler
		, OnExpandActionFunction function )
	{
		auto it = m_onExpandActions.find( handler );

		if ( it == m_onExpandActions.end() )
		{
			m_onExpandActions.emplace( handler, function );
		}
	}

	void UserInputListener::registerCursorAction( OnCursorActionFunction function )
	{
		m_onCursorAction = function;
	}

	void UserInputListener::registerClipboardTextAction( OnClipboardTextActionFunction function )
	{
		m_onClipboardTextAction = function;
	}

	void UserInputListener::unregisterMouseMoveAction( castor::String const & handler )
	{
		auto it = m_onMouseMoveActions.find( handler );

		if ( it != m_onMouseMoveActions.end() )
		{
			m_onMouseMoveActions.erase( it );
		}
	}

	void UserInputListener::unregisterClickAction( castor::String const & handler )
	{
		auto it = m_onClickActions.find( handler );

		if ( it != m_onClickActions.end() )
		{
			m_onClickActions.erase( it );
		}
	}

	void UserInputListener::unregisterSelectAction( castor::String const & handler )
	{
		auto it = m_onSelectActions.find( handler );

		if ( it != m_onSelectActions.end() )
		{
			m_onSelectActions.erase( it );
		}
	}

	void UserInputListener::unregisterTextAction( castor::String const & handler )
	{
		auto it = m_onTextActions.find( handler );

		if ( it != m_onTextActions.end() )
		{
			m_onTextActions.erase( it );
		}
	}

	void UserInputListener::unregisterExpandAction( castor::String const & handler )
	{
		auto it = m_onExpandActions.find( handler );

		if ( it != m_onExpandActions.end() )
		{
			m_onExpandActions.erase( it );
		}
	}

	void UserInputListener::unregisterCursorAction()
	{
		m_onCursorAction = {};
	}

	void UserInputListener::unregisterClipboardTextAction()
	{
		m_onClipboardTextAction = {};
	}

	void UserInputListener::onMouseMoveAction( castor::String const & handler )const
	{
		auto it = m_onMouseMoveActions.find( handler );

		if ( it != m_onMouseMoveActions.end() )
		{
			it->second( getMousePosition() );
		}
	}

	void UserInputListener::onClickAction( castor::String const & handler )const
	{
		auto it = m_onClickActions.find( handler );

		if ( it != m_onClickActions.end() )
		{
			it->second();
		}
	}

	void UserInputListener::onSelectAction( castor::String const & handler, int index )const
	{
		auto it = m_onSelectActions.find( handler );

		if ( it != m_onSelectActions.end() )
		{
			it->second( index );
		}
	}

	void UserInputListener::onTextAction( castor::String const & handler
		, castor::U32String const & text )const
	{
		auto it = m_onTextActions.find( handler );

		if ( it != m_onTextActions.end() )
		{
			it->second( text );
		}
	}

	void UserInputListener::onExpandAction( castor::String const & handler
		, bool expand )const
	{
		auto it = m_onExpandActions.find( handler );

		if ( it != m_onExpandActions.end() )
		{
			it->second( expand );
		}
	}

	void UserInputListener::onCursorAction( MouseCursor cursor )const
	{
		if ( m_onCursorAction )
		{
			m_onCursorAction( cursor );
		}
	}

	castor::U32String UserInputListener::onClipboardTextAction( bool set
		, castor::U32String text )const
	{
		return m_onClipboardTextAction
			? m_onClipboardTextAction( set, std::move( text ) )
			: castor::U32String{};
	}

	void UserInputListener::enableHandler( castor::String const & handler )
	{
		auto it = std::find_if( m_handlers.begin()
			, m_handlers.end()
			, [&handler]( EventHandlerUPtr const & lookup )
			{
				return lookup->getName() == handler;
			} );

		if ( it != m_handlers.end() )
		{
			( *it )->enable();
		}
	}

	void UserInputListener::disableHandler( castor::String const & handler )
	{
		auto it = std::find_if( m_handlers.begin()
			, m_handlers.end()
			, [&handler]( EventHandlerUPtr const & lookup )
			{
				return lookup->getName() == handler;
			} );

		if ( it != m_handlers.end() )
		{
			( *it )->disable();
		}
	}

	bool UserInputListener::fireMouseMove( castor::Position const & position, bool ctrl, bool alt, bool shift )
	{
		bool result = false;
		doUpdateCommandKeys( ctrl, alt, shift );

		if ( doHasHandlers() )
		{
			m_mouse.position = position;
			auto current = doGetMouseTargetableHandler( position );
			auto last = m_lastMouseTarget;

			if ( current != last )
			{
				if ( last )
				{
					last->pushEvent( MouseEvent( MouseEventType::eLeave, position ) );
					last = nullptr;
					m_lastMouseTarget = nullptr;
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

	bool UserInputListener::fireMouseButtonPushed( MouseButton button, bool ctrl, bool alt, bool shift )
	{
		bool result = false;
		doUpdateCommandKeys( ctrl, alt, shift );

		if ( doHasHandlers() )
		{
			m_mouse.buttons[size_t( button )] = true;
			m_mouse.changed = button;
			auto current = doGetMouseTargetableHandler( m_mouse.position );

			if ( current )
			{
				auto active = m_activeHandler;

				if ( current != active )
				{
					if ( active )
					{
						active->pushEvent( HandlerEvent( HandlerEventType::eDeactivate, current ) );
					}

					current->pushEvent( HandlerEvent( HandlerEventType::eActivate, active ) );
				}

				current->pushEvent( MouseEvent( MouseEventType::ePushed, m_mouse.position, button ) );
				result = true;
				m_activeHandler = current;
			}
		}

		return result;
	}

	bool UserInputListener::fireMouseButtonReleased( MouseButton button, bool ctrl, bool alt, bool shift )
	{
		bool result = false;
		doUpdateCommandKeys( ctrl, alt, shift );

		if ( doHasHandlers() )
		{
			m_mouse.buttons[size_t( button )] = false;
			m_mouse.changed = button;
			auto current = doGetMouseTargetableHandler( m_mouse.position );

			if ( current )
			{
				current->pushEvent( MouseEvent( MouseEventType::eReleased, m_mouse.position, button ) );
				result = true;
				m_activeHandler = current;
			}
			else
			{
				auto active = m_activeHandler;

				if ( active )
				{
					active->pushEvent( HandlerEvent( HandlerEventType::eDeactivate, current ) );
				}

				m_activeHandler = nullptr;
			}
		}

		return result;
	}

	bool UserInputListener::fireMouseWheel( castor::Position const & offsets, bool ctrl, bool alt, bool shift )
	{
		bool result = false;
		doUpdateCommandKeys( ctrl, alt, shift );

		if ( doHasHandlers() )
		{
			m_mouse.wheel += offsets;
			auto current = doGetMouseTargetableHandler( m_mouse.position );

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
		doUpdateCommandKeys( ctrl, alt, shift );

		if ( doHasHandlers() )
		{
			auto active = m_activeHandler;

			if ( active
				&& active->isEnabled() )
			{
				active->pushEvent( KeyboardEvent( KeyboardEventType::ePushed, key, m_keyboard.ctrl, m_keyboard.alt, m_keyboard.shift ) );
				result = true;
			}
		}

		return result;
	}

	bool UserInputListener::fireKeyUp( KeyboardKey key, bool ctrl, bool alt, bool shift )
	{
		bool result = false;
		doUpdateCommandKeys( ctrl, alt, shift );

		if ( doHasHandlers() )
		{
			auto active = m_activeHandler;

			if ( active
				&& active->isEnabled() )
			{
				active->pushEvent( KeyboardEvent( KeyboardEventType::eReleased, key, m_keyboard.ctrl, m_keyboard.alt, m_keyboard.shift ) );
				result = true;
			}
		}

		return result;
	}

	bool UserInputListener::fireChar( KeyboardKey key, castor::String const & value )
	{
		bool result = false;

		if ( doHasHandlers() )
		{
			auto active = m_activeHandler;

			if ( active
				&& active->isEnabled() )
			{
				active->pushEvent( KeyboardEvent( KeyboardEventType::eChar, key, value, m_keyboard.ctrl, m_keyboard.alt, m_keyboard.shift ) );
				result = true;
			}
		}

		return result;
	}

	void UserInputListener::doUpdateCommandKeys( bool ctrl
		, bool alt
		, bool shift )
	{
		m_keyboard.ctrl = ctrl;
		m_keyboard.alt = alt;
		m_keyboard.shift = shift;
	}
}
