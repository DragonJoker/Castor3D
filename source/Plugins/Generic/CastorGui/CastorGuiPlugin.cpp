#include "CastorGui/ControlsManager.hpp"
#include "CastorGui/CastorGui_Parsers.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/Cache.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

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
			addParser( parsers, section, cuT( "background_material" ), &parserControlBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "foreground_material" ), &parserControlForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( parsers, section, cuT( "border_material" ), &parserControlBorderMaterial, { makeParameter< ParameterType::eName >() } );
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

			addParser( result, uint32_t( GUISection::eGUI ), cuT( "default_font" ), &parserDefaultFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "button" ), &parserButton, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "static" ), &parserStatic, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "slider" ), &parserSlider, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "combobox" ), &parserComboBox, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "listbox" ), &parserListBox, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "edit" ), &parserEdit, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eGUI ), cuT( "}" ), &parserGuiEnd );

			createDefaultParsers( result, uint32_t( GUISection::eButton ), &parserButtonEnd );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "text_material" ), &parserButtonTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "highlighted_background_material" ), &parserButtonHighlightedBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "highlighted_foreground_material" ), &parserButtonHighlightedForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "highlighted_text_material" ), &parserButtonHighlightedTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "pushed_background_material" ), &parserButtonPushedBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "pushed_foreground_material" ), &parserButtonPushedForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "pushed_text_material" ), &parserButtonPushedTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "font" ), &parserButtonFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "caption" ), &parserButtonCaption, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "horizontal_align" ), &parserButtonHAlign, { makeParameter< ParameterType::eCheckedText >( mapHAligns ) } );
			addParser( result, uint32_t( GUISection::eButton ), cuT( "vertical_align" ), &parserButtonVAlign, { makeParameter< ParameterType::eCheckedText >( mapVAligns ) } );

			createDefaultParsers( result, uint32_t( GUISection::eListBox ), &parserListBoxEnd );
			addParser( result, uint32_t( GUISection::eListBox ), cuT( "font" ), &parserListBoxFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eListBox ), cuT( "item" ), &parserListBoxItem, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( GUISection::eListBox ), cuT( "text_material" ), &parserListBoxTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eListBox ), cuT( "selected_item_background_material" ), &parserListBoxSelectedItemBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eListBox ), cuT( "selected_item_foreground_material" ), &parserListBoxSelectedItemForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eListBox ), cuT( "highlighted_item_background_material" ), &parserListBoxHighlightedItemBackgroundMaterial, { makeParameter< ParameterType::eName >() } );

			createDefaultParsers( result, uint32_t( GUISection::eComboBox ), &parserComboBoxEnd );
			addParser( result, uint32_t( GUISection::eComboBox ), cuT( "font" ), &parserComboBoxFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eComboBox ), cuT( "item" ), &parserComboBoxItem, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( GUISection::eComboBox ), cuT( "text_material" ), &parserComboBoxTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eComboBox ), cuT( "selected_item_background_material" ), &parserComboBoxSelectedItemBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eComboBox ), cuT( "selected_item_foreground_material" ), &parserComboBoxSelectedItemForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eComboBox ), cuT( "highlighted_item_background_material" ), &parserComboBoxHighlightedItemBackgroundMaterial, { makeParameter< ParameterType::eName >() } );

			createDefaultParsers( result, uint32_t( GUISection::eStatic ), &parserStaticEnd );
			addParser( result, uint32_t( GUISection::eStatic ), cuT( "font" ), &parserStaticFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eStatic ), cuT( "caption" ), &parserStaticCaption, { makeParameter< ParameterType::eText >() } );
			addParser( result, uint32_t( GUISection::eStatic ), cuT( "horizontal_align" ), &parserStaticHAlign, { makeParameter< ParameterType::eCheckedText >( mapHAligns ) } );
			addParser( result, uint32_t( GUISection::eStatic ), cuT( "vertical_align" ), &parserStaticVAlign, { makeParameter< ParameterType::eCheckedText >( mapVAligns ) } );
			addParser( result, uint32_t( GUISection::eStatic ), cuT( "text_material" ), &parserStaticTextMaterial, { makeParameter< ParameterType::eName >() } );

			createDefaultParsers( result, uint32_t( GUISection::eEdit ), &parserEditEnd );
			addParser( result, uint32_t( GUISection::eEdit ), cuT( "font" ), &parserEditFont, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eEdit ), cuT( "text_material" ), &parserEditTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParser( result, uint32_t( GUISection::eEdit ), cuT( "multiline" ), &parserEditMultiLine, { makeParameter< ParameterType::eBool >() } );
			addParser( result, uint32_t( GUISection::eEdit ), cuT( "caption" ), &parserEditCaption, { makeParameter< ParameterType::eText >() } );

			createDefaultParsers( result, uint32_t( GUISection::eSlider ), &parserSliderEnd );

			return result;
		}

		StrUInt32Map CreateSections()
		{
			return
			{
				{ uint32_t( GUISection::eGUI ), cuT( "gui" ) },
				{ uint32_t( GUISection::eButton ), cuT( "button" ) },
				{ uint32_t( GUISection::eStatic ), cuT( "static" ) },
				{ uint32_t( GUISection::eSlider ), cuT( "slider" ) },
				{ uint32_t( GUISection::eComboBox ), cuT( "combobox" ) },
				{ uint32_t( GUISection::eListBox ), cuT( "listbox" ) },
				{ uint32_t( GUISection::eEdit ), cuT( "edit" ) },
			};
		}
	}
}

extern "C"
{
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
		engine->registerParsers( CastorGui::PLUGIN_NAME, std::move( CastorGui::CreateParsers( engine ) ) );
		engine->registerSections( CastorGui::PLUGIN_NAME, std::move( CastorGui::CreateSections() ) );
		engine->setUserInputListener( std::make_shared< CastorGui::ControlsManager >( *engine ) );
	}

	C3D_CGui_API void OnUnload( castor3d::Engine * engine )
	{
		engine->setUserInputListener( nullptr );
		engine->unregisterParsers( CastorGui::PLUGIN_NAME );
		engine->unregisterSections( CastorGui::PLUGIN_NAME );
	}
}
