#include <Engine.hpp>
#include <Cache/ListenerCache.hpp>

#include <Render/RenderSystem.hpp>
#include <Scene/SceneFileParser.hpp>

#include <Log/Logger.hpp>

#include "ControlsManager.hpp"
#include "CastorGui_Parsers.hpp"

using namespace Castor3D;
using namespace Castor;

namespace CastorGui
{
	namespace
	{
		void ParseError( String const & p_error )
		{
			StringStream l_strError;
			l_strError << cuT( "Error, : " ) << p_error;
			Logger::LogError( l_strError.str() );
		}

		void AddParser( FileParser::AttributeParsersBySection & p_parsers, uint32_t p_section, String const & p_name, ParserFunction p_function, ParserParameterArray && p_array = ParserParameterArray() )
		{
			auto l_sectionIt = p_parsers.find( p_section );

			if ( l_sectionIt != p_parsers.end() && l_sectionIt->second.find( p_name ) != l_sectionIt->second.end() )
			{
				ParseError( cuT( "Parser " ) + p_name + cuT( " for section " ) + string::to_string( p_section ) + cuT( " already exists." ) );
			}
			else
			{
				p_parsers[p_section][p_name] = { p_function, p_array };
			}
		}

		void CreateDefaultParsers( FileParser::AttributeParsersBySection & p_parsers, uint32_t p_section, ParserFunction p_endFunction )
		{
			AddParser( p_parsers, p_section, cuT( "pixel_position" ), &Parser_ControlPixelPosition, { MakeParameter< ParameterType::ePosition >() } );
			AddParser( p_parsers, p_section, cuT( "pixel_size" ), &Parser_ControlPixelSize, { MakeParameter< ParameterType::eSize >() } );
			AddParser( p_parsers, p_section, cuT( "pixel_border_size" ), &Parser_ControlPixelBorderSize, { MakeParameter< ParameterType::eRectangle >() } );
			AddParser( p_parsers, p_section, cuT( "background_material" ), &Parser_ControlBackgroundMaterial, { MakeParameter< ParameterType::eName >() } );
			AddParser( p_parsers, p_section, cuT( "border_material" ), &Parser_ControlBorderMaterial, { MakeParameter< ParameterType::eName >() } );
			AddParser( p_parsers, p_section, cuT( "border_inner_uv" ), &Parser_ControlBorderInnerUv, { MakeParameter< ParameterType::ePoint4D >() } );
			AddParser( p_parsers, p_section, cuT( "border_outer_uv" ), &Parser_ControlBorderOuterUv, { MakeParameter< ParameterType::ePoint4D >() } );
			AddParser( p_parsers, p_section, cuT( "center_uv" ), &Parser_ControlCenterUv, { MakeParameter< ParameterType::ePoint4D >() } );
			AddParser( p_parsers, p_section, cuT( "visible" ), &Parser_ControlVisible, { MakeParameter< ParameterType::eBool >() } );
			AddParser( p_parsers, p_section, cuT( "button" ), &Parser_Button, { MakeParameter< ParameterType::eName >() } );
			AddParser( p_parsers, p_section, cuT( "static" ), &Parser_Static, { MakeParameter< ParameterType::eName >() } );
			AddParser( p_parsers, p_section, cuT( "slider" ), &Parser_Slider, { MakeParameter< ParameterType::eName >() } );
			AddParser( p_parsers, p_section, cuT( "combobox" ), &Parser_ComboBox, { MakeParameter< ParameterType::eName >() } );
			AddParser( p_parsers, p_section, cuT( "listbox" ), &Parser_ListBox, { MakeParameter< ParameterType::eName >() } );
			AddParser( p_parsers, p_section, cuT( "edit" ), &Parser_Edit, { MakeParameter< ParameterType::eName >() } );
			AddParser( p_parsers, p_section, cuT( "}" ), p_endFunction );
		}

		FileParser::AttributeParsersBySection CreateParsers( Castor3D::Engine * p_engine )
		{
			FileParser::AttributeParsersBySection l_return;

			AddParser( l_return, uint32_t( CSCNSection::eRoot ), cuT( "gui" ), &Parser_Gui );

			AddParser( l_return, uint32_t( GUISection::eGUI ), cuT( "default_font" ), &Parser_DefaultFont, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eGUI ), cuT( "button" ), &Parser_Button, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eGUI ), cuT( "static" ), &Parser_Static, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eGUI ), cuT( "slider" ), &Parser_Slider, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eGUI ), cuT( "combobox" ), &Parser_ComboBox, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eGUI ), cuT( "listbox" ), &Parser_ListBox, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eGUI ), cuT( "edit" ), &Parser_Edit, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eGUI ), cuT( "}" ), &Parser_GuiEnd );

			CreateDefaultParsers( l_return, uint32_t( GUISection::eButton ), &Parser_ButtonEnd );
			AddParser( l_return, uint32_t( GUISection::eButton ), cuT( "text_material" ), &Parser_ButtonTextMaterial, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eButton ), cuT( "highlighted_background_material" ), &Parser_ButtonHighlightedBackgroundMaterial, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eButton ), cuT( "highlighted_foreground_material" ), &Parser_ButtonHighlightedForegroundMaterial, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eButton ), cuT( "highlighted_text_material" ), &Parser_ButtonHighlightedTextMaterial, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eButton ), cuT( "pushed_background_material" ), &Parser_ButtonPushedBackgroundMaterial, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eButton ), cuT( "pushed_foreground_material" ), &Parser_ButtonPushedForegroundMaterial, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eButton ), cuT( "pushed_text_material" ), &Parser_ButtonPushedTextMaterial, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eButton ), cuT( "font" ), &Parser_ButtonFont, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eButton ), cuT( "caption" ), &Parser_ButtonCaption, { MakeParameter< ParameterType::eText >() } );

			CreateDefaultParsers( l_return, uint32_t( GUISection::eListBox ), &Parser_ListBoxEnd );
			AddParser( l_return, uint32_t( GUISection::eListBox ), cuT( "font" ), &Parser_ListBoxFont, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eListBox ), cuT( "item" ), &Parser_ListBoxItem, { MakeParameter< ParameterType::eText >() } );
			AddParser( l_return, uint32_t( GUISection::eListBox ), cuT( "selected_item_background_material" ), &Parser_ListBoxSelectedItemBackgroundMaterial, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eListBox ), cuT( "selected_item_foreground_material" ), &Parser_ListBoxSelectedItemForegroundMaterial, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eListBox ), cuT( "highlighted_item_background_material" ), &Parser_ListBoxHighlightedItemBackgroundMaterial, { MakeParameter< ParameterType::eName >() } );

			CreateDefaultParsers( l_return, uint32_t( GUISection::eComboBox ), &Parser_ComboBoxEnd );
			AddParser( l_return, uint32_t( GUISection::eComboBox ), cuT( "font" ), &Parser_ComboBoxFont, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eComboBox ), cuT( "item" ), &Parser_ComboBoxItem, { MakeParameter< ParameterType::eText >() } );
			AddParser( l_return, uint32_t( GUISection::eComboBox ), cuT( "selected_item_background_material" ), &Parser_ComboBoxSelectedItemBackgroundMaterial, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eComboBox ), cuT( "selected_item_foreground_material" ), &Parser_ComboBoxSelectedItemForegroundMaterial, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eComboBox ), cuT( "highlighted_item_background_material" ), &Parser_ComboBoxHighlightedItemBackgroundMaterial, { MakeParameter< ParameterType::eName >() } );

			CreateDefaultParsers( l_return, uint32_t( GUISection::eStatic ), &Parser_StaticEnd );
			AddParser( l_return, uint32_t( GUISection::eStatic ), cuT( "font" ), &Parser_StaticFont, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eStatic ), cuT( "caption" ), &Parser_StaticCaption, { MakeParameter< ParameterType::eText >() } );

			CreateDefaultParsers( l_return, uint32_t( GUISection::eEdit ), &Parser_EditEnd );
			AddParser( l_return, uint32_t( GUISection::eEdit ), cuT( "font" ), &Parser_EditFont, { MakeParameter< ParameterType::eName >() } );
			AddParser( l_return, uint32_t( GUISection::eEdit ), cuT( "multiline" ), &Parser_EditMultiLine, { MakeParameter< ParameterType::eBool >() } );
			AddParser( l_return, uint32_t( GUISection::eEdit ), cuT( "caption" ), &Parser_EditCaption, { MakeParameter< ParameterType::eText >() } );

			CreateDefaultParsers( l_return, uint32_t( GUISection::eSlider ), &Parser_SliderEnd );

			return l_return;
		}

		StrUIntMap CreateSections()
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
	C3D_CGui_API void GetRequiredVersion( Castor3D::Version * p_version )
	{
		*p_version = Castor3D::Version();
	}

	C3D_CGui_API void GetType( Castor3D::PluginType * p_type )
	{
		*p_type = Castor3D::PluginType::eGeneric;
	}

	C3D_CGui_API void GetName( char const ** p_name )
	{
		static String const Name = cuT( "Castor GUI" );
		*p_name = Name.c_str();
	}

	C3D_CGui_API void OnLoad( Castor3D::Engine * p_engine, Castor3D::Plugin * p_plugin )
	{
		p_engine->RegisterParsers( CastorGui::PLUGIN_NAME, std::move( CastorGui::CreateParsers( p_engine ) ) );
		p_engine->RegisterSections( CastorGui::PLUGIN_NAME, std::move( CastorGui::CreateSections() ) );
		p_engine->SetUserInputListener( std::make_shared< CastorGui::ControlsManager >( *p_engine ) );
	}

	C3D_CGui_API void OnUnload( Castor3D::Engine * p_engine )
	{
		p_engine->SetUserInputListener( nullptr );
		p_engine->UnregisterParsers( CastorGui::PLUGIN_NAME );
		p_engine->UnregisterSections( CastorGui::PLUGIN_NAME );
	}
}
