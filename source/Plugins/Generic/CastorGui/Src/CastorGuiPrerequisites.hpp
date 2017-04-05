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
#ifndef ___CI_CASTOR_GUI_PREREQUISITES_H___
#define ___CI_CASTOR_GUI_PREREQUISITES_H___

#include <Plugin/GenericPlugin.hpp>

#include <Design/Signal.hpp>
#include <Graphics/Colour.hpp>

#ifndef CASTOR_PLATFORM_WINDOWS
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
	enum class ControlType
		: uint8_t
	{
		eNone,
		//! StaticCtrl
		eStatic,
		//! EditCtrl
		eEdit,
		//! SliderCtrl
		eSlider,
		//! ComboBoxCtrl
		eComboBox,
		//! ListBoxCtrl
		eListBox,
		//! ButtonCtrl
		eButton,
	};
	/*!
	 *\author		Sylvain DOREMUS
	 *\date		06/03/2015
	 *\version		0.1.0
	 *\brief		Enumeration of supported combobox events
	*/
	enum class ComboBoxEvent
		: uint8_t
	{
		//! Item selected
		eSelected,
		//! The combo box events count
		eCount,
	};
	/*!
	 *\author		Sylvain DOREMUS
	 *\date		06/03/2015
	 *\version		0.1.0
	 *\brief		Enumeration of supported listbox events
	*/
	enum class ListBoxEvent
		: uint8_t
	{
		//! Item selected
		eSelected,
		//! The combobox events count
		eCount,
	};
	/*!
	 *\author		Sylvain DOREMUS
	 *\date		06/03/2015
	 *\version		0.1.0
	 *\brief		Enumeration of supported button events
	*/
	enum class ButtonEvent
		: uint8_t
	{
		//! Button clicked
		eClicked,
		//! The button events count
		eCount,
	};
	/*!
	 *\author		Sylvain DOREMUS
	 *\date		06/03/2015
	 *\version		0.1.0
	 *\brief		Enumeration of supported slider events
	*/
	enum class SliderEvent
		: uint8_t
	{
		//! Slider thumb release
		eThumbRelease,
		//! Slider thumb track
		eThumbTrack,
		//! The slider events count
		eCount,
	};
	/*!
	 *\author		Sylvain DOREMUS
	 *\date		06/03/2015
	 *\version		0.1.0
	 *\brief		Enumeration of supported edit events
	*/
	enum class EditEvent
		: uint8_t
	{
		//! The edit text has changed
		eUpdated,
		//! The return key has been hit (the edit must have EditStyle::eProcessEnter style)
		eReturn,
		//! The edit events count
		eCount,
	};
	/*!
	 *\author		Sylvain DOREMUS
	 *\date		11/02/2015
	 *\version		0.1.0
	 *\brief		Comb control supported styles
	*/
	enum class ComboBoxStyle
		: uint32_t
	{
		//! Read only combo box
		eReadOnly = 0x00000001,
	};
	/*!
	 *\author		Sylvain DOREMUS
	 *\date		11/02/2015
	 *\version		0.1.0
	 *\brief		Edit control supported styles
	*/
	enum class EditStyle
		: uint32_t
	{
		//! The edit control process 'return' hit as an event and not as a newline
		eProcessEnter = 0x00000100,
		//! The edit control is a multiline edit control
		eMultiline = 0x00000200,
		//! The edit control process 'tab' hit as the tab character and not as an event
		eProcessTab = 0x00000400,
		//! Words are not cut
		eWordWrap = 0x00000800,
	};
	/*!
	 *\author		Sylvain DOREMUS
	 *\date		11/02/2015
	 *\version		0.1.0
	 *\brief		Slider control supported styles
	*/
	enum class SliderStyle
		: uint32_t
	{
		// The slider is displaye horizontally (default mode)
		eHorizontal = 0x00010000,
		// The slider is displaye vertically
		eVertical = 0x000200000,
	};
	/*!
	 *\author		Sylvain DOREMUS
	 *\date		11/02/2015
	 *\version		0.1.0
	 *\brief		Static control supported styles
	*/
	enum class StaticStyle
		: uint32_t
	{
		//! The static's text is left aligned
		eHAlignLeft = 0x01000000,
		//! The static's text is horizontally centered
		eHAlignCenter = 0x02000000,
		//! The static's text is right aligned
		eHAlignRight = 0x04000000,
		//! The static's text is aligned on top
		eVAlignTop = 0x10000000,
		//! The static's text is vertically centered
		eVAlignCenter = 0x20000000,
		//! The static's text is aligned on bottom
		eVAlignBottom = 0x40000000,
	};
	/*!
	 *\author		Sylvain DOREMUS
	 *\date		23/05/2012
	 *\version		0.1.0
	 *\brief		Enumeration of supported GUI events in ControlInfos
	*/
	enum class GuiEvent
		: uint8_t
	{
		//! Event to ask the main window to show a message box
		eShowMessageBox,
		//! Event to ask the main window to resize to the given size
		eResizeParent,
	};
	/*!
	 *\author		Sylvain DOREMUS
	 *\date		10/03/2015
	 *\version		0.1.0
	 *\brief		Mouse cursors enumeration
	*/
	enum class MouseCursor
	{
		//! The usual arrow cursor
		eArrow,
		//! The hand cursor
		eHand,
		//! The text cursor
		eText,
		//! The cursors count
		eCount,
	};

	template< class Derived > class EventHandler;
	class ControlsManager;

	class Control;
	class ButtonCtrl;
	class ComboBoxCtrl;
	class EditCtrl;
	class ListBoxCtrl;
	class SliderCtrl;
	class StaticCtrl;

	using OnButtonEventFunction = std::function< void() >;
	using OnButtonEvent = Castor::Signal< OnButtonEventFunction >;
	using OnButtonEventConnection = OnButtonEvent::connection;

	using OnComboEventFunction = std::function< void( int ) >;
	using OnComboEvent = Castor::Signal< OnComboEventFunction >;
	using OnComboEventConnection = OnComboEvent::connection;

	using OnEditEventFunction = std::function< void( Castor::String const & ) >;
	using OnEditEvent = Castor::Signal< OnEditEventFunction >;
	using OnEditEventConnection = OnEditEvent::connection;

	using OnListEventFunction = std::function< void( int ) >;
	using OnListEvent = Castor::Signal< OnListEventFunction >;
	using OnListEventConnection = OnListEvent::connection;

	using OnSliderEventFunction = std::function< void( int ) >;
	using OnSliderEvent = Castor::Signal< OnSliderEventFunction >;
	using OnSliderEventConnection = OnSliderEvent::connection;

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
	Castor3D::MaterialSPtr CreateMaterial( Castor3D::Engine & p_engine, Castor::String const & p_name, Castor::Colour const & p_colour );

	/** Creates a texture material.
	 *\param[in]	p_engine	The engine.
	 *\param[in]	p_name		The material name.
	 *\param[in]	p_texture	The material texture.
	 *\return		The created material.
	*/
	Castor3D::MaterialSPtr CreateMaterial( Castor3D::Engine & p_engine, Castor::String const & p_name, Castor3D::TextureLayoutSPtr p_texture );
}

#endif
