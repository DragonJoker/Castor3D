/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UserInputListener_H___
#define ___C3D_UserInputListener_H___

#include "Castor3D/Event/UserInput/EventHandler.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"

namespace castor3d
{
	class UserInputListener
		: public castor::OwnedBy< Engine >
	{
	public:
		using OnClickActionFunction = std::function< void() >;
		using OnMouseMoveActionFunction = std::function< void( castor::Position const & ) >;
		using OnSelectActionFunction = std::function< void( int ) >;
		using OnTextActionFunction = std::function< void( castor::U32String const & ) >;
		using OnExpandActionFunction = std::function< void( bool ) >;
		using OnCursorActionFunction = std::function< void( MouseCursor ) >;

	public:
		/**@name General */
		//@{

		/**
		 *\~english
		 *\~brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	name	The listener's name.
		 *\~french
		 *\~brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	name	Le nom du listener.
		 */
		C3D_API UserInputListener( Engine & engine
			, castor::String const & name );
		/**
		 *\~english
		 *\~brief		Destructor.
		 *\~french
		 *\~brief		Destructeur.
		 */
		C3D_API virtual ~UserInputListener();
		/**
		 *\~english
		 *\~brief		Initialises the listener.
		 *\return		true, hopefully :P.
		 *\~french
		 *\~brief		Initialise le listener.
		 *\return		true, espérons :P.
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\~brief		Cleans up the listener.
		 *\~french
		 *\~brief		Nettoie le listener.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\~brief		Processes all queued events.
		 *\~french
		 *\~brief		Traite tous les évènements.
		 */
		C3D_API void processEvents();

		//@}
		/**@name Getters */
		//@{
		
		castor::Position const & getMousePosition()const
		{
			return m_mouse.position;
		}

		inline EventHandler * getActiveControl()const
		{
			return m_activeHandler;
		}

		inline EventHandler * getFocusedControl()const
		{
			return m_lastMouseTarget;
		}

		inline FrameListener & getFrameListener()const
		{
			return *m_frameListener;
		}

		//@}
		/**@name Output events */
		//@{
		
		/**
		 *\~english
		 *\~brief		Registers a function to call when a mouse move event is raised by the given handler.
		 *\param[in]	handler		The handler.
		 *\param[in]	function	The function.
		 *\~french
		 *\~brief		Enregistre une fonction à appeler lorsqu'un évènement de déplacement de souris est lancé par le handler donné.
		 *\param[in]	handler		Le handler.
		 *\param[in]	function	La fonction.
		 */
		C3D_API void registerMouseMoveAction( castor::String const & handler
			, OnMouseMoveActionFunction function );
		/**
		 *\~english
		 *\~brief		Registers a function to call when a mouse click event is raised by the given handler.
		 *\param[in]	handler		The clicked handler.
		 *\param[in]	function	The function.
		 *\~french
		 *\~brief		Enregistre une fonction à appeler lorsqu'un évènement de clic de souris est lancé par le handler donné.
		 *\param[in]	handler		Le handler cliqué.
		 *\param[in]	function	La fonction.
		 */
		C3D_API void registerClickAction( castor::String const & handler
			, OnClickActionFunction function );
		/**
		 *\~english
		 *\~brief		Registers a function to call when a select event is raised by the given handler.
		 *\param[in]	handler		The handler.
		 *\param[in]	function	The function.
		 *\~french
		 *\~brief		Enregistre une fonction à appeler lorsqu'un évènement de sélection est lancé par le handler donné.
		 *\param[in]	handler		Le handler.
		 *\param[in]	function	La fonction.
		 */
		C3D_API void registerSelectAction( castor::String const & handler
			, OnSelectActionFunction function );
		/**
		 *\~english
		 *\~brief		Registers a function to call when a text event is raised by the given handler.
		 *\param[in]	handler		The handler.
		 *\param[in]	function	The function.
		 *\~french
		 *\~brief		Enregistre une fonction à appeler lorsqu'un évènement de texte est lancé par le handler donné.
		 *\param[in]	handler		Le handler.
		 *\param[in]	function	La fonction.
		 */
		C3D_API void registerTextAction( castor::String const & handler
			, OnTextActionFunction function );
		/**
		 *\~english
		 *\~brief		Registers a function to call when an expand event is raised by the given handler.
		 *\param[in]	handler		The handler.
		 *\param[in]	function	The function.
		 *\~french
		 *\~brief		Enregistre une fonction à appeler lorsqu'un évènement d'expansion est lancé par le handler donné.
		 *\param[in]	handler		Le handler.
		 *\param[in]	function	La fonction.
		 */
		C3D_API void registerExpandAction( castor::String const & handler
			, OnExpandActionFunction function );
		/**
		 *\~english
		 *\~brief		Registers a function to call when a mouse cursor event is raised by the given handler.
		 *\param[in]	handler		The handler.
		 *\param[in]	function	The function.
		 *\~french
		 *\~brief		Enregistre une fonction à appeler lorsqu'un évènement de curseur de souris est lancé par le handler donné.
		 *\param[in]	handler		Le handler.
		 *\param[in]	function	La fonction.
		 */
		C3D_API void registerCursorAction( OnCursorActionFunction function );
		/**
		 *\~english
		 *\~brief		Unregisters ths function to call when a mouse move event is raised by the given handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\~brief		Désenregistre la fonction à appeler lorsqu'un évènement de déplacement de souris est lancé par le handler donné.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void unregisterMouseMoveAction( castor::String const & handler );
		/**
		 *\~english
		 *\~brief		Unregisters ths function to call when a mouse click event is raised by the given handler.
		 *\param[in]	handler	The clicked handler.
		 *\~french
		 *\~brief		Désenregistre la fonction à appeler lorsqu'un évènement de clic de souris est lancé par le handler donné.
		 *\param[in]	handler	Le handler cliqué.
		 */
		C3D_API void unregisterClickAction( castor::String const & handler );
		/**
		 *\~english
		 *\~brief		Unregisters ths function to call when a select event is raised by the given handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\~brief		Désenregistre la fonction à appeler lorsqu'un évènement de sélection est lancé par le handler donné.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void unregisterSelectAction( castor::String const & handler );
		/**
		 *\~english
		 *\~brief		Unregisters ths function to call when a text event is raised by the given handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\~brief		Désenregistre la fonction à appeler lorsqu'un évènement de texte est lancé par le handler donné.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void unregisterTextAction( castor::String const & handler );
		/**
		 *\~english
		 *\~brief		Unregisters ths function to call when an expand event is raised by the given handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\~brief		Désenregistre la fonction à appeler lorsqu'un évènement d'expansion est lancé par le handler donné.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void unregisterExpandAction( castor::String const & handler );
		/**
		 *\~english
		 *\~brief		Unregisters ths function to call when mouse cursor event is raised by the given handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\~brief		Désenregistre la fonction à appeler lorsqu'un évènement de curseur de souris est lancé par le handler donné.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void unregisterCursorAction();
		/**
		 *\~english
		 *\~brief		Event raised by the handler when the mouse is moved over it.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\~brief		Evènement lancé par le handler si la souris est déplacée au dessus de sa surface.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void onMouseMoveAction( castor::String const & handler )const;
		/**
		 *\~english
		 *\~brief		Event raised by the handler when it is clicked, if it supports clicking.
		 *\param[in]	handler	The clicked handler.
		 *\~french
		 *\~brief		Evènement lancé par le handler s'il est cliqué et qu'il le supporte.
		 *\param[in]	handler	Le handler cliqué.
		 */
		C3D_API void onClickAction( castor::String const & handler )const;
		/**
		 *\~english
		 *\~brief		Event raised by the handler when one of its sub elements is selected.
		 *\param[in]	handler	The handler.
		 *\param[in]	index	The selection index.
		 *\~french
		 *\~brief		Evènement lancé par le handler lorsqu'un de ses sous éléments est sélectionné.
		 *\param[in]	handler	Le handler.
		 *\param[in]	index	L'indice de la sélection.
		 */
		C3D_API void onSelectAction( castor::String const & handler
			, int index )const;
		/**
		 *\~english
		 *\~brief		Event raised by the handler when its caption has changed.
		 *\param[in]	handler	The handler.
		 *\param[in]	text	The new text.
		 *\~french
		 *\~brief		Evènement lancé par le handler lorsque son texte a changé.
		 *\param[in]	handler	Le handler.
		 *\param[in]	text	Le nouveau texte.
		 */
		C3D_API void onTextAction( castor::String const & handler
			, castor::U32String const & text )const;
		/**
		 *\~english
		 *\~brief		Event raised by the handler when its expansion has changed.
		 *\param[in]	handler	The handler.
		 *\param[in]	expand	The new value.
		 *\~french
		 *\~brief		Evènement lancé par le handler lorsque son expansion est changée.
		 *\param[in]	handler	Le handler.
		 *\param[in]	expand	La nouvelle valeur.
		 */
		C3D_API void onExpandAction( castor::String const & handler
			, bool expand )const;
		/**
		 *\~english
		 *\~brief		Event raised by the handler when its mouse cursor has changed.
		 *\param[in]	handler	The handler.
		 *\param[in]	expand	The new value.
		 *\~french
		 *\~brief		Evènement lancé par le handler lorsque son cureur de souris est changé.
		 *\param[in]	handler	Le handler.
		 *\param[in]	expand	La nouvelle valeur.
		 */
		C3D_API void onCursorAction( MouseCursor cursor )const;
		/**
		 *\~english
		 *\~brief		Enables handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\~brief		Active un handler.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void enableHandler( castor::String const & handler );
		/**
		 *\~english
		 *\~brief		Disables handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\~brief		Désactive un handler.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void disableHandler( castor::String const & handler );

		//@}
		/**@name Mouse input events */
		//@{

		/**
		 *\~english
		 *\~brief		Fires a mouse move event.
		 *\param[in]	position	The mouse position.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\~brief		Lance un évènement de déplacement de souris.
		 *\param[in]	position	La position de la souris.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireMouseMove( castor::Position const & position );
		/**
		 *\~english
		 *\~brief		Fires a mouse pushed event.
		 *\param[in]	button	The mouse button.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\~brief		Lance un évènement de bouton de souris appuyé.
		 *\param[in]	button	Le bouton de la souris.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireMouseButtonPushed( MouseButton button );
		/**
		 *\~english
		 *\~brief		Fires a mouse released event.
		 *\param[in]	button	The mouse button.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\~brief		Lance un évènement de bouton de souris relâché.
		 *\param[in]	button	Le bouton de la souris.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireMouseButtonReleased( MouseButton button );
		/**
		 *\~english
		 *\~brief		Fires a mouse wheel event.
		 *\param[in]	offsets	The mouse wheel offsets (x and y).
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\~brief		Lance un évènement de molette de souris.
		 *\param[in]	offsets	Les déplacements de la molette (x et y).
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireMouseWheel( castor::Position const & offsets );

		//@}
		/**@name Keyboard input events */
		//@{

		/**
		 *\~english
		 *\~brief		Fires a keyboard key down event.
		 *\param[in]	key		The key code.
		 *\param[in]	ctrl	Tells if the Ctrl key is down.
		 *\param[in]	alt		Tells if the Alt key is down.
		 *\param[in]	shift	Tells if the Shift key is down.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\~brief		Lance un évènement de touche de clavier appuyée.
		 *\param[in]	key		Le code de la touche.
		 *\param[in]	ctrl	Dit si la touche Ctrl est enfoncée.
		 *\param[in]	alt		Dit si la touche Alt est enfoncée.
		 *\param[in]	shift	Dit si la touche Shift est enfoncée.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireKeydown( KeyboardKey key
			, bool ctrl
			, bool alt
			, bool shift );
		/**
		 *\~english
		 *\~brief		Fires a keyboard key up event.
		 *\param[in]	key		The key code.
		 *\param[in]	ctrl	Tells if the Ctrl key is down.
		 *\param[in]	alt		Tells if the Alt key is down.
		 *\param[in]	shift	Tells if the Shift key is down.
		 *\return		\p true if the event is processed by a control.
		 *\~french
		 *\~brief		Lance un évènement de touche de clavier relâchée.
		 *\param[in]	key		Le code de la touche.
		 *\param[in]	ctrl	Dit si la touche Ctrl est enfoncée.
		 *\param[in]	alt		Dit si la touche Alt est enfoncée.
		 *\param[in]	shift	Dit si la touche Shift est enfoncée.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		*/
		C3D_API bool fireKeyUp( KeyboardKey key
			, bool ctrl
			, bool alt
			, bool shift );
		/**
		 *\~english
		 *\~brief		Fires a printable key event.
		 *\param[in]	key	The key code.
		 *\param[in]	c	The character coorresponding to the key, displayable as is.
		 *\return		\p true if the event is processed by a control.
		 *\~french
		 *\~brief		Lance un évènement de caractère imprimable.
		 *\param[in]	key	Le code de la touche.
		 *\param[in]	c	Le caractère correspondant à la touche, affichable tel quel.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireChar( KeyboardKey key
			, castor::String const & c );

		//@}

	protected:
		/**
		 *\~english
		 *\return		The handlers, in a thread-safe way.
		 *\~french
		 *\return		Les gestionnaires, de manière thread-safe.
		 */
		inline std::vector< EventHandlerSPtr > doGetHandlers()const
		{
			auto lock( castor::makeUniqueLock( m_mutexHandlers ) );
			return m_handlers;
		}
		/**
		 *\~english
		 *\return		\p true if there are handlers.
		 *\~french
		 *\return		\p true s'il y a des gestionnaires.
		 */
		inline bool doHasHandlers()const
		{
			auto lock( castor::makeUniqueLock( m_mutexHandlers ) );
			return !m_handlers.empty();
		}
		/**
		 *\~english
		 *\~brief		Removes a handler.
		 *\remarks		You *MUST* call this function when adding a handler to your lists, if you want to have it updated.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\~brief		Supprime un gestionnaire.
		 *\remarks		Vous *DEVEZ* appeler cette fonction lors de l'ajout d'un gestionnaire dans vos listes, si vous voulez qu'il soit mis à jour.
		 *\param[in]	handler	Le gestionnaire.
		 */
		inline void doAddHandler( EventHandlerSPtr handler )
		{
			auto lock( castor::makeUniqueLock( m_mutexHandlers ) );

			if ( std::find( std::begin( m_handlers ), std::end( m_handlers ), handler ) != std::end( m_handlers ) )
			{
				CU_Exception( "This handler already exists in the listener." );
			}

			m_handlers.push_back( handler );
		}
		/**
		 *\~english
		 *\~brief		Removes a handler.
		 *\remarks		You *MUST* call this function when adding a handler to your lists, if you want to have it updated.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\~brief		Supprime un gestionnaire.
		 *\remarks		Vous *DEVEZ* appeler cette fonction lors de l'ajout d'un gestionnaire dans vos listes, si vous voulez qu'il soit mis à jour.
		 *\param[in]	handler	Le gestionnaire.
		 */
		inline void doRemoveHandler( EventHandler const & handler )
		{
			auto lock( castor::makeUniqueLock( m_mutexHandlers ) );
			m_handlers.erase( std::find_if( std::begin( m_handlers )
				, std::end( m_handlers )
				, [&handler]( auto lookup )
				{
					return lookup.get() == &handler;
				} ) );
		}

	private:
		/**
		 *\~english
		 *\~brief		Retrieves the first handler which can be targeted by mouse.
		 *\param[in]	position	The mouse position.
		 *\return		The handler, \p nullptr if none.
		 *\~french
		 *\~brief		Récupère le premier gestionnaire pouvant être ciblé par la souris.
		 *\param[in]	position	la position de la souris.
		 *\return		Le gestionnaire, \p nullptr si aucun.
		 */
		C3D_API virtual EventHandler * doGetMouseTargetableHandler( castor::Position const & position )const = 0;

		/**@name General */
		//@{

		/**
		 *copydoc		castor3d::UserInputListener::Initialise
		 */
		C3D_API virtual bool doInitialise() = 0;
		/**
		 *copydoc		castor3d::UserInputListener::Cleanup
		 */
		C3D_API virtual void doCleanup() = 0;

		//@}

	protected:
		//!\~english	The mutex used to protect the handlers array.
		//!\~french		Le mutex de protection du tableau de gestionnaires.
		mutable std::mutex m_mutexHandlers;
		//!\~english	The handlers array.
		//!\~french		Le tableau de gestionnaires.
		std::vector< EventHandlerSPtr > m_handlers;
		//!\~english	The associated frame listener.
		//!\~french		Le frame listener associé.
		FrameListenerSPtr m_frameListener;

	private:
		MouseState m_mouse;
		KeyboardState m_keyboard;
		bool m_enabled{};
		EventHandler * m_activeHandler{};
		EventHandler * m_lastMouseTarget{};
		std::map< castor::String, OnMouseMoveActionFunction > m_onMouseMoveActions;
		std::map< castor::String, OnClickActionFunction > m_onClickActions;
		std::map< castor::String, OnSelectActionFunction > m_onSelectActions;
		std::map< castor::String, OnTextActionFunction > m_onTextActions;
		std::map< castor::String, OnExpandActionFunction > m_onExpandActions;
		OnCursorActionFunction m_onCursorAction;
	};
}

#endif
