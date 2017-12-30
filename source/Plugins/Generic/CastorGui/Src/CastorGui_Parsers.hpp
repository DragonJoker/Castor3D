/*
See LICENSE file in root folder
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
	enum class GUISection
		: uint32_t
	{
		eGUI = MAKE_SECTION_NAME( 'C', 'G', 'U', 'I' ),
		eButton = MAKE_SECTION_NAME( 'B', 'U', 'T', 'N' ),
		eStatic = MAKE_SECTION_NAME( 'S', 'T', 'T', 'C' ),
		eSlider = MAKE_SECTION_NAME( 'S', 'L', 'D', 'R' ),
		eComboBox = MAKE_SECTION_NAME( 'C', 'M', 'B', 'O' ),
		eListBox = MAKE_SECTION_NAME( 'L', 'S', 'B', 'X' ),
		eEdit = MAKE_SECTION_NAME( 'E', 'D', 'I', 'T' ),
	};

	DECLARE_ATTRIBUTE_PARSER( parserGui )
	DECLARE_ATTRIBUTE_PARSER( parserDefaultFont )
	DECLARE_ATTRIBUTE_PARSER( parserGuiEnd )

	DECLARE_ATTRIBUTE_PARSER( parserButton )
	DECLARE_ATTRIBUTE_PARSER( parserButtonFont )
	DECLARE_ATTRIBUTE_PARSER( parserButtonCaption )
	DECLARE_ATTRIBUTE_PARSER( parserButtonTextMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserButtonHighlightedBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserButtonHighlightedForegroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserButtonHighlightedTextMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserButtonPushedBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserButtonPushedForegroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserButtonPushedTextMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserButtonHAlign )
	DECLARE_ATTRIBUTE_PARSER( parserButtonVAlign )
	DECLARE_ATTRIBUTE_PARSER( parserButtonEnd )
	DECLARE_ATTRIBUTE_PARSER( parserComboBox )
	DECLARE_ATTRIBUTE_PARSER( parserComboBoxFont )
	DECLARE_ATTRIBUTE_PARSER( parserComboBoxItem )
	DECLARE_ATTRIBUTE_PARSER( parserComboBoxSelectedItemBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserComboBoxSelectedItemForegroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserComboBoxHighlightedItemBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserComboBoxEnd )
	DECLARE_ATTRIBUTE_PARSER( parserEdit )
	DECLARE_ATTRIBUTE_PARSER( parserEditFont )
	DECLARE_ATTRIBUTE_PARSER( parserEditCaption )
	DECLARE_ATTRIBUTE_PARSER( parserEditMultiLine )
	DECLARE_ATTRIBUTE_PARSER( parserEditEnd )
	DECLARE_ATTRIBUTE_PARSER( parserListBox )
	DECLARE_ATTRIBUTE_PARSER( parserListBoxFont )
	DECLARE_ATTRIBUTE_PARSER( parserListBoxItem )
	DECLARE_ATTRIBUTE_PARSER( parserListBoxSelectedItemBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserListBoxSelectedItemForegroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserListBoxHighlightedItemBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserListBoxEnd )
	DECLARE_ATTRIBUTE_PARSER( parserSlider )
	DECLARE_ATTRIBUTE_PARSER( parserSliderEnd )
	DECLARE_ATTRIBUTE_PARSER( parserStatic )
	DECLARE_ATTRIBUTE_PARSER( parserStaticFont )
	DECLARE_ATTRIBUTE_PARSER( parserStaticCaption )
	DECLARE_ATTRIBUTE_PARSER( parserStaticHAlign )
	DECLARE_ATTRIBUTE_PARSER( parserStaticVAlign )
	DECLARE_ATTRIBUTE_PARSER( parserStaticEnd )

	DECLARE_ATTRIBUTE_PARSER( parserControlPixelPosition )
	DECLARE_ATTRIBUTE_PARSER( parserControlPixelSize )
	DECLARE_ATTRIBUTE_PARSER( parserControlPixelBorderSize )
	DECLARE_ATTRIBUTE_PARSER( parserControlBackgroundMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserControlBorderMaterial )
	DECLARE_ATTRIBUTE_PARSER( parserControlBorderInnerUv )
	DECLARE_ATTRIBUTE_PARSER( parserControlBorderOuterUv )
	DECLARE_ATTRIBUTE_PARSER( parserControlCenterUv )
	DECLARE_ATTRIBUTE_PARSER( parserControlVisible )

}

#endif
