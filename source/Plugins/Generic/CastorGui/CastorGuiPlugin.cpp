#include "CastorGui/ControlsManager.hpp"
#include "CastorGui/CastorGui_Parsers.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/FileParser/ParserParameter.hpp>
#include <CastorUtils/Log/Logger.hpp>

using namespace castor3d;
using namespace castor;

namespace CastorGui
{
	namespace
	{
		void parseError( String const & p_error )
		{
			StringStream strError{ makeStringStream() };
			strError << cuT( "Error, : " ) << p_error;
			log::error << strError.str() << std::endl;
		}

		void addParser( AttributeParsers & parsers
			, uint32_t section
			, String const & name
			, ParserFunction function
			, ParserParameterArray && array = ParserParameterArray() )
		{
			auto nameIt = parsers.find( name );

			if ( nameIt != parsers.end()
				&& nameIt->second.find( section ) != nameIt->second.end() )
			{
				parseError( cuT( "Parser " ) + name + cuT( " for section " ) + castor::string::toString( section ) + cuT( " already exists." ) );
			}
			else
			{
				parsers[name][section] = { function, array };
			}
		}

		void createDefaultParsers( AttributeParsers & parsers
			, uint32_t section
			, ParserFunction endFunction )
		{
			addParser( parsers, section, cuT( "pixel_position" ), &parserControlPixelPosition, { makeParameter< ParameterType::ePosition >() } );
			addParser( parsers, section, cuT( "pixel_size" ), &parserControlPixelSize, { makeParameter< ParameterType::eSize >() } );
			addParser( parsers, section, cuT( "pixel_border_size" ), &parserControlPixelBorderSize, { makeParameter< ParameterType::eRectangle >() } );
			addParser( parsers, section, cuT( "border_inner_uv" ), &parserControlBorderInnerUv, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( parsers, section, cuT( "border_outer_uv" ), &parserControlBorderOuterUv, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( parsers, section, cuT( "center_uv" ), &parserControlCenterUv, { makeParameter< ParameterType::ePoint4D >() } );
			addParser( parsers, section, cuT( "visible" ), &parserControlVisible, { makeParameter< ParameterType::eBool >() } );
			addParser( parsers, section, cuT( "button" ), &parserButton, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "static" ), &parserStatic, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "slider" ), &parserSlider, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "combobox" ), &parserComboBox, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "listbox" ), &parserListBox, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "edit" ), &parserEdit, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "}" ), endFunction );
		}

		void createDefaultStyleParsers( AttributeParsers & parsers
			, uint32_t section
			, ParserFunction endFunction )
		{
			addParser( parsers, section, cuT( "background_material" ), &parserStyleBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "foreground_material" ), &parserStyleForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "border_material" ), &parserStyleBorderMaterial, { makeParameter< ParameterType::eName >() } );
		}

		AttributeParsers CreateParsers( castor3d::Engine * engine )
		{
			static UInt32StrMap mapHAligns
			{
				{ "left", uint32_t( HAlign::eLeft ) },
				{ "center", uint32_t( HAlign::eCenter ) },
				{ "right", uint32_t( HAlign::eRight ) }
			};
			static UInt32StrMap mapVAligns
			{
				{ "top", uint32_t( VAlign::eTop ) },
				{ "center", uint32_t( VAlign::eCenter ) },
				{ "bottom", uint32_t( VAlign::eBottom ) }
			};
			AttributeParsers result;

			addParser( result, uint32_t( CSCNSection::eRoot ), cuT( "gui" ), &parserGui );
			addParser( result, uint32_t( CSCNSection::eScene ), cuT( "gui" ), &parserGui );

			addParser( result, uint32_t( GUISection::eGUI ), cuT( "button" ), &parserButton, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "static" ), &parserStatic, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "slider" ), &parserSlider, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "combobox" ), &parserComboBox, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "listbox" ), &parserListBox, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "edit" ), &parserEdit, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "theme" ), &parserTheme, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "}" ), &parserGuiEnd );

			createDefaultParsers( result, uint32_t( GUISection::eButton ), &parserButtonEnd );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "theme" ), &parserButtonTheme, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "caption" ), &parserButtonCaption, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "horizontal_align" ), &parserButtonHAlign, { makeParameter< ParameterType::eCheckedText >( mapHAligns ) } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "vertical_align" ), &parserButtonVAlign, { makeParameter< ParameterType::eCheckedText >( mapVAligns ) } );

			createDefaultParsers( result, uint32_t( GUISection::eComboBox ), &parserComboBoxEnd );
			addParser( result, uint32_t( GUISection::eComboBox ), cuT( "theme" ), &parserComboBoxTheme, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eComboBox ), cuT( "item" ), &parserComboBoxItem, { makeParameter< ParameterType::eText >() } );

			createDefaultParsers( result, uint32_t( GUISection::eEdit ), &parserEditEnd );
			addParser( result, uint32_t( GUISection::eEdit ), cuT( "theme" ), &parserEditTheme, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eEdit ), cuT( "multiline" ), &parserEditMultiLine, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( GUISection::eEdit ), cuT( "caption" ), &parserEditCaption, { makeParameter< ParameterType::eText >() } );

			createDefaultParsers( result, uint32_t( GUISection::eListBox ), &parserListBoxEnd );
			addParser( result, uint32_t( GUISection::eListBox ), cuT( "theme" ), &parserListBoxTheme, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eListBox ), cuT( "item" ), &parserListBoxItem, { makeParameter< ParameterType::eText >() } );

			createDefaultParsers( result, uint32_t( GUISection::eSlider ), &parserSliderEnd );
			addParser( result, uint32_t( GUISection::eSlider ), cuT( "theme" ), &parserSliderTheme, { makeParameter< ParameterType::eName >() } );

			createDefaultParsers( result, uint32_t( GUISection::eStatic ), &parserStaticEnd );
			addParser( result, uint32_t( GUISection::eStatic ), cuT( "theme" ), &parserStaticTheme, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eStatic ), cuT( "horizontal_align" ), &parserStaticHAlign, { makeParameter< ParameterType::eCheckedText >( mapHAligns ) } );
			addParser( result, uint32_t( GUISection::eStatic ), cuT( "vertical_align" ), &parserStaticVAlign, { makeParameter< ParameterType::eCheckedText >( mapVAligns ) } );
			addParser( result, uint32_t( GUISection::eStatic ), cuT( "caption" ), &parserStaticCaption, { makeParameter< ParameterType::eText >() } );

			addParser( result, uint32_t( GUISection::eTheme ), cuT( "default_font" ), &parserDefaultFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "button_style" ), &parserButtonStyle );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "static_style" ), &parserStaticStyle );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "slider_style" ), &parserSliderStyle );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "combobox_style" ), &parserComboBoxStyle );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "listbox_style" ), &parserListBoxStyle );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "edit_style" ), &parserEditStyle );
			addParser( result, uint32_t( GUISection::eTheme ), cuT( "}" ), &parserThemeEnd );

			createDefaultStyleParsers( result, uint32_t( GUISection::eButtonStyle ), &parserStyleButtonEnd );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "text_material" ), &parserStyleButtonTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "highlighted_background_material" ), &parserStyleButtonHighlightedBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "highlighted_foreground_material" ), &parserStyleButtonHighlightedForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "highlighted_text_material" ), &parserStyleButtonHighlightedTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "pushed_background_material" ), &parserStyleButtonPushedBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "pushed_foreground_material" ), &parserStyleButtonPushedForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "pushed_text_material" ), &parserStyleButtonPushedTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButtonStyle ), cuT( "font" ), &parserStyleButtonFont, { makeParameter< ParameterType::eName >() } );

			createDefaultStyleParsers( result, uint32_t( GUISection::eComboStyle ), &parserStyleComboBoxEnd );
			addParser( result, uint32_t( GUISection::eComboStyle ), cuT( "button_style" ), &parserComboButtonStyle );
			addParser( result, uint32_t( GUISection::eComboStyle ), cuT( "listbox_style" ), &parserComboListBoxStyle );

			createDefaultStyleParsers( result, uint32_t( GUISection::eEditStyle ), &parserStyleEditEnd );
			addParser( result, uint32_t( GUISection::eEditStyle ), cuT( "font" ), &parserStyleEditFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eEditStyle ), cuT( "text_material" ), &parserStyleEditTextMaterial, { makeParameter< ParameterType::eName >() } );

			createDefaultStyleParsers( result, uint32_t( GUISection::eListStyle ), &parserStyleListBoxEnd );
			addParser( result, uint32_t( GUISection::eListStyle ), cuT( "item_style" ), &parserItemStaticStyle );
			addParser( result, uint32_t( GUISection::eListStyle ), cuT( "selected_item_style" ), &parserSelItemStaticStyle );
			addParser( result, uint32_t( GUISection::eListStyle ), cuT( "highlighted_item_style" ), &parserHighItemStaticStyle );

			createDefaultStyleParsers( result, uint32_t( GUISection::eSliderStyle ), &parserStyleSliderEnd );
			addParser( result, uint32_t( GUISection::eSliderStyle ), cuT( "line_style" ), &parserLineStaticStyle );
			addParser( result, uint32_t( GUISection::eSliderStyle ), cuT( "tick_style" ), &parserTickStaticStyle );

			createDefaultStyleParsers( result, uint32_t( GUISection::eStaticStyle ), &parserStyleStaticEnd );
			addParser( result, uint32_t( GUISection::eStaticStyle ), cuT( "font" ), &parserStyleStaticFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eStaticStyle ), cuT( "text_material" ), &parserStyleStaticTextMaterial, { makeParameter< ParameterType::eName >() } );

			return result;
		}

		StrUInt32Map CreateSections()
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
				{ uint32_t( GUISection::eButton ), cuT( "button" ) },
				{ uint32_t( GUISection::eStatic ), cuT( "static" ) },
				{ uint32_t( GUISection::eSlider ), cuT( "slider" ) },
				{ uint32_t( GUISection::eComboBox ), cuT( "combobox" ) },
				{ uint32_t( GUISection::eListBox ), cuT( "listbox" ) },
				{ uint32_t( GUISection::eEdit ), cuT( "edit" ) },
			};
		}

		void * createContext( castor::FileParserContext & context )
		{
			CastorGui::ParserContext * userContext = new CastorGui::ParserContext;
			userContext->engine = static_cast< castor3d::SceneFileParser * >( context.parser )->getEngine();
			return userContext;
		}
	}
}

extern "C"
{
	C3D_CGui_API void getRequiredVersion( castor3d::Version * p_version );
	C3D_CGui_API void getType( castor3d::PluginType * p_type );
	C3D_CGui_API void getName( char const ** p_name );
	C3D_CGui_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin );
	C3D_CGui_API void OnUnload( castor3d::Engine * engine );

	C3D_CGui_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_CGui_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eGeneric;
	}

	C3D_CGui_API void getName( char const ** p_name )
	{
		static String const Name = cuT( "Castor GUI" );
		*p_name = Name.c_str();
	}

	C3D_CGui_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
	{
		engine->registerParsers( CastorGui::PLUGIN_NAME
			, std::move( CastorGui::CreateParsers( engine ) )
			, std::move( CastorGui::CreateSections() )
			, &CastorGui::createContext );
		engine->setUserInputListener( std::make_shared< CastorGui::ControlsManager >( *engine ) );
	}

	C3D_CGui_API void OnUnload( castor3d::Engine * engine )
	{
		engine->setUserInputListener( nullptr );
		engine->unregisterParsers( CastorGui::PLUGIN_NAME );
	}
}
