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
#ifndef ___C3D_KEYBOARD_EVENT_H___
#define ___C3D_KEYBOARD_EVENT_H___

#include "UserInputEvent.hpp"

namespace Castor3D
{
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Structure holding mouse state.
	*\~french
	*\brief		Structure contenant l'état du clavier.
	*/
	struct KeyboardState
	{
		//!\~english The control key state.	\~french L'état de la touche Ctrl.
		bool m_ctrl;
		//!\~english The alt key state.	\~french L'état de la touche Alt.
		bool m_alt;
		//!\~english The shift key state.	\~french L'état de la touche Shift.
		bool m_shift;
	};
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Description of a keyboard event.
	*\~french
	*\brief		Description d'un évènement clavier.
	*/
	class KeyboardEvent
		: public UserInputEvent
	{
	public:
		/**
		*\~english
		*\~brief		Constructor.
		 *\param[in]	p_type	The keyboard event type.
		 *\param[in]	p_key	The key code.
		 *\param[in]	p_ctrl	Tells if the Ctrl key is down.
		 *\param[in]	p_alt	Tells if the Alt key is down.
		 *\param[in]	p_shift	Tells if the Shift key is down.
		*\~french
		*\~brief		Constructeur.
		 *\param[in]	p_type	Le type d'évènement clavier.
		 *\param[in]	p_key	Le code de la touche.
		 *\param[in]	p_ctrl	Dit si la touche Ctrl est enfoncée.
		 *\param[in]	p_alt	Dit si la touche Alt est enfoncée.
		 *\param[in]	p_shift	Dit si la touche Shift est enfoncée.
		 */
		KeyboardEvent( eKEYBOARD_EVENT p_type, eKEYBOARD_KEY p_key, bool p_ctrl, bool p_alt, bool p_shift )
			: UserInputEvent{ eUSER_INPUT_EVENT_TYPE_KEYBOARD }
			, m_keyboardEventType{ p_type }
			, m_key{ p_key }
			, m_ctrl{ p_ctrl }
			, m_alt{ p_alt }
			, m_shift{ p_shift }
			, m_char{}
		{
		}
		/**
		*\~english
		*\~brief		Constructor.
		 *\param[in]	p_type	The keyboard event type.
		 *\param[in]	p_key	The keycode.
		 *\param[in]	p_char	The character corresponding to the key code.
		 *\param[in]	p_ctrl	Tells if the Ctrl key is down.
		 *\param[in]	p_alt	Tells if the Alt key is down.
		 *\param[in]	p_shift	Tells if the Shift key is down.
		*\~french
		*\~brief		Constructeur.
		 *\param[in]	p_type	Le type d'évènement clavier.
		 *\param[in]	p_key	Le code de la touche.
		 *\param[in]	p_char	Le caractère correspondant qu code de la touche.
		 *\param[in]	p_ctrl	Dit si la touche Ctrl est enfoncée.
		 *\param[in]	p_alt	Dit si la touche Alt est enfoncée.
		 *\param[in]	p_shift	Dit si la touche Shift est enfoncée.
		 */
		KeyboardEvent( eKEYBOARD_EVENT p_type, eKEYBOARD_KEY p_key, Castor::String const & p_char, bool p_ctrl, bool p_alt, bool p_shift )
			: UserInputEvent{ eUSER_INPUT_EVENT_TYPE_KEYBOARD }
			, m_keyboardEventType{ p_type }
			, m_key{ p_key }
			, m_ctrl{ p_ctrl }
			, m_alt{ p_alt }
			, m_shift{ p_shift }
			, m_char{ p_char }
		{
		}
		/**
		*\~english
		*\~brief		Destructor.
		*\~french
		*\~brief		Destructeur.
		 */
		virtual ~KeyboardEvent()
		{
		}
		/**
		*\~english
		 *\return		The keyboard event type.
		*\~french
		*\~return		Le type d'évènement clavier.
		 */
		inline eKEYBOARD_EVENT GetKeyboardEventType()const
		{
			return m_keyboardEventType;
		}
		/**
		*\~english
		 *\return		The key code.
		*\~french
		*\~return		Le code de la touche.
		 */
		inline eKEYBOARD_KEY GetKey()const
		{
			return m_key;
		}
		/**
		*\~english
		 *\return		The character corresponding to the key code.
		*\~french
		*\~return		Le caractère correspondant à la touche.
		 */
		inline Castor::String const & GetChar()const
		{
			return m_char;
		}
		/**
		*\~english
		 *\return		The Ctrl key down status.
		*\~french
		*\~return		L'état de la touche Ctrl.
		 */
		inline bool IsCtrlDown()const
		{
			return m_ctrl;
		}
		/**
		*\~english
		 *\return		The Alt key down status.
		*\~french
		*\~return		L'état de la touche Alt.
		 */
		inline bool IsAltDown()const
		{
			return m_alt;
		}
		/**
		*\~english
		 *\return		The Shift key down status.
		*\~french
		*\~return		L'état de la touche Shift.
		 */
		inline bool IsShiftDown()const
		{
			return m_shift;
		}

	private:
		//!\~english The keyboard event type.	\~french Le type d'évènement clavier.
		eKEYBOARD_EVENT const m_keyboardEventType;
		//!\~english The key code.	\~french Le code de la touche.
		eKEYBOARD_KEY const m_key;
		//!\~english The character.	\~french Le caractère représenté par la touche.
		Castor::String const m_char;
		//!\~english Tells if the Ctrl key is down.	\~french Dit si la touche Ctrl est enfoncée.
		bool const m_ctrl;
		//!\~english Tells if the Alt key is down.	\~french Dit si la touche Alt est enfoncée.
		bool const m_alt;
		//!\~english Tells if the Shift key is down.	\~french Dit si la touche Shift est enfoncée.
		bool const m_shift;
	};
}

#endif
