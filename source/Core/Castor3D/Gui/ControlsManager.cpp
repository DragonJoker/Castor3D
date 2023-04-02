#include "Castor3D/Gui/ControlsManager.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Gui/Gui_Parsers.hpp"
#include "Castor3D/Gui/Controls/CtrlControl.hpp"
#include "Castor3D/Gui/Controls/CtrlButton.hpp"
#include "Castor3D/Gui/Controls/CtrlComboBox.hpp"
#include "Castor3D/Gui/Controls/CtrlEdit.hpp"
#include "Castor3D/Gui/Controls/CtrlExpandablePanel.hpp"
#include "Castor3D/Gui/Controls/CtrlListBox.hpp"
#include "Castor3D/Gui/Controls/CtrlScrollBar.hpp"
#include "Castor3D/Gui/Controls/CtrlSlider.hpp"
#include "Castor3D/Gui/Controls/CtrlStatic.hpp"
#include "Castor3D/Gui/Theme/StyleButton.hpp"
#include "Castor3D/Gui/Theme/StyleComboBox.hpp"
#include "Castor3D/Gui/Theme/StyleEdit.hpp"
#include "Castor3D/Gui/Theme/StyleExpandablePanel.hpp"
#include "Castor3D/Gui/Theme/StyleListBox.hpp"
#include "Castor3D/Gui/Theme/StylePanel.hpp"
#include "Castor3D/Gui/Theme/StyleSlider.hpp"
#include "Castor3D/Gui/Theme/StyleStatic.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>

CU_ImplementCUSmartPtr( castor3d, ControlsManager )

namespace castor3d
{
	namespace ctrlmgr
	{
		using LockType = std::unique_lock< std::mutex >;

		static void createDefaultParsers( castor::AttributeParsers & parsers
			, GUISection section
			, castor::ParserFunction themeFunction
			, castor::ParserFunction styleFunction
			, castor::ParserFunction endFunction
			, bool hasBorders = true )
		{
			using namespace castor;

			if ( themeFunction )
			{
				addParserT( parsers, section, cuT( "theme" ), themeFunction, { makeParameter< ParameterType::eName >() } );
			}

			if ( styleFunction )
			{
				addParserT( parsers, section, cuT( "style" ), styleFunction, { makeParameter< ParameterType::eName >() } );
			}

			if ( themeFunction || styleFunction )
			{
				addParserT( parsers, section, cuT( "visible" ), &parserControlVisible, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
				addParserT( parsers, section, cuT( "pixel_position" ), &parserControlPixelPosition, { makeParameter< ParameterType::ePosition >() } );
				addParserT( parsers, section, cuT( "pixel_size" ), &parserControlPixelSize, { makeParameter< ParameterType::eSize >() } );
				addParserT( parsers, section, cuT( "movable" ), &parserControlMovable, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
				addParserT( parsers, section, cuT( "resizable" ), &parserControlResizable, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			}

			if ( hasBorders )
			{
				addParserT( parsers, section, cuT( "pixel_border_size" ), &parserControlPixelBorderSize, { makeParameter< ParameterType::ePoint4U >() } );
				addParserT( parsers, section, cuT( "border_inner_uv" ), &parserControlBorderInnerUv, { makeParameter< ParameterType::ePoint4D >() } );
				addParserT( parsers, section, cuT( "border_outer_uv" ), &parserControlBorderOuterUv, { makeParameter< ParameterType::ePoint4D >() } );
				addParserT( parsers, section, cuT( "center_uv" ), &parserControlCenterUv, { makeParameter< ParameterType::ePoint4D >() } );
			}

			if ( endFunction )
			{
				addParserT( parsers, section, cuT( "}" ), endFunction );
			}
		}

		static void createScrollableParsers( castor::AttributeParsers & result
			, GUISection section )
		{
			using namespace castor;
			addParserT( result, section, cuT( "vertical_scrollbar" ), &parserScrollableVerticalScroll, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, section, cuT( "horizontal_scrollbar" ), &parserScrollableHorizontalScroll, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, section, cuT( "vertical_scrollbar_style" ), &parserScrollableVerticalScrollBarStyle, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, cuT( "horizontal_scrollbar_style" ), &parserScrollableHorizontalScrollBarStyle, { makeParameter< ParameterType::eName >() } );
		}

		static void createDefaultStyleParsers( castor::AttributeParsers & parsers
			, GUISection section )
		{
			using namespace castor;
			addParserT( parsers, section, cuT( "background_invisible" ), &parserStyleBackgroundInvisible, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( parsers, section, cuT( "foreground_invisible" ), &parserStyleForegroundInvisible, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( parsers, section, cuT( "border_invisible" ), &parserStyleForegroundInvisible, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( parsers, section, cuT( "background_material" ), &parserStyleBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, cuT( "foreground_material" ), &parserStyleForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, cuT( "border_material" ), &parserStyleForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, cuT( "}" ), &parserStyleEnd );
		}

		static void createDefaultLayoutParsers( castor::AttributeParsers & parsers
			, GUISection section )
		{
			using namespace castor;
			addParserT( parsers, section, cuT( "box_layout" ), &parserControlBoxLayout );
		}

		template< typename SectionT >
		static void createControlsParsers( castor::AttributeParsers & parsers
			, SectionT section )
		{
			using namespace castor;
			addParserT( parsers, section, cuT( "button" ), &parserButton, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, cuT( "static" ), &parserStatic, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, cuT( "slider" ), &parserSlider, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, cuT( "combobox" ), &parserComboBox, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, cuT( "listbox" ), &parserListBox, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, cuT( "edit" ), &parserEdit, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, cuT( "panel" ), &parserPanel, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, cuT( "progress" ), &parserProgress, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, cuT( "expandable_panel" ), &parserExpandablePanel, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, cuT( "frame" ), &parserFrame, { makeParameter< ParameterType::eName >() } );
		}

		template< typename SectionT >
		static void createStylesParsers( castor::AttributeParsers & result
			, SectionT section )
		{
			using namespace castor;
			addParserT( result, section, cuT( "default_font" ), &parserStyleDefaultFont, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, cuT( "button_style" ), &parserStyleButtonStyle, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, cuT( "static_style" ), &parserStyleStaticStyle, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, cuT( "slider_style" ), &parserStyleSliderStyle, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, cuT( "combobox_style" ), &parserStyleComboBoxStyle, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, cuT( "listbox_style" ), &parserStyleListBoxStyle, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, cuT( "edit_style" ), &parserStyleEditStyle, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, cuT( "panel_style" ), &parserStylePanelStyle, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, cuT( "progress_style" ), &parserStyleProgressStyle, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, cuT( "expandable_panel_style" ), &parserStyleExpandablePanelStyle, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, cuT( "frame_style" ), &parserStyleFrameStyle, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, cuT( "scrollbar_style" ), &parserStyleScrollBarStyle, { makeParameter< ParameterType::eName >() } );
		}

		template< typename SectionT >
		static void createGuiParsers( castor::AttributeParsers & result
			, SectionT section )
		{
			using namespace castor;
			createControlsParsers( result, section );
			createStylesParsers( result, section );
			addParserT( result, section, cuT( "theme" ), &parserTheme, { makeParameter< ParameterType::eName >() } );
		}

		static void createButtonParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eButton
				, &parserButtonTheme
				, &parserButtonStyle
				, &parserControlEnd );
			addParserT( result, GUISection::eButton, cuT( "caption" ), &parserButtonCaption, { makeParameter< ParameterType::eText >() } );
			addParserT( result, GUISection::eButton, cuT( "horizontal_align" ), &parserButtonHAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
			addParserT( result, GUISection::eButton, cuT( "vertical_align" ), &parserButtonVAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );

			createDefaultStyleParsers( result, GUISection::eButtonStyle );
			addParserT( result, GUISection::eButtonStyle, cuT( "text_material" ), &parserStyleButtonTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "highlighted_background_material" ), &parserStyleButtonHighlightedBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "highlighted_foreground_material" ), &parserStyleButtonHighlightedForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "highlighted_text_material" ), &parserStyleButtonHighlightedTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "pushed_background_material" ), &parserStyleButtonPushedBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "pushed_foreground_material" ), &parserStyleButtonPushedForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "pushed_text_material" ), &parserStyleButtonPushedTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "disabled_background_material" ), &parserStyleButtonDisabledBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "disabled_foreground_material" ), &parserStyleButtonDisabledForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "disabled_text_material" ), &parserStyleButtonDisabledTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "font" ), &parserStyleButtonFont, { makeParameter< ParameterType::eName >() } );
		}

		static void createComboBoxParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eComboBox
				, &parserComboBoxTheme
				, &parserComboBoxStyle
				, &parserControlEnd );
			addParserT( result, GUISection::eComboBox, cuT( "item" ), &parserComboBoxItem, { makeParameter< ParameterType::eText >() } );

			createDefaultStyleParsers( result, GUISection::eComboStyle );
			addParserT( result, GUISection::eComboStyle, cuT( "expand_style" ), &parserStyleComboButton );
			addParserT( result, GUISection::eComboStyle, cuT( "elements_style" ), &parserStyleComboListBox );
		}

		static void createEditParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eEdit
				, &parserEditTheme
				, &parserEditStyle
				, &parserControlEnd );
			createScrollableParsers( result, GUISection::eEdit );
			addParserT( result, GUISection::eEdit, cuT( "multiline" ), &parserEditMultiLine, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eEdit, cuT( "caption" ), &parserEditCaption, { makeParameter< ParameterType::eText >() } );

			createDefaultStyleParsers( result, GUISection::eEditStyle );
			addParserT( result, GUISection::eEditStyle, cuT( "font" ), &parserStyleEditFont, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eEditStyle, cuT( "text_material" ), &parserStyleEditTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eEditStyle, cuT( "selection_material" ), &parserStyleEditSelectionMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eEditStyle, cuT( "scrollbar_style" ), &parserStyleScrollBarStyle, { makeParameter< ParameterType::eName >() } );
		}

		static void createExpandablePanelParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eExpandablePanel
				, &parserExpandablePanelTheme
				, &parserExpandablePanelStyle
				, &parserControlEnd
				, false );
			addParserT( result, GUISection::eExpandablePanel, cuT( "header" ), &parserExpandablePanelHeader );
			addParserT( result, GUISection::eExpandablePanel, cuT( "expand" ), &parserExpandablePanelExpand );
			addParserT( result, GUISection::eExpandablePanel, cuT( "content" ), &parserExpandablePanelContent );
			addParserT( result, GUISection::eExpandablePanel, cuT( "expand_caption" ), &parserExpandablePanelExpandCaption, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eExpandablePanel, cuT( "retract_caption" ), &parserExpandablePanelRetractCaption, { makeParameter< ParameterType::eName > ( ) } );

			createDefaultStyleParsers( result, GUISection::eExpandablePanelStyle );
			addParserT( result, GUISection::eExpandablePanelStyle, cuT( "header_style" ), &parserStyleExpandablePanelHeader );
			addParserT( result, GUISection::eExpandablePanelStyle, cuT( "expand_style" ), &parserStyleExpandablePanelExpand );
			addParserT( result, GUISection::eExpandablePanelStyle, cuT( "content_style" ), &parserStyleExpandablePanelContent );
		}

		static void createExpandablePanelHeaderParsers( castor::AttributeParsers & result )
		{
			createDefaultParsers( result
				, GUISection::eExpandablePanelHeader
				, nullptr
				, nullptr
				, &parserExpandablePanelHeaderEnd );
			createControlsParsers( result, GUISection::eExpandablePanelHeader );
			createDefaultLayoutParsers( result, GUISection::eExpandablePanelHeader );
		}

		static void createExpandablePanelExpandParsers( castor::AttributeParsers & result )
		{
			createDefaultParsers( result
				, GUISection::eExpandablePanelExpand
				, nullptr
				, nullptr
				, &parserExpandablePanelExpandEnd );
		}

		static void createExpandablePanelContentParsers( castor::AttributeParsers & result )
		{
			createDefaultParsers( result
				, GUISection::eExpandablePanelContent
				, nullptr
				, nullptr
				, &parserExpandablePanelContentEnd );
			createControlsParsers( result, GUISection::eExpandablePanelContent );
			createDefaultLayoutParsers( result, GUISection::eExpandablePanelContent );
			createScrollableParsers( result, GUISection::eExpandablePanelContent );
		}

		static void createFrameParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eFrame
				, &parserFrameTheme
				, &parserFrameStyle
				, &parserControlEnd );
			addParserT( result, GUISection::eFrame, cuT( "header_horizontal_align" ), &parserFrameHeaderHAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
			addParserT( result, GUISection::eFrame, cuT( "header_vertical_align" ), &parserFrameHeaderVAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );
			addParserT( result, GUISection::eFrame, cuT( "header_caption" ), &parserFrameHeaderCaption, { makeParameter< ParameterType::eText >() } );
			addParserT( result, GUISection::eFrame, cuT( "min_size" ), &parserFrameMinSize, { makeParameter< ParameterType::eSize >() } );
			addParserT( result, GUISection::eFrame, cuT( "content" ), &parserFrameContent );

			createDefaultStyleParsers( result, GUISection::eFrameStyle );
			addParserT( result, GUISection::eFrameStyle, cuT( "header_font" ), &parserStyleFrameHeaderFont, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eFrameStyle, cuT( "header_text_material" ), &parserStyleFrameHeaderTextMaterial, { makeParameter< ParameterType::eName >() } );
		}

		static void createFrameContentParsers( castor::AttributeParsers & result )
		{
			createDefaultParsers( result
				, GUISection::eFrameContent
				, nullptr
				, nullptr
				, &parserFrameContentEnd );
			createControlsParsers( result, GUISection::eFrameContent );
			createScrollableParsers( result, GUISection::eFrameContent );
			createDefaultLayoutParsers( result, GUISection::eFrameContent );
		}

		static void createListBoxParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eListBox
				, &parserListBoxTheme
				, &parserListBoxStyle
				, &parserControlEnd );
			addParserT( result, GUISection::eListBox, cuT( "item" ), &parserListBoxItem, { makeParameter< ParameterType::eText >() } );

			createDefaultStyleParsers( result, GUISection::eListStyle );
			addParserT( result, GUISection::eListStyle, cuT( "item_style" ), &parserStyleListBoxItemStatic );
			addParserT( result, GUISection::eListStyle, cuT( "selected_item_style" ), &parserStyleListBoxSelItemStatic );
			addParserT( result, GUISection::eListStyle, cuT( "highlighted_item_style" ), &parserStyleListBoxHighItemStatic );
		}

		static void createPanelParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::ePanel
				, &parserPanelTheme
				, &parserPanelStyle
				, &parserControlEnd );
			createControlsParsers( result, GUISection::ePanel );
			createScrollableParsers( result, GUISection::ePanel );
			createDefaultLayoutParsers( result, GUISection::ePanel );

			createStylesParsers( result, GUISection::ePanelStyle );
			createDefaultStyleParsers( result, GUISection::ePanelStyle );
		}

		static void createProgressParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eProgress
				, &parserProgressTheme
				, &parserProgressStyle
				, &parserControlEnd );
			addParserT( result, GUISection::eProgress, cuT( "container_border_size" ), &parserProgressContainerBorderSize, { makeParameter< ParameterType::ePoint4U >() } );
			addParserT( result, GUISection::eProgress, cuT( "bar_border_size" ), &parserProgressBarBorderSize, { makeParameter< ParameterType::ePoint4U >() } );
			addParserT( result, GUISection::eProgress, cuT( "left_to_right" ), &parserProgressLeftToRight, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eProgress, cuT( "right_to_left" ), &parserProgressRightToLeft, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eProgress, cuT( "top_to_bottom" ), &parserProgressTopToBottom, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eProgress, cuT( "bottom_to_top" ), &parserProgressBottomToTop, { makeDefaultedParameter< ParameterType::eBool >( true ) } );

			createDefaultStyleParsers( result, GUISection::eProgressStyle );
			addParserT( result, GUISection::eProgressStyle, cuT( "title_font" ), &parserStyleProgressTitleFont, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eProgressStyle, cuT( "title_material" ), &parserStyleProgressTitleMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eProgressStyle, cuT( "container_style" ), &parserStyleProgressContainer );
			addParserT( result, GUISection::eProgressStyle, cuT( "bar_style" ), &parserStyleProgressProgress );
			addParserT( result, GUISection::eProgressStyle, cuT( "text_font" ), &parserStyleProgressTextFont, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eProgressStyle, cuT( "text_material" ), &parserStyleProgressTextMaterial, { makeParameter< ParameterType::eName >() } );
		}

		static void createScrollBarParsers( castor::AttributeParsers & result )
		{
			addParserT( result, GUISection::eScrollBarStyle, cuT( "begin_style" ), &parserStyleScrollBarBeginButton );
			addParserT( result, GUISection::eScrollBarStyle, cuT( "end_style" ), &parserStyleScrollBarEndButton );
			addParserT( result, GUISection::eScrollBarStyle, cuT( "bar_style" ), &parserStyleScrollBarBar );
			addParserT( result, GUISection::eScrollBarStyle, cuT( "thumb_style" ), &parserStyleScrollBarThumb );
			addParserT( result, GUISection::eScrollBarStyle, cuT( "}" ), &parserStyleEnd );
		}

		static void createSliderParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eSlider
				, &parserSliderTheme
				, &parserSliderStyle
				, &parserControlEnd );

			createDefaultStyleParsers( result, GUISection::eSliderStyle );
			addParserT( result, GUISection::eSliderStyle, cuT( "line_style" ), &parserStyleSliderLineStatic );
			addParserT( result, GUISection::eSliderStyle, cuT( "tick_style" ), &parserStyleSliderTickStatic );
		}

		static void createStaticParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eStatic
				, &parserStaticTheme
				, &parserStaticStyle
				, &parserControlEnd );
			addParserT( result, GUISection::eStatic, cuT( "horizontal_align" ), &parserStaticHAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
			addParserT( result, GUISection::eStatic, cuT( "vertical_align" ), &parserStaticVAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );
			addParserT( result, GUISection::eStatic, cuT( "caption" ), &parserStaticCaption, { makeParameter< ParameterType::eText >() } );

			createDefaultStyleParsers( result, GUISection::eStaticStyle );
			addParserT( result, GUISection::eStaticStyle, cuT( "font" ), &parserStyleStaticFont, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eStaticStyle, cuT( "text_material" ), &parserStyleStaticTextMaterial, { makeParameter< ParameterType::eName >() } );
		}

		static void createThemeParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParserT( result, GUISection::eTheme, cuT( "default_font" ), &parserStyleDefaultFont, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eTheme, cuT( "button_style" ), &parserStyleButtonStyle, { makeDefaultedParameter< ParameterType::eName >( "Button" ) } );
			addParserT( result, GUISection::eTheme, cuT( "static_style" ), &parserStyleStaticStyle, { makeDefaultedParameter< ParameterType::eName >( "Static" ) } );
			addParserT( result, GUISection::eTheme, cuT( "slider_style" ), &parserStyleSliderStyle, { makeDefaultedParameter< ParameterType::eName >( "Slider" ) } );
			addParserT( result, GUISection::eTheme, cuT( "combobox_style" ), &parserStyleComboBoxStyle, { makeDefaultedParameter< ParameterType::eName >( "ComboBox" ) } );
			addParserT( result, GUISection::eTheme, cuT( "listbox_style" ), &parserStyleListBoxStyle, { makeDefaultedParameter< ParameterType::eName >( "ListBox" ) } );
			addParserT( result, GUISection::eTheme, cuT( "edit_style" ), &parserStyleEditStyle, { makeDefaultedParameter< ParameterType::eName >( "Edit" ) } );
			addParserT( result, GUISection::eTheme, cuT( "panel_style" ), &parserStylePanelStyle, { makeDefaultedParameter< ParameterType::eName >( "Panel" ) } );
			addParserT( result, GUISection::eTheme, cuT( "progress_style" ), &parserStyleProgressStyle, { makeDefaultedParameter< ParameterType::eName >( "Progress" ) } );
			addParserT( result, GUISection::eTheme, cuT( "expandable_panel_style" ), &parserStyleExpandablePanelStyle, { makeDefaultedParameter< ParameterType::eName >( "ExpandablePanel" ) } );
			addParserT( result, GUISection::eTheme, cuT( "frame_style" ), &parserStyleFrameStyle, { makeDefaultedParameter< ParameterType::eName >( "Frame" ) } );
			addParserT( result, GUISection::eTheme, cuT( "scrollbar_style" ), &parserStyleScrollBarStyle, { makeDefaultedParameter< ParameterType::eName >( "ScrollBar" ) } );
			addParserT( result, GUISection::eTheme, cuT( "}" ), &parserThemeEnd );
		}

		static void createBoxLayoutParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParserT( result, GUISection::eBoxLayout, cuT( "layout_ctrl" ), &parserLayoutCtrl, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eBoxLayout, cuT( "layout_staspace" ), &parserBoxLayoutStaticSpacer, { makeParameter< ParameterType::eUInt32 >() } );
			addParserT( result, GUISection::eBoxLayout, cuT( "layout_dynspace" ), &parserBoxLayoutDynamicSpacer );
			addParserT( result, GUISection::eBoxLayout, cuT( "horizontal" ), &parserBoxLayoutHorizontal, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eBoxLayout, cuT( "}" ), &parserLayoutEnd );
		}

		static void createLayoutCtrlParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParserT( result, GUISection::eLayoutCtrl, cuT( "horizontal_align" ), &parserLayoutCtrlHAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "vertical_align" ), &parserLayoutCtrlVAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "stretch" ), &parserLayoutCtrlStretch, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "reserve_if_hidden" ), &parserLayoutCtrlReserveIfHidden, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "padding" ), &parserLayoutCtrlPadding, { makeParameter< ParameterType::ePoint4U >() } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "pad_left" ), &parserLayoutCtrlPadLeft, { makeParameter< ParameterType::eUInt32 >() } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "pad_top" ), &parserLayoutCtrlPadTop, { makeParameter< ParameterType::eUInt32 >() } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "pad_right" ), &parserLayoutCtrlPadRight, { makeParameter< ParameterType::eUInt32 >() } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "pad_bottom" ), &parserLayoutCtrlPadBottom, { makeParameter< ParameterType::eUInt32 >() } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "}" ), &parserLayoutCtrlEnd );
		}

		template< typename StyleT >
		StyleT * getThemeStyle( castor::String const & name
			, std::map< castor::String, ThemeUPtr > const & themes )
		{
			StyleT * style{};
			auto themeIt = std::find_if( themes.begin()
				, themes.end()
				, [&name, &style]( auto const & lookup )
				{
					if ( name.find( lookup.first + "/" ) == 0u )
					{
						style = lookup.second->template getStyle< StyleT >( name.substr( lookup.first.size() + 1u ) );
					}

					return style != nullptr;
				} );
			return ( themeIt != themes.end() )
				? style
				: nullptr;
		}

		template< typename ConnectionT >
		void removeElem( Control const & control
			, std::map< Control const *, ConnectionT > & map )
		{
			auto it = map.find( &control );

			if ( it != map.end() )
			{
				map.erase( it );
			}
		}
	}

	castor::String ControlsManager::Name = "c3d.gui";

	ControlsManager::ControlsManager( Engine & engine )
		: UserInputListener{ engine, Name }
		, StylesHolder{ castor::String{}, engine }
	{
	}

	ThemeRPtr ControlsManager::createTheme( castor::String const & name
		, Scene * scene )
	{
		auto ires = m_themes.emplace( name, nullptr );
		auto it = ires.first;

		if ( ires.second )
		{
			it->second = castor::makeUnique< Theme >( name, scene, *getEngine() );
		}

		return it->second.get();
	}

	ThemeRPtr ControlsManager::getTheme( castor::String const & name )const
	{
		auto it = m_themes.find( name );

		if ( it == m_themes.end() )
		{
			return nullptr;
		}

		return it->second.get();
	}

	ButtonStyleRPtr ControlsManager::getButtonStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< ButtonStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getButtonStyle( name );
	}

	ComboBoxStyleRPtr ControlsManager::getComboBoxStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< ComboBoxStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getComboBoxStyle( name );
	}

	EditStyleRPtr ControlsManager::getEditStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< EditStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getEditStyle( name );
	}

	ExpandablePanelStyleRPtr ControlsManager::getExpandablePanelStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< ExpandablePanelStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getExpandablePanelStyle( name );
	}

	FrameStyleRPtr ControlsManager::getFrameStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< FrameStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getFrameStyle( name );
	}

	ListBoxStyleRPtr ControlsManager::getListBoxStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< ListBoxStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getListBoxStyle( name );
	}

	PanelStyleRPtr ControlsManager::getPanelStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< PanelStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getPanelStyle( name );
	}

	ProgressStyleRPtr ControlsManager::getProgressStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< ProgressStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getProgressStyle( name );
	}

	ScrollBarStyleRPtr ControlsManager::getScrollBarStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< ScrollBarStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getScrollBarStyle( name );
	}

	SliderStyleRPtr ControlsManager::getSliderStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< SliderStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getSliderStyle( name );
	}

	StaticStyleRPtr ControlsManager::getStaticStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< StaticStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getStaticStyle( name );
	}

	void ControlsManager::create( ControlSPtr control )
	{
		addControl( control );
		control->create( shared_from_this() );
	}

	void ControlsManager::destroy( ControlSPtr control )
	{
		control->destroy();
		removeControl( control->getId() );
	}

	void ControlsManager::addControl( ControlSPtr control )
	{
		doAddHandler( control );
		{
			ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };

			if ( m_controlsById.find( control->getId() ) != m_controlsById.end() )
			{
				CU_Exception( "A control with ID " + castor::string::toString( control->getId() ) + " [" + control->getName() + "] already exists in the manager" );
			}

			m_controlsById.insert( std::make_pair( control->getId(), control ) );

			if ( !control->getParent() )
			{
				m_rootControls.push_back( control.get() );
			}
		}
		doMarkDirty();
	}

	void ControlsManager::removeControl( ControlID id )
	{
		EventHandler * handler;
		{
			ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };
			auto it = m_controlsById.find( id );

			if ( it == m_controlsById.end() )
			{
				CU_Exception( "This control does not exist in the manager." );
			}

			auto control = it->second.lock().get();
			handler = control;
			m_controlsById.erase( it );

			if ( !control->getParent() )
			{
				auto rootit = std::find( m_rootControls.begin()
					, m_rootControls.end()
					, control );

				if ( rootit != m_rootControls.end() )
				{
					m_rootControls.erase( rootit );
				}
			}
		}

		doMarkDirty();
		doRemoveHandler( *handler );
	}

	ControlSPtr ControlsManager::getControl( ControlID id )const
	{
		auto controls = doGetControlsById();
		auto it = controls.find( id );

		if ( it == controls.end() )
		{
			CU_Exception( "This control does not exist in the manager" );
		}

		return it->second.lock();
	}

	ControlSPtr ControlsManager::findControl( castor::String const & name )const
	{
		auto controls = doGetHandlers();
		auto it = std::find_if( controls.begin()
			, controls.end()
			, [&name]( EventHandlerSPtr lookup )
			{
				return lookup->getName() == name;
			} );
		return it == controls.end()
			? nullptr
			: std::static_pointer_cast< Control >( *it );
	}

	bool ControlsManager::setMovedControl( ControlRPtr control
		, MouseEvent const & event )
	{
		if ( m_movedControl
			&& m_movedControl->isMoving()
			&& m_movedControl != control
			&& control != nullptr )
		{
			return false;
		}

		if ( m_movedControl )
		{
			m_movedControl->endMove( event );
		}

		m_movedControl = control;
		return true;
	}

	bool ControlsManager::setResizedControl( ControlRPtr control
		, MouseEvent const & event )
	{
		if ( m_resizedControl
			&& m_resizedControl->isResizing()
			&& m_resizedControl != control
			&& control != nullptr )
		{
			return false;
		}

		if ( m_resizedControl )
		{
			m_resizedControl->endResize( event );
		}

		m_resizedControl = control;
		return true;
	}

	std::vector< Control * > ControlsManager::getRootControls()const
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };
		return m_rootControls;
	}

	castor::AttributeParsers ControlsManager::createParsers( castor3d::Engine & engine )
	{
		using namespace castor;
		AttributeParsers result;

		addParserT( result, CSCNSection::eRoot, cuT( "gui" ), &parserGui );
		addParserT( result, CSCNSection::eScene, cuT( "gui" ), &parserGui );

		ctrlmgr::createGuiParsers( result, CSCNSection::eRoot );
		ctrlmgr::createGuiParsers( result, CSCNSection::eScene );
		ctrlmgr::createGuiParsers( result, GUISection::eGUI );
		ctrlmgr::createThemeParsers( result );
		ctrlmgr::createButtonParsers( result );
		ctrlmgr::createComboBoxParsers( result );
		ctrlmgr::createEditParsers( result );
		ctrlmgr::createListBoxParsers( result );
		ctrlmgr::createScrollBarParsers( result );
		ctrlmgr::createSliderParsers( result );
		ctrlmgr::createStaticParsers( result );
		ctrlmgr::createPanelParsers( result );
		ctrlmgr::createProgressParsers( result );
		ctrlmgr::createExpandablePanelParsers( result );
		ctrlmgr::createExpandablePanelHeaderParsers( result );
		ctrlmgr::createExpandablePanelExpandParsers( result );
		ctrlmgr::createExpandablePanelContentParsers( result );
		ctrlmgr::createFrameParsers( result );
		ctrlmgr::createFrameContentParsers( result );
		ctrlmgr::createBoxLayoutParsers( result );
		ctrlmgr::createLayoutCtrlParsers( result );

		return result;
	}

	castor::StrUInt32Map ControlsManager::createSections()
	{
		return
		{
			{ uint32_t( GUISection::eGUI ), cuT( "gui" ) },
			{ uint32_t( GUISection::eTheme ), cuT( "theme" ) },
			{ uint32_t( GUISection::eButtonStyle ), cuT( "button_style" ) },
			{ uint32_t( GUISection::eEditStyle ), cuT( "edit_style" ) },
			{ uint32_t( GUISection::eComboStyle ), cuT( "combobox_style" ) },
			{ uint32_t( GUISection::eListStyle ), cuT( "listbox_style" ) },
			{ uint32_t( GUISection::eSliderStyle ), cuT( "slider_style" ) },
			{ uint32_t( GUISection::eStaticStyle ), cuT( "static_style" ) },
			{ uint32_t( GUISection::ePanelStyle ), cuT( "panel_style" ) },
			{ uint32_t( GUISection::eProgressStyle ), cuT( "progress_style" ) },
			{ uint32_t( GUISection::eExpandablePanelStyle ), cuT( "expandable_panel_style" ) },
			{ uint32_t( GUISection::eFrameStyle ), cuT( "frame_style" ) },
			{ uint32_t( GUISection::eScrollBarStyle ), cuT( "scrollbar_style" ) },
			{ uint32_t( GUISection::eButton ), cuT( "button" ) },
			{ uint32_t( GUISection::eStatic ), cuT( "static" ) },
			{ uint32_t( GUISection::eSlider ), cuT( "slider" ) },
			{ uint32_t( GUISection::eComboBox ), cuT( "combobox" ) },
			{ uint32_t( GUISection::eListBox ), cuT( "listbox" ) },
			{ uint32_t( GUISection::eEdit ), cuT( "edit" ) },
			{ uint32_t( GUISection::ePanel ), cuT( "panel" ) },
			{ uint32_t( GUISection::eProgress ), cuT( "progress" ) },
			{ uint32_t( GUISection::eExpandablePanel ), cuT( "expandable_panel" ) },
			{ uint32_t( GUISection::eExpandablePanelHeader ), cuT( "header" ) },
			{ uint32_t( GUISection::eExpandablePanelExpand ), cuT( "expand" ) },
			{ uint32_t( GUISection::eExpandablePanelContent ), cuT( "content" ) },
			{ uint32_t( GUISection::eFrame ), cuT( "frame" ) },
			{ uint32_t( GUISection::eFrameContent ), cuT( "content" ) },
			{ uint32_t( GUISection::eBoxLayout ), cuT( "box_layout" ) },
			{ uint32_t( GUISection::eLayoutCtrl ), cuT( "layout_ctrl" ) },
		};
	}

	void * ControlsManager::createContext( castor::FileParserContext & context )
	{
		GuiParserContext * userContext = new GuiParserContext;
		userContext->engine = static_cast< castor3d::SceneFileParser * >( context.parser )->getEngine();
		auto & manager = static_cast< ControlsManager & >( *userContext->engine->getUserInputListener() );
		userContext->stylesHolder.push( &manager );
		return userContext;
	}

	void ControlsManager::connectEvents( ButtonCtrl & control )
	{
		m_onButtonClicks.emplace( &control, control.connect( ButtonEvent::eClicked
			, [this, &control]()
			{
				onClickAction( control.getName() );
			} ) );
	}

	void ControlsManager::connectEvents( ComboBoxCtrl & control )
	{
		m_onComboSelects.emplace( &control, control.connect( ComboBoxEvent::eSelected
			, [this, &control]( int index )
			{
				onSelectAction( control.getName(), index );
			} ) );
	}

	void ControlsManager::connectEvents( EditCtrl & control )
	{
		m_onEditUpdates.emplace( &control, control.connect( EditEvent::eUpdated
			, [this, &control]( castor::U32String const & text )
			{
				onTextAction( control.getName(), text );
			} ) );
	}

	void ControlsManager::connectEvents( ExpandablePanelCtrl & control )
	{
		m_onPanelExpands.emplace( &control, control.connect( ExpandablePanelEvent::eExpand
			, [this, &control]()
			{
				onExpandAction( control.getName(), true );
			} ) );
		m_onPanelRetracts.emplace( &control, control.connect( ExpandablePanelEvent::eRetract
			, [this, &control]()
			{
				onExpandAction( control.getName(), false );
			} ) );
	}

	void ControlsManager::connectEvents( ListBoxCtrl & control )
	{
		m_onListSelects.emplace( &control, control.connect( ListBoxEvent::eSelected
			, [this, &control]( int index )
			{
				onSelectAction( control.getName(), index );
			} ) );
	}

	void ControlsManager::connectEvents( ScrollBarCtrl & control )
	{
		m_onScrollTracks.emplace( &control, control.connect( ScrollBarEvent::eThumbTrack
			, [this, &control]( int index )
			{
				onSelectAction( control.getName(), index );
			} ) );
		m_onScrollReleases.emplace( &control, control.connect( ScrollBarEvent::eThumbRelease
			, [this, &control]( int index )
			{
				onSelectAction( control.getName(), index );
			} ) );
	}

	void ControlsManager::connectEvents( SliderCtrl & control )
	{
		m_onSliderTracks.emplace( &control, control.connect( SliderEvent::eThumbTrack
			, [this, &control]( int index )
			{
				onSelectAction( control.getName(), index );
			} ) );
		m_onSliderReleases.emplace( &control, control.connect( SliderEvent::eThumbRelease
			, [this, &control]( int index )
			{
				onSelectAction( control.getName(), index );
			} ) );
	}

	void ControlsManager::disconnectEvents( ButtonCtrl & control )
	{
		ctrlmgr::removeElem( control, m_onButtonClicks );
	}

	void ControlsManager::disconnectEvents( ComboBoxCtrl & control )
	{
		ctrlmgr::removeElem( control, m_onComboSelects );
	}

	void ControlsManager::disconnectEvents( EditCtrl & control )
	{
		ctrlmgr::removeElem( control, m_onEditUpdates );
	}

	void ControlsManager::disconnectEvents( ExpandablePanelCtrl & control )
	{
		ctrlmgr::removeElem( control, m_onPanelExpands );
		ctrlmgr::removeElem( control, m_onPanelRetracts );
	}

	void ControlsManager::disconnectEvents( ListBoxCtrl & control )
	{
		ctrlmgr::removeElem( control, m_onListSelects );
	}

	void ControlsManager::disconnectEvents( ScrollBarCtrl & control )
	{
		ctrlmgr::removeElem( control, m_onScrollTracks );
		ctrlmgr::removeElem( control, m_onScrollReleases );
	}

	void ControlsManager::disconnectEvents( SliderCtrl & control )
	{
		ctrlmgr::removeElem( control, m_onSliderTracks );
		ctrlmgr::removeElem( control, m_onSliderReleases );
	}

	bool ControlsManager::doInitialise()
	{
		return true;
	}

	void ControlsManager::doCleanup()
	{
		for ( auto cit : m_controlsById )
		{
			if ( auto control = cit.second.lock() )
			{
				control->destroy();
			}
		}

		auto lock( castor::makeUniqueLock( m_mutexControlsById ) );

		m_onButtonClicks.clear();
		m_onComboSelects.clear();
		m_onEditUpdates.clear();
		m_onListSelects.clear();
		m_onScrollTracks.clear();
		m_onScrollReleases.clear();
		m_onSliderTracks.clear();
		m_onSliderReleases.clear();
		m_onPanelExpands.clear();
		m_onPanelRetracts.clear();
		m_themes.clear();

		if ( auto event = m_event.exchange( nullptr ) )
		{
			event->skip();
		}

		m_controlsByZIndex.clear();
		m_rootControls.clear();
		m_controlsById.clear();
		m_movedControl = {};
		m_resizedControl = {};
		cleanupStyles();
	}

	EventHandler * ControlsManager::doGetMouseTargetableHandler( castor::Position const & position )const
	{
		if ( m_movedControl
			&& m_movedControl->isMoving() )
		{
			return m_movedControl;
		}

		if ( m_resizedControl
			&& m_resizedControl->isResizing() )
		{
			return m_resizedControl;
		}

		auto controls = doGetControlsByZIndex();
		EventHandler * result{};
		auto it = controls.rbegin();

		while ( !result && it != controls.rend() )
		{
			Control * control = *it;

			if ( control
				&& !control->isBackgroundInvisible()
				&& control->catchesMouseEvents()
				&& control->getAbsolutePosition().x() <= position.x()
				&& control->getAbsolutePosition().x() + int32_t( control->getSize().getWidth() ) > position.x()
				&& control->getAbsolutePosition().y() <= position.y()
				&& control->getAbsolutePosition().y() + int32_t( control->getSize().getHeight() ) > position.y() )
			{
				auto cursor = control->getCursor();

				if ( control->isResizable() )
				{
					auto [isOnN, isOnW, isOnS, isOnE] = control->isInResizeRange( position );

					if ( ( isOnN && isOnW )
						|| ( isOnS && isOnE ) )
					{
						cursor = MouseCursor::eSizeNWSE;
					}
					else if ( ( isOnN && isOnE )
						|| ( isOnS && isOnW ) )
					{
						cursor = MouseCursor::eSizeNESW;
					}
					else if ( isOnN || isOnS )
					{
						cursor = MouseCursor::eSizeNS;
					}
					else if ( isOnW || isOnE )
					{
						cursor = MouseCursor::eSizeWE;
					}
				}

				onCursorAction( cursor );
				result = control;
			}

			++it;
		}

		if ( !result )
		{
			onCursorAction( MouseCursor::eArrow );
		}

		return result;
	}

	void ControlsManager::doUpdate()
	{
		std::vector< Control * > result;
		std::vector< Control * > top;
		auto controls = getRootControls();
		result.reserve( controls.size() );
		top.reserve( controls.size() );
		uint32_t index{};

		for ( auto control : controls )
		{
			if ( control )
			{
				control->updateZIndex( index, result, top );
			}
		}

		for ( auto control : top )
		{
			control->adjustZIndex( index );
		}

		result.insert( result.end()
			, top.begin()
			, top.end() );
		doSetControlsByZIndex( std::move( result ) );
	}

	void ControlsManager::doSetControlsByZIndex( std::vector< Control * > v )
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsByZIndex ) };
		m_controlsByZIndex = std::move( v );
	}

	std::vector< Control * > ControlsManager::doGetControlsByZIndex()const
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsByZIndex ) };
		return m_controlsByZIndex;
	}

	std::map< ControlID, ControlWPtr > ControlsManager::doGetControlsById()const
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };
		return m_controlsById;
	}

	void ControlsManager::doMarkDirty()
	{
		if ( !m_event )
		{
			m_event = getEngine()->postEvent( makeCpuFunctorEvent( EventType::ePostRender
				, [this]()
				{
					if ( m_event.exchange( nullptr ) )
					{
						doUpdate();
					}
				} ) );
		}
	}
}
