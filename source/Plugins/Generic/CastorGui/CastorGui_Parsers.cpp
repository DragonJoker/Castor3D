#include "CastorGui/CastorGui_Parsers.hpp"

#include "CastorGui/ControlsManager.hpp"
#include "CastorGui/Controls/CtrlButton.hpp"
#include "CastorGui/Controls/CtrlComboBox.hpp"
#include "CastorGui/Controls/CtrlEdit.hpp"
#include "CastorGui/Controls/CtrlListBox.hpp"
#include "CastorGui/Controls/CtrlSlider.hpp"
#include "CastorGui/Controls/CtrlStatic.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

using namespace CastorGui;
using namespace castor3d;
using namespace castor;

namespace CastorGui
{
	namespace
	{
		ControlsManager & getControlsManager( FileParserContext & context )
		{
			return static_cast< ControlsManager & >( *static_cast< castor3d::SceneFileParser * >( context.parser )->getEngine()->getUserInputListener() );
		}

		ControlsManager & getControlsManager( ParserContext & context )
		{
			return static_cast< ControlsManager & >( *context.engine->getUserInputListener() );
		}

		ParserContext & getParserContext( FileParserContext & context )
		{
			return *static_cast< ParserContext * >( context.getUserContext( PLUGIN_NAME ) );
		}

		template< typename ControlT >
		std::shared_ptr< ControlT > createControl( ParserContext & context
			, String const & controlName
			, String const & themeName
			, std::shared_ptr< ControlT > & control )
		{
			auto theme = getControlsManager( context ).getTheme( themeName );
			CU_Require( theme != nullptr );

			if constexpr ( ControlT::Type == ControlType::eButton )
			{
				control = std::make_shared< ControlT >( controlName
					, theme->getButtonStyle()
					, context.getTop()
					, context.ctrlId++ );
			}
			else if constexpr ( ControlT::Type == ControlType::eComboBox )
			{
				control = std::make_shared< ControlT >( controlName
					, theme->getComboBoxStyle()
					, context.getTop()
					, context.ctrlId++ );
			}
			else if constexpr ( ControlT::Type == ControlType::eEdit )
			{
				control = std::make_shared< ControlT >( controlName
					, theme->getEditStyle()
					, context.getTop()
					, context.ctrlId++ );
			}
			else if constexpr ( ControlT::Type == ControlType::eListBox )
			{
				control = std::make_shared< ControlT >( controlName
					, theme->getListBoxStyle()
					, context.getTop()
					, context.ctrlId++ );
			}
			else if constexpr ( ControlT::Type == ControlType::eSlider )
			{
				control = std::make_shared< ControlT >( controlName
					, theme->getSliderStyle()
					, context.getTop()
					, context.ctrlId++ );
			}
			else if constexpr ( ControlT::Type == ControlType::eStatic )
			{
				control = std::make_shared< ControlT >( controlName
					, theme->getStaticStyle()
					, context.getTop()
					, context.ctrlId++ );
			}
			else
			{
				CU_Exception( "Unsupported control type" );
			}

			control->addFlag( context.flags );
			context.parents.push( control );
			context.flags = 0;
			return control;
		}

		template< typename T > void finishControl( ControlsManager & manager
			, ParserContext & context
			, std::shared_ptr< T > control )
		{
			if ( control )
			{
				manager.create( control );
				context.parents.pop();
			}
		}
	}

	//*********************************************************************************************

	ControlRPtr ParserContext::getTop()const
	{
		ControlRPtr result{};

		if ( !parents.empty() )
		{
			result = parents.top().get();
		}

		return result;
	}

	void ParserContext::Pop()
	{
		button.reset();
		edit.reset();
		listbox.reset();
		slider.reset();
		staticTxt.reset();
		combo.reset();

		if ( !parents.empty() )
		{
			ControlSPtr top;
			top = parents.top();

			switch ( top->getType() )
			{
			case ControlType::eStatic:
				staticTxt = std::static_pointer_cast< StaticCtrl >( top );
				break;
			case ControlType::eEdit:
				edit = std::static_pointer_cast< EditCtrl >( top );
				break;
			case ControlType::eSlider:
				slider = std::static_pointer_cast< SliderCtrl >( top );
				break;
			case ControlType::eComboBox:
				combo = std::static_pointer_cast< ComboBoxCtrl >( top );
				break;
			case ControlType::eListBox:
				listbox = std::static_pointer_cast< ListBoxCtrl >( top );
				break;
			case ControlType::eButton:
				button = std::static_pointer_cast< ButtonCtrl >( top );
				break;
			default:
				CU_Failure( "Unsupported Control Type" );
				break;
			}
		}
	}

	//*********************************************************************************************

	CU_ImplementAttributeParser( parserGui )
	{
	}
	CU_EndAttributePush( CastorGui::GUISection::eGUI )

	CU_ImplementAttributeParser( parserTheme )
	{
		ControlsManager & ctrlsManager = getControlsManager( context );
		ParserContext & guiContext = getParserContext( context );
		castor::String name;
		params[0]->get( name );
		guiContext.theme = ctrlsManager.createTheme( name );
	}
	CU_EndAttributePush( GUISection::eTheme )

	CU_ImplementAttributeParser( parserGuiEnd )
	{
		delete reinterpret_cast< ParserContext * >( context.unregisterUserContext( PLUGIN_NAME ) );
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserButton )
	{
		ParserContext & guiContext = getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( CastorGui::GUISection::eButton )

	CU_ImplementAttributeParser( parserButtonTheme )
	{
		castor::String themeName;
		params[0]->get( themeName );
		ParserContext & guiContext = getParserContext( context );
		createControl( guiContext
			, guiContext.controlName
			, themeName
			, guiContext.button );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonHAlign )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto control = guiContext.button )
		{
			uint32_t value;
			params[0]->get( value );
			control->setHAlign( HAlign( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "No button control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonVAlign )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto control = guiContext.button )
		{
			uint32_t value;
			params[0]->get( value );
			control->setVAlign( VAlign( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "No button control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonCaption )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto control = guiContext.button )
		{
			String text;
			params[0]->get( text );
			control->setCaption( text );
		}
		else
		{
			CU_ParsingError( cuT( "No button control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		finishControl( getControlsManager( context ), guiContext, guiContext.button );
		guiContext.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserComboBox )
	{
		ParserContext & guiContext = getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( CastorGui::GUISection::eComboBox )

	CU_ImplementAttributeParser( parserComboBoxTheme )
	{
		castor::String themeName;
		params[0]->get( themeName );
		ParserContext & guiContext = getParserContext( context );
		createControl( guiContext
			, guiContext.controlName
			, themeName
			, guiContext.combo );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserComboBoxItem )
	{
		ParserContext & guiContext = getParserContext( context );
		ComboBoxCtrlSPtr combo = guiContext.combo;

		if ( combo )
		{
			String text;
			params[0]->get( text );
			combo->appendItem( text );
		}
		else
		{
			CU_ParsingError( cuT( "No combobox control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserComboBoxEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		finishControl( getControlsManager( context ), guiContext, guiContext.combo );
		guiContext.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserEdit )
	{
		ParserContext & guiContext = getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( CastorGui::GUISection::eEdit )

	CU_ImplementAttributeParser( parserEditTheme )
	{
		castor::String themeName;
		params[0]->get( themeName );
		ParserContext & guiContext = getParserContext( context );
		createControl( guiContext
			, guiContext.controlName
			, themeName
			, guiContext.edit );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserEditCaption )
	{
		ParserContext & guiContext = getParserContext( context );
		EditCtrlSPtr edit = guiContext.edit;

		if ( edit )
		{
			String text;
			params[0]->get( text );
			edit->setCaption( text );
		}
		else
		{
			CU_ParsingError( cuT( "No edit control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserEditMultiLine )
	{
		ParserContext & guiContext = getParserContext( context );
		EditCtrlSPtr edit = guiContext.edit;

		if ( edit )
		{
			bool value;
			params[0]->get( value );
			guiContext.edit->addFlag( uint32_t( EditFlag::eMultiline )/*
				| uint32_t( EditFlag::eProcessEnter )
				| uint32_t( EditFlag::eProcessTab )*/ );
		}
		else
		{
			CU_ParsingError( cuT( "No edit control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserEditEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		finishControl( getControlsManager( context ), guiContext, guiContext.edit );
		guiContext.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserListBox )
	{
		ParserContext & guiContext = getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( CastorGui::GUISection::eListBox )

	CU_ImplementAttributeParser( parserListBoxTheme )
	{
		castor::String themeName;
		params[0]->get( themeName );
		ParserContext & guiContext = getParserContext( context );
		createControl( guiContext
			, guiContext.controlName
			, themeName
			, guiContext.listbox );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserListBoxItem )
	{
		ParserContext & guiContext = getParserContext( context );
		ListBoxCtrlSPtr listbox = guiContext.listbox;

		if ( listbox )
		{
			String text;
			params[0]->get( text );
			listbox->appendItem( text );
		}
		else
		{
			CU_ParsingError( cuT( "No listbox control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserListBoxEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		finishControl( getControlsManager( context ), guiContext, guiContext.listbox );
		guiContext.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSlider )
	{
		ParserContext & guiContext = getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( CastorGui::GUISection::eSlider )

	CU_ImplementAttributeParser( parserSliderTheme )
	{
		castor::String themeName;
		params[0]->get( themeName );
		ParserContext & guiContext = getParserContext( context );
		createControl( guiContext
			, guiContext.controlName
			, themeName
			, guiContext.slider );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSliderEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		finishControl( getControlsManager( context ), guiContext, guiContext.slider );
		guiContext.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStatic )
	{
		ParserContext & guiContext = getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( CastorGui::GUISection::eStatic )

	CU_ImplementAttributeParser( parserStaticTheme )
	{
		castor::String themeName;
		params[0]->get( themeName );
		ParserContext & guiContext = getParserContext( context );
		createControl( guiContext
			, guiContext.controlName
			, themeName
			, guiContext.staticTxt );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStaticHAlign )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto control = guiContext.staticTxt )
		{
			uint32_t value;
			params[0]->get( value );
			control->setHAlign( HAlign( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "No static control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStaticVAlign )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto control = guiContext.staticTxt )
		{
			uint32_t value;
			params[0]->get( value );
			control->setVAlign( VAlign( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "No static control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStaticCaption )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto control = guiContext.staticTxt )
		{
			String text;
			params[0]->get( text );
			control->setCaption( text );
		}
		else
		{
			CU_ParsingError( cuT( "No static control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStaticEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		finishControl( getControlsManager( context ), guiContext, guiContext.staticTxt );
		guiContext.Pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserControlPixelPosition )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto control = guiContext.getTop() )
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

		if ( auto control = guiContext.getTop() )
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

		if ( auto control = guiContext.getTop() )
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

	CU_ImplementAttributeParser( parserControlBorderInnerUv )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto control = guiContext.getTop() )
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

		if ( auto control = guiContext.getTop() )
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

		if ( auto control = guiContext.getTop() )
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

		if ( auto control = guiContext.getTop() )
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

	CU_ImplementAttributeParser( parserDefaultFont )
	{
		ParserContext & guiContext = getParserContext( context );
		auto & cache = guiContext.engine->getFontCache();
		String name;
		params[0]->get( name );
		auto font = cache.find( name );

		if ( font.lock() )
		{
			guiContext.theme->setDefaultFont( font );
		}
		else
		{
			CU_ParsingError( cuT( "Unknown font: " ) + name );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonStyle )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.buttonStyle = guiContext.theme->createButtonStyle();
		guiContext.style = guiContext.buttonStyle;
	}
	CU_EndAttributePush( GUISection::eButtonStyle )

	CU_ImplementAttributeParser( parserStyleButtonEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.buttonStyle = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserComboBoxStyle )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.comboStyle = guiContext.theme->createComboBoxStyle();
		guiContext.style = guiContext.comboStyle;
	}
	CU_EndAttributePush( GUISection::eComboStyle )

	CU_ImplementAttributeParser( parserComboButtonStyle )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.buttonStyle = &guiContext.comboStyle->getButtonStyle();
		guiContext.style = guiContext.buttonStyle;
	}
	CU_EndAttributePush( GUISection::eButtonStyle )

	CU_ImplementAttributeParser( parserComboListBoxStyle )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.listboxStyle = &guiContext.comboStyle->getListBoxStyle();
		guiContext.style = guiContext.listboxStyle;
	}
	CU_EndAttributePush( GUISection::eListStyle )

	CU_ImplementAttributeParser( parserStyleComboBoxEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.comboStyle = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserEditStyle )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.editStyle = guiContext.theme->createEditStyle();
		guiContext.style = guiContext.editStyle;
	}
	CU_EndAttributePush( GUISection::eEditStyle )

	CU_ImplementAttributeParser( parserStyleEditEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.editStyle = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserListBoxStyle )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.listboxStyle = guiContext.theme->createListBoxStyle();
		guiContext.style = guiContext.listboxStyle;
	}
	CU_EndAttributePush( GUISection::eListStyle )

	CU_ImplementAttributeParser( parserItemStaticStyle )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.staticStyle = &guiContext.listboxStyle->getItemStyle();
		guiContext.style = guiContext.staticStyle;
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserSelItemStaticStyle )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.staticStyle = &guiContext.listboxStyle->getSelectedItemStyle();
		guiContext.style = guiContext.staticStyle;
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserHighItemStaticStyle )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.staticStyle = &guiContext.listboxStyle->getHighlightedItemStyle();
		guiContext.style = guiContext.staticStyle;
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserStyleListBoxEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.listboxStyle = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSliderStyle )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.sliderStyle = guiContext.theme->createSliderStyle();
		guiContext.style = guiContext.sliderStyle;
	}
	CU_EndAttributePush( GUISection::eSliderStyle )

	CU_ImplementAttributeParser( parserStyleSliderEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.sliderStyle = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStaticStyle )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.staticStyle = guiContext.theme->createStaticStyle();
		guiContext.style = guiContext.staticStyle;
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserLineStaticStyle )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.staticStyle = &guiContext.sliderStyle->getLineStyle();
		guiContext.style = guiContext.staticStyle;
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserTickStaticStyle )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.staticStyle = &guiContext.sliderStyle->getTickStyle();
		guiContext.style = guiContext.staticStyle;
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserStyleStaticEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.staticStyle = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserThemeEnd )
	{
		ParserContext & guiContext = getParserContext( context );
		guiContext.theme = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStyleButtonFont )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			String name;
			params[0]->get( name );
			style->setFont( name );
		}
		else
		{
			CU_ParsingError( cuT( "No button style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleButtonTextMaterial )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.engine->getMaterialCache().find( name ).lock().get();

			if ( material )
			{
				style->setTextMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleButtonHighlightedBackgroundMaterial )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.engine->getMaterialCache().find( name ).lock().get();

			if ( material )
			{
				style->setHighlightedBackgroundMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleButtonHighlightedForegroundMaterial )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.engine->getMaterialCache().find( name ).lock().get();

			if ( material )
			{
				style->setHighlightedForegroundMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleButtonHighlightedTextMaterial )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.engine->getMaterialCache().find( name ).lock().get();

			if ( material )
			{
				style->setHighlightedTextMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleButtonPushedBackgroundMaterial )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.engine->getMaterialCache().find( name ).lock().get();

			if ( material )
			{
				style->setPushedBackgroundMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleButtonPushedForegroundMaterial )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.engine->getMaterialCache().find( name ).lock().get();

			if ( material )
			{
				style->setPushedForegroundMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleButtonPushedTextMaterial )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.engine->getMaterialCache().find( name ).lock().get();

			if ( material )
			{
				style->setPushedTextMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No button style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleEditFont )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.editStyle )
		{
			String name;
			params[0]->get( name );
			style->setFont( name );
		}
		else
		{
			CU_ParsingError( cuT( "No edit style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleEditTextMaterial )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.editStyle )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.engine->getMaterialCache().find( name ).lock().get();

			if ( material )
			{
				style->setTextMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No edit style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleStaticFont )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.staticStyle )
		{
			String name;
			params[0]->get( name );
			style->setFont( name );
		}
		else
		{
			CU_ParsingError( cuT( "No static style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleStaticTextMaterial )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.staticStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->getMaterialCache().find( name ).lock().get();

			if ( material )
			{
				style->setTextMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material [" + name + "] not found." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No static style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleBackgroundMaterial )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.style )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.engine->getMaterialCache().find( name ).lock().get();

			if ( material )
			{
				style->setBackgroundMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleForegroundMaterial )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.style )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.engine->getMaterialCache().find( name ).lock().get();

			if ( material )
			{
				style->setForegroundMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleBorderMaterial )
	{
		ParserContext & guiContext = getParserContext( context );

		if ( auto style = guiContext.style )
		{
			String name;
			params[0]->get( name );
			auto material = guiContext.engine->getMaterialCache().find( name ).lock().get();

			if ( material )
			{
				style->setForegroundMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material not found: [" + name + "]." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No style initialised." ) );
		}
	}
	CU_EndAttribute()
}
