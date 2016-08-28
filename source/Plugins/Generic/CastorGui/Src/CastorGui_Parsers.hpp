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
#ifndef ___CI_CASTOR_GUI_PARSERS_H___
#define ___CI_CASTOR_GUI_PARSERS_H___

#include "CastorGuiPrerequisites.hpp"

#include <FileParser/FileParser.hpp>
#include <FileParser/FileParserContext.hpp>

namespace CastorGui
{
	//! Scene file sections Enum
	/*!
	The enumeration which defines all the sections and subsections of a scene file
	*/
	typedef enum eSECTION
		: uint32_t
	{
		eSECTION_GUI = MAKE_SECTION_NAME( 'C', 'G', 'U', 'I' ),
		eSECTION_BUTTON = MAKE_SECTION_NAME( 'B', 'U', 'T', 'N' ),
		eSECTION_STATIC = MAKE_SECTION_NAME( 'S', 'T', 'T', 'C' ),
		eSECTION_SLIDER = MAKE_SECTION_NAME( 'S', 'L', 'D', 'R' ),
		eSECTION_COMBOBOX = MAKE_SECTION_NAME( 'C', 'M', 'B', 'O' ),
		eSECTION_LISTBOX = MAKE_SECTION_NAME( 'L', 'S', 'B', 'X' ),
		eSECTION_EDIT = MAKE_SECTION_NAME( 'E', 'D', 'I', 'T' ),
	}	eSECTION;

	DECLARE_ATTRIBUTE_PARSER( Parser_Gui )
	DECLARE_ATTRIBUTE_PARSER( Parser_DefaultFont )
	DECLARE_ATTRIBUTE_PARSER( Parser_GuiEnd )

	DECLARE_ATTRIBUTE_PARSER( Parser_Button )
	DECLARE_ATTRIBUTE_PARSER( Parser_ButtonFont )
	DECLARE_ATTRIBUTE_PARSER( Parser_ButtonCaption )
	DECLARE_ATTRIBUTE_PARSER( Parser_ButtonTextMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ButtonHighlightedBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ButtonHighlightedForegroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ButtonHighlightedTextMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ButtonPushedBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ButtonPushedForegroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ButtonPushedTextMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ButtonEnd )
	DECLARE_ATTRIBUTE_PARSER( Parser_ComboBox )
	DECLARE_ATTRIBUTE_PARSER( Parser_ComboBoxFont )
	DECLARE_ATTRIBUTE_PARSER( Parser_ComboBoxItem )
	DECLARE_ATTRIBUTE_PARSER( Parser_ComboBoxSelectedItemBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ComboBoxSelectedItemForegroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ComboBoxHighlightedItemBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ComboBoxEnd )
	DECLARE_ATTRIBUTE_PARSER( Parser_Edit )
	DECLARE_ATTRIBUTE_PARSER( Parser_EditFont )
	DECLARE_ATTRIBUTE_PARSER( Parser_EditCaption )
	DECLARE_ATTRIBUTE_PARSER( Parser_EditMultiLine )
	DECLARE_ATTRIBUTE_PARSER( Parser_EditEnd )
	DECLARE_ATTRIBUTE_PARSER( Parser_ListBox )
	DECLARE_ATTRIBUTE_PARSER( Parser_ListBoxFont )
	DECLARE_ATTRIBUTE_PARSER( Parser_ListBoxItem )
	DECLARE_ATTRIBUTE_PARSER( Parser_ListBoxSelectedItemBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ListBoxSelectedItemForegroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ListBoxHighlightedItemBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ListBoxEnd )
	DECLARE_ATTRIBUTE_PARSER( Parser_Slider )
	DECLARE_ATTRIBUTE_PARSER( Parser_SliderEnd )
	DECLARE_ATTRIBUTE_PARSER( Parser_Static )
	DECLARE_ATTRIBUTE_PARSER( Parser_StaticFont )
	DECLARE_ATTRIBUTE_PARSER( Parser_StaticCaption )
	DECLARE_ATTRIBUTE_PARSER( Parser_StaticEnd )

	DECLARE_ATTRIBUTE_PARSER( Parser_ControlPixelPosition )
	DECLARE_ATTRIBUTE_PARSER( Parser_ControlPixelSize )
	DECLARE_ATTRIBUTE_PARSER( Parser_ControlPixelBorderSize )
	DECLARE_ATTRIBUTE_PARSER( Parser_ControlBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ControlBorderMaterial )
	DECLARE_ATTRIBUTE_PARSER( Parser_ControlBorderInnerUv )
	DECLARE_ATTRIBUTE_PARSER( Parser_ControlBorderOuterUv )
	DECLARE_ATTRIBUTE_PARSER( Parser_ControlCenterUv )
	DECLARE_ATTRIBUTE_PARSER( Parser_ControlVisible )

}

#endif
