/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___CI_CASTOR_GUI_PREREQUISITES_H___
#define ___CI_CASTOR_GUI_PREREQUISITES_H___

#include <Castor3DPrerequisites.hpp>
#include <GenericPlugin.hpp>

#include <Colour.hpp>

#ifndef _WIN32
#	define C3D_CGui_API
#else
#	ifdef CastorGui_EXPORTS
#		define C3D_CGui_API __declspec( dllexport )
#	else
#		define C3D_CGui_API __declspec( dllimport )
#	endif
#endif

namespace CastorGui
{
	const int DEFAULT_HEIGHT = 25;
	const Castor::String DEFAULT_FONT_NAME = _T( "Arial" );
	const uint32_t DEFAULT_FONT_HEIGHT = 11;
	/*!
	@author
		Sylvain DOREMUS
	@date
		23/05/2012
	@version
		1.3.0.0
	@brief
		Enumeration of supported controls in a OptionsPanel
	*/
	typedef enum eCONTROL_TYPE
		: uint8_t
	{
		eCONTROL_TYPE_NONE,
		//! StaticCtrl
		eCONTROL_TYPE_STATIC,
		//! EditCtrl
		eCONTROL_TYPE_EDIT,
		//! SliderCtrl
		eCONTROL_TYPE_SLIDER,
		//! ComboBoxCtrl
		eCONTROL_TYPE_COMBO,
		//! ListBoxCtrl
		eCONTROL_TYPE_LIST,
		//! ButtonCtrl
		eCONTROL_TYPE_BUTTON,
	}	eCONTROL_TYPE;

	/*!
	@author
		Sylvain DOREMUS
	@date
		06/03/2015
	@version
		2.0.0
	@brief
		Enumeration of supported combobox events
	*/
	typedef enum eCOMBOBOX_EVENT
		: uint8_t
	{
		//! Item selected
		eCOMBOBOX_EVENT_SELECTED,
		//! The combo box events count
		eCOMBOBOX_EVENT_COUNT,
	}	eCOMBOBOX_EVENT;

	/*!
	@author
		Sylvain DOREMUS
	@date
		06/03/2015
	@version
		2.0.0
	@brief
		Enumeration of supported listbox events
	*/
	typedef enum eLISTBOX_EVENT
		: uint8_t
	{
		//! Item selected
		eLISTBOX_EVENT_SELECTED,
		//! The combobox events count
		eLISTBOX_EVENT_COUNT,
	}	eLISTBOX_EVENT;

	/*!
	@author
		Sylvain DOREMUS
	@date
		06/03/2015
	@version
		2.0.0
	@brief
		Enumeration of supported button events
	*/
	typedef enum eBUTTON_EVENT
		: uint8_t
	{
		//! Button clicked
		eBUTTON_EVENT_CLICKED,
		//! The button events count
		eBUTTON_EVENT_COUNT,
	}	eBUTTON_EVENT;

	/*!
	@author
		Sylvain DOREMUS
	@date
		06/03/2015
	@version
		2.0.0
	@brief
		Enumeration of supported slider events
	*/
	typedef enum eSLIDER_EVENT
		: uint8_t
	{
		//! Slider thumb release
		eSLIDER_EVENT_THUMBRELEASE,
		//! Slider thumb track
		eSLIDER_EVENT_THUMBTRACK,
		//! The slider events count
		eSLIDER_EVENT_COUNT,
	}	eSLIDER_EVENT;

	/*!
	@author
		Sylvain DOREMUS
	@date
		06/03/2015
	@version
		2.0.0
	@brief
		Enumeration of supported edit events
	*/
	typedef enum eEDIT_EVENT
		: uint8_t
	{
		//! The edit text has changed
		eEDIT_EVENT_UPDATED,
		//! The return key has been hit (the edit must have eEDIT_STYLE_PROCESS_ENTER style)
		eEDIT_EVENT_RETURN,
		//! The edit events count
		eEDIT_EVENT_COUNT,
	}	eEDIT_EVENT;

	/*!
	@author
		Sylvain DOREMUS
	@date
		11/02/2015
	@version
		2.0.0
	@brief
		Comb control supported styles
	*/
	typedef enum eCOMBO_STYLE
		: uint32_t
	{
		//! Read only combo box
		eCOMBO_STYLE_READONLY = 0x00000001,
	}	eCOMBO_STYLE;

	/*!
	@author
		Sylvain DOREMUS
	@date
		11/02/2015
	@version
		2.0.0
	@brief
		Edit control supported styles
	*/
	typedef enum eEDIT_STYLE
		: uint32_t
	{
		//! The edit control process 'return' hit as an event and not as a newline
		eEDIT_STYLE_PROCESS_ENTER = 0x00000100,
		//! The edit control is a multiline edit control
		eEDIT_STYLE_MULTILINE = 0x00000200,
		//! The edit control process 'tab' hit as the tab character and not as an event
		eEDIT_STYLE_PROCESS_TAB = 0x00000400,
		//! Words are not cut
		eEDIT_STYLE_WORDWRAP = 0x00000800,
	}	eEDIT_STYLE;

	/*!
	@author
		Sylvain DOREMUS
	@date
		11/02/2015
	@version
		2.0.0
	@brief
		Slider control supported styles
	*/
	typedef enum eSLIDER_STYLE
		: uint32_t
	{
		// The slider is displaye horizontally (default mode)
		eSLIDER_STYLE_HORIZONTAL = 0x00010000,
		// The slider is displaye vertically
		eSLIDER_STYLE_VERTICAL = 0x000200000,
	}	eSLIDER_STYLE;

	/*!
	@author
		Sylvain DOREMUS
	@date
		11/02/2015
	@version
		2.0.0
	@brief
		Static control supported styles
	*/
	typedef enum eSTATIC_STYLE
		: uint32_t
	{
		//! The static's text is left aligned
		eSTATIC_STYLE_HALIGN_LEFT	= 0x01000000,
		//! The static's text is horizontally centered
		eSTATIC_STYLE_HALIGN_CENTER	= 0x02000000,
		//! The static's text is right aligned
		eSTATIC_STYLE_HALIGN_RIGHT	= 0x04000000,
		//! The static's text is aligned on top
		eSTATIC_STYLE_VALIGN_TOP	= 0x10000000,
		//! The static's text is vertically centered
		eSTATIC_STYLE_VALIGN_CENTER	= 0x20000000,
		//! The static's text is aligned on bottom
		eSTATIC_STYLE_VALIGN_BOTTOM	= 0x40000000,
	}	eSTATIC_STYLE;

	/*!
	@author
		Sylvain DOREMUS
	@date
		23/05/2012
	@version
		1.3.0.0
	@brief
		Enumeration of supported GUI events in ControlInfos
	*/
	typedef enum eGUI_EVENT
		: uint8_t
	{
		//! Event to ask the main window to show a message box
		eGUI_EVENT_SHOW_MESSAGE_BOX,
		//! Event to ask the main window to resize to the given size
		eGUI_EVENT_RESIZE_PARENT,
	}	eGUI_EVENT;

	/*!
	@author
		Sylvain DOREMUS
	@date
		02/03/2015
	@version
		2.0.0
	@brief
		Mouse buttons enumeration
	*/
	typedef enum eMOUSE_BUTTON
	{
		//! The left button
		eMOUSE_BUTTON_LEFT,
		//! The middle button
		eMOUSE_BUTTON_MIDDLE,
		//! The right button
		eMOUSE_BUTTON_RIGHT,
		//! The buttons count
		eMOUSE_BUTTON_COUNT,
	}	eMOUSE_BUTTON;

	/*!
	@author
		Sylvain DOREMUS
	@date
		02/03/2015
	@version
		2.0.0
	@brief
		Keyboard keys enumeration
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

	/*!
	@author
		Sylvain DOREMUS
	@date
		10/03/2015
	@version
		2.0.0
	@brief
		Mouse cursors enumeration
	*/
	typedef enum eMOUSE_CURSOR
	{
		//! The usual arrow cursor
		eMOUSE_CURSOR_ARROW,
		//! The hand cursor
		eMOUSE_CURSOR_HAND,
		//! The text cursor
		eMOUSE_CURSOR_TEXT,
		//! The cursors count
		eMOUSE_CURSOR_COUNT,
	}	eMOUSE_CURSOR;

	/*!
	@author
		Sylvain DOREMUS
	@date
		12/03/2015
	@version
		2.0.0
	@brief
		Enumeration of supported control events
	*/
	typedef enum eCONTROL_EVENT
		: uint8_t
	{
		//! Control activated
		eCONTROL_EVENT_ACTIVATE,
		//! Control deactivated
		eCONTROL_EVENT_DEACTIVATE,
		//! The control events count
		eCONTROL_EVENT_COUNT,
	}	eCONTROL_EVENT;

	/*!
	@author
		Sylvain DOREMUS
	@date
		02/03/2015
	@version
		2.0.0
	@brief
		Supported mouse events enumeration
	*/
	typedef enum eMOUSE_EVENT
	{
		//! The mouse enters into the control client area
		eMOUSE_EVENT_MOUSE_ENTER,
		//! The mouse leaves the control client area
		eMOUSE_EVENT_MOUSE_LEAVE,
		//! The mouse moves into the control client area
		eMOUSE_EVENT_MOUSE_MOVE,
		//! The mouse remains into the client area during hover time
		eMOUSE_EVENT_MOUSE_HOVER,
		//! One mouse button is pushed
		eMOUSE_EVENT_MOUSE_BUTTON_PUSHED,
		//! One mouse button is released
		eMOUSE_EVENT_MOUSE_BUTTON_RELEASED,
		//! One mouse wheel is rolled
		eMOUSE_EVENT_MOUSE_WHEEL,
		//! The mouse events count
		eMOUSE_EVENT_COUNT,
	}	eMOUSE_EVENT;

	/*!
	@author
		Sylvain DOREMUS
	@date
		02/03/2015
	@version
		2.0.0
	@brief
		Supported keyboard events enumeration
	*/
	typedef enum eKEYBOARD_EVENT
	{
		//! A key is pushed
		eKEYBOARD_EVENT_KEY_PUSHED,
		//! A key is released
		eKEYBOARD_EVENT_KEY_RELEASED,
		//! A printable key is pressed, the KeyboardEvent will hold the character to display
		eKEYBOARD_EVENT_CHAR,
		//! The keyboard events count
		eKEYBOARD_EVENT_COUNT,
	}	eKEYBOARD_EVENT;

	template< class Derived > class EventHandler;
	class ControlsManager;
	class Control;
	class ButtonCtrl;
	class ComboBoxCtrl;
	class EditCtrl;
	class ListBoxCtrl;
	class SliderCtrl;
	class StaticCtrl;
	typedef std::vector< std::shared_ptr< Control > > ControlArray;


	/** Creates a panel overlay
	@param[in] p_engine
		The engine
	@param[in] p_name
		The overlay name
	@param[in] p_position
		The overlay position
	@param[in] p_size
		The overlay size
	@param[in] p_material
		The overlay material
	@param[in] p_parent
		The parent overlay
	@return
		The created overlay
	*/
	Castor3D::PanelOverlaySPtr CreatePanel( Castor3D::Engine * p_engine, Castor::String const & p_name, Castor::Point2d const & p_position, Castor::Point2d const & p_size, Castor3D::MaterialSPtr p_material, Castor3D::OverlaySPtr p_parent = nullptr );

	/** Creates a panel overlay
	@param[in] p_engine
		The engine
	@param[in] p_name
		The overlay name
	@param[in] p_position
		The overlay pixel position
	@param[in] p_size
		The overlay pixel size
	@param[in] p_material
		The overlay material
	@param[in] p_parent
		The parent overlay
	@return
		The created overlay
	*/
	Castor3D::PanelOverlaySPtr CreatePanel( Castor3D::Engine * p_engine, Castor::String const & p_name, Castor::Position const & p_position, Castor::Size const & p_size, Castor3D::MaterialSPtr p_material, Castor3D::OverlaySPtr p_parent = nullptr );

	/** Creates a border panel overlay
	@param[in] p_engine
		The engine
	@param[in] p_name
		The overlay name
	@param[in] p_position
		The overlay position
	@param[in] p_size
		The overlay size
	@param[in] p_material
		The overlay material
	@param[in] p_bordersSize
		The overlay borders size
	@param[in] p_bordersMaterial
		The overlay borders material
	@param[in] p_parent
		The parent overlay
	@return
		The created overlay
	*/
	Castor3D::BorderPanelOverlaySPtr CreateBorderPanel( Castor3D::Engine * p_engine, Castor::String const & p_name, Castor::Point2d const & p_position, Castor::Point2d const & p_size, Castor3D::MaterialSPtr p_material, Castor::Point4d const & p_bordersSize, Castor3D::MaterialSPtr p_bordersMaterial, Castor3D::OverlaySPtr p_parent = nullptr );

	/** Creates a panel overlay
	@param[in] p_engine
		The engine
	@param[in] p_name
		The overlay name
	@param[in] p_position
		The overlay pixel position
	@param[in] p_size
		The overlay pixel size
	@param[in] p_material
		The overlay material
	@param[in] p_bordersSize
		The overlay borders pixel size
	@param[in] p_bordersMaterial
		The overlay borders material
	@param[in] p_parent
		The parent overlay
	@return
		The created overlay
	*/
	Castor3D::BorderPanelOverlaySPtr CreateBorderPanel( Castor3D::Engine * p_engine, Castor::String const & p_name, Castor::Position const & p_position, Castor::Size const & p_size, Castor3D::MaterialSPtr p_material, Castor::Rectangle const & p_bordersSize, Castor3D::MaterialSPtr p_bordersMaterial, Castor3D::OverlaySPtr p_parent = nullptr );

	/** Creates a border panel overlay
	@param[in] p_engine
		The engine
	@param[in] p_name
		The overlay name
	@param[in] p_position
		The overlay position
	@param[in] p_size
		The overlay size
	@param[in] p_material
		The overlay material
	@param[in] p_font
		The overlay font
	@param[in] p_parent
		The parent overlay
	@return
		The created overlay
	*/
	Castor3D::TextOverlaySPtr CreateText( Castor3D::Engine * p_engine, Castor::String const & p_name, Castor::Point2d const & p_position, Castor::Point2d const & p_size, Castor3D::MaterialSPtr p_material, Castor::FontSPtr p_font, Castor3D::OverlaySPtr p_parent = nullptr );

	/** Creates a panel overlay
	@param[in] p_engine
		The engine
	@param[in] p_name
		The overlay name
	@param[in] p_position
		The overlay pixel position
	@param[in] p_size
		The overlay pixel size
	@param[in] p_material
		The overlay material
	@param[in] p_font
		The overlay font
	@param[in] p_parent
		The parent overlay
	@return
		The created overlay
	*/
	Castor3D::TextOverlaySPtr CreateText( Castor3D::Engine * p_engine, Castor::String const & p_name, Castor::Position const & p_position, Castor::Size const & p_size, Castor3D::MaterialSPtr p_material, Castor::FontSPtr p_font, Castor3D::OverlaySPtr p_parent = nullptr );
}

#endif
