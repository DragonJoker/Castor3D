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
#ifndef ___C3D_CASTOR_GUI_PREREQUISITES_H___
#define ___C3D_CASTOR_GUI_PREREQUISITES_H___

#include <Graphics/Colour.hpp>

namespace Castor3D
{
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Supported events types enumeration.
	*\~french
	*\brief		Enum�ration des types �v�nements support�s.
	*/
	typedef enum eUSER_INPUT_EVENT_TYPE
	{
		//!\~english Mouse event type.	\~french Type d'�v�nement de souris.
		eUSER_INPUT_EVENT_TYPE_MOUSE,
		//!\~english Keyboard event type.	\~french Type d'�v�nement de clavier.
		eUSER_INPUT_EVENT_TYPE_KEYBOARD,
		//!\~english Keyboard event type.	\~french Type d'�v�nement de clavier.
		eEVENT_TYPE_HANDLER,
		CASTOR_ENUM_BOUNDS( eUSER_INPUT_EVENT_TYPE, eUSER_INPUT_EVENT_TYPE_MOUSE )
	}	eUSER_INPUT_EVENT_TYPE;
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Enumeration of supported mouse events.
	*\~english
	*\brief		Enum�ration des �v�nement de souris support�s.
	*/
	typedef enum eMOUSE_EVENT
		: uint8_t
	{
		//!\~english The mouse moves.	\~french La souris se d�place.
		eMOUSE_EVENT_MOVE,
		//!\~english The mouse enters an handler's zone.	\~french La souris entre dans la zone d'action d'un gestionnaire.
		eMOUSE_EVENT_ENTER,
		//!\~english The mouse remains still in an handler's zone during hover time.	\~french La souris reste immobile pendant un temps d�termin�, dans la zone d'action d'un gestionnaire.
		eMOUSE_EVENT_HOVER,
		//!\~english The mouse leaves an handler's zone.	\~french La souris quitte la zone d'action d'un gestionnaire.
		eMOUSE_EVENT_LEAVE,
		//!\~english One mouse button is pushed.	\~french Un bouton appuy�.
		eMOUSE_EVENT_BUTTON_PUSHED,
		//!\~english One mouse button is released.	\~french Un bouton rel�ch�.
		eMOUSE_EVENT_BUTTON_RELEASED,
		//!\~english One mouse wheel is rolled.	\~french Une roulette de la souris est tourn�e.
		eMOUSE_EVENT_WHEEL,
		CASTOR_ENUM_BOUNDS( eMOUSE_EVENT, eMOUSE_EVENT_MOVE )
	}	eMOUSE_EVENT;
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Enumeration of supported keyboard events.
	*\~english
	*\brief		Enum�ration des �v�nement clavier support�s.
	*/
	typedef enum eKEYBOARD_EVENT
		: uint8_t
	{
		//!\~english A key is pushed.	\~french Une touche est appuy�e.
		eKEYBOARD_EVENT_KEY_PUSHED,
		//!\~english A key is released.	\~french Une touche est rel�ch�e.
		eKEYBOARD_EVENT_KEY_RELEASED,
		//!\~english A displayable char key is stroked.	\~french Une touche de caract�re imprimable est tap�e.
		eKEYBOARD_EVENT_CHAR,
		CASTOR_ENUM_BOUNDS( eKEYBOARD_EVENT, eKEYBOARD_EVENT_KEY_PUSHED )
	}	eKEYBOARD_EVENT;
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Enumeration of supported handler events.
	*\~english
	*\brief		Enum�ration des �v�nements de gestionnaire support�es.
	*/
	typedef enum eHANDLER_EVENT
		: uint8_t
	{
		//!\~english Handler activated.	\~french Gestionnaire activ�.
		eHANDLER_EVENT_ACTIVATE,
		//!\~english Handler deactivated.	\~french Gestionnaire d�sactiv�.
		eHANDLER_EVENT_DEACTIVATE,
		CASTOR_ENUM_BOUNDS( eHANDLER_EVENT, eHANDLER_EVENT_ACTIVATE )
	}	eHANDLER_EVENT;
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Mouse buttons enumeration.
	*\~french
	*\brief		Enum�ration des boutons de la souris.
	*/
	typedef enum eMOUSE_BUTTON
	{
		//!\~english Left mouse button.	\~french Le bouton gauche.
		eMOUSE_BUTTON_LEFT,
		//!\~english Middle mouse button.	\~french Le bouton du milieu.
		eMOUSE_BUTTON_MIDDLE,
		//!\~english Right mouse button.	\~french Le bouton droit.
		eMOUSE_BUTTON_RIGHT,
		CASTOR_ENUM_BOUNDS( eMOUSE_BUTTON, eMOUSE_BUTTON_LEFT )
	}	eMOUSE_BUTTON;
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Keyboard keys enumeration.
	*\~french
	*\brief		Enum�ration des touches du clavier.
	*/
	typedef enum eKEYBOARD_KEY
	{
		eKEYBOARD_KEY_NONE,
		eKEY_BACKSPACE = 0x08,
		eKEY_TAB = 0x0A,
		eKEY_RETURN = 0x0D,
		eKEY_ESCAPE = 0x1B,
		eKEY_SPACE = 0x20,

		// values from 0x21 to 0x7E are reserved for the standard ASCII characters

		eKEY_DELETE = 0x7F,

		// values from 0x80 to 0xFF are reserved for ASCII extended characters

		eKEY_START   = 0x100,
		eKEY_LBUTTON,
		eKEY_RBUTTON,
		eKEY_CANCEL,
		eKEY_MBUTTON,
		eKEY_CLEAR,
		eKEY_SHIFT,
		eKEY_ALT,
		eKEY_CONTROL,
		eKEY_MENU,
		eKEY_PAUSE,
		eKEY_CAPITAL,
		eKEY_END,
		eKEY_HOME,
		eKEY_LEFT,
		eKEY_UP,
		eKEY_RIGHT,
		eKEY_DOWN,
		eKEY_SELECT,
		eKEY_PRINT,
		eKEY_EXECUTE,
		eKEY_SNAPSHOT,
		eKEY_INSERT,
		eKEY_HELP,
		eKEY_NUMPAD0,
		eKEY_NUMPAD1,
		eKEY_NUMPAD2,
		eKEY_NUMPAD3,
		eKEY_NUMPAD4,
		eKEY_NUMPAD5,
		eKEY_NUMPAD6,
		eKEY_NUMPAD7,
		eKEY_NUMPAD8,
		eKEY_NUMPAD9,
		eKEY_MULTIPLY,
		eKEY_ADD,
		eKEY_SEPARATOR,
		eKEY_SUBTRACT,
		eKEY_DECIMAL,
		eKEY_DIVIDE,
		eKEY_F1,
		eKEY_F2,
		eKEY_F3,
		eKEY_F4,
		eKEY_F5,
		eKEY_F6,
		eKEY_F7,
		eKEY_F8,
		eKEY_F9,
		eKEY_F10,
		eKEY_F11,
		eKEY_F12,
		eKEY_F13,
		eKEY_F14,
		eKEY_F15,
		eKEY_F16,
		eKEY_F17,
		eKEY_F18,
		eKEY_F19,
		eKEY_F20,
		eKEY_F21,
		eKEY_F22,
		eKEY_F23,
		eKEY_F24,
		eKEY_NUMLOCK,
		eKEY_SCROLL,
		eKEY_PAGEUP,
		eKEY_PAGEDOWN,
		eKEY_NUMPAD_SPACE,
		eKEY_NUMPAD_TAB,
		eKEY_NUMPAD_ENTER,
		eKEY_NUMPAD_F1,
		eKEY_NUMPAD_F2,
		eKEY_NUMPAD_F3,
		eKEY_NUMPAD_F4,
		eKEY_NUMPAD_HOME,
		eKEY_NUMPAD_LEFT,
		eKEY_NUMPAD_UP,
		eKEY_NUMPAD_RIGHT,
		eKEY_NUMPAD_DOWN,
		eKEY_NUMPAD_PAGEUP,
		eKEY_NUMPAD_PAGEDOWN,
		eKEY_NUMPAD_END,
		eKEY_NUMPAD_BEGIN,
		eKEY_NUMPAD_INSERT,
		eKEY_NUMPAD_DELETE,
		eKEY_NUMPAD_EQUAL,
		eKEY_NUMPAD_MULTIPLY,
		eKEY_NUMPAD_ADD,
		eKEY_NUMPAD_SEPARATOR,
		eKEY_NUMPAD_SUBTRACT,
		eKEY_NUMPAD_DECIMAL,
		eKEY_NUMPAD_DIVIDE,
	}	eKEYBOARD_KEY;

	class EventHandler;
	class UserInputEvent;
	class KeyboardEvent;
	class MouseEvent;
	class HandlerEvent;
	class UserInputListener;

	DECLARE_SMART_PTR( EventHandler );
	DECLARE_SMART_PTR( UserInputEvent );
	DECLARE_SMART_PTR( KeyboardEvent );
	DECLARE_SMART_PTR( MouseEvent );
	DECLARE_SMART_PTR( HandlerEvent );
	DECLARE_SMART_PTR( UserInputListener );

	template< class Derived > class NonClientEventHandler;
}

#endif
