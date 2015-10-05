#include <Engine.hpp>
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

		void AddParser( FileParser::AttributeParsersBySection & p_parsers, uint32_t p_section, String const & p_name, ParserFunction p_function, uint32_t p_count, ... )
		{
			auto && l_sectionIt = p_parsers.find( p_section );

			if ( l_sectionIt != p_parsers.end() && l_sectionIt->second.find( p_name ) != l_sectionIt->second.end() )
			{
				ParseError( cuT( "Parser " ) + p_name + cuT( " for section " ) + string::to_string( p_section ) + cuT( " already exists." ) );
			}
			else
			{
				ParserParameterArray l_params;
				va_list l_valist;
				va_start( l_valist, p_count );

				for ( uint32_t i = 0; i < p_count; ++i )
				{
					ePARAMETER_TYPE l_eParamType =  ePARAMETER_TYPE( va_arg( l_valist, int ) );

					switch ( l_eParamType )
					{
					case ePARAMETER_TYPE_NAME:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_NAME > >( p_name ) );
						break;

					case ePARAMETER_TYPE_TEXT:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_TEXT > >( p_name ) );
						break;

					case ePARAMETER_TYPE_PATH:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_PATH > >( p_name ) );
						break;

					case ePARAMETER_TYPE_CHECKED_TEXT:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_CHECKED_TEXT > >( p_name, *reinterpret_cast< UIntStrMap * >( va_arg( l_valist, void * ) ) ) );
						break;

					case ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_BITWISE_ORED_CHECKED_TEXT > >( p_name, *reinterpret_cast< UIntStrMap * >( va_arg( l_valist, void * ) ) ) );
						break;

					case ePARAMETER_TYPE_BOOL:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_BOOL > >( p_name ) );
						break;

					case ePARAMETER_TYPE_INT8:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT8 > >( p_name ) );
						break;

					case ePARAMETER_TYPE_INT16:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT16 > >( p_name ) );
						break;

					case ePARAMETER_TYPE_INT32:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT32 > >( p_name ) );
						break;

					case ePARAMETER_TYPE_INT64:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_INT64 > >( p_name ) );
						break;

					case ePARAMETER_TYPE_UINT8:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT8 > >( p_name ) );
						break;

					case ePARAMETER_TYPE_UINT16:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT16 > >( p_name ) );
						break;

					case ePARAMETER_TYPE_UINT32:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT32 > >( p_name ) );
						break;

					case ePARAMETER_TYPE_UINT64:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_UINT64 > >( p_name ) );
						break;

					case ePARAMETER_TYPE_FLOAT:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_FLOAT > >( p_name ) );
						break;

					case ePARAMETER_TYPE_DOUBLE:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_DOUBLE > >( p_name ) );
						break;

					case ePARAMETER_TYPE_LONGDOUBLE:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_LONGDOUBLE > >( p_name ) );
						break;

					case ePARAMETER_TYPE_PIXELFORMAT:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_PIXELFORMAT > >( p_name ) );
						break;

					case ePARAMETER_TYPE_POINT2I:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT2I > >( p_name ) );
						break;

					case ePARAMETER_TYPE_POINT3I:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT3I > >( p_name ) );
						break;

					case ePARAMETER_TYPE_POINT4I:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT4I > >( p_name ) );
						break;

					case ePARAMETER_TYPE_POINT2F:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT2F > >( p_name ) );
						break;

					case ePARAMETER_TYPE_POINT3F:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT3F > >( p_name ) );
						break;

					case ePARAMETER_TYPE_POINT4F:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT4F > >( p_name ) );
						break;

					case ePARAMETER_TYPE_POINT2D:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT2D > >( p_name ) );
						break;

					case ePARAMETER_TYPE_POINT3D:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT3D > >( p_name ) );
						break;

					case ePARAMETER_TYPE_POINT4D:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POINT4D > >( p_name ) );
						break;

					case ePARAMETER_TYPE_SIZE:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_SIZE > >( p_name ) );
						break;

					case ePARAMETER_TYPE_POSITION:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_POSITION > >( p_name ) );
						break;

					case ePARAMETER_TYPE_RECTANGLE:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_RECTANGLE > >( p_name ) );
						break;

					case ePARAMETER_TYPE_COLOUR:
						l_params.push_back( std::make_shared< ParserParameter< ePARAMETER_TYPE_COLOUR > >( p_name ) );
						break;
					}
				}

				va_end( l_valist );
				p_parsers[p_section][p_name] = { p_function, l_params };
			}
		}

		void CreateDefaultParsers( FileParser::AttributeParsersBySection & p_parsers, CastorGui::eSECTION p_section, ParserFunction p_endFunction )
		{
			AddParser( p_parsers, p_section, cuT( "pixel_position" ), &Parser_ControlPixelPosition, 1, ePARAMETER_TYPE_POSITION );
			AddParser( p_parsers, p_section, cuT( "pixel_size" ), &Parser_ControlPixelSize, 1, ePARAMETER_TYPE_SIZE );
			AddParser( p_parsers, p_section, cuT( "pixel_border_size" ), &Parser_ControlPixelBorderSize, 1, ePARAMETER_TYPE_RECTANGLE );
			AddParser( p_parsers, p_section, cuT( "background_material" ), &Parser_ControlBackgroundMaterial, 1, ePARAMETER_TYPE_NAME );
			AddParser( p_parsers, p_section, cuT( "border_material" ), &Parser_ControlBorderMaterial, 1, ePARAMETER_TYPE_NAME );
			AddParser( p_parsers, p_section, cuT( "border_inner_uv" ), &Parser_ControlBorderInnerUv, 1, ePARAMETER_TYPE_POINT4D );
			AddParser( p_parsers, p_section, cuT( "border_outer_uv" ), &Parser_ControlBorderOuterUv, 1, ePARAMETER_TYPE_POINT4D );
			AddParser( p_parsers, p_section, cuT( "center_uv" ), &Parser_ControlCenterUv, 1, ePARAMETER_TYPE_POINT4D );
			AddParser( p_parsers, p_section, cuT( "button" ), &Parser_Button, 1, ePARAMETER_TYPE_NAME );
			AddParser( p_parsers, p_section, cuT( "static" ), &Parser_Static, 1, ePARAMETER_TYPE_NAME );
			AddParser( p_parsers, p_section, cuT( "slider" ), &Parser_Slider, 1, ePARAMETER_TYPE_NAME );
			AddParser( p_parsers, p_section, cuT( "combobox" ), &Parser_ComboBox, 1, ePARAMETER_TYPE_NAME );
			AddParser( p_parsers, p_section, cuT( "listbox" ), &Parser_ListBox, 1, ePARAMETER_TYPE_NAME );
			AddParser( p_parsers, p_section, cuT( "edit" ), &Parser_Edit, 1, ePARAMETER_TYPE_NAME );
			AddParser( p_parsers, p_section, cuT( "}" ), p_endFunction, 0 );
		}

		FileParser::AttributeParsersBySection CreateParsers( Castor3D::Engine * p_engine )
		{
			FileParser::AttributeParsersBySection l_return;

			AddParser( l_return, eSECTION_ROOT, cuT( "gui" ), &Parser_Gui, 0 );

			AddParser( l_return, eSECTION_GUI, cuT( "default_font" ), &Parser_DefaultFont, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_GUI, cuT( "button" ), &Parser_Button, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_GUI, cuT( "static" ), &Parser_Static, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_GUI, cuT( "slider" ), &Parser_Slider, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_GUI, cuT( "combobox" ), &Parser_ComboBox, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_GUI, cuT( "listbox" ), &Parser_ListBox, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_GUI, cuT( "edit" ), &Parser_Edit, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_GUI, cuT( "}" ), &Parser_GuiEnd, 0 );

			CreateDefaultParsers( l_return, eSECTION_BUTTON, &Parser_ButtonEnd );
			AddParser( l_return, eSECTION_BUTTON, cuT( "text_material" ), &Parser_ButtonTextMaterial, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_BUTTON, cuT( "highlighted_background_material" ), &Parser_ButtonHighlightedBackgroundMaterial, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_BUTTON, cuT( "highlighted_foreground_material" ), &Parser_ButtonHighlightedForegroundMaterial, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_BUTTON, cuT( "highlighted_text_material" ), &Parser_ButtonHighlightedTextMaterial, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_BUTTON, cuT( "pushed_background_material" ), &Parser_ButtonPushedBackgroundMaterial, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_BUTTON, cuT( "pushed_foreground_material" ), &Parser_ButtonPushedForegroundMaterial, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_BUTTON, cuT( "pushed_text_material" ), &Parser_ButtonPushedTextMaterial, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_BUTTON, cuT( "font" ), &Parser_ButtonFont, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_BUTTON, cuT( "caption" ), &Parser_ButtonCaption, 1, ePARAMETER_TYPE_TEXT );

			CreateDefaultParsers( l_return, eSECTION_LISTBOX, &Parser_ListBoxEnd );
			AddParser( l_return, eSECTION_LISTBOX, cuT( "font" ), &Parser_ListBoxFont, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_LISTBOX, cuT( "item" ), &Parser_ListBoxItem, 1, ePARAMETER_TYPE_TEXT );
			AddParser( l_return, eSECTION_LISTBOX, cuT( "selected_item_background_material" ), &Parser_ListBoxSelectedItemBackgroundMaterial, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_LISTBOX, cuT( "selected_item_foreground_material" ), &Parser_ListBoxSelectedItemForegroundMaterial, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_LISTBOX, cuT( "highlighted_item_background_material" ), &Parser_ListBoxHighlightedItemBackgroundMaterial, 1, ePARAMETER_TYPE_NAME );

			CreateDefaultParsers( l_return, eSECTION_COMBOBOX, &Parser_ComboBoxEnd );
			AddParser( l_return, eSECTION_COMBOBOX, cuT( "font" ), &Parser_ComboBoxFont, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_COMBOBOX, cuT( "item" ), &Parser_ComboBoxItem, 1, ePARAMETER_TYPE_TEXT );
			AddParser( l_return, eSECTION_COMBOBOX, cuT( "selected_item_background_material" ), &Parser_ComboBoxSelectedItemBackgroundMaterial, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_COMBOBOX, cuT( "selected_item_foreground_material" ), &Parser_ComboBoxSelectedItemForegroundMaterial, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_COMBOBOX, cuT( "highlighted_item_background_material" ), &Parser_ComboBoxHighlightedItemBackgroundMaterial, 1, ePARAMETER_TYPE_NAME );

			CreateDefaultParsers( l_return, eSECTION_STATIC, &Parser_StaticEnd );
			AddParser( l_return, eSECTION_STATIC, cuT( "font" ), &Parser_StaticFont, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_STATIC, cuT( "caption" ), &Parser_StaticCaption, 1, ePARAMETER_TYPE_TEXT );

			CreateDefaultParsers( l_return, eSECTION_EDIT, &Parser_EditEnd );
			AddParser( l_return, eSECTION_EDIT, cuT( "font" ), &Parser_EditFont, 1, ePARAMETER_TYPE_NAME );
			AddParser( l_return, eSECTION_EDIT, cuT( "multiline" ), &Parser_EditMultiLine, 1, ePARAMETER_TYPE_BOOL );
			AddParser( l_return, eSECTION_EDIT, cuT( "caption" ), &Parser_EditCaption, 1, ePARAMETER_TYPE_TEXT );

			CreateDefaultParsers( l_return, eSECTION_SLIDER, &Parser_SliderEnd );

			return l_return;
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
	p_engine->AddFrameListener( CastorGui::PLUGIN_NAME, std::make_shared< CastorGui::ControlsManager >( p_engine ) );
}

C3D_CGui_API void OnUnload( Castor3D::Engine * p_engine )
{
	p_engine->DestroyFrameListener( CastorGui::PLUGIN_NAME );
	p_engine->UnregisterParsers( CastorGui::PLUGIN_NAME );
}
