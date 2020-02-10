/*
See LICENSE file in root folder
*/
#ifndef ___C3D_KEYBOARD_EVENT_H___
#define ___C3D_KEYBOARD_EVENT_H___

#include "Castor3D/Event/UserInput/UserInputEvent.hpp"

namespace castor3d
{
	struct KeyboardState;
	{
		//!\~english	The control key state.
		//!\~french		L'état de la touche Ctrl.
		bool m_ctrl;
		//!\~english	The alt key state.
		//!\~french		L'état de la touche Alt.
		bool m_alt;
		//!\~english	The shift key state.
		//!\~french		L'état de la touche Shift.
		bool m_shift;
	};

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
		//!\~english	The keyboard event type.
		//!\~french		Le type d'évènement clavier.
		KeyboardEventType const m_keyboardEventType;
		//!\~english	The key code.
		//!\~french		Le code de la touche.
		KeyboardKey const m_key;
		//!\~english	The character.
		//!\~french		Le caractère représenté par la touche.
		castor::String const m_char;
		//!\~english	Tells if the Ctrl key is down.
		//!\~french		Dit si la touche Ctrl est enfoncée.
		bool const m_ctrl;
		//!\~english	Tells if the Alt key is down.
		//!\~french		Dit si la touche Alt est enfoncée.
		bool const m_alt;
		//!\~english	Tells if the Shift key is down.
		//!\~french		Dit si la touche Shift est enfoncée.
		bool const m_shift;
	};
}

#endif
