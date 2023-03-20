/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GuiParsers_H___
#define ___C3D_GuiParsers_H___

#include "Castor3D/Gui/GuiModule.hpp"
#include "Castor3D/Gui/Layout/LayoutItemFlags.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

#include <stack>

namespace castor3d
{
	struct GuiParserContext
	{
		std::stack< ControlSPtr > parents{};
		std::stack< ControlStyleRPtr > styles{};
		std::stack< StylesHolderRPtr > stylesHolder{};
		Engine * engine{};
		castor::String controlName{};
		ButtonCtrlSPtr button{};
		ComboBoxCtrlSPtr combo{};
		EditCtrlSPtr edit{};
		ListBoxCtrlSPtr listbox{};
		SliderCtrlSPtr slider{};
		StaticCtrlSPtr staticTxt{};
		PanelCtrlSPtr panel{};
		ExpandablePanelCtrlSPtr expandablePanel{};
		ThemeRPtr theme{};
		ButtonStyleRPtr buttonStyle{};
		ComboBoxStyleRPtr comboStyle{};
		EditStyleRPtr editStyle{};
		ListBoxStyleRPtr listboxStyle{};
		SliderStyleRPtr sliderStyle{};
		StaticStyleRPtr staticStyle{};
		PanelStyleRPtr panelStyle{};
		ExpandablePanelStyleRPtr expandablePanelStyle{};
		LayoutUPtr layout{};
		LayoutItemFlags layoutCtrlFlags{};

		C3D_API ControlRPtr getTopControl()const;
		C3D_API void popControl();
		C3D_API void pushStylesHolder( StylesHolder * style );
		C3D_API void popStylesHolder( StylesHolder const * style );
		C3D_API ControlStyleRPtr getTopStyle()const;
		C3D_API void popStyle();

		template< typename StyleT >
		void pushStyle( StyleT * style
			, StyleT *& result )
		{
			styles.push( style );
			result = style;

			if constexpr ( std::is_base_of_v< StylesHolder, StyleT > )
			{
				pushStylesHolder( style );
			}
		}
	};

	//! Scene file sections Enum
	/**
	The enumeration which defines all the sections and subsections of a scene file
	*/
	enum class GUISection
		: uint32_t
	{
		eGUI = CU_MakeSectionName( 'C', 'G', 'U', 'I' ),
		eTheme = CU_MakeSectionName( 'C', 'G', 'T', 'H' ),
		eButtonStyle = CU_MakeSectionName( 'C', 'T', 'B', 'T' ),
		eEditStyle = CU_MakeSectionName( 'C', 'T', 'E', 'D' ),
		eComboStyle = CU_MakeSectionName( 'C', 'T', 'C', 'X' ),
		eListStyle = CU_MakeSectionName( 'C', 'T', 'L', 'B' ),
		eSliderStyle = CU_MakeSectionName( 'C', 'T', 'S', 'L' ),
		eStaticStyle = CU_MakeSectionName( 'C', 'T', 'S', 'T' ),
		ePanelStyle = CU_MakeSectionName( 'P', 'N', 'S', 'T' ),
		eExpandablePanelStyle = CU_MakeSectionName( 'X', 'P', 'S', 'T' ),
		eButton = CU_MakeSectionName( 'B', 'U', 'T', 'N' ),
		eStatic = CU_MakeSectionName( 'S', 'T', 'T', 'C' ),
		eSlider = CU_MakeSectionName( 'S', 'L', 'D', 'R' ),
		eComboBox = CU_MakeSectionName( 'C', 'M', 'B', 'O' ),
		eListBox = CU_MakeSectionName( 'L', 'S', 'B', 'X' ),
		eEdit = CU_MakeSectionName( 'E', 'D', 'I', 'T' ),
		ePanel = CU_MakeSectionName( 'P', 'A', 'N', 'L' ),
		eExpandablePanel = CU_MakeSectionName( 'X', 'P', 'N', 'L' ),
		eExpandablePanelHeader = CU_MakeSectionName( 'X', 'P', 'H', 'D' ),
		eExpandablePanelExpand = CU_MakeSectionName( 'X', 'P', 'X', 'p' ),
		eExpandablePanelContent = CU_MakeSectionName( 'X', 'P', 'C', 'T' ),
		eBoxLayout = CU_MakeSectionName( 'B', 'X', 'L', 'T' ),
		eLayoutCtrl = CU_MakeSectionName( 'L', 'T', 'C', 'T' ),
	};

	CU_DeclareAttributeParser( parserGui )
	CU_DeclareAttributeParser( parserTheme )

	CU_DeclareAttributeParser( parserButton )
	CU_DeclareAttributeParser( parserButtonTheme )
	CU_DeclareAttributeParser( parserButtonStyle )
	CU_DeclareAttributeParser( parserButtonHAlign )
	CU_DeclareAttributeParser( parserButtonVAlign )
	CU_DeclareAttributeParser( parserButtonCaption )
	CU_DeclareAttributeParser( parserButtonEnd )
	CU_DeclareAttributeParser( parserComboBox )
	CU_DeclareAttributeParser( parserComboBoxTheme )
	CU_DeclareAttributeParser( parserComboBoxStyle )
	CU_DeclareAttributeParser( parserComboBoxItem )
	CU_DeclareAttributeParser( parserComboBoxEnd )
	CU_DeclareAttributeParser( parserEdit )
	CU_DeclareAttributeParser( parserEditTheme )
	CU_DeclareAttributeParser( parserEditStyle )
	CU_DeclareAttributeParser( parserEditCaption )
	CU_DeclareAttributeParser( parserEditMultiLine )
	CU_DeclareAttributeParser( parserEditEnd )
	CU_DeclareAttributeParser( parserListBox )
	CU_DeclareAttributeParser( parserListBoxTheme )
	CU_DeclareAttributeParser( parserListBoxStyle )
	CU_DeclareAttributeParser( parserListBoxItem )
	CU_DeclareAttributeParser( parserListBoxEnd )
	CU_DeclareAttributeParser( parserSlider )
	CU_DeclareAttributeParser( parserSliderTheme )
	CU_DeclareAttributeParser( parserSliderStyle )
	CU_DeclareAttributeParser( parserSliderEnd )
	CU_DeclareAttributeParser( parserStatic )
	CU_DeclareAttributeParser( parserStaticTheme )
	CU_DeclareAttributeParser( parserStaticStyle )
	CU_DeclareAttributeParser( parserStaticHAlign )
	CU_DeclareAttributeParser( parserStaticVAlign )
	CU_DeclareAttributeParser( parserStaticCaption )
	CU_DeclareAttributeParser( parserStaticEnd )
	CU_DeclareAttributeParser( parserPanel )
	CU_DeclareAttributeParser( parserPanelTheme )
	CU_DeclareAttributeParser( parserPanelStyle )
	CU_DeclareAttributeParser( parserPanelEnd )
	CU_DeclareAttributeParser( parserExpandablePanel )
	CU_DeclareAttributeParser( parserExpandablePanelTheme )
	CU_DeclareAttributeParser( parserExpandablePanelStyle )
	CU_DeclareAttributeParser( parserExpandablePanelHeader )
	CU_DeclareAttributeParser( parserExpandablePanelExpand )
	CU_DeclareAttributeParser( parserExpandablePanelContent )
	CU_DeclareAttributeParser( parserExpandablePanelEnd )
	CU_DeclareAttributeParser( parserExpandablePanelExpandCaption )
	CU_DeclareAttributeParser( parserExpandablePanelRetractCaption )
	CU_DeclareAttributeParser( parserExpandablePanelHeaderEnd )
	CU_DeclareAttributeParser( parserExpandablePanelExpandEnd )
	CU_DeclareAttributeParser( parserExpandablePanelContentEnd )

	CU_DeclareAttributeParser( parserControlPixelPosition )
	CU_DeclareAttributeParser( parserControlPixelSize )
	CU_DeclareAttributeParser( parserControlPixelBorderSize )
	CU_DeclareAttributeParser( parserControlBorderInnerUv )
	CU_DeclareAttributeParser( parserControlBorderOuterUv )
	CU_DeclareAttributeParser( parserControlCenterUv )
	CU_DeclareAttributeParser( parserControlVisible )
	CU_DeclareAttributeParser( parserControlBoxLayout )
	CU_DeclareAttributeParser( parserControlMovable )
	CU_DeclareAttributeParser( parserControlResizable )

	CU_DeclareAttributeParser( parserThemeEnd )

	CU_DeclareAttributeParser( parserStyleButtonFont )
	CU_DeclareAttributeParser( parserStyleButtonTextMaterial )
	CU_DeclareAttributeParser( parserStyleButtonHighlightedBackgroundMaterial )
	CU_DeclareAttributeParser( parserStyleButtonHighlightedForegroundMaterial )
	CU_DeclareAttributeParser( parserStyleButtonHighlightedTextMaterial )
	CU_DeclareAttributeParser( parserStyleButtonPushedBackgroundMaterial )
	CU_DeclareAttributeParser( parserStyleButtonPushedForegroundMaterial )
	CU_DeclareAttributeParser( parserStyleButtonPushedTextMaterial )
	CU_DeclareAttributeParser( parserStyleButtonDisabledBackgroundMaterial )
	CU_DeclareAttributeParser( parserStyleButtonDisabledForegroundMaterial )
	CU_DeclareAttributeParser( parserStyleButtonDisabledTextMaterial )
	CU_DeclareAttributeParser( parserStyleButtonEnd )
	CU_DeclareAttributeParser( parserStyleComboButton )
	CU_DeclareAttributeParser( parserStyleComboListBox )
	CU_DeclareAttributeParser( parserStyleComboBoxEnd )
	CU_DeclareAttributeParser( parserStyleEditFont )
	CU_DeclareAttributeParser( parserStyleEditTextMaterial )
	CU_DeclareAttributeParser( parserStyleEditEnd )
	CU_DeclareAttributeParser( parserStyleListBoxItemStatic )
	CU_DeclareAttributeParser( parserStyleListBoxSelItemStatic )
	CU_DeclareAttributeParser( parserStyleListBoxHighItemStatic )
	CU_DeclareAttributeParser( parserStyleListBoxEnd )
	CU_DeclareAttributeParser( parserStyleSliderLineStatic )
	CU_DeclareAttributeParser( parserStyleSliderTickStatic )
	CU_DeclareAttributeParser( parserStyleSliderEnd )
	CU_DeclareAttributeParser( parserStyleStaticFont )
	CU_DeclareAttributeParser( parserStyleStaticTextMaterial )
	CU_DeclareAttributeParser( parserStyleStaticEnd )
	CU_DeclareAttributeParser( parserStylePanelEnd )
	CU_DeclareAttributeParser( parserStyleExpandablePanelHeader )
	CU_DeclareAttributeParser( parserStyleExpandablePanelExpand )
	CU_DeclareAttributeParser( parserStyleExpandablePanelPanel )
	CU_DeclareAttributeParser( parserStyleExpandablePanelEnd )

	CU_DeclareAttributeParser( parserStyleBackgroundMaterial )
	CU_DeclareAttributeParser( parserStyleForegroundMaterial )
	CU_DeclareAttributeParser( parserStyleBackgroundInvisible )
	CU_DeclareAttributeParser( parserStyleForegroundInvisible )
	CU_DeclareAttributeParser( parserStyleDefaultFont )
	CU_DeclareAttributeParser( parserStyleButtonStyle )
	CU_DeclareAttributeParser( parserStyleComboBoxStyle )
	CU_DeclareAttributeParser( parserStyleEditStyle )
	CU_DeclareAttributeParser( parserStyleListBoxStyle )
	CU_DeclareAttributeParser( parserStyleSliderStyle )
	CU_DeclareAttributeParser( parserStyleStaticStyle )
	CU_DeclareAttributeParser( parserStylePanelStyle )
	CU_DeclareAttributeParser( parserStyleExpandablePanelStyle )

	CU_DeclareAttributeParser( parserLayoutCtrl )
	CU_DeclareAttributeParser( parserLayoutEnd )

	CU_DeclareAttributeParser( parserBoxLayoutHorizontal )
	CU_DeclareAttributeParser( parserBoxLayoutStaticSpacer )
	CU_DeclareAttributeParser( parserBoxLayoutDynamicSpacer )

	CU_DeclareAttributeParser( parserLayoutCtrlHAlign )
	CU_DeclareAttributeParser( parserLayoutCtrlVAlign )
	CU_DeclareAttributeParser( parserLayoutCtrlStretch )
	CU_DeclareAttributeParser( parserLayoutCtrlReserveIfHidden )
	CU_DeclareAttributeParser( parserLayoutCtrlPadding )
	CU_DeclareAttributeParser( parserLayoutCtrlPadLeft )
	CU_DeclareAttributeParser( parserLayoutCtrlPadTop )
	CU_DeclareAttributeParser( parserLayoutCtrlPadRight )
	CU_DeclareAttributeParser( parserLayoutCtrlPadBottom )
	CU_DeclareAttributeParser( parserLayoutCtrlEnd )
}

#endif
