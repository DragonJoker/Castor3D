#include "Castor3D/Gui/ControlsManager.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Gui/Gui_Parsers.hpp"
#include "Castor3D/Gui/Controls/CtrlControl.hpp"
#include "Castor3D/Gui/Controls/CtrlButton.hpp"
#include "Castor3D/Gui/Controls/CtrlComboBox.hpp"
#include "Castor3D/Gui/Controls/CtrlEdit.hpp"
#include "Castor3D/Gui/Controls/CtrlExpandablePanel.hpp"
#include "Castor3D/Gui/Controls/CtrlListBox.hpp"
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
			, uint32_t section
			, castor::ParserFunction themeFunction
			, castor::ParserFunction styleFunction
			, castor::ParserFunction endFunction )
		{
			using namespace castor;

			if ( themeFunction )
			{
				addParser( parsers, section, cuT( "theme" ), themeFunction, { makeParameter< ParameterType::eName >() } );
			}

			if ( styleFunction )
			{
				addParser( parsers, section, cuT( "style" ), styleFunction, { makeParameter< ParameterType::eName >() } );
			}

			addParser( parsers, section, cuT( "visible" ), &parserControlVisible, { makeParameter< ParameterType::eBool >() } );
			addParser( parsers, section, cuT( "pixel_position" ), &parserControlPixelPosition, { makeParameter< ParameterType::ePosition >() } );
			addParser( parsers, section, cuT( "pixel_size" ), &parserControlPixelSize, { makeParameter< ParameterType::eSize >() } );
			addParser( parsers, section, cuT( "pixel_border_size" ), &parserControlPixelBorderSize, { makeParameter< ParameterType::ePoint4U >() } );
			addParser( parsers, section, cuT( "border_inner_uv" ), &parserControlBorderInnerUv, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( parsers, section, cuT( "border_outer_uv" ), &parserControlBorderOuterUv, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( parsers, section, cuT( "center_uv" ), &parserControlCenterUv, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( parsers, section, cuT( "}" ), endFunction );
		}

		static void createDefaultStyleParsers( castor::AttributeParsers & parsers
			, uint32_t section
			, castor::ParserFunction endFunction )
		{
			using namespace castor;
			addParser( parsers, section, cuT( "background_material" ), &parserStyleBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "foreground_material" ), &parserStyleForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "border_material" ), &parserStyleBorderMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "}" ), endFunction );
		}

		static void createDefaultLayoutParsers( castor::AttributeParsers & parsers
			, uint32_t section )
		{
			using namespace castor;
			addParser( parsers, section, cuT( "box_layout" ), &parserControlBoxLayout );
		}

		static void createControlsParsers( castor::AttributeParsers & parsers
			, uint32_t section )
		{
			using namespace castor;
			addParser( parsers, section, cuT( "button" ), &parserButton, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "static" ), &parserStatic, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "slider" ), &parserSlider, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "combobox" ), &parserComboBox, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "listbox" ), &parserListBox, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "edit" ), &parserEdit, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "panel" ), &parserPanel, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "expandable_panel" ), &parserExpandablePanel, { makeParameter< ParameterType::eName >() } );
		}

		static void createStylesParsers( castor::AttributeParsers & result
			, uint32_t section )
		{
			using namespace castor;
			addParser( result, section, cuT( "button_style" ), &parserStyleButtonStyle, { makeParameter< ParameterType::eName >() } );
			addParser( result, section, cuT( "static_style" ), &parserStyleStaticStyle, { makeParameter< ParameterType::eName >() } );
			addParser( result, section, cuT( "slider_style" ), &parserStyleSliderStyle, { makeParameter< ParameterType::eName >() } );
			addParser( result, section, cuT( "combobox_style" ), &parserStyleComboBoxStyle, { makeParameter< ParameterType::eName >() } );
			addParser( result, section, cuT( "listbox_style" ), &parserStyleListBoxStyle, { makeParameter< ParameterType::eName >() } );
			addParser( result, section, cuT( "edit_style" ), &parserStyleEditStyle, { makeParameter< ParameterType::eName >() } );
			addParser( result, section, cuT( "panel_style" ), &parserStylePanelStyle, { makeParameter< ParameterType::eName >() } );
			addParser( result, section, cuT( "expandable_panel_style" ), &parserStyleExpandablePanelStyle, { makeParameter< ParameterType::eName >() } );
		}

		static void createGuiParsers( castor::AttributeParsers & result
			, uint32_t section )
		{
			using namespace castor;
			createControlsParsers( result, section );
			createStylesParsers( result, section );
			addParser( result, section, cuT( "theme" ), &parserTheme, { makeParameter< ParameterType::eName >() } );

			if ( section == uint32_t( GUISection::eGUI ) )
			{
				addParser( result, section, cuT( "}" ), &parserGuiEnd );
			}
		}

		static void createButtonParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, uint32_t( GUISection::eButton )
				, &parserButtonTheme
				, &parserButtonStyle
				, &parserButtonEnd );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "caption" ), &parserButtonCaption, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "horizontal_align" ), &parserButtonHAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "vertical_align" ), &parserButtonVAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );

			createDefaultStyleParsers( result, uint32_t( GUISection::eButtonStyle ), &parserStyleButtonEnd );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "text_material" ), &parserStyleButtonTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "highlighted_background_material" ), &parserStyleButtonHighlightedBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "highlighted_foreground_material" ), &parserStyleButtonHighlightedForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "highlighted_text_material" ), &parserStyleButtonHighlightedTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "pushed_background_material" ), &parserStyleButtonPushedBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "pushed_foreground_material" ), &parserStyleButtonPushedForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "pushed_text_material" ), &parserStyleButtonPushedTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "disabled_background_material" ), &parserStyleButtonDisabledBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "disabled_foreground_material" ), &parserStyleButtonDisabledForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "disabled_text_material" ), &parserStyleButtonDisabledTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "font" ), &parserStyleButtonFont, { makeParameter< ParameterType::eName >() } );
		}

		static void createComboBoxParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, uint32_t( GUISection::eComboBox )
				, &parserComboBoxTheme
				, &parserComboBoxStyle
				, &parserComboBoxEnd );
			addParser( result, uint32_t( GUISection::eComboBox ), cuT( "item" ), &parserComboBoxItem, { makeParameter< ParameterType::eText >() } );

			createDefaultStyleParsers( result, uint32_t( GUISection::eComboStyle ), &parserStyleComboBoxEnd );
			addParser( result, uint32_t( GUISection::eComboStyle ), cuT( "expand_style" ), &parserStyleComboButton );
			addParser( result, uint32_t( GUISection::eComboStyle ), cuT( "elements_style" ), &parserStyleComboListBox );
		}

		static void createEditParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, uint32_t( GUISection::eEdit )
				, &parserEditTheme
				, &parserEditStyle
				, &parserEditEnd );
			addParser( result, uint32_t( GUISection::eEdit ), cuT( "multiline" ), &parserEditMultiLine, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( GUISection::eEdit ), cuT( "caption" ), &parserEditCaption, { makeParameter< ParameterType::eText >() } );

			createDefaultStyleParsers( result, uint32_t( GUISection::eEditStyle ), &parserStyleEditEnd );
			addParser( result, uint32_t( GUISection::eEditStyle ), cuT( "font" ), &parserStyleEditFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eEditStyle ), cuT( "text_material" ), &parserStyleEditTextMaterial, { makeParameter< ParameterType::eName >() } );
		}

		static void createListBoxParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, uint32_t( GUISection::eListBox )
				, &parserListBoxTheme
				, &parserListBoxStyle
				, &parserListBoxEnd );
			addParser( result, uint32_t( GUISection::eListBox ), cuT( "item" ), &parserListBoxItem, { makeParameter< ParameterType::eText >() } );

			createDefaultStyleParsers( result, uint32_t( GUISection::eListStyle ), &parserStyleListBoxEnd );
			addParser( result, uint32_t( GUISection::eListStyle ), cuT( "item_style" ), &parserStyleListBoxItemStatic );
			addParser( result, uint32_t( GUISection::eListStyle ), cuT( "selected_item_style" ), &parserStyleListBoxSelItemStatic );
			addParser( result, uint32_t( GUISection::eListStyle ), cuT( "highlighted_item_style" ), &parserStyleListBoxHighItemStatic );
		}

		static void createSliderParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, uint32_t( GUISection::eSlider )
				, &parserSliderTheme
				, &parserSliderStyle
				, &parserSliderEnd );

			createDefaultStyleParsers( result, uint32_t( GUISection::eSliderStyle ), &parserStyleSliderEnd );
			addParser( result, uint32_t( GUISection::eSliderStyle ), cuT( "line_style" ), &parserStyleSliderLineStatic );
			addParser( result, uint32_t( GUISection::eSliderStyle ), cuT( "tick_style" ), &parserStyleSliderTickStatic );
		}

		static void createStaticParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, uint32_t( GUISection::eStatic )
				, &parserStaticTheme
				, &parserStaticStyle
				, &parserStaticEnd );
			addParser( result, uint32_t( GUISection::eStatic ), cuT( "horizontal_align" ), &parserStaticHAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
			addParser( result, uint32_t( GUISection::eStatic ), cuT( "vertical_align" ), &parserStaticVAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );
			addParser( result, uint32_t( GUISection::eStatic ), cuT( "caption" ), &parserStaticCaption, { makeParameter< ParameterType::eText >() } );

			createDefaultStyleParsers( result, uint32_t( GUISection::eStaticStyle ), &parserStyleStaticEnd );
			addParser( result, uint32_t( GUISection::eStaticStyle ), cuT( "font" ), &parserStyleStaticFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eStaticStyle ), cuT( "text_material" ), &parserStyleStaticTextMaterial, { makeParameter< ParameterType::eName >() } );
		}

		static void createPanelParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, uint32_t( GUISection::ePanel )
				, &parserPanelTheme
				, &parserPanelStyle
				, &parserPanelEnd );
			createControlsParsers( result, uint32_t( GUISection::ePanel ) );
			createDefaultLayoutParsers( result, uint32_t( GUISection::ePanel ) );

			createStylesParsers( result, uint32_t( GUISection::ePanel ) );
			addParser( result, uint32_t( GUISection::ePanelStyle ), cuT( "background_material" ), &parserStyleBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::ePanelStyle ), cuT( "border_material" ), &parserStyleBorderMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::ePanelStyle ), cuT( "}" ), &parserStylePanelEnd );
		}

		static void createExpandablePanelParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, uint32_t( GUISection::eExpandablePanel )
				, &parserExpandablePanelTheme
				, &parserExpandablePanelStyle
				, &parserExpandablePanelEnd );
			createDefaultLayoutParsers( result, uint32_t( GUISection::eExpandablePanel ) );
			addParser( result, uint32_t( GUISection::eExpandablePanel ), cuT( "header" ), &parserExpandablePanelHeader );
			addParser( result, uint32_t( GUISection::eExpandablePanel ), cuT( "expandable" ), &parserExpandablePanelPanel );

			addParser( result, uint32_t( GUISection::eExpandablePanelStyle ), cuT( "background_material" ), &parserStyleBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eExpandablePanelStyle ), cuT( "border_material" ), &parserStyleBorderMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eExpandablePanelStyle ), cuT( "header_style" ), &parserStyleExpandablePanelHeader );
			addParser( result, uint32_t( GUISection::eExpandablePanelStyle ), cuT( "expand_style" ), &parserStyleExpandablePanelExpand );
			addParser( result, uint32_t( GUISection::eExpandablePanelStyle ), cuT( "content_style" ), &parserStyleExpandablePanelPanel );
			addParser( result, uint32_t( GUISection::eExpandablePanelStyle ), cuT( "}" ), &parserStyleExpandablePanelEnd );
		}

		static void createExpandablePanelHeaderParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, uint32_t( GUISection::eExpandablePanelHeader )
				, ParserFunction{}
				, ParserFunction{}
				, &parserExpandablePanelHeaderEnd );
			createControlsParsers( result, uint32_t( GUISection::eExpandablePanelHeader ) );
			createDefaultLayoutParsers( result, uint32_t( GUISection::eExpandablePanelHeader ) );
		}

		static void createExpandablePanelPanelParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, uint32_t( GUISection::eExpandablePanelPanel )
				, ParserFunction{}
				, ParserFunction{}
				, &parserExpandablePanelPanelEnd );
			createControlsParsers( result, uint32_t( GUISection::eExpandablePanelPanel ) );
			createDefaultLayoutParsers( result, uint32_t( GUISection::eExpandablePanelPanel ) );
		}

		static void createThemeParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "default_font" ), &parserThemeDefaultFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "button_style" ), &parserThemeButtonStyle, { makeDefaultedParameter< ParameterType::eName >( "Button" ) } );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "static_style" ), &parserThemeStaticStyle, { makeDefaultedParameter< ParameterType::eName >( "Static" ) } );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "slider_style" ), &parserThemeSliderStyle, { makeDefaultedParameter< ParameterType::eName >( "Slider" ) } );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "combobox_style" ), &parserThemeComboBoxStyle, { makeDefaultedParameter< ParameterType::eName >( "ComboBox" ) } );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "listbox_style" ), &parserThemeListBoxStyle, { makeDefaultedParameter< ParameterType::eName >( "ListBox" ) } );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "edit_style" ), &parserThemeEditStyle, { makeDefaultedParameter< ParameterType::eName >( "Edit" ) } );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "panel_style" ), &parserThemePanelStyle, { makeDefaultedParameter< ParameterType::eName >( "Panel" ) } );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "expandable_panel_style" ), &parserThemeExpandablePanelStyle, { makeDefaultedParameter< ParameterType::eName >( "ExpandablePanel" ) } );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "}" ), &parserThemeEnd );
		}

		static void createBoxLayoutParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( GUISection::eBoxLayout ), cuT( "layout_ctrl" ), &parserLayoutCtrl, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eBoxLayout ), cuT( "layout_staspace" ), &parserBoxLayoutStaticSpacer, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( GUISection::eBoxLayout ), cuT( "layout_dynspace" ), &parserBoxLayoutDynamicSpacer );
			addParser( result, uint32_t( GUISection::eBoxLayout ), cuT( "horizontal" ), &parserBoxLayoutHorizontal, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( GUISection::eBoxLayout ), cuT( "}" ), &parserLayoutEnd );
		}

		static void createLayoutCtrlParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParser( result, uint32_t( GUISection::eLayoutCtrl ), cuT( "horizontal_align" ), &parserLayoutCtrlHAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
			addParser( result, uint32_t( GUISection::eLayoutCtrl ), cuT( "vertical_align" ), &parserLayoutCtrlVAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );
			addParser( result, uint32_t( GUISection::eLayoutCtrl ), cuT( "stretch" ), &parserLayoutCtrlStretch, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( GUISection::eLayoutCtrl ), cuT( "reserve_if_hidden" ), &parserLayoutCtrlReserveIfHidden, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( GUISection::eLayoutCtrl ), cuT( "padding" ), &parserLayoutCtrlPadding, { makeParameter< ParameterType::ePoint4U >() } );
			addParser( result, uint32_t( GUISection::eLayoutCtrl ), cuT( "pad_left" ), &parserLayoutCtrlPadLeft, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( GUISection::eLayoutCtrl ), cuT( "pad_top" ), &parserLayoutCtrlPadTop, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( GUISection::eLayoutCtrl ), cuT( "pad_right" ), &parserLayoutCtrlPadRight, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( GUISection::eLayoutCtrl ), cuT( "pad_bottom" ), &parserLayoutCtrlPadBottom, { makeParameter< ParameterType::eUInt32 >() } );
			addParser( result, uint32_t( GUISection::eLayoutCtrl ), cuT( "}" ), &parserLayoutCtrlEnd );
		}

		template< typename StyleT >
		StyleT * createControlStyle( Engine & engine
			, castor::String const & name
			, std::map< castor::String, castor::UniquePtr< StyleT > > & controls )
		{
			auto it = controls.find( name );

			if ( it != controls.end() )
			{
				log::warn << "ControlManager: Duplicate style [" + name + "]" << std::endl;
			}
			else
			{
				if constexpr ( std::is_same_v< StyleT, PanelStyle > )
				{
					it = controls.emplace( name
						, castor::makeUnique< StyleT >( name
							, engine ) ).first;
				}
				else
				{
					it = controls.emplace( name
						, castor::makeUnique< StyleT >( name
							, engine
							, engine.getFontCache().begin()->first ) ).first;
				}
			}

			return it->second.get();
		}

		template< typename StyleT >
		StyleT * getControlStyle( castor::String const & name
			, std::map< castor::String, ThemeUPtr > const & themes
			, std::map< castor::String, castor::UniquePtr< StyleT > > const & controls )
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

			if ( !style || themeIt == themes.end() )
			{
				auto it = controls.find( name );

				if ( it != controls.end() )
				{
					style = it->second.get();
				}
			}

			return style;
		}
	}

	castor::String ControlsManager::Name = "c3d.gui";

	ControlsManager::ControlsManager( Engine & engine )
		: UserInputListener{ engine, Name }
		, m_changed{ false }
	{
	}

	ThemeRPtr ControlsManager::createTheme( castor::String const & name )
	{
		auto ires = m_themes.emplace( name, nullptr );
		auto it = ires.first;

		if ( ires.second )
		{
			it->second = castor::makeUnique< Theme >( name, *getEngine() );
		}

		return it->second.get();
	}

	ButtonStyleRPtr ControlsManager::createButtonStyle( castor::String const & name )
	{
		return ctrlmgr::createControlStyle( *getEngine(), name, m_buttonStyles );
	}

	ComboBoxStyleRPtr ControlsManager::createComboBoxStyle( castor::String const & name )
	{
		return ctrlmgr::createControlStyle( *getEngine(), name, m_comboBoxStyles );
	}

	EditStyleRPtr ControlsManager::createEditStyle( castor::String const & name )
	{
		return ctrlmgr::createControlStyle( *getEngine(), name, m_editStyles );
	}

	ListBoxStyleRPtr ControlsManager::createListBoxStyle( castor::String const & name )
	{
		return ctrlmgr::createControlStyle( *getEngine(), name, m_listBoxStyles );
	}

	SliderStyleRPtr ControlsManager::createSliderStyle( castor::String const & name )
	{
		return ctrlmgr::createControlStyle( *getEngine(), name, m_sliderStyles );
	}

	StaticStyleRPtr ControlsManager::createStaticStyle( castor::String const & name )
	{
		return ctrlmgr::createControlStyle( *getEngine(), name, m_staticStyles );
	}

	PanelStyleRPtr ControlsManager::createPanelStyle( castor::String const & name )
	{
		return ctrlmgr::createControlStyle( *getEngine(), name, m_panelStyles );
	}

	ExpandablePanelStyleRPtr ControlsManager::createExpandablePanelStyle( castor::String const & name )
	{
		return ctrlmgr::createControlStyle( *getEngine(), name, m_expandablePanelStyles );
	}

	ThemeRPtr ControlsManager::getTheme( castor::String const & name )
	{
		auto it = m_themes.find( name );

		if ( it == m_themes.end() )
		{
			return nullptr;
		}

		return it->second.get();
	}

	ButtonStyleRPtr ControlsManager::getButtonStyle( castor::String const & name )
	{
		return ctrlmgr::getControlStyle( name, m_themes, m_buttonStyles );
	}

	ComboBoxStyleRPtr ControlsManager::getComboBoxStyle( castor::String const & name )
	{
		return ctrlmgr::getControlStyle( name, m_themes, m_comboBoxStyles );
	}

	EditStyleRPtr ControlsManager::getEditStyle( castor::String const & name )
	{
		return ctrlmgr::getControlStyle( name, m_themes, m_editStyles );
	}

	ExpandablePanelStyleRPtr ControlsManager::getExpandablePanelStyle( castor::String const & name )
	{
		return ctrlmgr::getControlStyle( name, m_themes, m_expandablePanelStyles );
	}

	ListBoxStyleRPtr ControlsManager::getListBoxStyle( castor::String const & name )
	{
		return ctrlmgr::getControlStyle( name, m_themes, m_listBoxStyles );
	}

	PanelStyleRPtr ControlsManager::getPanelStyle( castor::String const & name )
	{
		return ctrlmgr::getControlStyle( name, m_themes, m_panelStyles );
	}

	SliderStyleRPtr ControlsManager::getSliderStyle( castor::String const & name )
	{
		return ctrlmgr::getControlStyle( name, m_themes, m_sliderStyles );
	}

	StaticStyleRPtr ControlsManager::getStaticStyle( castor::String const & name )
	{
		return ctrlmgr::getControlStyle( name, m_themes, m_staticStyles );
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
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };

		if ( m_controlsById.find( control->getId() ) != m_controlsById.end() )
		{
			CU_Exception( "A control with this ID already exists in the manager" );
		}

		m_controlsById.insert( std::make_pair( control->getId(), control ) );
		m_changed = true;
	}

	void ControlsManager::removeControl( ControlID id )
	{
		doRemoveControl( id );
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

	castor::AttributeParsers ControlsManager::createParsers( castor3d::Engine & engine )
	{
		using namespace castor;
		AttributeParsers result;

		addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "gui" ), &parserGui );
		addParser( result, uint32_t( CSCNSection::eScene ), cuT( "gui" ), &parserGui );

		ctrlmgr::createGuiParsers( result, uint32_t( CSCNSection::eRoot ) );
		ctrlmgr::createGuiParsers( result, uint32_t( CSCNSection::eScene ) );
		ctrlmgr::createGuiParsers( result, uint32_t( GUISection::eGUI ) );
		ctrlmgr::createThemeParsers( result );
		ctrlmgr::createButtonParsers( result );
		ctrlmgr::createComboBoxParsers( result );
		ctrlmgr::createEditParsers( result );
		ctrlmgr::createListBoxParsers( result );
		ctrlmgr::createSliderParsers( result );
		ctrlmgr::createStaticParsers( result );
		ctrlmgr::createPanelParsers( result );
		ctrlmgr::createExpandablePanelParsers( result );
		ctrlmgr::createExpandablePanelHeaderParsers( result );
		ctrlmgr::createExpandablePanelPanelParsers( result );
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
			{ uint32_t( GUISection::eExpandablePanelStyle ), cuT( "expandable_panel_style" ) },
			{ uint32_t( GUISection::eButton ), cuT( "button" ) },
			{ uint32_t( GUISection::eStatic ), cuT( "static" ) },
			{ uint32_t( GUISection::eSlider ), cuT( "slider" ) },
			{ uint32_t( GUISection::eComboBox ), cuT( "combobox" ) },
			{ uint32_t( GUISection::eListBox ), cuT( "listbox" ) },
			{ uint32_t( GUISection::eEdit ), cuT( "edit" ) },
			{ uint32_t( GUISection::ePanel ), cuT( "panel" ) },
			{ uint32_t( GUISection::eExpandablePanel ), cuT( "expandable_panel" ) },
			{ uint32_t( GUISection::eExpandablePanelHeader ), cuT( "header" ) },
			{ uint32_t( GUISection::eExpandablePanelPanel ), cuT( "expandable" ) },
			{ uint32_t( GUISection::eBoxLayout ), cuT( "box_layout" ) },
			{ uint32_t( GUISection::eLayoutCtrl ), cuT( "layout_ctrl" ) },
		};
	}

	void * ControlsManager::createContext( castor::FileParserContext & context )
	{
		GuiParserContext * userContext = new GuiParserContext;
		userContext->engine = static_cast< castor3d::SceneFileParser * >( context.parser )->getEngine();
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
			, [this, &control]( castor::String const & text )
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

	void ControlsManager::connectEvents( SliderCtrl & control )
	{
		m_onSliderTracks.emplace( &control, control.connect( SliderEvent::eThumbTrack
			, [this, &control]( int index )
			{
				onSelectAction( control.getName(), index );
			} ) );
		m_onSliderReleases.emplace( &control, control.connect( SliderEvent::eThumbTrack
			, [this, &control]( int index )
			{
				onSelectAction( control.getName(), index );
			} ) );
	}

	void ControlsManager::disconnectEvents( ButtonCtrl & control )
	{
		auto it = m_onButtonClicks.find( &control );

		if ( it != m_onButtonClicks.end() )
		{
			m_onButtonClicks.erase( it );
		}
	}

	void ControlsManager::disconnectEvents( ComboBoxCtrl & control )
	{
		auto it = m_onComboSelects.find( &control );

		if ( it != m_onComboSelects.end() )
		{
			m_onComboSelects.erase( it );
		}
	}

	void ControlsManager::disconnectEvents( EditCtrl & control )
	{
		auto it = m_onEditUpdates.find( &control );

		if ( it != m_onEditUpdates.end() )
		{
			m_onEditUpdates.erase( it );
		}
	}

	void ControlsManager::disconnectEvents( ExpandablePanelCtrl & control )
	{
		auto it = m_onPanelExpands.find( &control );

		if ( it != m_onPanelExpands.end() )
		{
			m_onPanelExpands.erase( it );
		}

		it = m_onPanelRetracts.find( &control );

		if ( it != m_onPanelRetracts.end() )
		{
			m_onPanelRetracts.erase( it );
		}
	}

	void ControlsManager::disconnectEvents( ListBoxCtrl & control )
	{
		auto it = m_onListSelects.find( &control );

		if ( it != m_onListSelects.end() )
		{
			m_onListSelects.erase( it );
		}
	}

	void ControlsManager::disconnectEvents( SliderCtrl & control )
	{
		auto it = m_onSliderTracks.find( &control );

		if ( it != m_onSliderTracks.end() )
		{
			m_onSliderTracks.erase( it );
		}

		it = m_onSliderReleases.find( &control );

		if ( it != m_onSliderReleases.end() )
		{
			m_onSliderReleases.erase( it );
		}
	}

	bool ControlsManager::doInitialise()
	{
		return true;
	}

	void ControlsManager::doCleanup()
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexHandlers ) };

		for ( auto handler : m_handlers )
		{
			std::static_pointer_cast< Control >( handler )->destroy();
		}
	}

	EventHandler * ControlsManager::doGetMouseTargetableHandler( castor::Position const & position )const
	{
		if ( m_changed )
		{
			doUpdate();
		}

		auto controls = doGetControlsByZIndex();
		EventHandler * result{};
		auto it = controls.rbegin();

		while ( !result && it != controls.rend() )
		{
			Control * control = *it;

			if ( control
				&& control->catchesMouseEvents()
				&& control->getAbsolutePosition().x() <= position.x()
				&& control->getAbsolutePosition().x() + int32_t( control->getSize().getWidth() ) > position.x()
				&& control->getAbsolutePosition().y() <= position.y()
				&& control->getAbsolutePosition().y() + int32_t( control->getSize().getHeight() ) > position.y() )
			{
				result = control;
			}

			++it;
		}

		return result;
	}

	void ControlsManager::doUpdate()const
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsByZIndex ) };
		{
			auto handlers = doGetHandlers();
			m_controlsByZIndex.clear();

			for ( auto handler : handlers )
			{
				m_controlsByZIndex.push_back( static_cast< Control * >( handler.get() ) );
			}
		}

		std::sort( m_controlsByZIndex.begin()
			, m_controlsByZIndex.end()
			, []( Control * lhs, Control * rhs )
			{
				bool result = false;
				auto lhsBg = lhs ? lhs->getBackground() : nullptr;
				auto rhsBg = rhs ? rhs->getBackground() : nullptr;

				if ( !lhs || !lhsBg )
				{
					result = true;
				}
				else if ( !rhs || !rhsBg )
				{
					result = true;
				}
				else
				{
					uint64_t a = lhsBg->getIndex() + lhsBg->getLevel() * 1000ull;
					uint64_t b = rhsBg->getIndex() + rhsBg->getLevel() * 1000ull;
					result = a < b;
				}

				return result;
			} );
	}

	void ControlsManager::doRemoveControl( ControlID id )
	{
		EventHandler * handler;
		{
			ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };
			auto it = m_controlsById.find( id );

			if ( it == m_controlsById.end() )
			{
				CU_Exception( "This control does not exist in the manager." );
			}

			m_controlsById.erase( it );
			handler = it->second.lock().get();
		}

		m_changed = true;
		doRemoveHandler( *handler );
	}

	std::vector< Control * > ControlsManager::doGetControlsByZIndex()const
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };
		std::vector< Control * > result;

		if ( !m_controlsByZIndex.empty() )
		{
			result = m_controlsByZIndex;
		}

		return result;
	}

	std::map< ControlID, ControlWPtr > ControlsManager::doGetControlsById()const
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };
		std::map< ControlID, ControlWPtr > result;

		if ( !m_controlsById.empty() )
		{
			result = m_controlsById;
		}

		return result;
	}
}
