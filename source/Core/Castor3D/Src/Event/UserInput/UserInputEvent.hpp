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
#ifndef ___C3D_GUI_EVENT_H___
#define ___C3D_GUI_EVENT_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Description of a user input event.
	*\remarks	You may use this one, but prefer using MouseEvent or KeyboardEvent.
	*\~french
	*\brief		Description d'un évènement de tpye entrée utilisateur.
	*\remarks	Vous pouvez utiliser cette classe, mais il est conseillé d'utiliser les  classes MouseEvent ou KeyboardEvent.
	*/
	class UserInputEvent
	{
	public:
		/**
		 *\~english
		 *\~brief		Constructor.
		 *\param[in]	p_type	The user input event type.
		 *\~french
		 *\~brief		Constructeur.
		 *\param[in]	p_type	Le type d'évènement entrée utilisateur.
		 */
		UserInputEvent( eUSER_INPUT_EVENT_TYPE p_type )
			: m_eventType( p_type )
		{
		}
		/**
		 *\~english
		 *\~brief		Destructor.
		 *\~french
		 *\~brief		Destructeur.
		 */
		virtual ~UserInputEvent()
		{
		}
		/**
		 *\~english
		 *\return		The user input event type.
		 *\~french
		 *\return		Le type d'évènement entrée utilisateur.
		 */
		inline eUSER_INPUT_EVENT_TYPE GetEventType()const
		{
			return m_eventType;
		}

	private:
		//!\~english The user input event type.	\~french Le type d'évènement entrée utilisateur.
		eUSER_INPUT_EVENT_TYPE m_eventType;
	};
}

#endif
