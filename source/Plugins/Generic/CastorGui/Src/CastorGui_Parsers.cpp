#include "CastorGui_Parsers.hpp"

#include "ControlsManager.hpp"
#include "CtrlButton.hpp"
#include "CtrlComboBox.hpp"
#include "CtrlEdit.hpp"
#include "CtrlListBox.hpp"
#include "CtrlSlider.hpp"
#include "CtrlStatic.hpp"

#include <Engine.hpp>
#include <Event/Frame/FrameListener.hpp>
#include <Material/Material.hpp>

#include <Overlay/BorderPanelOverlay.hpp>
#include <Scene/SceneFileParser.hpp>

#include <stack>

using namespace CastorGui;
using namespace Castor3D;
using namespace Castor;

namespace CastorGui
{
	struct ParserContext
	{
		std::stack< ControlSPtr > m_parents;
		Engine * m_engine;
		ButtonCtrlSPtr m_button;
		ComboBoxCtrlSPtr m_combo;
		EditCtrlSPtr m_edit;
		ListBoxCtrlSPtr m_listbox;
		SliderCtrlSPtr m_slider;
		StaticCtrlSPtr m_static;
		uint32_t m_flags = 0;
		uint32_t m_ctrlId = 0;

		ControlRPtr GetTop()const
		{
			ControlRPtr result = NULL;

			if ( !m_parents.empty() )
			{
				result = m_parents.top().get();
			}

			return result;
		}

		void Pop()
		{
			m_button.reset();
			m_edit.reset();
			m_listbox.reset();
			m_slider.reset();
			m_static.reset();
			m_combo.reset();

			if ( !m_parents.empty() )
			{
				ControlSPtr top;
				top = m_parents.top();

				switch ( top->GetType() )
				{
				case ControlType::eStatic:
					m_static = std::static_pointer_cast< StaticCtrl >( top );
					break;

				case ControlType::eEdit:
					m_edit = std::static_pointer_cast< EditCtrl >( top );
					break;

				case ControlType::eSlider:
					m_slider = std::static_pointer_cast< SliderCtrl >( top );
					break;

				case ControlType::eComboBox:
					m_combo = std::static_pointer_cast< ComboBoxCtrl >( top );
					break;

				case ControlType::eListBox:
					m_listbox = std::static_pointer_cast< ListBoxCtrl >( top );
					break;

				case ControlType::eButton:
					m_button = std::static_pointer_cast< ButtonCtrl >( top );
					break;
				}
			}
		}
	};

	ControlsManager & GetControlsManager( FileParserContextSPtr p_context )
	{
		return static_cast< ControlsManager & >( *std::static_pointer_cast< SceneFileContext >( p_context )->m_pParser->GetEngine()->GetUserInputListener() );
	}

	ParserContext & GetParserContext( FileParserContextSPtr p_context )
	{
		return *static_cast< ParserContext * >( p_context->GetUserContext( PLUGIN_NAME ) );
	}

	template< typename T >
	std::shared_ptr< T > CreateControl( ParserContext & p_context, String const & p_name, std::shared_ptr< T > & p_control )
	{
		p_control = std::make_shared< T >( p_name, *p_context.m_engine, p_context.GetTop(), p_context.m_ctrlId++ );
		p_control->AddStyle( p_context.m_flags );
		p_context.m_parents.push( p_control );
		p_context.m_flags = 0;
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_Gui )
	{
		ParserContext * context = new ParserContext;
		context->m_engine = std::static_pointer_cast< SceneFileContext >( p_context )->m_pParser->GetEngine();
		p_context->RegisterUserContext( PLUGIN_NAME, context );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eGUI )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_DefaultFont )
	{
		ControlsManager & ctrlsManager = GetControlsManager( p_context );
		ParserContext & context = GetParserContext( p_context );
		auto & cache = context.m_engine->GetFontCache();
		String name;
		p_params[0]->Get( name );
		FontSPtr font = cache.Find( name );

		if ( font )
		{
			ctrlsManager.SetDefaultFont( font );
		}
		else
		{
			PARSING_ERROR( cuT( "Unknown font: " ) + name );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_GuiEnd )
	{
		delete reinterpret_cast< ParserContext * >( p_context->UnregisterUserContext( PLUGIN_NAME ) );
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_Button )
	{
		ControlsManager & ctrlsManager = GetControlsManager( p_context );
		ParserContext & context = GetParserContext( p_context );
		String name;
		p_params[0]->Get( name );
		CreateControl( context, name, context.m_button );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eButton )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonFont )
	{
		ParserContext & context = GetParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->Get( name );
			button->SetFont( name );
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonCaption )
	{
		ParserContext & context = GetParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String text;
			p_params[0]->Get( text );
			button->SetCaption( text );
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonTextMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				button->SetTextMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonHighlightedBackgroundMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				button->SetHighlightedBackgroundMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonHighlightedForegroundMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				button->SetHighlightedForegroundMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonHighlightedTextMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				button->SetHighlightedTextMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonPushedBackgroundMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				button->SetPushedBackgroundMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonPushedForegroundMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				button->SetPushedForegroundMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonPushedTextMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				button->SetPushedTextMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonHAlign )
	{
		ParserContext & context = GetParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			uint32_t value;
			p_params[0]->Get( value );
			button->SetHAlign( HAlign( value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonVAlign )
	{
		ParserContext & context = GetParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			uint32_t value;
			p_params[0]->Get( value );
			button->SetVAlign( VAlign( value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonEnd )
	{
		ParserContext & context = GetParserContext( p_context );
		FinishControl( GetControlsManager( p_context ), context, context.m_button );
		context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ComboBox )
	{
		ControlsManager & ctrlsManager = GetControlsManager( p_context );
		ParserContext & context = GetParserContext( p_context );
		String name;
		p_params[0]->Get( name );
		CreateControl( context, name, context.m_combo );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eComboBox )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ComboBoxFont )
	{
		ParserContext & context = GetParserContext( p_context );
		ComboBoxCtrlSPtr combo = context.m_combo;

		if ( combo )
		{
			String name;
			p_params[0]->Get( name );
			combo->SetFont( name );
		}
		else
		{
			PARSING_ERROR( cuT( "No combo box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ComboBoxItem )
	{
		ParserContext & context = GetParserContext( p_context );
		ComboBoxCtrlSPtr combo = context.m_combo;

		if ( combo )
		{
			String text;
			p_params[0]->Get( text );
			combo->AppendItem( text );
		}
		else
		{
			PARSING_ERROR( cuT( "No combo box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ComboBoxSelectedItemBackgroundMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ComboBoxCtrlSPtr combo = context.m_combo;

		if ( combo )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				combo->SetSelectedItemBackgroundMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No combo box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ComboBoxSelectedItemForegroundMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ComboBoxCtrlSPtr combo = context.m_combo;

		if ( combo )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				combo->SetSelectedItemForegroundMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No combo box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ComboBoxHighlightedItemBackgroundMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ComboBoxCtrlSPtr combo = context.m_combo;

		if ( combo )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				combo->SetHighlightedItemBackgroundMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No combo box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ComboBoxEnd )
	{
		ParserContext & context = GetParserContext( p_context );
		FinishControl( GetControlsManager( p_context ), context, context.m_combo );
		context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_Edit )
	{
		ControlsManager & ctrlsManager = GetControlsManager( p_context );
		ParserContext & context = GetParserContext( p_context );
		String name;
		p_params[0]->Get( name );
		CreateControl( context, name, context.m_edit );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eEdit )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_EditFont )
	{
		ParserContext & context = GetParserContext( p_context );
		EditCtrlSPtr edit = context.m_edit;

		if ( edit )
		{
			String name;
			p_params[0]->Get( name );
			edit->SetFont( name );
		}
		else
		{
			PARSING_ERROR( cuT( "No edit initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_EditCaption )
	{
		ParserContext & context = GetParserContext( p_context );
		EditCtrlSPtr edit = context.m_edit;

		if ( edit )
		{
			String text;
			p_params[0]->Get( text );
			edit->SetCaption( text );
		}
		else
		{
			PARSING_ERROR( cuT( "No edit initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_EditMultiLine )
	{
		ParserContext & context = GetParserContext( p_context );
		EditCtrlSPtr edit = context.m_edit;

		if ( edit )
		{
			bool value;
			p_params[0]->Get( value );
			context.m_flags |= value ? ( uint32_t( EditStyle::eMultiline )
											 | uint32_t( EditStyle::eProcessEnter )
											 | uint32_t( EditStyle::eProcessTab ) ) : 0;
		}
		else
		{
			PARSING_ERROR( cuT( "No edit initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_EditEnd )
	{
		ParserContext & context = GetParserContext( p_context );
		FinishControl( GetControlsManager( p_context ), context, context.m_edit );
		context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ListBox )
	{
		ControlsManager & ctrlsManager = GetControlsManager( p_context );
		ParserContext & context = GetParserContext( p_context );
		String name;
		p_params[0]->Get( name );
		CreateControl( context, name, context.m_listbox );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eListBox )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ListBoxFont )
	{
		ParserContext & context = GetParserContext( p_context );
		ListBoxCtrlSPtr listbox = context.m_listbox;

		if ( listbox )
		{
			String name;
			p_params[0]->Get( name );
			listbox->SetFont( name );
		}
		else
		{
			PARSING_ERROR( cuT( "No list box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ListBoxItem )
	{
		ParserContext & context = GetParserContext( p_context );
		ListBoxCtrlSPtr listbox = context.m_listbox;

		if ( listbox )
		{
			String text;
			p_params[0]->Get( text );
			listbox->AppendItem( text );
		}
		else
		{
			PARSING_ERROR( cuT( "No list box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ListBoxSelectedItemBackgroundMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ListBoxCtrlSPtr listbox = context.m_listbox;

		if ( listbox )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				listbox->SetSelectedItemBackgroundMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No list box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ListBoxHighlightedItemBackgroundMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ListBoxCtrlSPtr listbox = context.m_listbox;

		if ( listbox )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				listbox->SetHighlightedItemBackgroundMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No list box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ListBoxSelectedItemForegroundMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ListBoxCtrlSPtr listbox = context.m_listbox;

		if ( listbox )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				listbox->SetSelectedItemForegroundMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No list box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ListBoxEnd )
	{
		ParserContext & context = GetParserContext( p_context );
		FinishControl( GetControlsManager( p_context ), context, context.m_listbox );
		context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_Slider )
	{
		ControlsManager & ctrlsManager = GetControlsManager( p_context );
		ParserContext & context = GetParserContext( p_context );
		String name;
		p_params[0]->Get( name );
		CreateControl( context, name, context.m_slider );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eSlider )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SliderEnd )
	{
		ParserContext & context = GetParserContext( p_context );
		FinishControl( GetControlsManager( p_context ), context, context.m_slider );
		context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_Static )
	{
		ParserContext & context = GetParserContext( p_context );
		String name;
		p_params[0]->Get( name );
		CreateControl( context, name, context.m_static );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eStatic )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_StaticFont )
	{
		ParserContext & context = GetParserContext( p_context );
		StaticCtrlSPtr ctrl = context.m_static;

		if ( ctrl )
		{
			String name;
			p_params[0]->Get( name );
			ctrl->SetFont( name );
		}
		else
		{
			PARSING_ERROR( cuT( "No static initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_StaticCaption )
	{
		ParserContext & context = GetParserContext( p_context );
		StaticCtrlSPtr ctrl = context.m_static;

		if ( ctrl )
		{
			String text;
			p_params[0]->Get( text );
			ctrl->SetCaption( text );
		}
		else
		{
			PARSING_ERROR( cuT( "No static initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_StaticHAlign )
	{
		ParserContext & context = GetParserContext( p_context );
		StaticCtrlSPtr ctrl = context.m_static;

		if ( ctrl )
		{
			uint32_t value;
			p_params[0]->Get( value );
			ctrl->SetHAlign( HAlign( value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No static initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_StaticVAlign )
	{
		ParserContext & context = GetParserContext( p_context );
		StaticCtrlSPtr ctrl = context.m_static;

		if ( ctrl )
		{
			uint32_t value;
			p_params[0]->Get( value );
			ctrl->SetVAlign( VAlign( value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No static initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_StaticEnd )
	{
		ParserContext & context = GetParserContext( p_context );
		FinishControl( GetControlsManager( p_context ), context, context.m_static );
		context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlPixelPosition )
	{
		ParserContext & context = GetParserContext( p_context );
		ControlRPtr control = context.GetTop();

		if ( control )
		{
			Position position;
			p_params[0]->Get( position );
			control->SetPosition( position );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlPixelSize )
	{
		ParserContext & context = GetParserContext( p_context );
		ControlRPtr control = context.GetTop();

		if ( control )
		{
			Size size;
			p_params[0]->Get( size );
			control->SetSize( size );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlPixelBorderSize )
	{
		ParserContext & context = GetParserContext( p_context );
		ControlRPtr control = context.GetTop();

		if ( control )
		{
			Rectangle size;
			p_params[0]->Get( size );
			control->SetBackgroundBorders( size );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlBackgroundMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ControlRPtr control = context.GetTop();

		if ( control )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				control->SetBackgroundMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlBorderMaterial )
	{
		ParserContext & context = GetParserContext( p_context );
		ControlRPtr control = context.GetTop();

		if ( control )
		{
			String name;
			p_params[0]->Get( name );
			auto material = context.m_engine->GetMaterialCache().Find( name );

			if ( material )
			{
				control->SetForegroundMaterial( material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlBorderInnerUv )
	{
		ParserContext & context = GetParserContext( p_context );
		ControlRPtr control = context.GetTop();

		if ( control )
		{
			Point4d value;
			p_params[0]->Get( value );
			control->GetBackground()->SetBorderInnerUV( value );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlBorderOuterUv )
	{
		ParserContext & context = GetParserContext( p_context );
		ControlRPtr control = context.GetTop();

		if ( control )
		{
			Point4d value;
			p_params[0]->Get( value );
			control->GetBackground()->SetBorderOuterUV( value );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlCenterUv )
	{
		ParserContext & context = GetParserContext( p_context );
		ControlRPtr control = context.GetTop();

		if ( control )
		{
			Point4d value;
			p_params[0]->Get( value );
			control->GetBackground()->SetUV( value );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlVisible )
	{
		ParserContext & context = GetParserContext( p_context );
		ControlRPtr control = context.GetTop();

		if ( control )
		{
			bool value;
			p_params[0]->Get( value );
			control->SetVisible( value );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()
}
