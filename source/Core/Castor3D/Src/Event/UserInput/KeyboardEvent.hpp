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
	*\brief		Structure contenant l'�tat du clavier.
	*/
	struct KeyboardState
	{
		//!\~english The control key state.	\~french L'�tat de la touche Ctrl.
		bool m_ctrl;
		//!\~english The alt key state.	\~french L'�tat de la touche Alt.
		bool m_alt;
		//!\~english The shift key state.	\~french L'�tat de la touche Shift.
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
		 *\param[in]	p_ctrl	Dit si la touche Ctrl est enfonc�e.
		 *\param[in]	p_alt	Dit si la touche Alt est enfonc�e.
		 *\param[in]	p_shift	Dit si la touche Shift est enfonc�e.
		 */
		KeyboardEvent( KeyboardEventType p_type, KeyboardKey p_key, bool p_ctrl, bool p_alt, bool p_shift )
			: UserInputEvent{ UserInputEventType::eKeyboard }
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
		 *\param[in]	p_char	Le caract�re correspondant qu code de la touche.
		 *\param[in]	p_ctrl	Dit si la touche Ctrl est enfonc�e.
		 *\param[in]	p_alt	Dit si la touche Alt est enfonc�e.
		 *\param[in]	p_shift	Dit si la touche Shift est enfonc�e.
		 */
		KeyboardEvent( KeyboardEventType p_type, KeyboardKey p_key, Castor::String const & p_char, bool p_ctrl, bool p_alt, bool p_shift )
			: UserInputEvent{ UserInputEventType::eKeyboard }
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
		inline KeyboardEventType GetKeyboardEventType()const
		{
			return m_keyboardEventType;
		}
		/**
		*\~english
		 *\return		The key code.
		*\~french
		*\~return		Le code de la touche.
		 */
		inline KeyboardKey GetKey()const
		{
			return m_key;
		}
		/**
		*\~english
		 *\return		The character corresponding to the key code.
		*\~french
		*\~return		Le caract�re correspondant � la touche.
		 */
		inline Castor::String const & GetChar()const
		{
			return m_char;
		}
		/**
		*\~english
		 *\return		The Ctrl key down status.
		*\~french
		*\~return		L'�tat de la touche Ctrl.
		 */
		inline bool IsCtrlDown()const
		{
			return m_ctrl;
		}
		/**
		*\~english
		 *\return		The Alt key down status.
		*\~french
		*\~return		L'�tat de la touche Alt.
		 */
		inline bool IsAltDown()const
		{
			return m_alt;
		}
		/**
		*\~english
		 *\return		The Shift key down status.
		*\~french
		*\~return		L'�tat de la touche Shift.
		 */
		inline bool IsShiftDown()const
		{
			return m_shift;
		}

	private:
		//!\~english The keyboard event type.	\~french Le type d'évènement clavier.
		KeyboardEventType const m_keyboardEventType;
		//!\~english The key code.	\~french Le code de la touche.
		KeyboardKey const m_key;
		//!\~english The character.	\~french Le caract�re repr�sent� par la touche.
		Castor::String const m_char;
		//!\~english Tells if the Ctrl key is down.	\~french Dit si la touche Ctrl est enfonc�e.
		bool const m_ctrl;
		//!\~english Tells if the Alt key is down.	\~french Dit si la touche Alt est enfonc�e.
		bool const m_alt;
		//!\~english Tells if the Shift key is down.	\~french Dit si la touche Shift est enfonc�e.
		bool const m_shift;
	};
}

#endif
