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
			ControlRPtr l_return = NULL;

			if ( !m_parents.empty() )
			{
				l_return = m_parents.top().get();
			}

			return l_return;
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
				ControlSPtr l_top;
				l_top = m_parents.top();

				switch ( l_top->GetType() )
				{
				case ControlType::eStatic:
					m_static = std::static_pointer_cast< StaticCtrl >( l_top );
					break;

				case ControlType::eEdit:
					m_edit = std::static_pointer_cast< EditCtrl >( l_top );
					break;

				case ControlType::eSlider:
					m_slider = std::static_pointer_cast< SliderCtrl >( l_top );
					break;

				case ControlType::eComboBox:
					m_combo = std::static_pointer_cast< ComboBoxCtrl >( l_top );
					break;

				case ControlType::eListBox:
					m_listbox = std::static_pointer_cast< ListBoxCtrl >( l_top );
					break;

				case ControlType::eButton:
					m_button = std::static_pointer_cast< ButtonCtrl >( l_top );
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
	std::shared_ptr< T > CreateControl( ParserContext & p_context, std::shared_ptr< T > & p_control )
	{
		p_control = std::make_shared< T >( p_context.m_engine, p_context.GetTop(), p_context.m_ctrlId++ );
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
		ParserContext * l_context = new ParserContext;
		l_context->m_engine = std::static_pointer_cast< SceneFileContext >( p_context )->m_pParser->GetEngine();
		p_context->RegisterUserContext( PLUGIN_NAME, l_context );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eGUI )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_DefaultFont )
	{
		ControlsManager & l_ctrlsManager = GetControlsManager( p_context );
		ParserContext & l_context = GetParserContext( p_context );
		auto & l_cache = l_context.m_engine->GetFontCache();
		String l_name;
		p_params[0]->Get( l_name );
		FontSPtr l_font = l_cache.Find( l_name );

		if ( l_font )
		{
			l_ctrlsManager.SetDefaultFont( l_font );
		}
		else
		{
			PARSING_ERROR( cuT( "Unknown font: " ) + l_name );
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
		ParserContext & l_context = GetParserContext( p_context );
		CreateControl( l_context, l_context.m_button );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eButton )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonFont )
	{
		ParserContext & l_context = GetParserContext( p_context );
		ButtonCtrlSPtr l_button = l_context.m_button;

		if ( l_button )
		{
			String l_name;
			p_params[0]->Get( l_name );
			l_button->SetFont( l_name );
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonCaption )
	{
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
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonTextMaterial )
	{
		ParserContext & l_context = GetParserContext( p_context );
		ButtonCtrlSPtr l_button = l_context.m_button;

		if ( l_button )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_button->SetTextMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		ButtonCtrlSPtr l_button = l_context.m_button;

		if ( l_button )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_button->SetHighlightedBackgroundMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		ButtonCtrlSPtr l_button = l_context.m_button;

		if ( l_button )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_button->SetHighlightedForegroundMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		ButtonCtrlSPtr l_button = l_context.m_button;

		if ( l_button )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_button->SetHighlightedTextMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		ButtonCtrlSPtr l_button = l_context.m_button;

		if ( l_button )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_button->SetPushedBackgroundMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		ButtonCtrlSPtr l_button = l_context.m_button;

		if ( l_button )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_button->SetPushedForegroundMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		ButtonCtrlSPtr l_button = l_context.m_button;

		if ( l_button )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_button->SetPushedTextMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ButtonEnd )
	{
		ParserContext & l_context = GetParserContext( p_context );
		FinishControl( GetControlsManager( p_context ), l_context, l_context.m_button );
		l_context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ComboBox )
	{
		ParserContext & l_context = GetParserContext( p_context );
		CreateControl( l_context, l_context.m_combo );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eComboBox )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ComboBoxFont )
	{
		ParserContext & l_context = GetParserContext( p_context );
		ComboBoxCtrlSPtr l_combo = l_context.m_combo;

		if ( l_combo )
		{
			String l_name;
			p_params[0]->Get( l_name );
			l_combo->SetFont( l_name );
		}
		else
		{
			PARSING_ERROR( cuT( "No combo box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ComboBoxItem )
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
			PARSING_ERROR( cuT( "No combo box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ComboBoxSelectedItemBackgroundMaterial )
	{
		ParserContext & l_context = GetParserContext( p_context );
		ComboBoxCtrlSPtr l_combo = l_context.m_combo;

		if ( l_combo )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_combo->SetSelectedItemBackgroundMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		ComboBoxCtrlSPtr l_combo = l_context.m_combo;

		if ( l_combo )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_combo->SetSelectedItemForegroundMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		ComboBoxCtrlSPtr l_combo = l_context.m_combo;

		if ( l_combo )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_combo->SetHighlightedItemBackgroundMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		FinishControl( GetControlsManager( p_context ), l_context, l_context.m_combo );
		l_context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_Edit )
	{
		ParserContext & l_context = GetParserContext( p_context );
		CreateControl( l_context, l_context.m_edit );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eEdit )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_EditFont )
	{
		ParserContext & l_context = GetParserContext( p_context );
		EditCtrlSPtr l_edit = l_context.m_edit;

		if ( l_edit )
		{
			String l_name;
			p_params[0]->Get( l_name );
			l_edit->SetFont( l_name );
		}
		else
		{
			PARSING_ERROR( cuT( "No edit initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_EditCaption )
	{
		ParserContext & l_context = GetParserContext( p_context );
		EditCtrlSPtr l_edit = l_context.m_edit;

		if ( l_edit )
		{
			String l_text;
			p_params[0]->Get( l_text );
			l_edit->SetCaption( l_text );
		}
		else
		{
			PARSING_ERROR( cuT( "No edit initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_EditMultiLine )
	{
		ParserContext & l_context = GetParserContext( p_context );
		EditCtrlSPtr l_edit = l_context.m_edit;

		if ( l_edit )
		{
			bool l_value;
			p_params[0]->Get( l_value );
			l_context.m_flags |= l_value ? ( uint32_t( EditStyle::eMultiline )
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
		ParserContext & l_context = GetParserContext( p_context );
		FinishControl( GetControlsManager( p_context ), l_context, l_context.m_edit );
		l_context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ListBox )
	{
		ParserContext & l_context = GetParserContext( p_context );
		CreateControl( l_context, l_context.m_listbox );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eListBox )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ListBoxFont )
	{
		ParserContext & l_context = GetParserContext( p_context );
		ListBoxCtrlSPtr l_listbox = l_context.m_listbox;

		if ( l_listbox )
		{
			String l_name;
			p_params[0]->Get( l_name );
			l_listbox->SetFont( l_name );
		}
		else
		{
			PARSING_ERROR( cuT( "No list box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ListBoxItem )
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
			PARSING_ERROR( cuT( "No list box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ListBoxSelectedItemBackgroundMaterial )
	{
		ParserContext & l_context = GetParserContext( p_context );
		ListBoxCtrlSPtr l_listbox = l_context.m_listbox;

		if ( l_listbox )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_listbox->SetSelectedItemBackgroundMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		ListBoxCtrlSPtr l_listbox = l_context.m_listbox;

		if ( l_listbox )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_listbox->SetHighlightedItemBackgroundMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		ListBoxCtrlSPtr l_listbox = l_context.m_listbox;

		if ( l_listbox )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_listbox->SetSelectedItemForegroundMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		FinishControl( GetControlsManager( p_context ), l_context, l_context.m_listbox );
		l_context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_Slider )
	{
		ParserContext & l_context = GetParserContext( p_context );
		CreateControl( l_context, l_context.m_slider );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eSlider )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SliderEnd )
	{
		ParserContext & l_context = GetParserContext( p_context );
		FinishControl( GetControlsManager( p_context ), l_context, l_context.m_slider );
		l_context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_Static )
	{
		ParserContext & l_context = GetParserContext( p_context );
		CreateControl( l_context, l_context.m_static );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eStatic )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_StaticFont )
	{
		ParserContext & l_context = GetParserContext( p_context );
		StaticCtrlSPtr l_static = l_context.m_static;

		if ( l_static )
		{
			String l_name;
			p_params[0]->Get( l_name );
			l_static->SetFont( l_name );
		}
		else
		{
			PARSING_ERROR( cuT( "No static initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_StaticCaption )
	{
		ParserContext & l_context = GetParserContext( p_context );
		StaticCtrlSPtr l_static = l_context.m_static;

		if ( l_static )
		{
			String l_text;
			p_params[0]->Get( l_text );
			l_static->SetCaption( l_text );
		}
		else
		{
			PARSING_ERROR( cuT( "No static initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_StaticEnd )
	{
		ParserContext & l_context = GetParserContext( p_context );
		FinishControl( GetControlsManager( p_context ), l_context, l_context.m_static );
		l_context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlPixelPosition )
	{
		ParserContext & l_context = GetParserContext( p_context );
		ControlRPtr l_control = l_context.GetTop();

		if ( l_control )
		{
			Position l_position;
			p_params[0]->Get( l_position );
			l_control->SetPosition( l_position );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlPixelSize )
	{
		ParserContext & l_context = GetParserContext( p_context );
		ControlRPtr l_control = l_context.GetTop();

		if ( l_control )
		{
			Size l_size;
			p_params[0]->Get( l_size );
			l_control->SetSize( l_size );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlPixelBorderSize )
	{
		ParserContext & l_context = GetParserContext( p_context );
		ControlRPtr l_control = l_context.GetTop();

		if ( l_control )
		{
			Rectangle l_size;
			p_params[0]->Get( l_size );
			l_control->SetBackgroundBorders( l_size );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlBackgroundMaterial )
	{
		ParserContext & l_context = GetParserContext( p_context );
		ControlRPtr l_control = l_context.GetTop();

		if ( l_control )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_control->SetBackgroundMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		ControlRPtr l_control = l_context.GetTop();

		if ( l_control )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto l_material = l_context.m_engine->GetMaterialCache().Find( l_name );

			if ( l_material )
			{
				l_control->SetForegroundMaterial( l_material );
			}
			else
			{
				PARSING_ERROR( cuT( "Material not found: [" + l_name + "]." ) );
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
		ParserContext & l_context = GetParserContext( p_context );
		ControlRPtr l_control = l_context.GetTop();

		if ( l_control )
		{
			Point4d l_value;
			p_params[0]->Get( l_value );
			l_control->GetBackground()->SetBorderInnerUV( l_value );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlBorderOuterUv )
	{
		ParserContext & l_context = GetParserContext( p_context );
		ControlRPtr l_control = l_context.GetTop();

		if ( l_control )
		{
			Point4d l_value;
			p_params[0]->Get( l_value );
			l_control->GetBackground()->SetBorderOuterUV( l_value );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlCenterUv )
	{
		ParserContext & l_context = GetParserContext( p_context );
		ControlRPtr l_control = l_context.GetTop();

		if ( l_control )
		{
			Point4d l_value;
			p_params[0]->Get( l_value );
			l_control->GetBackground()->SetUV( l_value );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ControlVisible )
	{
		ParserContext & l_context = GetParserContext( p_context );
		ControlRPtr l_control = l_context.GetTop();

		if ( l_control )
		{
			bool l_value;
			p_params[0]->Get( l_value );
			l_control->SetVisible( l_value );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()
}
