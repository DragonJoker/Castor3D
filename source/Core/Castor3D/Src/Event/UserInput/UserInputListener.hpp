/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_USER_INPUT_LISTENER_H___
#define ___C3D_USER_INPUT_LISTENER_H___

#include "Castor3DPrerequisites.hpp"

#include <Graphics/Position.hpp>

#include "EventHandler.hpp"

#include "Event/Frame/FrameListener.hpp"

namespace Castor3D
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
		: public Castor::OwnedBy< Engine >
	{
	public:
		using OnClickActionFunction = std::function< void() >;
		using OnSelectActionFunction = std::function< void( int ) >;
		using OnTextActionFunction = std::function< void( Castor::String const & ) >;

	public:
		/**@name General */
		//@{

		/**
		 *\~english
		 *\~brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_name		The listener's name.
		 *\~french
		 *\~brief		Constructeur.
		 *\param[in]	p_engine	The moteur.
		 *\param[in]	p_name		The nom du listener.
		 */
		C3D_API UserInputListener( Engine & p_engine, Castor::String const & p_name );
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
		C3D_API bool Initialise();
		/**
		 *\~english
		 *\~brief		Cleans up the listener.
		 *\~french
		 *\~brief		Nettoie le listener.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\~brief		Processes all queued events.
		 *\~french
		 *\~brief		Traite tous les évènements.
		 */
		C3D_API void ProcessEvents();
		/**
		 *\~english
		 *\return		The current mouse position.
		 *\~french
		 *\return		La position actuelle de la souris.
		 */
		Castor::Position const & GetMousePosition()const
		{
			return m_mouse.m_position;
		}
		/**
		 *\~english
		 *\return		The active control.
		 *\~french
		 *\return		Le contrôle actif.
		 */
		inline EventHandlerSPtr GetActiveControl()const
		{
			return m_activeHandler.lock();
		}

		/**
		 *\~english
		 *\return		The focused control.
		 *\~french
		 *\return		Le contrôle ayant le focus.
		 */
		inline EventHandlerSPtr GetFocusedControl()const
		{
			return m_lastMouseTarget.lock();
		}

		//@}
		/**@name Output events */
		//@{
		
		/**
		 *\~english
		 *\~brief		Registers an event raised by the handler when it is clicked, if it supports clicking.
		 *\param[in]	p_handler	The clicked handler.
		 *\~french
		 *\~brief		Enregistre un évènement lancé par le handler s'il est cliqué et qu'il le supporte.
		 *\param[in]	p_handler	Le handler cliqué.
		 */
		C3D_API void RegisterClickAction( Castor::String const & p_handler
			, OnClickActionFunction p_function );
		/**
		 *\~english
		 *\~brief		Registers an event raised by the handler when one of its sub elements is selected.
		 *\param[in]	p_handler	The handler.
		 *\param[in]	p_index		The selection index.
		 *\~french
		 *\~brief		Enregistre un évènement lancé par le handler lorsqu'un de ses sous éléments est sélectionné.
		 *\param[in]	p_handler	Le handler.
		 *\param[in]	p_index		L'indice de la sélection.
		 */
		C3D_API void RegisterSelectAction( Castor::String const & p_handler
			, OnSelectActionFunction p_function );
		/**
		 *\~english
		 *\~brief		Registers an event raised by the handler when its caption has changed.
		 *\param[in]	p_handler	The handler.
		 *\param[in]	p_text		The new text.
		 *\~french
		 *\~brief		Enregistre un évènement lancé par le handler lorsque son texte a changé.
		 *\param[in]	p_handler	Le handler.
		 *\param[in]	p_text		Le nouveau texte.
		 */
		C3D_API void RegisterTextAction( Castor::String const & p_handler
			, OnTextActionFunction p_function );
		/**
		 *\~english
		 *\~brief		Unregisters an event raised by the handler when it is clicked, if it supports clicking.
		 *\param[in]	p_handler	The clicked handler.
		 *\~french
		 *\~brief		Désenregistre un évènement lancé par le handler s'il est cliqué et qu'il le supporte.
		 *\param[in]	p_handler	Le handler cliqué.
		 */
		C3D_API void UnregisterClickAction( Castor::String const & p_handler );
		/**
		 *\~english
		 *\~brief		Unregisters an event raised by the handler when one of its sub elements is selected.
		 *\param[in]	p_handler	The handler.
		 *\param[in]	p_index		The selection index.
		 *\~french
		 *\~brief		Désenregistre un évènement lancé par le handler lorsqu'un de ses sous éléments est sélectionné.
		 *\param[in]	p_handler	Le handler.
		 *\param[in]	p_index		L'indice de la sélection.
		 */
		C3D_API void UnregisterSelectAction( Castor::String const & p_handler );
		/**
		 *\~english
		 *\~brief		Unregisters an event raised by the handler when its caption has changed.
		 *\param[in]	p_handler	The handler.
		 *\param[in]	p_text		The new text.
		 *\~french
		 *\~brief		Désenregistre un évènement lancé par le handler lorsque son texte a changé.
		 *\param[in]	p_handler	Le handler.
		 *\param[in]	p_text		Le nouveau texte.
		 */
		C3D_API void UnregisterTextAction( Castor::String const & p_handler );
		/**
		 *\~english
		 *\~brief		Event raised by the handler when it is clicked, if it supports clicking.
		 *\param[in]	p_handler	The clicked handler.
		 *\~french
		 *\~brief		Evènement lancé par le handler s'il est cliqué et qu'il le supporte.
		 *\param[in]	p_handler	Le handler cliqué.
		 */
		C3D_API void OnClickAction( Castor::String const & p_handler );
		/**
		 *\~english
		 *\~brief		Event raised by the handler when one of its sub elements is selected.
		 *\param[in]	p_handler	The handler.
		 *\param[in]	p_index		The selection index.
		 *\~french
		 *\~brief		Evènement lancé par le handler lorsqu'un de ses sous éléments est sélectionné.
		 *\param[in]	p_handler	Le handler.
		 *\param[in]	p_index		L'indice de la sélection.
		 */
		C3D_API void OnSelectAction( Castor::String const & p_handler, int p_index );
		/**
		 *\~english
		 *\~brief		Event raised by the handler when its caption has changed.
		 *\param[in]	p_handler	The handler.
		 *\param[in]	p_text		The new text.
		 *\~french
		 *\~brief		Evènement lancé par le handler lorsque son texte a changé.
		 *\param[in]	p_handler	Le handler.
		 *\param[in]	p_text		Le nouveau texte.
		 */
		C3D_API void OnTextAction( Castor::String const & p_handler, Castor::String const & p_text );

		//@}
		/**@name Mouse input events */
		//@{

		/**
		 *\~english
		 *\~brief		Fires a mouse move event.
		 *\param[in]	p_position	The mouse position.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\~brief		Lance un évènement de déplacement de souris.
		 *\param[in]	p_position	La position de la souris.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool FireMouseMove( Castor::Position const & p_position );
		/**
		 *\~english
		 *\~brief		Fires a mouse pushed event.
		 *\param[in]	p_button	The mouse button.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\~brief		Lance un évènement de bouton de souris appuyé.
		 *\param[in]	p_button	Le bouton de la souris.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool FireMouseButtonPushed( MouseButton p_button );
		/**
		 *\~english
		 *\~brief		Fires a mouse released event.
		 *\param[in]	p_button	The mouse button.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\~brief		Lance un évènement de bouton de souris relâché.
		 *\param[in]	p_button	Le bouton de la souris.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool FireMouseButtonReleased( MouseButton p_button );
		/**
		 *\~english
		 *\~brief		Fires a mouse wheel event.
		 *\param[in]	p_offsets	The mouse wheel offsets (x and y).
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\~brief		Lance un évènement de molette de souris.
		 *\param[in]	p_offsets	Les déplacements de la molette (x et y).
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool FireMouseWheel( Castor::Position const & p_offsets );

		//@}
		/**@name Keyboard input events */
		//@{

		/**
		 *\~english
		 *\~brief		Fires a keyboard key down event.
		 *\param[in]	p_key		The key code.
		 *\param[in]	p_ctrl		Tells if the Ctrl key is down.
		 *\param[in]	p_alt		Tells if the Alt key is down.
		 *\param[in]	p_shift		Tells if the Shift key is down.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\~brief		Lance un évènement de touche de clavier appuyée.
		 *\param[in]	p_key		Le code de la touche.
		 *\param[in]	p_ctrl		Dit si la touche Ctrl est enfoncée.
		 *\param[in]	p_alt		Dit si la touche Alt est enfoncée.
		 *\param[in]	p_shift		Dit si la touche Shift est enfoncée.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool FireKeyDown( KeyboardKey p_key, bool p_ctrl, bool p_alt, bool p_shift );
		/**
		 *\~english
		 *\~brief		Fires a keyboard key up event.
		 *\param[in]	p_key		The key code.
		 *\param[in]	p_ctrl		Tells if the Ctrl key is down.
		 *\param[in]	p_alt		Tells if the Alt key is down.
		 *\param[in]	p_shift		Tells if the Shift key is down.
		 *\return		\p true if the event is processed by a control.
		 *\~french
		 *\~brief		Lance un évènement de touche de clavier relâchée.
		 *\param[in]	p_key		Le code de la touche.
		 *\param[in]	p_ctrl		Dit si la touche Ctrl est enfoncée.
		 *\param[in]	p_alt		Dit si la touche Alt est enfoncée.
		 *\param[in]	p_shift		Dit si la touche Shift est enfoncée.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		*/
		C3D_API bool FireKeyUp( KeyboardKey p_key, bool p_ctrl, bool p_alt, bool p_shift );
		/**
		 *\~english
		 *\~brief		Fires a printable key event.
		 *\param[in]	p_key		The key code.
		 *\param[in]	p_char		The character coorresponding to the key, displayable as is.
		 *\return		\p true if the event is processed by a control.
		 *\~french
		 *\~brief		Lance un évènement de caractère imprimable.
		 *\param[in]	p_key		Le code de la touche.
		 *\param[in]	p_char		Le caractère correspondant à la touche, affichable tel quel.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool FireChar( KeyboardKey p_key, Castor::String const & p_char );

		//@}

	protected:
		/**
		 *\~english
		 *\~brief		Retrieve the handlers in a thread-safe way.
		 *\return		The handlers.
		 *\~french
		 *\~brief		Récupère les gestionnaires de manière thread-safe.
		 *\return		The gestionnaires.
		 */
		inline std::vector< EventHandlerSPtr > DoGetHandlers()const
		{
			auto l_lock = Castor::make_unique_lock( m_mutexHandlers );
			return m_handlers;
		}
		/**
		 *\~english
		 *\~brief		Removes a handler.
		 *\remarks		You *MUST* call this function when adding a handler to your lists, if you want to have it updated.
		 *\param[in]	p_handler	The handler.
		 *\~french
		 *\~brief		Supprime un gestionnaire.
		 *\remarks		Vous *DEVEZ* appeler cette fonction lors de l'ajout d'un gestionnaire dans vos listes, si vous voulez qu'il soit mis à jour.
		 *\param[in]	p_handler	Le gestionnaire.
		 */
		inline void DoAddHandler( EventHandlerSPtr p_handler )
		{
			auto l_lock = Castor::make_unique_lock( m_mutexHandlers );

			if ( std::find( std::begin( m_handlers ), std::end( m_handlers ), p_handler ) != std::end( m_handlers ) )
			{
				CASTOR_EXCEPTION( "This handler already exists in the listener." );
			}

			m_handlers.push_back( p_handler );
		}
		/**
		 *\~english
		 *\~brief		Removes a handler.
		 *\remarks		You *MUST* call this function when adding a handler to your lists, if you want to have it updated.
		 *\param[in]	p_handler	The handler.
		 *\~french
		 *\~brief		Supprime un gestionnaire.
		 *\remarks		Vous *DEVEZ* appeler cette fonction lors de l'ajout d'un gestionnaire dans vos listes, si vous voulez qu'il soit mis à jour.
		 *\param[in]	p_handler	Le gestionnaire.
		 */
		inline void DoRemoveHandler( EventHandlerSPtr p_handler )
		{
			auto l_lock = Castor::make_unique_lock( m_mutexHandlers );
			m_handlers.erase( std::find( std::begin( m_handlers ), std::end( m_handlers ), p_handler ) );
		}

	private:
		/**
		 *\~english
		 *\~brief		Retrieves the first handler which can be targeted by mouse.
		 *\param[in]	p_position	The mouse position.
		 *\return		The handler, \p nullptr if none.
		 *\~french
		 *\~brief		Récupère le premier gestionnaire pouvant être ciblé par la souris.
		 *\param[in]	p_position	la position de la souris.
		 *\return		Le gestionnaire, \p nullptr si aucun.
		 */
		C3D_API virtual EventHandlerSPtr DoGetMouseTargetableHandler( Castor::Position const & p_position )const = 0;

		/**@name General */
		//@{

		/**
		 *copydoc		Castor3D::UserInputListener::Initialise
		 */
		C3D_API virtual bool DoInitialise() = 0;
		/**
		 *copydoc		Castor3D::UserInputListener::Cleanup
		 */
		C3D_API virtual void DoCleanup() = 0;

		//@}

	protected:
		//!\~english  The mutex used to protect the handlers array.	\~french Le mutex de protection du tableau de gestionnaires.
		mutable std::mutex m_mutexHandlers;
		//!\~english The handlers array.	\~french Le tableau de gestionnaires.
		std::vector< EventHandlerSPtr > m_handlers;

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
		//!\~english	The associated frame listener.
		//!\~french		Le frame listener associé.
		FrameListenerSPtr m_frameListener;
		//!\~english	The currently active handler.
		//!\~french		Le gestionnaire actif.
		EventHandlerWPtr m_activeHandler;
		//!\~english	The last mouse target handler.
		//!\~french		Le dernier gestionnaire cible de la souris.
		EventHandlerWPtr m_lastMouseTarget;
		//!\~english	The registered handler clicked event actions.
		//!\~french		Les actions enregistrées pour l'évènement handler cliqué.
		std::map< Castor::String, OnClickActionFunction > m_onClickActions;
		//!\~english	The registered handler sub element selected event actions.
		//!\~french		Les actions enregistrées pour l'évènement sous élément de handler sélectionné.
		std::map< Castor::String, OnSelectActionFunction > m_onSelectActions;
		//!\~english	The registered handler caption changed event actions.
		//!\~french		Les actions enregistrées pour l'évènement caption du handler changé.
		std::map< Castor::String, OnTextActionFunction > m_onTextActions;
	};
}

#endif
