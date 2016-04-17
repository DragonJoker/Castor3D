/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___C3D_MOUSE_EVENT_H___
#define ___C3D_MOUSE_EVENT_H___

#include "UserInputEvent.hpp"

#include <Position.hpp>

namespace Castor3D
{
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Structure holding mouse state.
	*\~french
	*\brief		Structure contenant l'état de la souris.
	*/
	struct MouseState
	{
		//!\~english The current position.	\~french La position actuelle.
		Castor::Position m_position;
		//!\~english The wheel position.	\~french La position de la molette.
		Castor::Position m_wheel;
		//!\~english The buttons state (true = down).	\~french L'état des boutons (true = enfoncé).
		std::array< bool, eMOUSE_BUTTON_COUNT > m_buttons;
		//!\~english The button which had the last change.	\~french Le bouton ayant reçu le dernier changement.
		eMOUSE_BUTTON m_changed;
	};
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Description of a mouse event.
	*\~french
	*\brief		Description d'un évènement de souris.
	*/
	class MouseEvent
		: public UserInputEvent
	{
	public:
		/**
		 *\~english
		 *\~brief		Constructor.
		 *\param[in]	p_type		The mouse event type.
		 *\param[in]	p_position	The position, of wheel or mouse, depending on the event type.
		 *\param[in]	p_button	The mouse button.
		 *\~french
		 *\~brief		Constructeur.
		 *\param[in]	p_type		Le type d'évènement de souris.
		 *\param[in]	p_position	La position, de la souris ou de la molette, selon le type d'évènement.
		 *\param[in]	p_button	Le bouton de la souris.
		 */
		MouseEvent( eMOUSE_EVENT p_type, Castor::Position const & p_position, eMOUSE_BUTTON p_button = eMOUSE_BUTTON_COUNT )
			: UserInputEvent{ eUSER_INPUT_EVENT_TYPE_MOUSE }
			, m_mouseEventType{ p_type }
			, m_position{ p_position }
			, m_button{ p_button }
		{
		}
		/**
		 *\~english
		 *\~brief		Destructor.
		 *\~french
		 *\~brief		Destructeur.
		 */
		virtual ~MouseEvent()
		{
		}
		/**
		 *\~english
		 *\return		The mouse event type.
		 *\~french
		 *\return		Le type d'évènement de souris.
		 */
		inline eMOUSE_EVENT GetMouseEventType()const
		{
			return m_mouseEventType;
		}
		/**
		 *\~english
		 *\return		The mouse button.
		 *\~french
		 *\return		Le bouton de la souris.
		 */
		inline eMOUSE_BUTTON GetButton()const
		{
			return m_button;
		}
		/**
		 *\~english
		 *\return		The position (wheel or mouse, depending on mouse event type).
		 *\~french
		 *\return		La position, de la souris ou de la molette, selon le type d'évènement.
		 */
		inline Castor::Position const & GetPosition()const
		{
			return m_position;
		}

	private:
		//!\~english The mouse event type.	\~french Le type d'évènement souris.
		eMOUSE_EVENT const m_mouseEventType;
		//!\~english The mouse button.	\~french Le bouton de la souris
		eMOUSE_BUTTON const m_button;
		//!\~english The position, (wheel or mouse, depending on mouse event type).	\~french La position (de la souris ou de la molette, selon le type d'évènement).
		Castor::Position const m_position;
	};
}

#endif
