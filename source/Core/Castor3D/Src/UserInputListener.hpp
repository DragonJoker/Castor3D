/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_USER_INPUT_LISTENER_H___
#define ___C3D_USER_INPUT_LISTENER_H___

#include "Castor3DPrerequisites.hpp"

#include <Position.hpp>
#include <FrameListener.hpp>

#include "EventHandler.hpp"

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

		/** Retrieves the active control
		*\return		The control
		*/
		inline EventHandlerSPtr GetActiveControl()const
		{
			return m_activeHandler.lock();
		}

		/** Retrieves the focused control
		*\return		The control
		*/
		inline EventHandlerSPtr GetFocusedControl()const
		{
			return m_lastMouseTarget.lock();
		}

		//@}
		/**@name Mouse events */
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
		C3D_API bool FireMouseButtonPushed( eMOUSE_BUTTON p_button );
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
		C3D_API bool FireMouseButtonReleased( eMOUSE_BUTTON p_button );
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
		/**@name Keyboard events */
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
		C3D_API bool FireKeyDown( eKEYBOARD_KEY p_key, bool p_ctrl, bool p_alt, bool p_shift );
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
		C3D_API bool FireKeyUp( eKEYBOARD_KEY p_key, bool p_ctrl, bool p_alt, bool p_shift );
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
		C3D_API bool FireChar( eKEYBOARD_KEY p_key, Castor::String const & p_char );

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
		//!\~english The current mouse state.	\~french L'état courant de la souris.
		MouseState m_mouse;
		//!\~english The current keyboard state.	\~french L'état courant du clavier.
		KeyboardState m_keyboard;
		//!\~english Tells if the listener needs to be refreshed or not.	\~french Dit si le listener doit être mis à jour.
		bool m_enabled;
		//!\~english The associated frame listener.	\~french Le frame listener associé.
		FrameListenerSPtr m_frameListener;
		//!\~english The currently active handler.	\~french Le gestionnaire actif.
		EventHandlerWPtr m_activeHandler;
		//!\~english The last mouse target handler.	\~french Le dernier gestionnaire cible de la souris.
		EventHandlerWPtr m_lastMouseTarget;
	};
}

#endif
