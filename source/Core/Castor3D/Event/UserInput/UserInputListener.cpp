#include "Castor3D/Event/UserInput/UserInputListener.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"

#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

using namespace castor;

namespace castor3d
{
	UserInputListener::UserInputListener( Engine & engine, String const & name )
		: OwnedBy< Engine >{ engine }
		, m_frameListener{ engine.getFrameListenerCache().add( name ) }
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

		using LockType = std::unique_lock< std::mutex >;
		LockType lock{ castor::makeUniqueLock( m_mutexHandlers ) };
		auto it = m_handlers.begin();

		while ( it != m_handlers.end() )
		{
			auto handler = *it;
			doRemoveHandler( *handler );
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

	void UserInputListener::unregisterMouseMoveAction( String const & handler )
	{
		auto it = m_onMouseMoveActions.find( handler );

		if ( it != m_onMouseMoveActions.end() )
		{
			m_onMouseMoveActions.erase( it );
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

	void UserInputListener::onMouseMoveAction( String const & handler )
	{
		auto it = m_onMouseMoveActions.find( handler );

		if ( it != m_onMouseMoveActions.end() )
		{
			it->second( getMousePosition() );
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
			m_mouse.position = position;
			auto current = doGetMouseTargetableHandler( position );
			auto last = m_lastMouseTarget;

			if ( current != last )
			{
				if ( last )
				{
					log::debug << position.x() << "x" << position.y() << std::endl;
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

	bool UserInputListener::fireMouseButtonPushed( MouseButton button )
	{
		bool result = false;

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

	bool UserInputListener::fireMouseButtonReleased( MouseButton button )
	{
		bool result = false;

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

	bool UserInputListener::fireMouseWheel( Position const & offsets )
	{
		bool result = false;

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

		if ( doHasHandlers() )
		{
			auto active = m_activeHandler;

			if ( active )
			{
				if ( key == KeyboardKey::eControl )
				{
					m_keyboard.ctrl = true;
				}

				if ( key == KeyboardKey::eAlt )
				{
					m_keyboard.alt = true;
				}

				if ( key == KeyboardKey::eShift )
				{
					m_keyboard.shift = true;
				}

				active->pushEvent( KeyboardEvent( KeyboardEventType::ePushed, key, m_keyboard.ctrl, m_keyboard.alt, m_keyboard.shift ) );
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
			auto active = m_activeHandler;

			if ( active )
			{
				if ( key == KeyboardKey::eControl )
				{
					m_keyboard.ctrl = false;
				}

				if ( key == KeyboardKey::eAlt )
				{
					m_keyboard.alt = false;
				}

				if ( key == KeyboardKey::eShift )
				{
					m_keyboard.shift = false;
				}

				active->pushEvent( KeyboardEvent( KeyboardEventType::eReleased, key, m_keyboard.ctrl, m_keyboard.alt, m_keyboard.shift ) );
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
			auto active = m_activeHandler;

			if ( active )
			{
				active->pushEvent( KeyboardEvent( KeyboardEventType::eChar, key, value, m_keyboard.ctrl, m_keyboard.alt, m_keyboard.shift ) );
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
					this->m_frameListener->postEvent( makeCpuFunctorEvent( EventType::ePostRender
					//??? this->m_frameListener->postEvent( makeCpuFunctorEvent( EventType::ePreRender
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
