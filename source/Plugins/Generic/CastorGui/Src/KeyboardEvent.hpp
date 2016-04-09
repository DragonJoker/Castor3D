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
#ifndef ___CI_KEYBOARD_EVENT_H___
#define ___CI_KEYBOARD_EVENT_H___

#include "GuiEvent.hpp"

namespace CastorGui
{
	/*!
	 *\author		Sylvain DOREMU
	 *\date		02/03/201
	 *\version		0.1.
	 *\brief		Structure holding mouse stat
	*/
	struct KeyboardState
	{
		//! The control key state
		bool m_ctrl;
		//! The alt key state
		bool m_alt;
		//! The shift key state
		bool m_shift;
	};
	/*!
	 *\author		Sylvain DOREMU
	 *\version		0.1.
	 *\brief		Description of a keyboard even
	*/
	class KeyboardEvent
		: public GuiEvent
	{
	public:
		/** Constructor
		*\param[in]	p_type		The keyboard event typ
		*\param[in]	p_key		The ke
		*\param[in]	p_ctrl		Tells if the Ctrl key is dow
		*\param[in]	p_alt		Tells if the Alt key is dow
		*\param[in]	p_shift		Tells if the Shift key is dow
		*/
		KeyboardEvent( eKEYBOARD_EVENT p_type, eKEYBOARD_KEY p_key, bool p_ctrl, bool p_alt, bool p_shift )
			: GuiEvent( eEVENT_TYPE_KEYBOARD )
			, m_keyboardEventType( p_type )
			, m_key( p_key )
			, m_ctrl( p_ctrl )
			, m_alt( p_alt )
			, m_shift( p_shift )
		{
		}

		/** Constructor
		*\param[in]	p_type		The keyboard event typ
		*\param[in]	p_key		The ke
		*\param[in]	p_char		The character corresponding to the ke
		*\param[in]	p_ctrl		Tells if the Ctrl key is dow
		*\param[in]	p_alt		Tells if the Alt key is dow
		*\param[in]	p_shift		Tells if the Shift key is dow
		*/
		KeyboardEvent( eKEYBOARD_EVENT p_type, eKEYBOARD_KEY p_key, Castor::String const & p_char, bool p_ctrl, bool p_alt, bool p_shift )
			: GuiEvent( eEVENT_TYPE_KEYBOARD )
			, m_keyboardEventType( p_type )
			, m_key( p_key )
			, m_char( p_char )
			, m_ctrl( p_ctrl )
			, m_alt( p_alt )
			, m_shift( p_shift )
		{
		}

		/** Destructor
		*/
		virtual ~KeyboardEvent()
		{
		}

		/** Retrieves the event type
		 *\return		The typ
		*/
		inline eKEYBOARD_EVENT GetKeyboardEventType()const
		{
			return m_keyboardEventType;
		}

		/** Retrieves the key code
		 *\return		The ke
		*/
		inline eKEYBOARD_KEY GetKey()const
		{
			return m_key;
		}

		/** Retrieves the character
		 *\return		The ke
		*/
		inline Castor::String const & GetChar()const
		{
			return m_char;
		}

		/** Tells if the Ctrl key is down
		 *\return		The statu
		*/
		inline bool IsCtrlDown()const
		{
			return m_ctrl;
		}

		/** Tells if the Alt key is down
		 *\return		The statu
		*/
		inline bool IsAltDown()const
		{
			return m_alt;
		}

		/** Tells if the Shift key is down
		 *\return		The statu
		*/
		inline bool IsShiftDown()const
		{
			return m_shift;
		}

	private:
		//! The event type
		eKEYBOARD_EVENT m_keyboardEventType;
		//! The key code
		eKEYBOARD_KEY m_key;
		//! The character
		Castor::String m_char;
		//! Tells if the Ctrl key is down
		bool m_ctrl;
		//! Tells if the Alt key is down
		bool m_alt;
		//! Tells if the Shift key is down
		bool m_shift;
	};
}

#endif
