/*
See LICENSE file in root folder
*/
#ifndef ___C3D_KEYBOARD_EVENT_H___
#define ___C3D_KEYBOARD_EVENT_H___

#include "UserInputEvent.hpp"

namespace castor3d
{
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Structure holding mouse state.
	*\~french
	*\brief		Structure contenant l'àtat du clavier.
	*/
	struct KeyboardState
	{
		//!\~english The control key state.	\~french L'àtat de la touche Ctrl.
		bool m_ctrl;
		//!\~english The alt key state.	\~french L'àtat de la touche Alt.
		bool m_alt;
		//!\~english The shift key state.	\~french L'àtat de la touche Shift.
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
		 *\param[in]	p_ctrl	Dit si la touche Ctrl est enfoncàe.
		 *\param[in]	p_alt	Dit si la touche Alt est enfoncàe.
		 *\param[in]	p_shift	Dit si la touche Shift est enfoncàe.
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
		 *\param[in]	p_char	Le caractàre correspondant qu code de la touche.
		 *\param[in]	p_ctrl	Dit si la touche Ctrl est enfoncàe.
		 *\param[in]	p_alt	Dit si la touche Alt est enfoncàe.
		 *\param[in]	p_shift	Dit si la touche Shift est enfoncàe.
		 */
		KeyboardEvent( KeyboardEventType p_type, KeyboardKey p_key, castor::String const & p_char, bool p_ctrl, bool p_alt, bool p_shift )
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
		*\~return		Le caractàre correspondant à la touche.
		 */
		inline castor::String const & getChar()const
		{
			return m_char;
		}
		/**
		*\~english
		 *\return		The Ctrl key down status.
		*\~french
		*\~return		L'àtat de la touche Ctrl.
		 */
		inline bool isCtrlDown()const
		{
			return m_ctrl;
		}
		/**
		*\~english
		 *\return		The Alt key down status.
		*\~french
		*\~return		L'àtat de la touche Alt.
		 */
		inline bool isAltDown()const
		{
			return m_alt;
		}
		/**
		*\~english
		 *\return		The Shift key down status.
		*\~french
		*\~return		L'àtat de la touche Shift.
		 */
		inline bool isShiftDown()const
		{
			return m_shift;
		}

	private:
		//!\~english The keyboard event type.	\~french Le type d'évènement clavier.
		KeyboardEventType const m_keyboardEventType;
		//!\~english The key code.	\~french Le code de la touche.
		KeyboardKey const m_key;
		//!\~english The character.	\~french Le caractàre repràsentà par la touche.
		castor::String const m_char;
		//!\~english Tells if the Ctrl key is down.	\~french Dit si la touche Ctrl est enfoncàe.
		bool const m_ctrl;
		//!\~english Tells if the Alt key is down.	\~french Dit si la touche Alt est enfoncàe.
		bool const m_alt;
		//!\~english Tells if the Shift key is down.	\~french Dit si la touche Shift est enfoncàe.
		bool const m_shift;
	};
}

#endif
