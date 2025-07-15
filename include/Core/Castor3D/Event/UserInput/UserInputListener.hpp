/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UserInputListener_H___
#define ___C3D_UserInputListener_H___

#include "Castor3D/Event/UserInput/EventHandler.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"

namespace c3d
{
	class UserInputListener
		: public OwnedBy< Engine >
	{
	public:
		using OnClickActionFunction = Function< void() >;
		using OnMouseMoveActionFunction = Function< void( Position const & ) >;
		using OnSelectActionFunction = Function< void( int ) >;
		using OnTextActionFunction = Function< void( U32String const & ) >;
		using OnExpandActionFunction = Function< void( bool ) >;
		using OnClipboardTextActionFunction = Function< U32String( bool, U32String ) >;
		using OnCursorActionFunction = Function< void( MouseCursor ) >;

	public:
		/**@name General */
		//@{
		UserInputListener( UserInputListener const & ) = delete;
		UserInputListener( UserInputListener && )noexcept = delete;
		UserInputListener & operator=( UserInputListener const & ) = delete;
		UserInputListener & operator=( UserInputListener && )noexcept = delete;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	name	The listener's name.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	name	Le nom du listener.
		 */
		C3D_API UserInputListener( Engine & engine
			, String const & name );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~UserInputListener()noexcept = default;
		/**
		 *\~english
		 *\brief		Initialises the listener.
		 *\return		true, hopefully :P.
		 *\~french
		 *\brief		Initialise le listener.
		 *\return		true, espérons :P.
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleans up the listener.
		 *\~french
		 *\brief		Nettoie le listener.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Processes all queued events.
		 *\~french
		 *\brief		Traite tous les évènements.
		 */
		C3D_API void processEvents();

		//@}
		/**@name Getters */
		//@{
		
		Position const & getMousePosition()const
		{
			return m_mouse.position;
		}

		inline EventHandlerRPtr getActiveControl()const
		{
			return m_activeHandler;
		}

		inline EventHandlerRPtr getFocusedControl()const
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
		 *\brief		Registers a function to call when a mouse move event is raised by the given handler.
		 *\param[in]	handler		The handler.
		 *\param[in]	function	The function.
		 *\~french
		 *\brief		Enregistre une fonction à appeler lorsqu'un évènement de déplacement de souris est lancé par le handler donné.
		 *\param[in]	handler		Le handler.
		 *\param[in]	function	La fonction.
		 */
		C3D_API void registerMouseMoveAction( String const & handler
			, OnMouseMoveActionFunction function );
		/**
		 *\~english
		 *\brief		Registers a function to call when a mouse click event is raised by the given handler.
		 *\param[in]	handler		The clicked handler.
		 *\param[in]	function	The function.
		 *\~french
		 *\brief		Enregistre une fonction à appeler lorsqu'un évènement de clic de souris est lancé par le handler donné.
		 *\param[in]	handler		Le handler cliqué.
		 *\param[in]	function	La fonction.
		 */
		C3D_API void registerClickAction( String const & handler
			, OnClickActionFunction function );
		/**
		 *\~english
		 *\brief		Registers a function to call when a select event is raised by the given handler.
		 *\param[in]	handler		The handler.
		 *\param[in]	function	The function.
		 *\~french
		 *\brief		Enregistre une fonction à appeler lorsqu'un évènement de sélection est lancé par le handler donné.
		 *\param[in]	handler		Le handler.
		 *\param[in]	function	La fonction.
		 */
		C3D_API void registerSelectAction( String const & handler
			, OnSelectActionFunction function );
		/**
		 *\~english
		 *\brief		Registers a function to call when a text event is raised by the given handler.
		 *\param[in]	handler		The handler.
		 *\param[in]	function	The function.
		 *\~french
		 *\brief		Enregistre une fonction à appeler lorsqu'un évènement de texte est lancé par le handler donné.
		 *\param[in]	handler		Le handler.
		 *\param[in]	function	La fonction.
		 */
		C3D_API void registerTextAction( String const & handler
			, OnTextActionFunction function );
		/**
		 *\~english
		 *\brief		Registers a function to call when an expand event is raised by the given handler.
		 *\param[in]	handler		The handler.
		 *\param[in]	function	The function.
		 *\~french
		 *\brief		Enregistre une fonction à appeler lorsqu'un évènement d'expansion est lancé par le handler donné.
		 *\param[in]	handler		Le handler.
		 *\param[in]	function	La fonction.
		 */
		C3D_API void registerExpandAction( String const & handler
			, OnExpandActionFunction function );
		/**
		 *\~english
		 *\brief		Registers a function to call when a mouse cursor event is raised by the given handler.
		 *\param[in]	function	The function.
		 *\~french
		 *\brief		Enregistre une fonction à appeler lorsqu'un évènement de curseur de souris est lancé par le handler donné.
		 *\param[in]	function	La fonction.
		 */
		C3D_API void registerCursorAction( OnCursorActionFunction function );
		/**
		 *\~english
		 *\brief		Registers a function to call when a clipboard event is raised by the given handler.
		 *\param[in]	function	The function.
		 *\~french
		 *\brief		Enregistre une fonction à appeler lorsqu'un évènement de presse-papier est lancé par le handler donné.
		 *\param[in]	function	La fonction.
		 */
		C3D_API void registerClipboardTextAction( OnClipboardTextActionFunction function );
		/**
		 *\~english
		 *\brief		Unregisters ths function to call when a mouse move event is raised by the given handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\brief		Désenregistre la fonction à appeler lorsqu'un évènement de déplacement de souris est lancé par le handler donné.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void unregisterMouseMoveAction( String const & handler );
		/**
		 *\~english
		 *\brief		Unregisters ths function to call when a mouse click event is raised by the given handler.
		 *\param[in]	handler	The clicked handler.
		 *\~french
		 *\brief		Désenregistre la fonction à appeler lorsqu'un évènement de clic de souris est lancé par le handler donné.
		 *\param[in]	handler	Le handler cliqué.
		 */
		C3D_API void unregisterClickAction( String const & handler );
		/**
		 *\~english
		 *\brief		Unregisters ths function to call when a select event is raised by the given handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\brief		Désenregistre la fonction à appeler lorsqu'un évènement de sélection est lancé par le handler donné.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void unregisterSelectAction( String const & handler );
		/**
		 *\~english
		 *\brief		Unregisters ths function to call when a text event is raised by the given handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\brief		Désenregistre la fonction à appeler lorsqu'un évènement de texte est lancé par le handler donné.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void unregisterTextAction( String const & handler );
		/**
		 *\~english
		 *\brief		Unregisters ths function to call when an expand event is raised by the given handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\brief		Désenregistre la fonction à appeler lorsqu'un évènement d'expansion est lancé par le handler donné.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void unregisterExpandAction( String const & handler );
		/**
		 *\~english
		 *\brief		Unregisters the function to call when mouse cursor event is raised by the given handler.
		 *\~french
		 *\brief		Désenregistre la fonction à appeler lorsqu'un évènement de curseur de souris est lancé par le handler donné.
		 */
		C3D_API void unregisterCursorAction();
		/**
		 *\~english
		 *\brief		Unregisters the function to call when a clipboard event is raised by the given handler.
		 *\~french
		 *\brief		Désenregistre la fonction à appeler lorsqu'un évènement de presse-papier est lancé par le handler donné.
		 */
		C3D_API void unregisterClipboardTextAction();
		/**
		 *\~english
		 *\brief		Event raised by the handler when the mouse is moved over it.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\brief		Evènement lancé par le handler si la souris est déplacée au dessus de sa surface.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void onMouseMoveAction( String const & handler )const;
		/**
		 *\~english
		 *\brief		Event raised by the handler when it is clicked, if it supports clicking.
		 *\param[in]	handler	The clicked handler.
		 *\~french
		 *\brief		Evènement lancé par le handler s'il est cliqué et qu'il le supporte.
		 *\param[in]	handler	Le handler cliqué.
		 */
		C3D_API void onClickAction( String const & handler )const;
		/**
		 *\~english
		 *\brief		Event raised by the handler when one of its sub elements is selected.
		 *\param[in]	handler	The handler.
		 *\param[in]	index	The selection index.
		 *\~french
		 *\brief		Evènement lancé par le handler lorsqu'un de ses sous éléments est sélectionné.
		 *\param[in]	handler	Le handler.
		 *\param[in]	index	L'indice de la sélection.
		 */
		C3D_API void onSelectAction( String const & handler
			, int index )const;
		/**
		 *\~english
		 *\brief		Event raised by the handler when its caption has changed.
		 *\param[in]	handler	The handler.
		 *\param[in]	text	The new text.
		 *\~french
		 *\brief		Evènement lancé par le handler lorsque son texte a changé.
		 *\param[in]	handler	Le handler.
		 *\param[in]	text	Le nouveau texte.
		 */
		C3D_API void onTextAction( String const & handler
			, U32String const & text )const;
		/**
		 *\~english
		 *\brief		Event raised by the handler when its expansion has changed.
		 *\param[in]	handler	The handler.
		 *\param[in]	expand	The new value.
		 *\~french
		 *\brief		Evènement lancé par le handler lorsque son expansion est changée.
		 *\param[in]	handler	Le handler.
		 *\param[in]	expand	La nouvelle valeur.
		 */
		C3D_API void onExpandAction( String const & handler
			, bool expand )const;
		/**
		 *\~english
		 *\brief		Event raised when mouse cursor has changed.
		 *\param[in]	cursor	The cursor.
		 *\~french
		 *\brief		Evènement lorsque le cureur de souris est changé.
		 *\param[in]	cursor	Le curseur.
		 */
		C3D_API void onCursorAction( MouseCursor cursor )const;
		/**
		 *\~english
		 *\brief		Event raised interaction with the clipboard is needed.
		 *\param[in]	set		\p true to define the clipboard text, \p false to retrieve it.
		 *\param[in]	text	The value to set the clipboard text to.
		 *\return		The clipboard text.
		 *\~french
		 *\brief		Evènement lancé lorsqu'une interaction avec le presse-papier est demandée.
		 *\param[in]	set		\p true pour définir le texte du presse-papier, \p false pour le récupérer.
		 *\param[in]	text	La valeur pour le presse-papier.
		 *\return		Le texte du presse-papier.
		 */
		C3D_API U32String onClipboardTextAction( bool set
			, U32String text )const;
		/**
		 *\~english
		 *\brief		Enables handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\brief		Active un handler.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void enableHandler( String const & handler );
		/**
		 *\~english
		 *\brief		Disables handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\brief		Désactive un handler.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void disableHandler( String const & handler );

		//@}
		/**@name Mouse input events */
		//@{

		/**
		 *\~english
		 *\brief		Fires a mouse move event.
		 *\param[in]	position	The mouse position.
		 *\param[in]	ctrl		Tells if the Ctrl key is down.
		 *\param[in]	alt			Tells if the Alt key is down.
		 *\param[in]	shift		Tells if the Shift key is down.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\brief		Lance un évènement de déplacement de souris.
		 *\param[in]	position	La position de la souris.
		 *\param[in]	ctrl		Dit si la touche Ctrl est enfoncée.
		 *\param[in]	alt			Dit si la touche Alt est enfoncée.
		 *\param[in]	shift		Dit si la touche Shift est enfoncée.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireMouseMove( Position const & position
			, bool ctrl
			, bool alt
			, bool shift );
		/**
		 *\~english
		 *\brief		Fires a mouse pushed event.
		 *\param[in]	button	The mouse button.
		 *\param[in]	ctrl	Tells if the Ctrl key is down.
		 *\param[in]	alt		Tells if the Alt key is down.
		 *\param[in]	shift	Tells if the Shift key is down.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\brief		Lance un évènement de bouton de souris appuyé.
		 *\param[in]	button	Le bouton de la souris.
		 *\param[in]	ctrl	Dit si la touche Ctrl est enfoncée.
		 *\param[in]	alt		Dit si la touche Alt est enfoncée.
		 *\param[in]	shift	Dit si la touche Shift est enfoncée.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireMouseButtonPushed( MouseButton button
			, bool ctrl
			, bool alt
			, bool shift );
		/**
		 *\~english
		 *\brief		Fires a mouse released event.
		 *\param[in]	button	The mouse button.
		 *\param[in]	ctrl	Tells if the Ctrl key is down.
		 *\param[in]	alt		Tells if the Alt key is down.
		 *\param[in]	shift	Tells if the Shift key is down.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\brief		Lance un évènement de bouton de souris relâché.
		 *\param[in]	button	Le bouton de la souris.
		 *\param[in]	ctrl	Dit si la touche Ctrl est enfoncée.
		 *\param[in]	alt		Dit si la touche Alt est enfoncée.
		 *\param[in]	shift	Dit si la touche Shift est enfoncée.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireMouseButtonReleased( MouseButton button
			, bool ctrl
			, bool alt
			, bool shift );
		/**
		 *\~english
		 *\brief		Fires a mouse wheel event.
		 *\param[in]	offsets	The mouse wheel offsets (x and y).
		 *\param[in]	ctrl	Tells if the Ctrl key is down.
		 *\param[in]	alt		Tells if the Alt key is down.
		 *\param[in]	shift	Tells if the Shift key is down.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\brief		Lance un évènement de molette de souris.
		 *\param[in]	offsets	Les déplacements de la molette (x et y).
		 *\param[in]	ctrl	Dit si la touche Ctrl est enfoncée.
		 *\param[in]	alt		Dit si la touche Alt est enfoncée.
		 *\param[in]	shift	Dit si la touche Shift est enfoncée.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireMouseWheel( Position const & offsets
			, bool ctrl
			, bool alt
			, bool shift );

		//@}
		/**@name Keyboard input events */
		//@{

		/**
		 *\~english
		 *\brief		Fires a keyboard key down event.
		 *\param[in]	key		The key code.
		 *\param[in]	ctrl	Tells if the Ctrl key is down.
		 *\param[in]	alt		Tells if the Alt key is down.
		 *\param[in]	shift	Tells if the Shift key is down.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\brief		Lance un évènement de touche de clavier appuyée.
		 *\param[in]	key		Le code de la touche.
		 *\param[in]	ctrl	Dit si la touche Ctrl est enfoncée.
		 *\param[in]	alt		Dit si la touche Alt est enfoncée.
		 *\param[in]	shift	Dit si la touche Shift est enfoncée.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireKeyDown( KeyboardKey key
			, bool ctrl
			, bool alt
			, bool shift );
		/**
		 *\~english
		 *\brief		Fires a keyboard key up event.
		 *\param[in]	key		The key code.
		 *\param[in]	ctrl	Tells if the Ctrl key is down.
		 *\param[in]	alt		Tells if the Alt key is down.
		 *\param[in]	shift	Tells if the Shift key is down.
		 *\return		\p true if the event is processed by a control.
		 *\~french
		 *\brief		Lance un évènement de touche de clavier relâchée.
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
		 *\brief		Fires a printable key event.
		 *\param[in]	key	The key code.
		 *\param[in]	c	The character coorresponding to the key, displayable as is.
		 *\return		\p true if the event is processed by a control.
		 *\~french
		 *\brief		Lance un évènement de caractère imprimable.
		 *\param[in]	key	Le code de la touche.
		 *\param[in]	c	Le caractère correspondant à la touche, affichable tel quel.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireChar( KeyboardKey key
			, String const & c );

		//@}

	protected:
		/**
		 *\~english
		 *\return		The handlers, in a thread-safe way.
		 *\~french
		 *\return		Les gestionnaires, de manière thread-safe.
		 */
		inline Vector< EventHandlerRPtr > doGetHandlers()const
		{
			auto lock( makeUniqueLock( m_mutexHandlers ) );
			Vector< EventHandlerRPtr > result;

			for ( auto & h : m_handlers )
			{
				result.push_back( h.get() );
			}

			return result;
		}
		/**
		 *\~english
		 *\return		\p true if there are handlers.
		 *\~french
		 *\return		\p true s'il y a des gestionnaires.
		 */
		inline bool doHasHandlers()const
		{
			auto lock( makeUniqueLock( m_mutexHandlers ) );
			return !m_handlers.empty();
		}
		/**
		 *\~english
		 *\brief		Removes a handler.
		 *\remarks		You *MUST* call this function when adding a handler to your lists, if you want to have it updated.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\brief		Supprime un gestionnaire.
		 *\remarks		Vous *DEVEZ* appeler cette fonction lors de l'ajout d'un gestionnaire dans vos listes, si vous voulez qu'il soit mis à jour.
		 *\param[in]	handler	Le gestionnaire.
		 */
		inline EventHandlerRPtr doAddHandler( EventHandlerUPtr handler )
		{
			auto lock( makeUniqueLock( m_mutexHandlers ) );

			if ( std::find( std::begin( m_handlers ), std::end( m_handlers ), handler ) != std::end( m_handlers ) )
			{
				CU_Exception( "This handler already exists in the listener." );
			}

			auto result = handler.get();
			m_handlers.push_back( c3d::move( handler ) );
			return result;
		}
		/**
		 *\~english
		 *\brief		Removes a handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\brief		Supprime un gestionnaire.
		 *\param[in]	handler	Le gestionnaire.
		 */
		inline void doRemoveHandlerNL( EventHandler const & handler )
		{
			auto it = std::find_if( std::begin( m_handlers )
				, std::end( m_handlers )
				, [&handler]( EventHandlerUPtr const & lookup )
				{
					return lookup.get() == &handler;
				} );

			if ( it != m_handlers.end() )
			{
				auto h = c3d::move( *it );
				m_handlers.erase( it );
			}
		}
		/**
		 *\~english
		 *\brief		Removes a handler.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\brief		Supprime un gestionnaire.
		 *\param[in]	handler	Le gestionnaire.
		 */
		inline void doRemoveHandler( EventHandler const & handler )
		{
			auto lock( makeUniqueLock( m_mutexHandlers ) );
			doRemoveHandlerNL( handler );
		}

	private:
		/**
		 *\~english
		 *\brief		Retrieves the first handler which can be targeted by mouse.
		 *\param[in]	position	The mouse position.
		 *\return		The handler, \p nullptr if none.
		 *\~french
		 *\brief		Récupère le premier gestionnaire pouvant être ciblé par la souris.
		 *\param[in]	position	la position de la souris.
		 *\return		Le gestionnaire, \p nullptr si aucun.
		 */
		virtual EventHandlerRPtr doGetMouseTargetableHandler( Position const & position )const = 0;
		/**
		 *copydoc		UserInputListener::Initialise
		 */
		virtual bool doInitialise() = 0;
		/**
		 *copydoc		UserInputListener::Cleanup
		 */
		virtual void doCleanup() = 0;
		/**
		 *brief		Updates keyboard command keys
		 */
		void doUpdateCommandKeys( bool ctrl
			, bool alt
			, bool shift );

	protected:
		//!\~english	The mutex used to protect the handlers array.
		//!\~french		Le mutex de protection du tableau de gestionnaires.
		mutable Mutex m_mutexHandlers;
		//!\~english	The handlers array.
		//!\~french		Le tableau de gestionnaires.
		Vector< EventHandlerUPtr > m_handlers;
		//!\~english	The associated frame listener.
		//!\~french		Le frame listener associé.
		FrameListenerRPtr m_frameListener;

	private:
		MouseState m_mouse;
		KeyboardState m_keyboard;
		bool m_enabled{};
		EventHandlerRPtr m_activeHandler{};
		EventHandlerRPtr m_lastMouseTarget{};
		StringMap< OnMouseMoveActionFunction > m_onMouseMoveActions;
		StringMap< OnClickActionFunction > m_onClickActions;
		StringMap< OnSelectActionFunction > m_onSelectActions;
		StringMap< OnTextActionFunction > m_onTextActions;
		StringMap< OnExpandActionFunction > m_onExpandActions;
		OnCursorActionFunction m_onCursorAction;
		OnClipboardTextActionFunction m_onClipboardTextAction;
	};
}

#endif
