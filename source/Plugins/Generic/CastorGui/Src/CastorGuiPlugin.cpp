#include <Engine.hpp>
#include <ListenerManager.hpp>
#include <RenderSystem.hpp>
#include <SceneFileParser.hpp>

#include <Logger.hpp>

#include "ControlsManager.hpp"
#include "CastorGui_Parsers.hpp"

using namespace Castor3D;
using namespace Castor;

namespace CastorGui
{
	namespace
	{
		void ParseError( String const & p_strError )
		{
			StringStream l_strError;
			l_strError << cuT( "Error, : " ) << p_strError;
			Logger::LogError( l_strError.str() );
		}

		void AddParser( FileParser::AttributeParsersBySection & p_parsers, uint32_t p_section, String const & p_name, ParserFunction p_function, ParserParameterArray && p_array = ParserParameterArray() )
		{
			auto && l_sectionIt = p_parsers.find( p_section );

			if ( l_sectionIt != p_parsers.end() && l_sectionIt->second.find( p_name ) != l_sectionIt->second.end() )
			{
				ParseError( cuT( "Parser " ) + p_name + cuT( " for section " ) + string::to_string( p_section ) + cuT( " already exists." ) );
			}
			else
			{
				p_parsers[p_section][p_name] = { p_function, p_array };
			}
		}

		void CreateDefaultParsers( FileParser::AttributeParsersBySection & p_parsers, CastorGui::eSECTION p_section, ParserFunction p_endFunction )
		{
			AddParser( p_parsers, p_section, cuT( "pixel_position" ), &Parser_ControlPixelPosition, { MakeParameter< ePARAMETER_TYPE_POSITION >() } );
			AddParser( p_parsers, p_section, cuT( "pixel_size" ), &Parser_ControlPixelSize, { MakeParameter< ePARAMETER_TYPE_SIZE >() } );
			AddParser( p_parsers, p_section, cuT( "pixel_border_size" ), &Parser_ControlPixelBorderSize, { MakeParameter< ePARAMETER_TYPE_RECTANGLE >() } );
			AddParser( p_parsers, p_section, cuT( "background_material" ), &Parser_ControlBackgroundMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( p_parsers, p_section, cuT( "border_material" ), &Parser_ControlBorderMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( p_parsers, p_section, cuT( "border_inner_uv" ), &Parser_ControlBorderInnerUv, { MakeParameter< ePARAMETER_TYPE_POINT4D >() } );
			AddParser( p_parsers, p_section, cuT( "border_outer_uv" ), &Parser_ControlBorderOuterUv, { MakeParameter< ePARAMETER_TYPE_POINT4D >() } );
			AddParser( p_parsers, p_section, cuT( "center_uv" ), &Parser_ControlCenterUv, { MakeParameter< ePARAMETER_TYPE_POINT4D >() } );
			AddParser( p_parsers, p_section, cuT( "button" ), &Parser_Button, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( p_parsers, p_section, cuT( "static" ), &Parser_Static, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( p_parsers, p_section, cuT( "slider" ), &Parser_Slider, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( p_parsers, p_section, cuT( "combobox" ), &Parser_ComboBox, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( p_parsers, p_section, cuT( "listbox" ), &Parser_ListBox, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( p_parsers, p_section, cuT( "edit" ), &Parser_Edit, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( p_parsers, p_section, cuT( "}" ), p_endFunction );
		}

		FileParser::AttributeParsersBySection CreateParsers( Castor3D::Engine * p_engine )
		{
			FileParser::AttributeParsersBySection l_return;

			AddParser( l_return, eSECTION_ROOT, cuT( "gui" ), &Parser_Gui );

			AddParser( l_return, eSECTION_GUI, cuT( "default_font" ), &Parser_DefaultFont, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_GUI, cuT( "button" ), &Parser_Button, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_GUI, cuT( "static" ), &Parser_Static, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_GUI, cuT( "slider" ), &Parser_Slider, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_GUI, cuT( "combobox" ), &Parser_ComboBox, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_GUI, cuT( "listbox" ), &Parser_ListBox, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_GUI, cuT( "edit" ), &Parser_Edit, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_GUI, cuT( "}" ), &Parser_GuiEnd );

			CreateDefaultParsers( l_return, eSECTION_BUTTON, &Parser_ButtonEnd );
			AddParser( l_return, eSECTION_BUTTON, cuT( "text_material" ), &Parser_ButtonTextMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_BUTTON, cuT( "highlighted_background_material" ), &Parser_ButtonHighlightedBackgroundMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_BUTTON, cuT( "highlighted_foreground_material" ), &Parser_ButtonHighlightedForegroundMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_BUTTON, cuT( "highlighted_text_material" ), &Parser_ButtonHighlightedTextMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_BUTTON, cuT( "pushed_background_material" ), &Parser_ButtonPushedBackgroundMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_BUTTON, cuT( "pushed_foreground_material" ), &Parser_ButtonPushedForegroundMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_BUTTON, cuT( "pushed_text_material" ), &Parser_ButtonPushedTextMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_BUTTON, cuT( "font" ), &Parser_ButtonFont, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_BUTTON, cuT( "caption" ), &Parser_ButtonCaption, { MakeParameter< ePARAMETER_TYPE_TEXT >() } );

			CreateDefaultParsers( l_return, eSECTION_LISTBOX, &Parser_ListBoxEnd );
			AddParser( l_return, eSECTION_LISTBOX, cuT( "font" ), &Parser_ListBoxFont, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_LISTBOX, cuT( "item" ), &Parser_ListBoxItem, { MakeParameter< ePARAMETER_TYPE_TEXT >() } );
			AddParser( l_return, eSECTION_LISTBOX, cuT( "selected_item_background_material" ), &Parser_ListBoxSelectedItemBackgroundMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_LISTBOX, cuT( "selected_item_foreground_material" ), &Parser_ListBoxSelectedItemForegroundMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_LISTBOX, cuT( "highlighted_item_background_material" ), &Parser_ListBoxHighlightedItemBackgroundMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );

			CreateDefaultParsers( l_return, eSECTION_COMBOBOX, &Parser_ComboBoxEnd );
			AddParser( l_return, eSECTION_COMBOBOX, cuT( "font" ), &Parser_ComboBoxFont, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_COMBOBOX, cuT( "item" ), &Parser_ComboBoxItem, { MakeParameter< ePARAMETER_TYPE_TEXT >() } );
			AddParser( l_return, eSECTION_COMBOBOX, cuT( "selected_item_background_material" ), &Parser_ComboBoxSelectedItemBackgroundMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_COMBOBOX, cuT( "selected_item_foreground_material" ), &Parser_ComboBoxSelectedItemForegroundMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_COMBOBOX, cuT( "highlighted_item_background_material" ), &Parser_ComboBoxHighlightedItemBackgroundMaterial, { MakeParameter< ePARAMETER_TYPE_NAME >() } );

			CreateDefaultParsers( l_return, eSECTION_STATIC, &Parser_StaticEnd );
			AddParser( l_return, eSECTION_STATIC, cuT( "font" ), &Parser_StaticFont, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_STATIC, cuT( "caption" ), &Parser_StaticCaption, { MakeParameter< ePARAMETER_TYPE_TEXT >() } );

			CreateDefaultParsers( l_return, eSECTION_EDIT, &Parser_EditEnd );
			AddParser( l_return, eSECTION_EDIT, cuT( "font" ), &Parser_EditFont, { MakeParameter< ePARAMETER_TYPE_NAME >() } );
			AddParser( l_return, eSECTION_EDIT, cuT( "multiline" ), &Parser_EditMultiLine, { MakeParameter< ePARAMETER_TYPE_BOOL >() } );
			AddParser( l_return, eSECTION_EDIT, cuT( "caption" ), &Parser_EditCaption, { MakeParameter< ePARAMETER_TYPE_TEXT >() } );

			CreateDefaultParsers( l_return, eSECTION_SLIDER, &Parser_SliderEnd );

			return l_return;
		}

		StrUIntMap CreateSections()
		{
			return
			{
				{ eSECTION_GUI, cuT( "gui" ) },
				{ eSECTION_BUTTON, cuT( "button" ) },
				{ eSECTION_STATIC, cuT( "static" ) },
				{ eSECTION_SLIDER, cuT( "slider" ) },
				{ eSECTION_COMBOBOX, cuT( "combobox" ) },
				{ eSECTION_LISTBOX, cuT( "listbox" ) },
				{ eSECTION_EDIT, cuT( "edit" ) },
			};
		}
	}
}

C3D_CGui_API void GetRequiredVersion( Version & p_version )
{
	p_version = Version();
}

C3D_CGui_API ePLUGIN_TYPE GetType()
{
	return ePLUGIN_TYPE_GENERIC;
}

C3D_CGui_API String GetName()
{
	return cuT( "Castor GUI" );
}

C3D_CGui_API void OnLoad( Castor3D::Engine * p_engine )
{
	p_engine->RegisterParsers( CastorGui::PLUGIN_NAME, std::move( CastorGui::CreateParsers( p_engine ) ) );
	p_engine->RegisterSections( CastorGui::PLUGIN_NAME, std::move( CastorGui::CreateSections() ) );
	p_engine->GetListenerManager().Insert( CastorGui::PLUGIN_NAME, std::make_shared< CastorGui::ControlsManager >( p_engine ) );
}

C3D_CGui_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->GetListenerManager().Remove( CastorGui::PLUGIN_NAME );
	p_engine->UnregisterParsers( CastorGui::PLUGIN_NAME );
	p_engine->UnregisterSections( CastorGui::PLUGIN_NAME );
}
