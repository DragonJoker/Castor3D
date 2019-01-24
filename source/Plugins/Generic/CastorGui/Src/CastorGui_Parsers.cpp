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

	CU_ImplementAttributeParser( parserGui )
	{
		ParserContext * context = new ParserContext;
		context->m_engine = std::static_pointer_cast< SceneFileContext >( p_context )->m_pParser->getEngine();
		p_context->registerUserContext( PLUGIN_NAME, context );
	}
	CU_EndAttributePush( CastorGui::GUISection::eGUI )

	CU_ImplementAttributeParser( parserDefaultFont )
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
			CU_ParsingError( cuT( "Unknown font: " ) + name );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserGuiEnd )
	{
		delete reinterpret_cast< ParserContext * >( p_context->unregisterUserContext( PLUGIN_NAME ) );
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserButton )
	{
		ParserContext & context = getParserContext( p_context );
		String name;
		p_params[0]->get( name );
		CreateControl( context, name, context.m_button );
	}
	CU_EndAttributePush( CastorGui::GUISection::eButton )

	CU_ImplementAttributeParser( parserButtonFont )
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
			CU_ParsingError( cuT( "No button initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonCaption )
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
			CU_ParsingError( cuT( "No button initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonTextMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonHighlightedBackgroundMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonHighlightedForegroundMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonHighlightedTextMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonPushedBackgroundMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonPushedForegroundMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonPushedTextMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonHAlign )
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
			CU_ParsingError( cuT( "No button initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonVAlign )
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
			CU_ParsingError( cuT( "No button initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonEnd )
	{
		ParserContext & context = getParserContext( p_context );
		FinishControl( getControlsManager( p_context ), context, context.m_button );
		context.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserComboBox )
	{
		ParserContext & context = getParserContext( p_context );
		String name;
		p_params[0]->get( name );
		CreateControl( context, name, context.m_combo );
	}
	CU_EndAttributePush( CastorGui::GUISection::eComboBox )

	CU_ImplementAttributeParser( parserComboBoxFont )
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
			CU_ParsingError( cuT( "No combo box initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserComboBoxItem )
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
			CU_ParsingError( cuT( "No combo box initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserComboBoxSelectedItemBackgroundMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No combo box initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserComboBoxSelectedItemForegroundMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No combo box initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserComboBoxHighlightedItemBackgroundMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No combo box initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserComboBoxEnd )
	{
		ParserContext & context = getParserContext( p_context );
		FinishControl( getControlsManager( p_context ), context, context.m_combo );
		context.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserEdit )
	{
		ParserContext & context = getParserContext( p_context );
		String name;
		p_params[0]->get( name );
		CreateControl( context, name, context.m_edit );
	}
	CU_EndAttributePush( CastorGui::GUISection::eEdit )

	CU_ImplementAttributeParser( parserEditFont )
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
			CU_ParsingError( cuT( "No edit initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserEditCaption )
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
			CU_ParsingError( cuT( "No edit initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserEditMultiLine )
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
			CU_ParsingError( cuT( "No edit initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserEditEnd )
	{
		ParserContext & context = getParserContext( p_context );
		FinishControl( getControlsManager( p_context ), context, context.m_edit );
		context.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserListBox )
	{
		ParserContext & context = getParserContext( p_context );
		String name;
		p_params[0]->get( name );
		CreateControl( context, name, context.m_listbox );
	}
	CU_EndAttributePush( CastorGui::GUISection::eListBox )

	CU_ImplementAttributeParser( parserListBoxFont )
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
			CU_ParsingError( cuT( "No list box initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserListBoxItem )
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
			CU_ParsingError( cuT( "No list box initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserListBoxSelectedItemBackgroundMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No list box initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserListBoxHighlightedItemBackgroundMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No list box initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserListBoxSelectedItemForegroundMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No list box initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserListBoxEnd )
	{
		ParserContext & context = getParserContext( p_context );
		FinishControl( getControlsManager( p_context ), context, context.m_listbox );
		context.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSlider )
	{
		ParserContext & context = getParserContext( p_context );
		String name;
		p_params[0]->get( name );
		CreateControl( context, name, context.m_slider );
	}
	CU_EndAttributePush( CastorGui::GUISection::eSlider )

	CU_ImplementAttributeParser( parserSliderEnd )
	{
		ParserContext & context = getParserContext( p_context );
		FinishControl( getControlsManager( p_context ), context, context.m_slider );
		context.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStatic )
	{
		ParserContext & context = getParserContext( p_context );
		String name;
		p_params[0]->get( name );
		CreateControl( context, name, context.m_static );
	}
	CU_EndAttributePush( CastorGui::GUISection::eStatic )

	CU_ImplementAttributeParser( parserStaticFont )
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
			CU_ParsingError( cuT( "No static initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStaticCaption )
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
			CU_ParsingError( cuT( "No static initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStaticHAlign )
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
			CU_ParsingError( cuT( "No static initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStaticVAlign )
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
			CU_ParsingError( cuT( "No static initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStaticEnd )
	{
		ParserContext & context = getParserContext( p_context );
		FinishControl( getControlsManager( p_context ), context, context.m_static );
		context.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserControlPixelPosition )
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
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserControlPixelSize )
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
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserControlPixelBorderSize )
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
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserControlBackgroundMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserControlBorderMaterial )
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
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserControlBorderInnerUv )
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
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserControlBorderOuterUv )
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
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserControlCenterUv )
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
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserControlVisible )
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
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttribute()
}
