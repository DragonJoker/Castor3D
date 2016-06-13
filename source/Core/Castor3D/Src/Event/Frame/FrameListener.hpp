/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_FRAME_LISTENER_H___
#define ___C3D_FRAME_LISTENER_H___

#include "Castor3DPrerequisites.hpp"

#include <Named.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		03/03/2010
	\~english
	\brief		User event synchronisation class
	\remark		The manager of the frame events. It can add frame events and applies them at the wanted times
	\~french
	\brief		Classe de synchronisation des évènements
	\remark		Le gestionnaire des évènements de frame, on peut y ajouter des évènements à traiter, qui le seront au moment voulu (en fonction de leur eEVENT_TYPE)
	*/
	class FrameListener
		: public Castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	The listener's name.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom du listener
		 */
		C3D_API explicit FrameListener( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~FrameListener();
		/**
		 *\~english
		 *\brief		Empties the event lists.
		 *\~french
		 *\brief		Vide les listes d'évènements.
		 */
		C3D_API void Flush();
		/**
		 *\~english
		 *\brief		Puts an event in the corresponding array
		 *\param[in]	p_event	The event to put
		 *\~french
		 *\brief		Ajoute un évènement à la liste d'évènements correspondant à sont type
		 *\param[in]	p_event	L'évènement à ajouter
		 */
		C3D_API void PostEvent( FrameEventSPtr p_event );
		/**
		 *\~english
		 *\brief		Applies all events of a given type, then discards them
		 *\param[in]	p_type	The type of events to fire
		 *\~french
		 *\brief		Traite tous les évènements d'un type donné
		 *\param[in]	p_type	Le type des évènements à traiter
		 *\return
		 */
		C3D_API bool FireEvents( eEVENT_TYPE p_type );

	protected:
		/**
		 *\~english
		 *\brief		Empties the event lists.
		 *\~french
		 *\brief		Vide les listes d'évènements.
		 */
		C3D_API virtual void DoFlush() {}

	protected:
		//!\~english The events arrays	\~french Les tableaux d'évènements
		std::array< FrameEventPtrArray,	eEVENT_TYPE_COUNT > m_events;
		//!\~english Mutex to make this class thread safe	\~french Mutex pour rendre cette classe thread safe
		std::recursive_mutex m_mutex;
	};
}

#endif
