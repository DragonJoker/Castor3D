/*
See LICENSE file in root folder
*/
#ifndef ___C3D_KeyboardEvent_H___
#define ___C3D_KeyboardEvent_H___

#include "Castor3D/Event/UserInput/UserInputEvent.hpp"

namespace castor3d
{
	class KeyboardEvent
		: public UserInputEvent
	{
	public:
		/**
		*\~english
		*\~brief		Constructor.
		 *\param[in]	type	The keyboard event type.
		 *\param[in]	key		The key code.
		 *\param[in]	ctrl	Tells if the Ctrl key is down.
		 *\param[in]	alt		Tells if the Alt key is down.
		 *\param[in]	shift	Tells if the Shift key is down.
		*\~french
		*\~brief		Constructeur.
		 *\param[in]	type	Le type d'évènement clavier.
		 *\param[in]	key		Le code de la touche.
		 *\param[in]	ctrl	Dit si la touche Ctrl est enfoncée.
		 *\param[in]	alt		Dit si la touche Alt est enfoncée.
		 *\param[in]	shift	Dit si la touche Shift est enfoncée.
		 */
		KeyboardEvent( KeyboardEventType type
			, KeyboardKey key
			, bool ctrl
			, bool alt
			, bool shift )
			: UserInputEvent{ UserInputEventType::eKeyboard }
			, m_keyboardEventType{ type }
			, m_key{ key }
			, m_ctrl{ ctrl }
			, m_alt{ alt }
			, m_shift{ shift }
			, m_char{}
		{
		}
		/**
		*\~english
		*\~brief		Constructor.
		 *\param[in]	type	The keyboard event type.
		 *\param[in]	key		The keycode.
		 *\param[in]	c		The character corresponding to the key code.
		 *\param[in]	ctrl	Tells if the Ctrl key is down.
		 *\param[in]	alt		Tells if the Alt key is down.
		 *\param[in]	shift	Tells if the Shift key is down.
		*\~french
		*\~brief		Constructeur.
		 *\param[in]	type	Le type d'évènement clavier.
		 *\param[in]	key		Le code de la touche.
		 *\param[in]	c		Le caractère correspondant qu code de la touche.
		 *\param[in]	ctrl	Dit si la touche Ctrl est enfoncée.
		 *\param[in]	alt		Dit si la touche Alt est enfoncée.
		 *\param[in]	shift	Dit si la touche Shift est enfoncée.
		 */
		KeyboardEvent( KeyboardEventType type
			, KeyboardKey key
			, castor::String const & c
			, bool ctrl
			, bool alt
			, bool shift )
			: UserInputEvent{ UserInputEventType::eKeyboard }
			, m_keyboardEventType{ type }
			, m_key{ key }
			, m_ctrl{ ctrl }
			, m_alt{ alt }
			, m_shift{ shift }
			, m_char{ c }
		{
		}
		/**
		*\~english
		 *\return		The keyboard event type.
		*\~french
		*\~return		Le type d'évènement clavier.
		 */
		inline KeyboardEventType getKeyboardEventType()const
		{
			return m_keyboardEventType;
		}
		/**
		*\~english
		 *\return		The key code.
		*\~french
		*\~return		Le code de la touche.
		 */
		inline KeyboardKey getKey()const
		{
			return m_key;
		}
		/**
		*\~english
		 *\return		The character corresponding to the key code.
		*\~french
		*\~return		Le caractère correspondant à la touche.
		 */
		inline castor::String const & getChar()const
		{
			return m_char;
		}
		/**
		*\~english
		 *\return		The Ctrl key down status.
		*\~french
		*\~return		L'état de la touche Ctrl.
		 */
		inline bool isCtrlDown()const
		{
			return m_ctrl;
		}
		/**
		*\~english
		 *\return		The Alt key down status.
		*\~french
		*\~return		L'état de la touche Alt.
		 */
		inline bool isAltDown()const
		{
			return m_alt;
		}
		/**
		*\~english
		 *\return		The Shift key down status.
		*\~french
		*\~return		L'état de la touche Shift.
		 */
		inline bool isShiftDown()const
		{
			return m_shift;
		}

	private:
		KeyboardEventType const m_keyboardEventType;
		KeyboardKey const m_key;
		castor::String const m_char;
		bool const m_ctrl;
		bool const m_alt;
		bool const m_shift;
	};
}

#endif
