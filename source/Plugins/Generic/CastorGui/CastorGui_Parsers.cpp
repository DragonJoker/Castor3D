#include "CastorGui/CastorGui_Parsers.hpp"

#include "CastorGui/ControlsManager.hpp"
#include "CastorGui/CtrlButton.hpp"
#include "CastorGui/CtrlComboBox.hpp"
#include "CastorGui/CtrlEdit.hpp"
#include "CastorGui/CtrlListBox.hpp"
#include "CastorGui/CtrlSlider.hpp"
#include "CastorGui/CtrlStatic.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

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

				default:
					CU_Failure( "Unsupported Control Type" );
					break;
				}
			}
		}
	};

	ControlsManager & getControlsManager( FileParserContextSPtr context )
	{
		return static_cast< ControlsManager & >( *std::static_pointer_cast< SceneFileContext >( context )->m_pParser->getEngine()->getUserInputListener() );
	}

	ParserContext & getParserContext( FileParserContextSPtr context )
	{
		return *static_cast< ParserContext * >( context->getUserContext( PLUGIN_NAME ) );
	}

	template< typename T >
	std::shared_ptr< T > CreateControl( ParserContext & context, String const & p_name, std::shared_ptr< T > & p_control )
	{
		p_control = std::make_shared< T >( p_name, *context.m_engine, context.getTop(), context.m_ctrlId++ );
		p_control->addStyle( context.m_flags );
		context.m_parents.push( p_control );
		context.m_flags = 0;
		return p_control;
	}

	template< typename T > void FinishControl( ControlsManager & p_manager, ParserContext & context, std::shared_ptr< T > p_control )
	{
		if ( p_control )
		{
			p_manager.create( p_control );
			context.m_parents.pop();
		}
	}

	CU_ImplementAttributeParser( parserGui )
	{
		ParserContext * guiContext = new ParserContext;
		guiContext->m_engine = std::static_pointer_cast< SceneFileContext >( context )->m_pParser->getEngine();
		context->registerUserContext( PLUGIN_NAME, guiContext );
	}
	CU_EndAttributePush( CastorGui::GUISection::eGUI )

	CU_ImplementAttributeParser( parserDefaultFont )
	{
		ControlsManager & ctrlsManager = getControlsManager( context );
		ParserContext & guiContext = getParserContext( context );
		auto & cache = guiContext.m_engine->getFontCache();
		String name;
		params[0]->get( name );
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
		delete reinterpret_cast< ParserContext * >( context->unregisterUserContext( PLUGIN_NAME ) );
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserButton )
	{
		ParserContext & guiContext = getParserContext( context );
		String name;
		params[0]->get( name );
		CreateControl( guiContext, name, guiContext.m_button );
	}
	CU_EndAttributePush( CastorGui::GUISection::eButton )

	CU_ImplementAttributeParser( parserButtonFont )
	{
		ParserContext & guiContext = getParserContext( context );
		ButtonCtrlSPtr button = guiContext.m_button;

		if ( button )
		{
			String name;
			params[0]->get( name );
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
		ParserContext & guiContext = getParserContext( context );
		ButtonCtrlSPtr button = guiContext.m_button;

		if ( button )
		{
			String text;
			params[0]->get( text );
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
		ParserContext & guiContext = getParserContext( context );
		ButtonCtrlSPtr button = guiContext.m_button;

		if ( button )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		ButtonCtrlSPtr button = guiContext.m_button;

		if ( button )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		ButtonCtrlSPtr button = guiContext.m_button;

		if ( button )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		ButtonCtrlSPtr button = guiContext.m_button;

		if ( button )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		ButtonCtrlSPtr button = guiContext.m_button;

		if ( button )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		ButtonCtrlSPtr button = guiContext.m_button;

		if ( button )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		ButtonCtrlSPtr button = guiContext.m_button;

		if ( button )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		ButtonCtrlSPtr button = guiContext.m_button;

		if ( button )
		{
			uint32_t value;
			params[0]->get( value );
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
		ParserContext & guiContext = getParserContext( context );
		ButtonCtrlSPtr button = guiContext.m_button;

		if ( button )
		{
			uint32_t value;
			params[0]->get( value );
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
		ParserContext & guiContext = getParserContext( context );
		FinishControl( getControlsManager( context ), guiContext, guiContext.m_button );
		guiContext.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserComboBox )
	{
		ParserContext & guiContext = getParserContext( context );
		String name;
		params[0]->get( name );
		CreateControl( guiContext, name, guiContext.m_combo );
	}
	CU_EndAttributePush( CastorGui::GUISection::eComboBox )

	CU_ImplementAttributeParser( parserComboBoxFont )
	{
		ParserContext & guiContext = getParserContext( context );
		ComboBoxCtrlSPtr combo = guiContext.m_combo;

		if ( combo )
		{
			String name;
			params[0]->get( name );
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
		ParserContext & guiContext = getParserContext( context );
		ComboBoxCtrlSPtr combo = guiContext.m_combo;

		if ( combo )
		{
			String text;
			params[0]->get( text );
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
		ParserContext & guiContext = getParserContext( context );
		ComboBoxCtrlSPtr combo = guiContext.m_combo;

		if ( combo )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		ComboBoxCtrlSPtr combo = guiContext.m_combo;

		if ( combo )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		ComboBoxCtrlSPtr combo = guiContext.m_combo;

		if ( combo )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		FinishControl( getControlsManager( context ), guiContext, guiContext.m_combo );
		guiContext.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserEdit )
	{
		ParserContext & guiContext = getParserContext( context );
		String name;
		params[0]->get( name );
		CreateControl( guiContext, name, guiContext.m_edit );
	}
	CU_EndAttributePush( CastorGui::GUISection::eEdit )

	CU_ImplementAttributeParser( parserEditFont )
	{
		ParserContext & guiContext = getParserContext( context );
		EditCtrlSPtr edit = guiContext.m_edit;

		if ( edit )
		{
			String name;
			params[0]->get( name );
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
		ParserContext & guiContext = getParserContext( context );
		EditCtrlSPtr edit = guiContext.m_edit;

		if ( edit )
		{
			String text;
			params[0]->get( text );
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
		ParserContext & guiContext = getParserContext( context );
		EditCtrlSPtr edit = guiContext.m_edit;

		if ( edit )
		{
			bool value;
			params[0]->get( value );
			guiContext.m_flags |= value ? ( uint32_t( EditStyle::eMultiline )
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
		ParserContext & guiContext = getParserContext( context );
		FinishControl( getControlsManager( context ), guiContext, guiContext.m_edit );
		guiContext.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserListBox )
	{
		ParserContext & guiContext = getParserContext( context );
		String name;
		params[0]->get( name );
		CreateControl( guiContext, name, guiContext.m_listbox );
	}
	CU_EndAttributePush( CastorGui::GUISection::eListBox )

	CU_ImplementAttributeParser( parserListBoxFont )
	{
		ParserContext & guiContext = getParserContext( context );
		ListBoxCtrlSPtr listbox = guiContext.m_listbox;

		if ( listbox )
		{
			String name;
			params[0]->get( name );
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
		ParserContext & guiContext = getParserContext( context );
		ListBoxCtrlSPtr listbox = guiContext.m_listbox;

		if ( listbox )
		{
			String text;
			params[0]->get( text );
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
		ParserContext & guiContext = getParserContext( context );
		ListBoxCtrlSPtr listbox = guiContext.m_listbox;

		if ( listbox )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		ListBoxCtrlSPtr listbox = guiContext.m_listbox;

		if ( listbox )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		ListBoxCtrlSPtr listbox = guiContext.m_listbox;

		if ( listbox )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		FinishControl( getControlsManager( context ), guiContext, guiContext.m_listbox );
		guiContext.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSlider )
	{
		ParserContext & guiContext = getParserContext( context );
		String name;
		params[0]->get( name );
		CreateControl( guiContext, name, guiContext.m_slider );
	}
	CU_EndAttributePush( CastorGui::GUISection::eSlider )

	CU_ImplementAttributeParser( parserSliderEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		FinishControl( getControlsManager( context ), guiContext, guiContext.m_slider );
		guiContext.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStatic )
	{
		ParserContext & guiContext = getParserContext( context );
		String name;
		params[0]->get( name );
		CreateControl( guiContext, name, guiContext.m_static );
	}
	CU_EndAttributePush( CastorGui::GUISection::eStatic )

	CU_ImplementAttributeParser( parserStaticFont )
	{
		ParserContext & guiContext = getParserContext( context );
		StaticCtrlSPtr ctrl = guiContext.m_static;

		if ( ctrl )
		{
			String name;
			params[0]->get( name );
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
		ParserContext & guiContext = getParserContext( context );
		StaticCtrlSPtr ctrl = guiContext.m_static;

		if ( ctrl )
		{
			String text;
			params[0]->get( text );
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
		ParserContext & guiContext = getParserContext( context );
		StaticCtrlSPtr ctrl = guiContext.m_static;

		if ( ctrl )
		{
			uint32_t value;
			params[0]->get( value );
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
		ParserContext & guiContext = getParserContext( context );
		StaticCtrlSPtr ctrl = guiContext.m_static;

		if ( ctrl )
		{
			uint32_t value;
			params[0]->get( value );
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
		ParserContext & guiContext = getParserContext( context );
		FinishControl( getControlsManager( context ), guiContext, guiContext.m_static );
		guiContext.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserControlPixelPosition )
	{
		ParserContext & guiContext = getParserContext( context );
		ControlRPtr control = guiContext.getTop();

		if ( control )
		{
			Position position;
			params[0]->get( position );
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
		ParserContext & guiContext = getParserContext( context );
		ControlRPtr control = guiContext.getTop();

		if ( control )
		{
			Size size;
			params[0]->get( size );
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
		ParserContext & guiContext = getParserContext( context );
		ControlRPtr control = guiContext.getTop();

		if ( control )
		{
			castor::Rectangle size;
			params[0]->get( size );
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
		ParserContext & guiContext = getParserContext( context );
		ControlRPtr control = guiContext.getTop();

		if ( control )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		ControlRPtr control = guiContext.getTop();

		if ( control )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.m_engine->getMaterialCache().find( name );

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
		ParserContext & guiContext = getParserContext( context );
		ControlRPtr control = guiContext.getTop();

		if ( control )
		{
			Point4d value;
			params[0]->get( value );
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
		ParserContext & guiContext = getParserContext( context );
		ControlRPtr control = guiContext.getTop();

		if ( control )
		{
			Point4d value;
			params[0]->get( value );
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
		ParserContext & guiContext = getParserContext( context );
		ControlRPtr control = guiContext.getTop();

		if ( control )
		{
			Point4d value;
			params[0]->get( value );
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
		ParserContext & guiContext = getParserContext( context );
		ControlRPtr control = guiContext.getTop();

		if ( control )
		{
			bool value;
			params[0]->get( value );
			control->setVisible( value );
		}
		else
		{
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttribute()
}
