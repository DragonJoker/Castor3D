/*
See LICENSE file in root folder
*/
#ifndef ___CI_CASTOR_GUI_PARSERS_H___
#define ___CI_CASTOR_GUI_PARSERS_H___

#include "CastorGui/CastorGuiPrerequisites.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

#include <stack>

namespace CastorGui
{
	struct ParserContext
	{
		std::stack< ControlSPtr > m_parents;
		castor3d::Engine * m_engine{};
		ButtonCtrlSPtr m_button;
		ComboBoxCtrlSPtr m_combo;
		EditCtrlSPtr m_edit;
		ListBoxCtrlSPtr m_listbox;
		SliderCtrlSPtr m_slider;
		StaticCtrlSPtr m_static;
		uint32_t m_flags{};
		uint32_t m_ctrlId{};

		C3D_CGui_API ControlRPtr getTop()const;
		C3D_CGui_API void Pop();
	};

	//! Scene file sections Enum
	/**
	The enumeration which defines all the sections and subsections of a scene file
	*/
	enum class GUISection
		: uint32_t
	{
		eGUI = CU_MakeSectionName( 'C', 'G', 'U', 'I' ),
		eButton = CU_MakeSectionName( 'B', 'U', 'T', 'N' ),
		eStatic = CU_MakeSectionName( 'S', 'T', 'T', 'C' ),
		eSlider = CU_MakeSectionName( 'S', 'L', 'D', 'R' ),
		eComboBox = CU_MakeSectionName( 'C', 'M', 'B', 'O' ),
		eListBox = CU_MakeSectionName( 'L', 'S', 'B', 'X' ),
		eEdit = CU_MakeSectionName( 'E', 'D', 'I', 'T' ),
	};

	CU_DeclareAttributeParser( parserGui )
	CU_DeclareAttributeParser( parserDefaultFont )
	CU_DeclareAttributeParser( parserGuiEnd )

	CU_DeclareAttributeParser( parserButton )
	CU_DeclareAttributeParser( parserButtonFont )
	CU_DeclareAttributeParser( parserButtonCaption )
	CU_DeclareAttributeParser( parserButtonTextMaterial )
	CU_DeclareAttributeParser( parserButtonHighlightedBackgroundMaterial )
	CU_DeclareAttributeParser( parserButtonHighlightedForegroundMaterial )
	CU_DeclareAttributeParser( parserButtonHighlightedTextMaterial )
	CU_DeclareAttributeParser( parserButtonPushedBackgroundMaterial )
	CU_DeclareAttributeParser( parserButtonPushedForegroundMaterial )
	CU_DeclareAttributeParser( parserButtonPushedTextMaterial )
	CU_DeclareAttributeParser( parserButtonHAlign )
	CU_DeclareAttributeParser( parserButtonVAlign )
	CU_DeclareAttributeParser( parserButtonEnd )
	CU_DeclareAttributeParser( parserComboBox )
	CU_DeclareAttributeParser( parserComboBoxFont )
	CU_DeclareAttributeParser( parserComboBoxItem )
	CU_DeclareAttributeParser( parserComboBoxTextMaterial )
	CU_DeclareAttributeParser( parserComboBoxSelectedItemBackgroundMaterial )
	CU_DeclareAttributeParser( parserComboBoxSelectedItemForegroundMaterial )
	CU_DeclareAttributeParser( parserComboBoxHighlightedItemBackgroundMaterial )
	CU_DeclareAttributeParser( parserComboBoxEnd )
	CU_DeclareAttributeParser( parserEdit )
	CU_DeclareAttributeParser( parserEditFont )
	CU_DeclareAttributeParser( parserEditTextMaterial )
	CU_DeclareAttributeParser( parserEditCaption )
	CU_DeclareAttributeParser( parserEditMultiLine )
	CU_DeclareAttributeParser( parserEditEnd )
	CU_DeclareAttributeParser( parserListBox )
	CU_DeclareAttributeParser( parserListBoxFont )
	CU_DeclareAttributeParser( parserListBoxItem )
	CU_DeclareAttributeParser( parserListBoxTextMaterial )
	CU_DeclareAttributeParser( parserListBoxSelectedItemBackgroundMaterial )
	CU_DeclareAttributeParser( parserListBoxSelectedItemForegroundMaterial )
	CU_DeclareAttributeParser( parserListBoxHighlightedItemBackgroundMaterial )
	CU_DeclareAttributeParser( parserListBoxEnd )
	CU_DeclareAttributeParser( parserSlider )
	CU_DeclareAttributeParser( parserSliderEnd )
	CU_DeclareAttributeParser( parserStatic )
	CU_DeclareAttributeParser( parserStaticFont )
	CU_DeclareAttributeParser( parserStaticCaption )
	CU_DeclareAttributeParser( parserStaticHAlign )
	CU_DeclareAttributeParser( parserStaticVAlign )
	CU_DeclareAttributeParser( parserStaticTextMaterial )
	CU_DeclareAttributeParser( parserStaticEnd )

	CU_DeclareAttributeParser( parserControlPixelPosition )
	CU_DeclareAttributeParser( parserControlPixelSize )
	CU_DeclareAttributeParser( parserControlPixelBorderSize )
	CU_DeclareAttributeParser( parserControlBackgroundMaterial )
	CU_DeclareAttributeParser( parserControlForegroundMaterial )
	CU_DeclareAttributeParser( parserControlBorderMaterial )
	CU_DeclareAttributeParser( parserControlBorderInnerUv )
	CU_DeclareAttributeParser( parserControlBorderOuterUv )
	CU_DeclareAttributeParser( parserControlCenterUv )
	CU_DeclareAttributeParser( parserControlVisible )

}

#endif
