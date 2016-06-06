/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (At your option ) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_EVENT_HANDLER_H___
#define ___C3D_EVENT_HANDLER_H___

#include <Signal.hpp>

#include "KeyboardEvent.hpp"
#include "MouseEvent.hpp"
#include "HandlerEvent.hpp"

#include <deque>
#include <mutex>

namespace Castor3D
{
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Description of an event handler, class that can receive event.
	*\~french
	*\brief		Description d'un gestionnaire d'évènements, classe pouvant recevoir des évènements.
	*/
	class EventHandler
	{
	protected:
		using EventHandlerFunction = std::function< void() >;
		using EventQueue = std::deque< std::pair< UserInputEventSPtr, EventHandlerFunction > >;

	public:
		//!\~english Catcher definition for mouse events.	\~french Définition d'une fonction de traitement d'évènement de souris.
		using ClientMouseFunction = std::function< void( MouseEvent const & ) >;
		//!\~english Catcher definition for keyboard events.	\~french Définition d'une fonction de traitement d'évènement de clavier.
		using ClientKeyboardFunction = std::function< void( KeyboardEvent const & ) >;
		//!\~english Catcher definition for handler events.	\~french Définition d'une fonction de traitement d'évènement de gestionnaire.
		using ClientHandlerFunction = std::function< void( HandlerEvent const & ) >;

		/**@name General */
		//@{

		/**
		*\~english
		*\~brief		Constructor.
		 *\param[in]	p_catchMouseEvents	Defines if the event handler catches mouse event.
		*\~french
		*\~brief		Constructeur.
		 *\param[in]	p_catchMouseEvents	Dit si le gestionnaire d'évènements récupère les évènements souris.
		 */
		explicit EventHandler( bool p_catchMouseEvents )
			: m_enabled{ true }
			, m_catchMouseEvents{ p_catchMouseEvents }
			, m_catchTabKey{ false }
			, m_catchReturnKey{ false }
		{
		}
		/**
		 *\~english
		 *\~brief		Destructor.
		 *\~french
		 *\~brief		Destructeur.
		 */
		virtual ~EventHandler()
		{
			DoSwapQueue();
		}
		/**
		 *\~english
		 *\~brief		Processes all queued events.
		 *\~french
		 *\~brief		Traite tous les évènement dans la file.
		 */
		void ProcessEvents()
		{
			EventQueue l_queue = DoSwapQueue();

			for ( auto && l_eventPair : l_queue )
			{
				l_eventPair.second();
			}
		}

		//@}
		/**@name Mouse events */
		//@{

		/**
		 *\~english
		 *\~brief		Adds a mouse event to the events queue.
		 *\param[in]	p_event	The mouse event.
		 *\~french
		 *\~brief		Ajoute un évènement de souris à la file.
		 *\param[in]	p_event	L'évènement.
		 */
		void PushEvent( MouseEvent const & p_event )
		{
			auto l_event = std::make_shared< MouseEvent >( p_event );
			auto l_lock = Castor::make_unique_lock( m_mutex );
			m_queue.push_back( { l_event, std::bind( &EventHandler::ProcessMouseEvent, this, l_event ) } );
		}
		/**
		 *\~english
		 *\~brief		Connects a function to a mouse event.
		 *\param[in]	p_event		The event type.
		 *\param[in]	p_function	The function.
		 *\~french
		 *\~brief		Connecte un fonction à un évènement souris.
		 *\param[in]	p_event		Le type d'évènement.
		 *\param[in]	p_function	La fonction.
		 */
		void Connect( eMOUSE_EVENT p_event, ClientMouseFunction p_function )
		{
			m_mouseSlots[p_event].connect( p_function );
		}

		//@}
		/**@name Keyboard events */
		//@{

		/**
		 *\~english
		 *\~brief		Adds a keyboard event to the events queue.
		 *\param[in]	p_event	The mouse event.
		 *\~french
		 *\~brief		Ajoute un évènement de clavier à la file.
		 *\param[in]	p_event	L'évènement.
		 */
		void PushEvent( KeyboardEvent const & p_event )
		{
			auto l_event = std::make_shared< KeyboardEvent >( p_event );
			auto l_lock = Castor::make_unique_lock( m_mutex );
			m_queue.push_back( { l_event, std::bind( &EventHandler::ProcessKeyboardEvent, this, l_event ) } );
		}
		/**
		 *\~english
		 *\~brief		Connects a function to a keyboard event.
		 *\param[in]	p_event		The event type.
		 *\param[in]	p_function	The function.
		 *\~french
		 *\~brief		Connecte un fonction à un évènement clavier.
		 *\param[in]	p_event		Le type d'évènement.
		 *\param[in]	p_function	La fonction.
		 */
		void Connect( eKEYBOARD_EVENT p_event, ClientKeyboardFunction p_function )
		{
			m_keyboardSlots[p_event].connect( p_function );
		}
		/**
		 *\~english
		 *\~brief		Tells if the control catches mouse events.
		 *\remarks		A control catches mouse events when it is enabled, and when it explicitly catches it (enabled by default, except for static controls).
		 *\return		false if the mouse events don't affect the control.
		 */
		bool CatchesMouseEvents()const
		{
			return m_enabled && m_catchMouseEvents && DoCatchesMouseEvents();
		}
		/**
		 *\~english
		 *\~brief		Sets if the control can catch mouse events.
		 *\param[in]	p_value		The new value.
		 */
		void SetCatchesMouseEvents( bool p_value )
		{
			m_catchMouseEvents = p_value;
		}

		//@}
		/**@name Keyboard events */
		//@{

		/**
		 *\~english
		 *\~brief		Adds a handler event to the events queue.
		 *\param[in]	p_event	The mouse event.
		 *\~french
		 *\~brief		Ajoute un évènement de gestionnaire à la file.
		 *\param[in]	p_event	L'évènement.
		 */
		void PushEvent( HandlerEvent const & p_event )
		{
			auto l_event = std::make_shared< HandlerEvent >( p_event );
			auto l_lock = Castor::make_unique_lock( m_mutex );
			m_queue.push_back( { l_event, std::bind( &EventHandler::ProcessHandlerEvent, this, l_event ) } );
		}
		/**
		 *\~english
		 *\~brief		Connects a function to a handler event.
		 *\param[in]	p_event		The event type.
		 *\param[in]	p_function	The function.
		 *\~french
		 *\~brief		Connecte un fonction à un évènement gestionnaire.
		 *\param[in]	p_event		Le type d'évènement.
		 *\param[in]	p_function	La fonction.
		 */
		void Connect( eHANDLER_EVENT p_event, ClientHandlerFunction p_function )
		{
			m_handlerSlots[p_event].connect( p_function );
		}
		/**
		 *\~english
		 *\~brief		Tells if the control catches 'tab' key.
		 *\remarks		A control catches 'tab' key when it is enabled, and when it explicitly catches it (disabled by default).
		 *\return		false if the 'tab' key doesn't affect the control.
		 */
		bool CatchesTabKey()const
		{
			return m_enabled && m_catchTabKey && DoCatchesTabKey();
		}
		/**
		 *\~english
		 *\~brief		Tells if the control catches 'return' key.
		 *\remarks		A control catches 'return' key when it is enabled, and when it explicitly catches it (disabled by default).
		 *\return		false if the 'return' key doesn't affect the control.
		 */
		bool CatchesReturnKey()const
		{
			return m_enabled && m_catchReturnKey && DoCatchesReturnKey();
		}

		//@}

	protected:
		/**
		 *\~english
		 *\~brief		Mouse event processing function.
		 *\param[in]	p_event	The event.
		 *\~french
		 *\~brief		Fonction de traitement d'un évènement souris.
		 *\param[in]	p_event	L'évènement.
		 */
		inline void ProcessMouseEvent( MouseEventSPtr p_event )
		{
			m_mouseSlots[p_event->GetMouseEventType()]( *p_event );
			DoProcessMouseEvent( p_event );
		}
		/**
		 *\~english
		 *\~brief		Keyboard event processing function.
		 *\param[in]	p_event	The event.
		 *\~french
		 *\~brief		Fonction de traitement d'un évènement clavier.
		 *\param[in]	p_event	L'évènement.
		 */
		inline void ProcessKeyboardEvent( KeyboardEventSPtr p_event )
		{
			m_keyboardSlots[p_event->GetKeyboardEventType()]( *p_event );
			DoProcessKeyboardEvent( p_event );
		}
		/**
		 *\~english
		 *\~brief		Handler event processing function.
		 *\param[in]	p_event	The event.
		 *\~french
		 *\~brief		Fonction de traitement d'un évènement de gestionnaire.
		 *\param[in]	p_event	L'évènement.
		 */
		inline void ProcessHandlerEvent( HandlerEventSPtr p_event )
		{
			m_handlerSlots[p_event->GetHandlerEventType()]( *p_event );
			DoProcessHandlerEvent( p_event );
		}

	private:
		/**
		*\~english
		*\~brief		Swaps this handler's queue content with an empty queue's one.
		*\return		The content of this handler's queue.
		*\~french
		*\~brief		Echange le contenu de la file de ce gestionnaire avec une file vide.
		*\return		Le contenu de la file de ce gestionnaire.
		*/
		inline EventQueue DoSwapQueue()
		{
			EventQueue l_queue;
			m_mutex.lock();
			std::swap( l_queue, m_queue );
			m_mutex.unlock();
			return l_queue;
		}
		/**
		 *\~english
		 *\~brief		Mouse event processing function.
		 *\param[in]	p_event	The event.
		 *\~french
		 *\~brief		Fonction de traitement d'un évènement souris.
		 *\param[in]	p_event	L'évènement.
		 */
		C3D_API virtual void DoProcessMouseEvent( MouseEventSPtr p_event ) = 0;
		/**
		 *\~english
		 *\~brief		Keyboard event processing function.
		 *\param[in]	p_event	The event.
		 *\~french
		 *\~brief		Fonction de traitement d'un évènement clavier.
		 *\param[in]	p_event	L'évènement.
		 */
		C3D_API virtual void DoProcessKeyboardEvent( KeyboardEventSPtr p_event ) = 0;
		/**
		 *\~english
		 *\~brief		Handler event processing function.
		 *\param[in]	p_event	The event.
		 *\~french
		 *\~brief		Fonction de traitement d'un évènement gestionnaire.
		 *\param[in]	p_event	L'évènement.
		 */
		C3D_API virtual void DoProcessHandlerEvent( HandlerEventSPtr p_event ) = 0;
		/**
		 *\~english
		 *\~brief		Tells if the control catches mouse events.
		 *\remarks		A control catches mouse events when it is  enabled, and when it explicitly catches it (enables by default, except for static controls).
		 *\return		false if the mouse events don't affect the control.
		 */
		C3D_API virtual bool DoCatchesMouseEvents()const = 0;
		/**
		 *\~english
		 *\~brief		Tells if the control catches 'tab' key.
		 *\remarks		A control catches 'tab' key when it is enabled, and when it explicitly catches it (disabled by default).
		 *\return		false if the 'tab' key doesn't affect the control.
		 */
		C3D_API virtual bool DoCatchesTabKey()const = 0;
		/**
		 *\~english
		 *\~brief		Tells if the control catches 'return' key.
		 *\remarks		A control catches 'return' key when it is enabled, and when it explicitly catches it (disabled by default).
		 *\return		false if the 'return' key doesn't affect the control.
		 */
		C3D_API virtual bool DoCatchesReturnKey()const = 0;

	protected:
		//!\~english The mouse events slots.	\~french Les slots d'évènements souris.
		Castor::Signal< ClientMouseFunction > m_mouseSlots[eMOUSE_EVENT_COUNT];
		//!\~english The keyboard events slots.	\~french Les slots d'évènements clavier.
		Castor::Signal< ClientKeyboardFunction > m_keyboardSlots[eKEYBOARD_EVENT_COUNT];
		//!\~english The handler events slots.	\~french Les slots d'évènements de gestionnaire.
		Castor::Signal< ClientHandlerFunction > m_handlerSlots[eHANDLER_EVENT_COUNT];

	private:
		//!\~english Activation status.	\~french Le statu d'activation
		bool m_enabled;
		//!\~english The mutex used to protect the events queue.	\~french Le mutex utilisé pour protéger la file d'évènements.
		std::mutex m_mutex;
		//!\~english The events queue.	\~french La file d'évènements.
		EventQueue m_queue;
		//!\~english Tells if the control catches mouse events.
		bool m_catchMouseEvents;
		//!\~english Tells if the control catches 'tab' key.
		bool m_catchTabKey;
		//!\~english Tells if the control catches 'return' key.
		bool m_catchReturnKey;
	};
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Description of an event handler, class that can receive event.
	*\~french
	*\brief		Description d'un gestionnaire d'évènements, classe pouvant recevoir des évènements.
	*/
	template< class Derived >
	class NonClientEventHandler
		: public std::enable_shared_from_this< Derived >
		, public EventHandler
	{
	protected:
		using EventHandlerFunction = EventHandler::EventHandlerFunction;
		using EventQueue = EventHandler::EventQueue;

	public:
		DECLARE_SMART_PTR( Derived );
		using ClientMouseFunction = EventHandler::ClientMouseFunction;
		using ClientKeyboardFunction = EventHandler::ClientKeyboardFunction;
		using ClientHandlerFunction = EventHandler::ClientHandlerFunction;
		//!\~english Catcher definition for non client mouse events.	\~french Définition d'une fonction de traitement d'évènement non client de souris.
		using NonClientMouseFunction = std::function< void( DerivedSPtr, MouseEvent const & ) >;
		//!\~english Catcher definition for non client keyboard events.	\~french Définition d'une fonction de traitement d'évènement non client de clavier.
		using NonClientKeyboardFunction = std::function< void( DerivedSPtr, KeyboardEvent const & ) >;
		//!\~english Catcher definition for non client handler events.	\~french Définition d'une fonction de traitement d'évènement non client de gestionnaire.
		using NonClientHandlerFunction = std::function< void( DerivedSPtr, HandlerEvent const & ) >;

	public:
		/**
		*\~english
		*\~brief		Constructor.
		 *\param[in]	p_catchMouseEvents	Defines if the event handler catches mouse event.
		*\~french
		*\~brief		Constructeur.
		 *\param[in]	p_catchMouseEvents	Dit si le gestionnaire d'évènements récupère les évènements souris.
		 */
		explicit NonClientEventHandler( bool p_catchMouseEvents )
			: EventHandler{ p_catchMouseEvents }
		{
		}
		/**
		 *\~english
		 *\~brief		Connects a function to a non client mouse event.
		 *\param[in]	p_event		The event type.
		 *\param[in]	p_function	The function.
		 *\~french
		 *\~brief		Connecte un fonction à un évènement souris non client.
		 *\param[in]	p_event		Le type d'évènement.
		 *\param[in]	p_function	La fonction.
		 */
		void ConnectNC( eMOUSE_EVENT p_event, NonClientMouseFunction p_function )
		{
			m_ncMouseSlots[p_event].connect( p_function );
		}
		/**
		 *\~english
		 *\~brief		Connects a function to a non client keyboard event.
		 *\param[in]	p_event		The event type.
		 *\param[in]	p_function	The function.
		 *\~french
		 *\~brief		Connecte un fonction à un évènement clavier non client.
		 *\param[in]	p_event		Le type d'évènement.
		 *\param[in]	p_function	La fonction.
		 */
		void ConnectNC( eKEYBOARD_EVENT p_event, NonClientKeyboardFunction p_function )
		{
			m_ncKeyboardSlots[p_event].connect( p_function );
		}
		/**
		 *\~english
		 *\~brief		Connects a function to a non client handler event.
		 *\param[in]	p_event		The event type.
		 *\param[in]	p_function	The function.
		 *\~french
		 *\~brief		Connecte un fonction à un évènement gestionnaire non client.
		 *\param[in]	p_event		Le type d'évènement.
		 *\param[in]	p_function	La fonction.
		 */
		void ConnectNC( eHANDLER_EVENT p_event, NonClientHandlerFunction p_function )
		{
			m_ncKeyboardSlots[p_event].connect( p_function );
		}

		//@}

	private:
		/**
		 *\copydoc		Castor3D::EventHandler::DoProcessMouseEvent
		 */
		inline void DoProcessMouseEvent( MouseEventSPtr p_event )
		{
			m_ncMouseSlots[p_event->GetMouseEventType()]( this->shared_from_this(), *p_event );
		}
		/**
		 *\copydoc		Castor3D::EventHandler::DoProcessKeyboardEvent
		 */
		inline void DoProcessKeyboardEvent( KeyboardEventSPtr p_event )
		{
			m_ncKeyboardSlots[p_event->GetKeyboardEventType()]( this->shared_from_this(), *p_event );
		}
		/**
		 *\copydoc		Castor3D::EventHandler::DoProcessHandlerEvent
		 */
		inline void DoProcessHandlerEvent( HandlerEventSPtr p_event )
		{
			m_ncHandlerSlots[p_event->GetHandlerEventType()]( this->shared_from_this(), *p_event );
		}

	protected:
		//!\~english The non client mouse events slots.	\~french Les slots d'évènements souris non clients.
		Castor::Signal< NonClientMouseFunction > m_ncMouseSlots[eMOUSE_EVENT_COUNT];
		//!\~english The non client keyboard events slots.	\~french Les slots d'évènements clavier non clients.
		Castor::Signal< NonClientKeyboardFunction > m_ncKeyboardSlots[eKEYBOARD_EVENT_COUNT];
		//!\~english The non client handler events slots.	\~french Les slots d'évènements de gestionnaire non clients.
		Castor::Signal< NonClientHandlerFunction > m_ncHandlerSlots[eHANDLER_EVENT_COUNT];
	};
}

#endif
