#include "CastorGui_Parsers.hpp"

#include "ControlsManager.hpp"
#include "CtrlButton.hpp"
#include "CtrlComboBox.hpp"
#include "CtrlEdit.hpp"
#include "CtrlListBox.hpp"
#include "CtrlSlider.hpp"
#include "CtrlStatic.hpp"

#include <SceneFileParser.hpp>

using namespace CastorGui;
using namespace Castor3D;
using namespace Castor;

namespace CastorGui
{
	struct ParserContext
	{
		std::stack< ControlSPtr > m_parents;
		ButtonCtrlSPtr m_button;
		ComboBoxCtrlSPtr m_combo;
		EditCtrlSPtr m_edit;
		ListBoxCtrlSPtr m_listbox;
		SliderCtrlSPtr m_slider;
		StaticCtrlSPtr m_static;
		uint32_t m_ctrlId = 0;

		ControlSPtr GetTop()const
		{
			ControlSPtr l_return;

			if ( !m_parents.empty() )
			{
				l_return = m_parents.top();
			}

			return l_return;
		}
	};

	ControlsManager & GetControlsManager( FileParserContextSPtr p_context )
	{
		return static_cast< ControlsManager & >( std::static_pointer_cast< SceneFileContext >( p_context )->m_pParser->GetEngine()->GetFrameListener( PLUGIN_NAME ) );
	}

	ParserContext & GetParserContext( FileParserContextSPtr p_context )
	{
		return *static_cast< ParserContext * >( p_context->GetUserContext( PLUGIN_NAME ) );
	}

	template< typename T >
	std::shared_ptr< T > CreateControl( ParserContext & p_context, std::shared_ptr< T > & p_control )
	{
		p_control = std::make_shared< T >( p_context.GetTop(), p_context.m_ctrlId++ );
		p_context.m_parents.push( p_control );
		return p_control;
	}

	template< typename T > void FinishControl( ControlsManager & p_manager, ParserContext & p_context, std::shared_ptr< T > p_control )
	{
		if ( p_control )
		{
			p_manager.Create( p_control );
			p_context.m_parents.pop();
		}
	}
}

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_Gui )
{
	p_context->RegisterUserContext( PLUGIN_NAME, new ParserContext );
}
END_ATTRIBUTE_PUSH( CastorGui::eSECTION_GUI )

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_DefaultFont )
{
	ControlsManager & l_ctrlsManager = GetControlsManager( p_context );
	SceneFileContextSPtr l_context = std::static_pointer_cast< SceneFileContext >( p_context );
	FontManager & l_fontManager = l_context->m_pParser->GetEngine()->GetFontManager();
	String l_name;
	p_params[0]->Get( l_name );
	FontSPtr l_font = l_fontManager.get_font( l_name );

	if ( l_font )
	{
		l_ctrlsManager.SetDefaultFont( l_font );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <gui::default_font> : Unknown font: " ) + l_name );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_GuiEnd )
{
	delete reinterpret_cast< ParserContext * >( p_context->UnregisterUserContext( PLUGIN_NAME ) );
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_Button )
{
	ParserContext & l_context = GetParserContext( p_context );
	CreateControl( l_context, l_context.m_button );
}
END_ATTRIBUTE_PUSH( CastorGui::eSECTION_BUTTON )

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_ButtonCaption )
{
	ControlsManager & l_ctrlsManager = GetControlsManager( p_context );
	ParserContext & l_context = GetParserContext( p_context );
	ButtonCtrlSPtr l_button = l_context.m_button;

	if ( l_button )
	{
		String l_text;
		p_params[0]->Get( l_text );
		l_button->SetCaption( l_text );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <gui::button::caption> : no button initialised." ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_ButtonEnd )
{
	ParserContext & l_context = GetParserContext( p_context );
	FinishControl( GetControlsManager( p_context ), l_context, l_context.m_button );
	l_context.m_button.reset();
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_Static )
{
	ParserContext & l_context = GetParserContext( p_context );
	CreateControl( l_context, l_context.m_static );
}
END_ATTRIBUTE_PUSH( CastorGui::eSECTION_STATIC )

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_StaticEnd )
{
	ParserContext & l_context = GetParserContext( p_context );
	FinishControl( GetControlsManager( p_context ), l_context, l_context.m_static );
	l_context.m_static.reset();
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_Slider )
{
	ParserContext & l_context = GetParserContext( p_context );
	CreateControl( l_context, l_context.m_slider );
}
END_ATTRIBUTE_PUSH( CastorGui::eSECTION_SLIDER )

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_SliderEnd )
{
	ParserContext & l_context = GetParserContext( p_context );
	FinishControl( GetControlsManager( p_context ), l_context, l_context.m_slider );
	l_context.m_slider.reset();
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_ComboBox )
{
	ParserContext & l_context = GetParserContext( p_context );
	CreateControl( l_context, l_context.m_combo );
}
END_ATTRIBUTE_PUSH( CastorGui::eSECTION_COMBOBOX )

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_ComboBoxItem )
{
	ParserContext & l_context = GetParserContext( p_context );
	ComboBoxCtrlSPtr l_combo = l_context.m_combo;

	if ( l_combo )
	{
		String l_text;
		p_params[0]->Get( l_text );
		l_combo->AppendItem( l_text );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <gui::combobox::item> : no combo box initialised." ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_ComboBoxEnd )
{
	ParserContext & l_context = GetParserContext( p_context );
	FinishControl( GetControlsManager( p_context ), l_context, l_context.m_combo );
	l_context.m_combo.reset();
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_ListBox )
{
	ParserContext & l_context = GetParserContext( p_context );
	CreateControl( l_context, l_context.m_listbox );
}
END_ATTRIBUTE_PUSH( CastorGui::eSECTION_LISTBOX )

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_ListBoxItem )
{
	ParserContext & l_context = GetParserContext( p_context );
	ListBoxCtrlSPtr l_listbox = l_context.m_listbox;

	if ( l_listbox )
	{
		String l_text;
		p_params[0]->Get( l_text );
		l_listbox->AppendItem( l_text );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <gui::listbox::item> : no list box initialised." ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_ListBoxEnd )
{
	ParserContext & l_context = GetParserContext( p_context );
	FinishControl( GetControlsManager( p_context ), l_context, l_context.m_listbox );
	l_context.m_listbox.reset();
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_Edit )
{
	ParserContext & l_context = GetParserContext( p_context );
	CreateControl( l_context, l_context.m_edit );
}
END_ATTRIBUTE_PUSH( CastorGui::eSECTION_EDIT )

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_EditEnd )
{
	ParserContext & l_context = GetParserContext( p_context );
	FinishControl( GetControlsManager( p_context ), l_context, l_context.m_edit );
	l_context.m_edit.reset();
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_ControlPixelPosition )
{
	ParserContext & l_context = GetParserContext( p_context );
	ControlSPtr l_control = l_context.GetTop();

	if ( l_control )
	{
		Position l_position;
		p_params[0]->Get( l_position );
		l_control->SetPosition( l_position );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <gui::control::position> : no control initialised." ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_ControlPixelSize )
{
	ParserContext & l_context = GetParserContext( p_context );
	ControlSPtr l_control = l_context.GetTop();

	if ( l_control )
	{
		Size l_size;
		p_params[0]->Get( l_size );
		l_control->SetSize( l_size );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <gui::control::size> : no control initialised." ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_ControlBackground )
{
	ControlsManager & l_ctrlsManager = GetControlsManager( p_context );
	ParserContext & l_context = GetParserContext( p_context );
	ControlSPtr l_control = l_context.GetTop();

	if ( l_control )
	{
		String l_name;
		p_params[0]->Get( l_name );
		MaterialManager & l_manager = l_ctrlsManager.GetEngine()->GetMaterialManager();
		l_control->SetBackgroundMaterial( l_manager.find( l_name ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <gui::control::background> : no control initialised." ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( CastorGui, Parser_ControlForeground )
{
	ControlsManager & l_ctrlsManager = GetControlsManager( p_context );
	ParserContext & l_context = GetParserContext( p_context );
	ControlSPtr l_control = l_context.GetTop();

	if ( l_control )
	{
		String l_name;
		p_params[0]->Get( l_name );
		MaterialManager & l_manager = l_ctrlsManager.GetEngine()->GetMaterialManager();
		l_control->SetForegroundMaterial( l_manager.find( l_name ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <gui::control::background> : no control initialised." ) );
	}
}
END_ATTRIBUTE()
