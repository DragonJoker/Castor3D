/*
See LICENSE file in root folder
*/
#ifndef ___C3D_USER_INPUT_LISTENER_H___
#define ___C3D_USER_INPUT_LISTENER_H___

#include "Castor3D/Castor3DPrerequisites.hpp"
#include "Castor3D/Event/UserInput/EventHandler.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"

#include <CastorUtils/Graphics/Position.hpp>

namespace castor3d
{
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Class used to to manage the user input events.
	*\~french
	*\brief		Classe utilisée pour gérer les évènements utilisateur.
	*/
	class UserInputListener
		: public castor::OwnedBy< Engine >
	{
	public:
		using OnClickActionFunction = std::function< void() >;
		using OnMouseMoveActionFunction = std::function< void( castor::Position const & ) >;
		using OnSelectActionFunction = std::function< void( int ) >;
		using OnTextActionFunction = std::function< void( castor::String const & ) >;

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
		/**
		 *\~english
		 *\return		The current mouse position.
		 *\~french
		 *\return		La position actuelle de la souris.
		 */
		castor::Position const & getMousePosition()const
		{
			return m_mouse.m_position;
		}
		/**
		 *\~english
		 *\return		The active control.
		 *\~french
		 *\return		Le contrôle actif.
		 */
		inline EventHandlerSPtr getActiveControl()const
		{
			return m_activeHandler.lock();
		}

		/**
		 *\~english
		 *\return		The focused control.
		 *\~french
		 *\return		Le contrôle ayant le focus.
		 */
		inline EventHandlerSPtr getFocusedControl()const
		{
			return m_lastMouseTarget.lock();
		}

		//@}
		/**@name Output events */
		//@{
		
		/**
		 *\~english
		 *\~brief		Registers an event raised by the handler when the mouse is moved over it.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\~brief		Enregistre un évènement lancé par le handler si la souris se déplace sur sa surface.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void registerMouseMoveAction( castor::String const & handler
			, OnMouseMoveActionFunction function );
		/**
		 *\~english
		 *\~brief		Registers an event raised by the handler when it is clicked, if it supports clicking.
		 *\param[in]	handler	The clicked handler.
		 *\~french
		 *\~brief		Enregistre un évènement lancé par le handler s'il est cliqué et qu'il le supporte.
		 *\param[in]	handler	Le handler cliqué.
		 */
		C3D_API void registerClickAction( castor::String const & handler
			, OnClickActionFunction function );
		/**
		 *\~english
		 *\~brief		Registers an event raised by the handler when one of its sub elements is selected.
		 *\param[in]	handler	The handler.
		 *\param[in]	index	The selection index.
		 *\~french
		 *\~brief		Enregistre un évènement lancé par le handler lorsqu'un de ses sous éléments est sélectionné.
		 *\param[in]	handler	Le handler.
		 *\param[in]	index	L'indice de la sélection.
		 */
		C3D_API void registerSelectAction( castor::String const & handler
			, OnSelectActionFunction function );
		/**
		 *\~english
		 *\~brief		Registers an event raised by the handler when its caption has changed.
		 *\param[in]	handler	The handler.
		 *\param[in]	text	The new text.
		 *\~french
		 *\~brief		Enregistre un évènement lancé par le handler lorsque son texte a changé.
		 *\param[in]	handler	Le handler.
		 *\param[in]	text	Le nouveau texte.
		 */
		C3D_API void registerTextAction( castor::String const & handler
			, OnTextActionFunction function );
		/**
		 *\~english
		 *\~brief		Unregisters an event raised by the handler when the mouse is moved over it.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\~brief		Désnregistre un évènement lancé par le handler si la souris se déplace sur sa surface.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void unregisterMouseMoveAction( castor::String const & handler );
		/**
		 *\~english
		 *\~brief		Unregisters an event raised by the handler when it is clicked, if it supports clicking.
		 *\param[in]	handler	The clicked handler.
		 *\~french
		 *\~brief		Désenregistre un évènement lancé par le handler s'il est cliqué et qu'il le supporte.
		 *\param[in]	handler	Le handler cliqué.
		 */
		C3D_API void unregisterClickAction( castor::String const & handler );
		/**
		 *\~english
		 *\~brief		Unregisters an event raised by the handler when one of its sub elements is selected.
		 *\param[in]	handler	The handler.
		 *\param[in]	index	The selection index.
		 *\~french
		 *\~brief		Désenregistre un évènement lancé par le handler lorsqu'un de ses sous éléments est sélectionné.
		 *\param[in]	handler	Le handler.
		 *\param[in]	index	L'indice de la sélection.
		 */
		C3D_API void unregisterSelectAction( castor::String const & handler );
		/**
		 *\~english
		 *\~brief		Unregisters an event raised by the handler when its caption has changed.
		 *\param[in]	handler	The handler.
		 *\param[in]	text	The new text.
		 *\~french
		 *\~brief		Désenregistre un évènement lancé par le handler lorsque son texte a changé.
		 *\param[in]	handler	Le handler.
		 *\param[in]	text	Le nouveau texte.
		 */
		C3D_API void unregisterTextAction( castor::String const & handler );
		/**
		 *\~english
		 *\~brief		Event raised by the handler when the mouse is moved over it.
		 *\param[in]	handler	The handler.
		 *\~french
		 *\~brief		Evènement lancé par le handler si la souris est déplacée au dessus de sa surface.
		 *\param[in]	handler	Le handler.
		 */
		C3D_API void onMouseMoveAction( castor::String const & handler );
		/**
		 *\~english
		 *\~brief		Event raised by the handler when it is clicked, if it supports clicking.
		 *\param[in]	handler	The clicked handler.
		 *\~french
		 *\~brief		Evènement lancé par le handler s'il est cliqué et qu'il le supporte.
		 *\param[in]	handler	Le handler cliqué.
		 */
		C3D_API void onClickAction( castor::String const & handler );
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
			, int index );
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
			, castor::String const & text );

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
		/**@name Overlay events */
		//@{

		/**
		 *\~english
		 *\~brief		Fires a material change event.
		 *\param[in]	overlay		The overlay name.
		 *\param[in]	material	The material name.
		 *\return		\p true if the event is processed by a control.
		 *\~french
		 *\~brief		Lance un évènement de changement de matériau.
		 *\param[in]	overlay		Le nom de l'incrustation.
		 *\param[in]	material	Le nom du matériau.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API virtual bool fireMaterialEvent( castor::String const & overlay
			, castor::String const & material );
		
		/**
		 *\~english
		 *\~brief		Fires a caption change event.
		 *\param[in]	overlay	The overlay name.
		 *\param[in]	caption	The new caption.
		 *\return		\p true if the event is processed by a control.
		 *\~french
		 *\~brief		Lance un évènement de changement de texte.
		 *\param[in]	overlay	Le nom de l'incrustation.
		 *\param[in]	caption	Le nouveau texte.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API virtual bool fireTextEvent( castor::String const & overlay
			, castor::String const & caption );

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
			using LockType = std::unique_lock< std::mutex >;
			LockType lock{ castor::makeUniqueLock( m_mutexHandlers ) };
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
			using LockType = std::unique_lock< std::mutex >;
			LockType lock{ castor::makeUniqueLock( m_mutexHandlers ) };
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
			using LockType = std::unique_lock< std::mutex >;
			LockType lock{ castor::makeUniqueLock( m_mutexHandlers ) };

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
		inline void doRemoveHandler( EventHandlerSPtr handler )
		{
			using LockType = std::unique_lock< std::mutex >;
			LockType lock{ castor::makeUniqueLock( m_mutexHandlers ) };
			m_handlers.erase( std::find( std::begin( m_handlers ), std::end( m_handlers ), handler ) );
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
		C3D_API virtual EventHandlerSPtr doGetMouseTargetableHandler( castor::Position const & position )const = 0;

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
		//!\~english	The current mouse state.
		//!\~french		L'état courant de la souris.
		MouseState m_mouse;
		//!\~english	The current keyboard state.
		//!\~french		L'état courant du clavier.
		KeyboardState m_keyboard;
		//!\~english	Tells if the listener needs to be refreshed or not.
		//!\~french		Dit si le listener doit être mis à jour.
		bool m_enabled;
		//!\~english	The currently active handler.
		//!\~french		Le gestionnaire actif.
		EventHandlerWPtr m_activeHandler;
		//!\~english	The last mouse target handler.
		//!\~french		Le dernier gestionnaire cible de la souris.
		EventHandlerWPtr m_lastMouseTarget;
		//!\~english	The registered handler mouse move event actions.
		//!\~french		Les actions enregistrées pour l'évènement déplacement de souris.
		std::map< castor::String, OnMouseMoveActionFunction > m_onMouseMoveActions;
		//!\~english	The registered handler clicked event actions.
		//!\~french		Les actions enregistrées pour l'évènement handler cliqué.
		std::map< castor::String, OnClickActionFunction > m_onClickActions;
		//!\~english	The registered handler sub element selected event actions.
		//!\~french		Les actions enregistrées pour l'évènement sous élément de handler sélectionné.
		std::map< castor::String, OnSelectActionFunction > m_onSelectActions;
		//!\~english	The registered handler caption changed event actions.
		//!\~french		Les actions enregistrées pour l'évènement caption du handler changé.
		std::map< castor::String, OnTextActionFunction > m_onTextActions;
	};
}

#endif
