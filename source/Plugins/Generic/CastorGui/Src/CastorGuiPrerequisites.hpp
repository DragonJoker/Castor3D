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
	const Castor::String PLUGIN_NAME = cuT( "CastorGui" );
	static const uint32_t DEFAULT_HEIGHT = 25;
	/*!
	 *\author		Sylvain DOREMUS
	 *\date		23/05/2012
	 *\version		0.1.0
	 *\brief		Enumeration of supported controls in a OptionsPanel
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
	 *\author		Sylvain DOREMUS
	 *\date		06/03/2015
	 *\version		0.1.0
	 *\brief		Enumeration of supported combobox events
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
	 *\author		Sylvain DOREMUS
	 *\date		06/03/2015
	 *\version		0.1.0
	 *\brief		Enumeration of supported listbox events
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
	 *\author		Sylvain DOREMUS
	 *\date		06/03/2015
	 *\version		0.1.0
	 *\brief		Enumeration of supported button events
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
	 *\author		Sylvain DOREMUS
	 *\date		06/03/2015
	 *\version		0.1.0
	 *\brief		Enumeration of supported slider events
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
	 *\author		Sylvain DOREMUS
	 *\date		06/03/2015
	 *\version		0.1.0
	 *\brief		Enumeration of supported edit events
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
	 *\author		Sylvain DOREMUS
	 *\date		11/02/2015
	 *\version		0.1.0
	 *\brief		Comb control supported styles
	*/
	typedef enum eCOMBO_STYLE
	: uint32_t
	{
		//! Read only combo box
		eCOMBO_STYLE_READONLY = 0x00000001,
	}	eCOMBO_STYLE;

	/*!
	 *\author		Sylvain DOREMUS
	 *\date		11/02/2015
	 *\version		0.1.0
	 *\brief		Edit control supported styles
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
	 *\author		Sylvain DOREMUS
	 *\date		11/02/2015
	 *\version		0.1.0
	 *\brief		Slider control supported styles
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
	 *\author		Sylvain DOREMUS
	 *\date		11/02/2015
	 *\version		0.1.0
	 *\brief		Static control supported styles
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
	 *\author		Sylvain DOREMUS
	 *\date		23/05/2012
	 *\version		0.1.0
	 *\brief		Enumeration of supported GUI events in ControlInfos
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
	 *\author		Sylvain DOREMUS
	 *\date		10/03/2015
	 *\version		0.1.0
	 *\brief		Mouse cursors enumeration
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

	template< class Derived > class EventHandler;
	class ControlsManager;

	class Control;
	class ButtonCtrl;
	class ComboBoxCtrl;
	class EditCtrl;
	class ListBoxCtrl;
	class SliderCtrl;
	class StaticCtrl;


	DECLARE_SMART_PTR( ControlsManager );
	DECLARE_SMART_PTR( Control );
	DECLARE_SMART_PTR( ButtonCtrl );
	DECLARE_SMART_PTR( ComboBoxCtrl );
	DECLARE_SMART_PTR( EditCtrl );
	DECLARE_SMART_PTR( ListBoxCtrl );
	DECLARE_SMART_PTR( SliderCtrl );
	DECLARE_SMART_PTR( StaticCtrl );

	typedef std::vector< ControlSPtr > ControlArray;

	/** Creates a colour material.
	 *\param[in]	p_engine	The engine.
	 *\param[in]	p_name		The material name.
	 *\param[in]	p_colour	The material colour.
	 *\return		The created material.
	*/
	Castor3D::MaterialSPtr CreateMaterial( Castor3D::Engine * p_engine, Castor::String const & p_name, Castor::Colour const & p_colour );

	/** Creates a texture material.
	 *\param[in]	p_engine	The engine.
	 *\param[in]	p_name		The material name.
	 *\param[in]	p_texture	The material texture.
	 *\return		The created material.
	*/
	Castor3D::MaterialSPtr CreateMaterial( Castor3D::Engine * p_engine, Castor::String const & p_name, Castor3D::TextureSPtr p_texture );
}

#endif
