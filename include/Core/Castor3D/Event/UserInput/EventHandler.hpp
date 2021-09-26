/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EventHandler_H___
#define ___C3D_EventHandler_H___

#include "Castor3D/Event/UserInput/KeyboardEvent.hpp"
#include "Castor3D/Event/UserInput/MouseEvent.hpp"
#include "Castor3D/Event/UserInput/HandlerEvent.hpp"

#include <CastorUtils/Design/Signal.hpp>

#include <deque>

#pragma warning( push )
#pragma warning( disable:4365 )
#include <mutex>
#pragma warning( pop )

namespace castor3d
{
	class EventHandler
	{
	protected:
		using EventHandlerFunction = std::function< void() >;
		using EventQueue = std::deque< std::pair< UserInputEventSPtr, EventHandlerFunction > >;

	public:
		//!\~english	Catcher definition for mouse events.
		//!\~french		Définition d'une fonction de traitement d'évènement de souris.
		using ClientMouseFunction = std::function< void( MouseEvent const & ) >;
		//!\~english	Catcher definition for keyboard events.
		//!\~french		Définition d'une fonction de traitement d'évènement de clavier.
		using ClientKeyboardFunction = std::function< void( KeyboardEvent const & ) >;
		//!\~english	Catcher definition for handler events.
		//!\~french		Définition d'une fonction de traitement d'évènement de gestionnaire.
		using ClientHandlerFunction = std::function< void( HandlerEvent const & ) >;
		//!\~english	Mouse event signal definition.
		//!\~french		Définition d'un signal d'évènement de souris.
		using OnClientMouseEvent = castor::Signal< ClientMouseFunction >;
		//!\~english	Keyboard event signal definition.
		//!\~french		Définition d'un signal d'évènement de clavier.
		using OnClientKeyboardEvent = castor::Signal< ClientKeyboardFunction >;
		//!\~english	Handler event signal definition.
		//!\~french		Définition d'un signal d'évènement de gestionnaire.
		using OnClientHandlerEvent = castor::Signal< ClientHandlerFunction >;
		//!\~english	Mouse event signal connection definition.
		//!\~french		Définition d'une connexion au signal d'évènement de souris.
		using OnClientMouseEventConnection = OnClientMouseEvent::connection;
		//!\~english	Keyboard event signal connection definition.
		//!\~french		Définition d'une connexion au signal d'évènement de clavier.
		using OnClientKeyboardEventConnection = OnClientKeyboardEvent::connection;
		//!\~english	Handler event signal connection definition.
		//!\~french		Définition d'une connexion au signal d'évènement de gestionnaire.
		using OnClientHandlerEventConnection = OnClientHandlerEvent::connection;

		/**@name General */
		//@{

		/**
		*\~english
		*\~brief		Constructor.
		 *\param[in]	catchMouseEvents	Defines if the event handler catches mouse event.
		*\~french
		*\~brief		Constructeur.
		 *\param[in]	catchMouseEvents	Dit si le gestionnaire d'évènements ràcupàre les évènements souris.
		 */
		explicit EventHandler( bool catchMouseEvents )
			: m_enabled{ true }
			, m_catchMouseEvents{ catchMouseEvents }
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
			doSwapQueue();
		}
		/**
		 *\~english
		 *\~brief		Processes all queued events.
		 *\~french
		 *\~brief		Traite tous les évènement dans la file.
		 */
		void processEvents()
		{
			EventQueue queue = doSwapQueue();

			for ( auto const & eventPair : queue )
			{
				eventPair.second();
			}
		}

		//@}
		/**@name Mouse events */
		//@{

		/**
		 *\~english
		 *\~brief		adds a mouse event to the events queue.
		 *\param[in]	event	The mouse event.
		 *\~french
		 *\~brief		Ajoute un évènement de souris à la file.
		 *\param[in]	event	L'évènement.
		 */
		void pushEvent( MouseEvent const & event )
		{
			auto mouseEvent = std::make_shared< MouseEvent >( event );
			using LockType = std::unique_lock< std::mutex >;
			LockType lock{ castor::makeUniqueLock( m_mutex ) };
			m_queue.emplace_back( mouseEvent
				, [this, mouseEvent]()
				{
					processMouseEvent( mouseEvent );
				} );
		}
		/**
		 *\~english
		 *\~brief		Connects a function to a mouse event.
		 *\param[in]	event		The event type.
		 *\param[in]	function	The function.
		 *\~french
		 *\~brief		Connecte un fonction à un évènement souris.
		 *\param[in]	event		Le type d'évènement.
		 *\param[in]	function	La fonction.
		 */
		void connect( MouseEventType event
			, ClientMouseFunction function )
		{
			m_mouseSlotsConnections[size_t( event )].push_back( m_mouseSlots[size_t( event )].connect( function ) );
		}

		//@}
		/**@name Keyboard events */
		//@{

		/**
		 *\~english
		 *\~brief		adds a keyboard event to the events queue.
		 *\param[in]	event	The mouse event.
		 *\~french
		 *\~brief		Ajoute un évènement de clavier à la file.
		 *\param[in]	event	L'évènement.
		 */
		void pushEvent( KeyboardEvent const & event )
		{
			auto mouseEvent = std::make_shared< KeyboardEvent >( event );
			using LockType = std::unique_lock< std::mutex >;
			LockType lock{ castor::makeUniqueLock( m_mutex ) };
			m_queue.emplace_back( mouseEvent
				, [this, mouseEvent]()
				{
					processKeyboardEvent( mouseEvent );
				} );
		}
		/**
		 *\~english
		 *\~brief		Connects a function to a keyboard event.
		 *\param[in]	event		The event type.
		 *\param[in]	function	The function.
		 *\~french
		 *\~brief		Connecte un fonction à un évènement clavier.
		 *\param[in]	event		Le type d'évènement.
		 *\param[in]	function	La fonction.
		 */
		void connect( KeyboardEventType event
			, ClientKeyboardFunction function )
		{
			m_keyboardSlotsConnections[size_t( event )].push_back( m_keyboardSlots[size_t( event )].connect( function ) );
		}
		/**
		 *\~english
		 *\~brief		Tells if the control catches mouse events.
		 *\remarks		A control catches mouse events when it is enabled, and when it explicitly catches it (enabled by default, except for static controls).
		 *\return		false if the mouse events don't affect the control.
		 */
		bool catchesMouseEvents()const
		{
			return m_enabled && m_catchMouseEvents && doCatchesMouseEvents();
		}
		/**
		 *\~english
		 *\~brief		Sets if the control can catch mouse events.
		 *\param[in]	value		The new value.
		 */
		void setCatchesMouseEvents( bool value )
		{
			m_catchMouseEvents = value;
		}

		//@}
		/**@name Keyboard events */
		//@{

		/**
		 *\~english
		 *\~brief		adds a handler event to the events queue.
		 *\param[in]	event	The mouse event.
		 *\~french
		 *\~brief		Ajoute un évènement de gestionnaire à la file.
		 *\param[in]	event	L'évènement.
		 */
		void pushEvent( HandlerEvent const & event )
		{
			auto handlerEvent = std::make_shared< HandlerEvent >( event );
			using LockType = std::unique_lock< std::mutex >;
			LockType lock{ castor::makeUniqueLock( m_mutex ) };
			m_queue.emplace_back( handlerEvent
				, [this, handlerEvent]()
				{
					processHandlerEvent( handlerEvent );
				} );
		}
		/**
		 *\~english
		 *\~brief		Connects a function to a handler event.
		 *\param[in]	event		The event type.
		 *\param[in]	function	The function.
		 *\~french
		 *\~brief		Connecte un fonction à un évènement gestionnaire.
		 *\param[in]	event		Le type d'évènement.
		 *\param[in]	function	La fonction.
		 */
		void connect( HandlerEventType event, ClientHandlerFunction function )
		{
			m_handlerSlotsConnections[size_t( event )].push_back( m_handlerSlots[size_t( event )].connect( function ) );
		}
		/**
		 *\~english
		 *\~brief		Tells if the control catches 'tab' key.
		 *\remarks		A control catches 'tab' key when it is enabled, and when it explicitly catches it (disabled by default).
		 *\return		false if the 'tab' key doesn't affect the control.
		 */
		bool catchesTabKey()const
		{
			return m_enabled && m_catchTabKey && doCatchesTabKey();
		}
		/**
		 *\~english
		 *\~brief		Tells if the control catches 'return' key.
		 *\remarks		A control catches 'return' key when it is enabled, and when it explicitly catches it (disabled by default).
		 *\return		false if the 'return' key doesn't affect the control.
		 */
		bool catchesReturnKey()const
		{
			return m_enabled && m_catchReturnKey && doCatchesReturnKey();
		}

		//@}

	protected:
		/**
		 *\~english
		 *\~brief		Mouse event processing function.
		 *\param[in]	event	The event.
		 *\~french
		 *\~brief		Fonction de traitement d'un évènement souris.
		 *\param[in]	event	L'évènement.
		 */
		void processMouseEvent( MouseEventSPtr event )
		{
			m_mouseSlots[size_t( event->getMouseEventType() )]( *event );
			doProcessMouseEvent( event );
		}
		/**
		 *\~english
		 *\~brief		Keyboard event processing function.
		 *\param[in]	event	The event.
		 *\~french
		 *\~brief		Fonction de traitement d'un évènement clavier.
		 *\param[in]	event	L'évènement.
		 */
		void processKeyboardEvent( KeyboardEventSPtr event )
		{
			m_keyboardSlots[size_t( event->getKeyboardEventType() )]( *event );
			doProcessKeyboardEvent( event );
		}
		/**
		 *\~english
		 *\~brief		Handler event processing function.
		 *\param[in]	event	The event.
		 *\~french
		 *\~brief		Fonction de traitement d'un évènement de gestionnaire.
		 *\param[in]	event	L'évènement.
		 */
		void processHandlerEvent( HandlerEventSPtr event )
		{
			m_handlerSlots[size_t( event->getHandlerEventType() )]( *event );
			doProcessHandlerEvent( event );
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
		EventQueue doSwapQueue()
		{
			EventQueue queue;
			m_mutex.lock();
			std::swap( queue, m_queue );
			m_mutex.unlock();
			return queue;
		}
		/**
		 *\~english
		 *\~brief		Mouse event processing function.
		 *\param[in]	event	The event.
		 *\~french
		 *\~brief		Fonction de traitement d'un évènement souris.
		 *\param[in]	event	L'évènement.
		 */
		C3D_API virtual void doProcessMouseEvent( MouseEventSPtr event ) = 0;
		/**
		 *\~english
		 *\~brief		Keyboard event processing function.
		 *\param[in]	event	The event.
		 *\~french
		 *\~brief		Fonction de traitement d'un évènement clavier.
		 *\param[in]	event	L'évènement.
		 */
		C3D_API virtual void doProcessKeyboardEvent( KeyboardEventSPtr event ) = 0;
		/**
		 *\~english
		 *\~brief		Handler event processing function.
		 *\param[in]	event	The event.
		 *\~french
		 *\~brief		Fonction de traitement d'un évènement gestionnaire.
		 *\param[in]	event	L'évènement.
		 */
		C3D_API virtual void doProcessHandlerEvent( HandlerEventSPtr event ) = 0;
		/**
		 *\~english
		 *\~brief		Tells if the control catches mouse events.
		 *\remarks		A control catches mouse events when it is  enabled, and when it explicitly catches it (enables by default, except for static controls).
		 *\return		false if the mouse events don't affect the control.
		 */
		C3D_API virtual bool doCatchesMouseEvents()const = 0;
		/**
		 *\~english
		 *\~brief		Tells if the control catches 'tab' key.
		 *\remarks		A control catches 'tab' key when it is enabled, and when it explicitly catches it (disabled by default).
		 *\return		false if the 'tab' key doesn't affect the control.
		 */
		C3D_API virtual bool doCatchesTabKey()const = 0;
		/**
		 *\~english
		 *\~brief		Tells if the control catches 'return' key.
		 *\remarks		A control catches 'return' key when it is enabled, and when it explicitly catches it (disabled by default).
		 *\return		false if the 'return' key doesn't affect the control.
		 */
		C3D_API virtual bool doCatchesReturnKey()const = 0;

	protected:
		//!\~english	The mouse events slots.
		//!\~french		Les slots d'évènements souris.
		std::array< OnClientMouseEvent, size_t( MouseEventType::eCount ) > m_mouseSlots;
		//!\~english	The mouse events slots connections.
		//!\~french		Les connexions aux slots d'évènements souris.
		std::array< std::vector< OnClientMouseEventConnection >, size_t( MouseEventType::eCount ) > m_mouseSlotsConnections;
		//!\~english	The keyboard events slots.
		//!\~french		Les slots d'évènements clavier.
		std::array< OnClientKeyboardEvent, size_t( KeyboardEventType::eCount ) > m_keyboardSlots;
		//!\~english	The handler events slots connections.
		//!\~french		Les connexions aux slots d'évènements de gestionnaire.
		std::array< std::vector< OnClientKeyboardEventConnection >, size_t( KeyboardEventType::eCount ) > m_keyboardSlotsConnections;
		//!\~english	The keyboard events slots.
		//!\~french		Les slots d'évènements clavier.
		std::array< OnClientHandlerEvent, size_t( HandlerEventType::eCount ) > m_handlerSlots;
		//!\~english	The handler events slots connections.
		//!\~french		Les connexions aux slots d'évènements de gestionnaire.
		std::array< std::vector< OnClientHandlerEventConnection >, size_t( HandlerEventType::eCount ) > m_handlerSlotsConnections;

	private:
		//!\~english	Activation status.
		//!\~french		Le statut d'activation.
		bool m_enabled;
		//!\~english	The mutex used to protect the events queue.
		//!\~french		Le mutex utilisà pour protàger la file d'évènements.
		std::mutex m_mutex;
		//!\~english	The events queue.
		//!\~french		La file d'évènements.
		EventQueue m_queue;
		//!\~english	Tells if the control catches mouse events.
		//!\~french		Dit si le contrôle traite les évènements souris.
		bool m_catchMouseEvents;
		//!\~english	Tells if the control catches 'tab' key.
		//!\~french		Dit si le contrôle traite la touche 'tab'.
		bool m_catchTabKey;
		//!\~english	Tells if the control catches 'return' key.
		//!\~french		Dit si le contrôle traite la touche 'entrée'.
		bool m_catchReturnKey;
	};
	/**
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
		CU_DeclareSmartPtr( Derived );
		using ClientMouseFunction = EventHandler::ClientMouseFunction;
		using ClientKeyboardFunction = EventHandler::ClientKeyboardFunction;
		using ClientHandlerFunction = EventHandler::ClientHandlerFunction;
		//!\~english	Catcher definition for non client mouse events.
		//!\~french		Définition d'une fonction de traitement d'évènement non client de souris.
		using NonClientMouseFunction = std::function< void( DerivedSPtr, MouseEvent const & ) >;
		//!\~english	Catcher definition for non client keyboard events.
		//!\~french		Définition d'une fonction de traitement d'évènement non client de clavier.
		using NonClientKeyboardFunction = std::function< void( DerivedSPtr, KeyboardEvent const & ) >;
		//!\~english	Catcher definition for non client handler events.
		//!\~french		Définition d'une fonction de traitement d'évènement non client de gestionnaire.
		using NonClientHandlerFunction = std::function< void( DerivedSPtr, HandlerEvent const & ) >;
		//!\~english	Non client mouse event signal definition.
		//!\~french		Définition d'un signal d'évènement non client de souris.
		using OnNonClientMouseEvent = castor::Signal< NonClientMouseFunction >;
		//!\~english	Non client keyboard event signal definition.
		//!\~french		Définition d'un signal d'évènement non client de clavier.
		using OnNonClientKeyboardEvent = castor::Signal< NonClientKeyboardFunction >;
		//!\~english	Non client handler event signal definition.
		//!\~french		Définition d'un signal d'évènement non client de gestionnaire.
		using OnNonClientHandlerEvent = castor::Signal< NonClientHandlerFunction >;
		//!\~english	Non client mouse event signal connection definition.
		//!\~french		Définition d'une connexion au signal d'évènement non client de souris.
		using OnNonClientMouseEventConnection = typename OnNonClientMouseEvent::connection;
		//!\~english	Non client keyboard event signal connection definition.
		//!\~french		Définition d'une connexion au signal d'évènement non client de clavier.
		using OnNonClientKeyboardEventConnection = typename OnNonClientKeyboardEvent::connection;
		//!\~english	Non client handler event signal connection definition.
		//!\~french		Définition d'une connexion au signal d'évènement non client de gestionnaire.
		using OnNonClientHandlerEventConnection = typename OnNonClientHandlerEvent::connection;

	public:
		/**
		*\~english
		*\~brief		Constructor.
		 *\param[in]	catchMouseEvents	Defines if the event handler catches mouse event.
		*\~french
		*\~brief		Constructeur.
		 *\param[in]	catchMouseEvents	Dit si le gestionnaire d'évènements ràcupàre les évènements souris.
		 */
		explicit NonClientEventHandler( bool catchMouseEvents )
			: EventHandler{ catchMouseEvents }
		{
		}
		/**
		 *\~english
		 *\~brief		Connects a function to a non client mouse event.
		 *\param[in]	event		The event type.
		 *\param[in]	function	The function.
		 *\~french
		 *\~brief		Connecte un fonction à un évènement souris non client.
		 *\param[in]	event		Le type d'évènement.
		 *\param[in]	function	La fonction.
		 */
		void connectNC( MouseEventType event
			, NonClientMouseFunction function )
		{
			m_ncMouseSlotsConnections[size_t( event )].push_back( m_ncMouseSlots[size_t( event )].connect( function ) );
		}
		/**
		 *\~english
		 *\~brief		Connects a function to a non client keyboard event.
		 *\param[in]	event		The event type.
		 *\param[in]	function	The function.
		 *\~french
		 *\~brief		Connecte un fonction à un évènement clavier non client.
		 *\param[in]	event		Le type d'évènement.
		 *\param[in]	function	La fonction.
		 */
		void connectNC( KeyboardEventType event
			, NonClientKeyboardFunction function )
		{
			m_ncKeyboardSlotsConnections[size_t( event )].push_back( m_ncKeyboardSlots[size_t( event )].connect( function ) );
		}
		/**
		 *\~english
		 *\~brief		Connects a function to a non client handler event.
		 *\param[in]	event		The event type.
		 *\param[in]	function	The function.
		 *\~french
		 *\~brief		Connecte un fonction à un évènement gestionnaire non client.
		 *\param[in]	event		Le type d'évènement.
		 *\param[in]	function	La fonction.
		 */
		void connectNC( HandlerEventType event
			, NonClientHandlerFunction function )
		{
			m_ncHandlerSlotsConnections[size_t( event )].push_back( m_ncHandlerSlots[size_t( event )].connect( function ) );
		}

		//@}

	private:
		void doProcessMouseEvent( MouseEventSPtr event )override
		{
			m_ncMouseSlots[size_t( event->getMouseEventType() )]( this->shared_from_this(), *event );
		}

		void doProcessKeyboardEvent( KeyboardEventSPtr event )override
		{
			m_ncKeyboardSlots[size_t( event->getKeyboardEventType() )]( this->shared_from_this(), *event );
		}

		void doProcessHandlerEvent( HandlerEventSPtr event )override
		{
			m_ncHandlerSlots[size_t( event->getHandlerEventType() )]( this->shared_from_this(), *event );
		}

	protected:
		//!\~english	The non client mouse events slots.
		//!\~french		Les slots d'évènements souris non clients.
		std::array< OnNonClientMouseEvent, size_t( MouseEventType::eCount ) > m_ncMouseSlots;
		//!\~english	The non client mouse events slots.
		//!\~french		Les slots d'évènements souris non clients.
		std::array< std::vector< OnNonClientMouseEventConnection >, size_t( MouseEventType::eCount ) > m_ncMouseSlotsConnections;
		//!\~english	The non client keyboard events slots.
		//!\~french		Les slots d'évènements clavier non clients.
		std::array< OnNonClientKeyboardEvent, size_t( KeyboardEventType::eCount ) > m_ncKeyboardSlots;
		//!\~english	The non client keyboard events slots.
		//!\~french		Les slots d'évènements clavier non clients.
		std::array< std::vector< OnNonClientKeyboardEventConnection >, size_t( KeyboardEventType::eCount ) > m_ncKeyboardSlotsConnections;
		//!\~english	The non client handler events slots.
		//!\~french		Les slots d'évènements de gestionnaire non clients.
		std::array< OnNonClientHandlerEvent, size_t( HandlerEventType::eCount ) > m_ncHandlerSlots;
		//!\~english	The non client handler events slots.
		//!\~french		Les slots d'évènements de gestionnaire non clients.
		std::array< std::vector< OnNonClientHandlerEventConnection >, size_t( HandlerEventType::eCount ) > m_ncHandlerSlotsConnections;
	};

	class MouseEventHandler
		: public EventHandler
	{
	public:
		MouseEventHandler()
			: EventHandler{ true }
		{
		}

	private:
		C3D_API void doProcessKeyboardEvent( KeyboardEventSPtr event )final override
		{
		}

		C3D_API void doProcessHandlerEvent( HandlerEventSPtr event )final override
		{
		}

		C3D_API bool doCatchesTabKey()const final override
		{
			return false;
		}

		C3D_API bool doCatchesReturnKey()const final override
		{
			return false;
		}

		C3D_API bool doCatchesMouseEvents()const final override
		{
			return true;
		}
	};
}

#endif
