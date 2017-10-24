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
using namespace castor3d;
using namespace castor;

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

		ControlRPtr getTop()const
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

				switch ( top->getType() )
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

	ControlsManager & getControlsManager( FileParserContextSPtr p_context )
	{
		return static_cast< ControlsManager & >( *std::static_pointer_cast< SceneFileContext >( p_context )->m_pParser->getEngine()->getUserInputListener() );
	}

	ParserContext & getParserContext( FileParserContextSPtr p_context )
	{
		return *static_cast< ParserContext * >( p_context->getUserContext( PLUGIN_NAME ) );
	}

	template< typename T >
	std::shared_ptr< T > CreateControl( ParserContext & p_context, String const & p_name, std::shared_ptr< T > & p_control )
	{
		p_control = std::make_shared< T >( p_name, *p_context.m_engine, p_context.getTop(), p_context.m_ctrlId++ );
		p_control->addStyle( p_context.m_flags );
		p_context.m_parents.push( p_control );
		p_context.m_flags = 0;
		return p_control;
	}

	template< typename T > void FinishControl( ControlsManager & p_manager, ParserContext & p_context, std::shared_ptr< T > p_control )
	{
		if ( p_control )
		{
			p_manager.create( p_control );
			p_context.m_parents.pop();
		}
	}

	IMPLEMENT_ATTRIBUTE_PARSER( parserGui )
	{
		ParserContext * context = new ParserContext;
		context->m_engine = std::static_pointer_cast< SceneFileContext >( p_context )->m_pParser->getEngine();
		p_context->registerUserContext( PLUGIN_NAME, context );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eGUI )

	IMPLEMENT_ATTRIBUTE_PARSER( parserDefaultFont )
	{
		ControlsManager & ctrlsManager = getControlsManager( p_context );
		ParserContext & context = getParserContext( p_context );
		auto & cache = context.m_engine->getFontCache();
		String name;
		p_params[0]->get( name );
		FontSPtr font = cache.find( name );

		if ( font )
		{
			ctrlsManager.setDefaultFont( font );
		}
		else
		{
			PARSING_ERROR( cuT( "Unknown font: " ) + name );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserGuiEnd )
	{
		delete reinterpret_cast< ParserContext * >( p_context->unregisterUserContext( PLUGIN_NAME ) );
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserButton )
	{
		ParserContext & context = getParserContext( p_context );
		String name;
		p_params[0]->get( name );
		CreateControl( context, name, context.m_button );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eButton )

	IMPLEMENT_ATTRIBUTE_PARSER( parserButtonFont )
	{
		ParserContext & context = getParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->get( name );
			button->setFont( name );
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserButtonCaption )
	{
		ParserContext & context = getParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String text;
			p_params[0]->get( text );
			button->setCaption( text );
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserButtonTextMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				button->setTextMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserButtonHighlightedBackgroundMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				button->setHighlightedBackgroundMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserButtonHighlightedForegroundMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				button->setHighlightedForegroundMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserButtonHighlightedTextMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				button->setHighlightedTextMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserButtonPushedBackgroundMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				button->setPushedBackgroundMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserButtonPushedForegroundMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				button->setPushedForegroundMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserButtonPushedTextMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				button->setPushedTextMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserButtonHAlign )
	{
		ParserContext & context = getParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			uint32_t value;
			p_params[0]->get( value );
			button->setHAlign( HAlign( value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserButtonVAlign )
	{
		ParserContext & context = getParserContext( p_context );
		ButtonCtrlSPtr button = context.m_button;

		if ( button )
		{
			uint32_t value;
			p_params[0]->get( value );
			button->setVAlign( VAlign( value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No button initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserButtonEnd )
	{
		ParserContext & context = getParserContext( p_context );
		FinishControl( getControlsManager( p_context ), context, context.m_button );
		context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserComboBox )
	{
		ControlsManager & ctrlsManager = getControlsManager( p_context );
		ParserContext & context = getParserContext( p_context );
		String name;
		p_params[0]->get( name );
		CreateControl( context, name, context.m_combo );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eComboBox )

	IMPLEMENT_ATTRIBUTE_PARSER( parserComboBoxFont )
	{
		ParserContext & context = getParserContext( p_context );
		ComboBoxCtrlSPtr combo = context.m_combo;

		if ( combo )
		{
			String name;
			p_params[0]->get( name );
			combo->setFont( name );
		}
		else
		{
			PARSING_ERROR( cuT( "No combo box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserComboBoxItem )
	{
		ParserContext & context = getParserContext( p_context );
		ComboBoxCtrlSPtr combo = context.m_combo;

		if ( combo )
		{
			String text;
			p_params[0]->get( text );
			combo->appendItem( text );
		}
		else
		{
			PARSING_ERROR( cuT( "No combo box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserComboBoxSelectedItemBackgroundMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ComboBoxCtrlSPtr combo = context.m_combo;

		if ( combo )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				combo->setSelectedItemBackgroundMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserComboBoxSelectedItemForegroundMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ComboBoxCtrlSPtr combo = context.m_combo;

		if ( combo )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				combo->setSelectedItemForegroundMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserComboBoxHighlightedItemBackgroundMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ComboBoxCtrlSPtr combo = context.m_combo;

		if ( combo )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				combo->setHighlightedItemBackgroundMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserComboBoxEnd )
	{
		ParserContext & context = getParserContext( p_context );
		FinishControl( getControlsManager( p_context ), context, context.m_combo );
		context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserEdit )
	{
		ControlsManager & ctrlsManager = getControlsManager( p_context );
		ParserContext & context = getParserContext( p_context );
		String name;
		p_params[0]->get( name );
		CreateControl( context, name, context.m_edit );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eEdit )

	IMPLEMENT_ATTRIBUTE_PARSER( parserEditFont )
	{
		ParserContext & context = getParserContext( p_context );
		EditCtrlSPtr edit = context.m_edit;

		if ( edit )
		{
			String name;
			p_params[0]->get( name );
			edit->setFont( name );
		}
		else
		{
			PARSING_ERROR( cuT( "No edit initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserEditCaption )
	{
		ParserContext & context = getParserContext( p_context );
		EditCtrlSPtr edit = context.m_edit;

		if ( edit )
		{
			String text;
			p_params[0]->get( text );
			edit->setCaption( text );
		}
		else
		{
			PARSING_ERROR( cuT( "No edit initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserEditMultiLine )
	{
		ParserContext & context = getParserContext( p_context );
		EditCtrlSPtr edit = context.m_edit;

		if ( edit )
		{
			bool value;
			p_params[0]->get( value );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserEditEnd )
	{
		ParserContext & context = getParserContext( p_context );
		FinishControl( getControlsManager( p_context ), context, context.m_edit );
		context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserListBox )
	{
		ControlsManager & ctrlsManager = getControlsManager( p_context );
		ParserContext & context = getParserContext( p_context );
		String name;
		p_params[0]->get( name );
		CreateControl( context, name, context.m_listbox );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eListBox )

	IMPLEMENT_ATTRIBUTE_PARSER( parserListBoxFont )
	{
		ParserContext & context = getParserContext( p_context );
		ListBoxCtrlSPtr listbox = context.m_listbox;

		if ( listbox )
		{
			String name;
			p_params[0]->get( name );
			listbox->setFont( name );
		}
		else
		{
			PARSING_ERROR( cuT( "No list box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserListBoxItem )
	{
		ParserContext & context = getParserContext( p_context );
		ListBoxCtrlSPtr listbox = context.m_listbox;

		if ( listbox )
		{
			String text;
			p_params[0]->get( text );
			listbox->appendItem( text );
		}
		else
		{
			PARSING_ERROR( cuT( "No list box initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserListBoxSelectedItemBackgroundMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ListBoxCtrlSPtr listbox = context.m_listbox;

		if ( listbox )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				listbox->setSelectedItemBackgroundMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserListBoxHighlightedItemBackgroundMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ListBoxCtrlSPtr listbox = context.m_listbox;

		if ( listbox )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				listbox->setHighlightedItemBackgroundMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserListBoxSelectedItemForegroundMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ListBoxCtrlSPtr listbox = context.m_listbox;

		if ( listbox )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				listbox->setSelectedItemForegroundMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserListBoxEnd )
	{
		ParserContext & context = getParserContext( p_context );
		FinishControl( getControlsManager( p_context ), context, context.m_listbox );
		context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSlider )
	{
		ControlsManager & ctrlsManager = getControlsManager( p_context );
		ParserContext & context = getParserContext( p_context );
		String name;
		p_params[0]->get( name );
		CreateControl( context, name, context.m_slider );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eSlider )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSliderEnd )
	{
		ParserContext & context = getParserContext( p_context );
		FinishControl( getControlsManager( p_context ), context, context.m_slider );
		context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserStatic )
	{
		ParserContext & context = getParserContext( p_context );
		String name;
		p_params[0]->get( name );
		CreateControl( context, name, context.m_static );
	}
	END_ATTRIBUTE_PUSH( CastorGui::GUISection::eStatic )

	IMPLEMENT_ATTRIBUTE_PARSER( parserStaticFont )
	{
		ParserContext & context = getParserContext( p_context );
		StaticCtrlSPtr ctrl = context.m_static;

		if ( ctrl )
		{
			String name;
			p_params[0]->get( name );
			ctrl->setFont( name );
		}
		else
		{
			PARSING_ERROR( cuT( "No static initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserStaticCaption )
	{
		ParserContext & context = getParserContext( p_context );
		StaticCtrlSPtr ctrl = context.m_static;

		if ( ctrl )
		{
			String text;
			p_params[0]->get( text );
			ctrl->setCaption( text );
		}
		else
		{
			PARSING_ERROR( cuT( "No static initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserStaticHAlign )
	{
		ParserContext & context = getParserContext( p_context );
		StaticCtrlSPtr ctrl = context.m_static;

		if ( ctrl )
		{
			uint32_t value;
			p_params[0]->get( value );
			ctrl->setHAlign( HAlign( value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No static initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserStaticVAlign )
	{
		ParserContext & context = getParserContext( p_context );
		StaticCtrlSPtr ctrl = context.m_static;

		if ( ctrl )
		{
			uint32_t value;
			p_params[0]->get( value );
			ctrl->setVAlign( VAlign( value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No static initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserStaticEnd )
	{
		ParserContext & context = getParserContext( p_context );
		FinishControl( getControlsManager( p_context ), context, context.m_static );
		context.Pop();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserControlPixelPosition )
	{
		ParserContext & context = getParserContext( p_context );
		ControlRPtr control = context.getTop();

		if ( control )
		{
			Position position;
			p_params[0]->get( position );
			control->setPosition( position );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserControlPixelSize )
	{
		ParserContext & context = getParserContext( p_context );
		ControlRPtr control = context.getTop();

		if ( control )
		{
			Size size;
			p_params[0]->get( size );
			control->setSize( size );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserControlPixelBorderSize )
	{
		ParserContext & context = getParserContext( p_context );
		ControlRPtr control = context.getTop();

		if ( control )
		{
			Rectangle size;
			p_params[0]->get( size );
			control->setBackgroundBorders( size );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserControlBackgroundMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ControlRPtr control = context.getTop();

		if ( control )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				control->setBackgroundMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserControlBorderMaterial )
	{
		ParserContext & context = getParserContext( p_context );
		ControlRPtr control = context.getTop();

		if ( control )
		{
			String name;
			p_params[0]->get( name );
			auto material = context.m_engine->getMaterialCache().find( name );

			if ( material )
			{
				control->setForegroundMaterial( material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserControlBorderInnerUv )
	{
		ParserContext & context = getParserContext( p_context );
		ControlRPtr control = context.getTop();

		if ( control )
		{
			Point4d value;
			p_params[0]->get( value );
			control->getBackground()->setBorderInnerUV( value );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserControlBorderOuterUv )
	{
		ParserContext & context = getParserContext( p_context );
		ControlRPtr control = context.getTop();

		if ( control )
		{
			Point4d value;
			p_params[0]->get( value );
			control->getBackground()->setBorderOuterUV( value );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserControlCenterUv )
	{
		ParserContext & context = getParserContext( p_context );
		ControlRPtr control = context.getTop();

		if ( control )
		{
			Point4d value;
			p_params[0]->get( value );
			control->getBackground()->setUV( value );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserControlVisible )
	{
		ParserContext & context = getParserContext( p_context );
		ControlRPtr control = context.getTop();

		if ( control )
		{
			bool value;
			p_params[0]->get( value );
			control->setVisible( value );
		}
		else
		{
			PARSING_ERROR( cuT( "No control initialised." ) );
		}
	}
	END_ATTRIBUTE()
}
