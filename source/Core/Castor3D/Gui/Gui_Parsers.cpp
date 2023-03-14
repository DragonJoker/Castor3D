#include "Castor3D/Gui/Gui_Parsers.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlButton.hpp"
#include "Castor3D/Gui/Controls/CtrlComboBox.hpp"
#include "Castor3D/Gui/Controls/CtrlEdit.hpp"
#include "Castor3D/Gui/Controls/CtrlListBox.hpp"
#include "Castor3D/Gui/Controls/CtrlPanel.hpp"
#include "Castor3D/Gui/Controls/CtrlSlider.hpp"
#include "Castor3D/Gui/Controls/CtrlStatic.hpp"
#include "Castor3D/Gui/Layout/LayoutBox.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"

namespace castor3d
{
	namespace guiparse
	{
		static ControlsManager & getControlsManager( castor::FileParserContext & context )
		{
			return static_cast< ControlsManager & >( *static_cast< SceneFileParser * >( context.parser )->getEngine()->getUserInputListener() );
		}

		static ControlsManager & getControlsManager( GuiParserContext & context )
		{
			return static_cast< ControlsManager & >( *context.engine->getUserInputListener() );
		}

		static GuiParserContext & getParserContext( castor::FileParserContext & context )
		{
			return *static_cast< GuiParserContext * >( context.getUserContext( ControlsManager::Name ) );
		}

		template< typename ControlT >
		std::shared_ptr< ControlT > createControl( GuiParserContext & context
			, SceneRPtr scene
			, castor::String const & controlName
			, castor::String const & themeName
			, std::shared_ptr< ControlT > & control )
		{
			auto theme = getControlsManager( context ).getTheme( themeName );
			CU_Require( theme != nullptr );

			if constexpr ( ControlT::Type == ControlType::eButton )
			{
				control = std::make_shared< ControlT >( scene
					, controlName
					, theme->getButtonStyle()
					, context.getTop() );
			}
			else if constexpr ( ControlT::Type == ControlType::eComboBox )
			{
				control = std::make_shared< ControlT >( scene
					, controlName
					, theme->getComboBoxStyle()
					, context.getTop() );
			}
			else if constexpr ( ControlT::Type == ControlType::eEdit )
			{
				control = std::make_shared< ControlT >( scene
					, controlName
					, theme->getEditStyle()
					, context.getTop() );
			}
			else if constexpr ( ControlT::Type == ControlType::eListBox )
			{
				control = std::make_shared< ControlT >( scene
					, controlName
					, theme->getListBoxStyle()
					, context.getTop() );
			}
			else if constexpr ( ControlT::Type == ControlType::eSlider )
			{
				control = std::make_shared< ControlT >( scene
					, controlName
					, theme->getSliderStyle()
					, context.getTop() );
			}
			else if constexpr ( ControlT::Type == ControlType::eStatic )
			{
				control = std::make_shared< ControlT >( scene
					, controlName
					, theme->getStaticStyle()
					, context.getTop() );
			}
			else if constexpr ( ControlT::Type == ControlType::ePanel )
			{
				control = std::make_shared< ControlT >( scene
					, controlName
					, theme->getPanelStyle()
					, context.getTop() );
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

		template< typename T >
		void finishControl( ControlsManager & manager
			, GuiParserContext & context
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

	ControlRPtr GuiParserContext::getTop()const
	{
		ControlRPtr result{};

		if ( !parents.empty() )
		{
			result = parents.top().get();
		}

		return result;
	}

	void GuiParserContext::pop()
	{
		button.reset();
		edit.reset();
		listbox.reset();
		slider.reset();
		staticTxt.reset();
		combo.reset();
		panel.reset();

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
			case ControlType::ePanel:
				panel = std::static_pointer_cast< PanelCtrl >( top );
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
	CU_EndAttributePush( GUISection::eGUI )

	CU_ImplementAttributeParser( parserTheme )
	{
		ControlsManager & ctrlsManager = guiparse::getControlsManager( context );
		auto & guiContext = guiparse::getParserContext( context );
		castor::String name;
		params[0]->get( name );
		guiContext.theme = ctrlsManager.createTheme( name );
	}
	CU_EndAttributePush( GUISection::eTheme )

	CU_ImplementAttributeParser( parserGuiEnd )
	{
		delete reinterpret_cast< GuiParserContext * >( context.unregisterUserContext( ControlsManager::Name ) );
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserButton )
	{
		auto & guiContext = guiparse::getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( GUISection::eButton )

	CU_ImplementAttributeParser( parserButtonTheme )
	{
		castor::String themeName;
		params[0]->get( themeName );
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl( guiContext
			, parsingContext.scene
			, guiContext.controlName
			, themeName
			, guiContext.button );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonHAlign )
	{
		auto & guiContext = guiparse::getParserContext( context );

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
		auto & guiContext = guiparse::getParserContext( context );

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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.button )
		{
			castor::String text;
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
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::finishControl( guiparse::getControlsManager( context ), guiContext, guiContext.button );
		guiContext.pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserComboBox )
	{
		auto & guiContext = guiparse::getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( GUISection::eComboBox )

	CU_ImplementAttributeParser( parserComboBoxTheme )
	{
		castor::String themeName;
		params[0]->get( themeName );
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl( guiContext
			, parsingContext.scene
			, guiContext.controlName
			, themeName
			, guiContext.combo );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserComboBoxItem )
	{
		auto & guiContext = guiparse::getParserContext( context );
		ComboBoxCtrlSPtr combo = guiContext.combo;

		if ( combo )
		{
			castor::String text;
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
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::finishControl( guiparse::getControlsManager( context ), guiContext, guiContext.combo );
		guiContext.pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserEdit )
	{
		auto & guiContext = guiparse::getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( GUISection::eEdit )

	CU_ImplementAttributeParser( parserEditTheme )
	{
		castor::String themeName;
		params[0]->get( themeName );
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl( guiContext
			, parsingContext.scene
			, guiContext.controlName
			, themeName
			, guiContext.edit );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserEditCaption )
	{
		auto & guiContext = guiparse::getParserContext( context );
		EditCtrlSPtr edit = guiContext.edit;

		if ( edit )
		{
			castor::String text;
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
		auto & guiContext = guiparse::getParserContext( context );
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
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::finishControl( guiparse::getControlsManager( context ), guiContext, guiContext.edit );
		guiContext.pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserListBox )
	{
		auto & guiContext = guiparse::getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( GUISection::eListBox )

	CU_ImplementAttributeParser( parserListBoxTheme )
	{
		castor::String themeName;
		params[0]->get( themeName );
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl( guiContext
			, parsingContext.scene
			, guiContext.controlName
			, themeName
			, guiContext.listbox );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserListBoxItem )
	{
		auto & guiContext = guiparse::getParserContext( context );
		ListBoxCtrlSPtr listbox = guiContext.listbox;

		if ( listbox )
		{
			castor::String text;
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
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::finishControl( guiparse::getControlsManager( context ), guiContext, guiContext.listbox );
		guiContext.pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSlider )
	{
		auto & guiContext = guiparse::getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( GUISection::eSlider )

	CU_ImplementAttributeParser( parserSliderTheme )
	{
		castor::String themeName;
		params[0]->get( themeName );
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl( guiContext
			, parsingContext.scene
			, guiContext.controlName
			, themeName
			, guiContext.slider );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSliderEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::finishControl( guiparse::getControlsManager( context ), guiContext, guiContext.slider );
		guiContext.pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStatic )
	{
		auto & guiContext = guiparse::getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( GUISection::eStatic )

	CU_ImplementAttributeParser( parserStaticTheme )
	{
		castor::String themeName;
		params[0]->get( themeName );
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl( guiContext
			, parsingContext.scene
			, guiContext.controlName
			, themeName
			, guiContext.staticTxt );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStaticHAlign )
	{
		auto & guiContext = guiparse::getParserContext( context );

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
		auto & guiContext = guiparse::getParserContext( context );

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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.staticTxt )
		{
			castor::String text;
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
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::finishControl( guiparse::getControlsManager( context ), guiContext, guiContext.staticTxt );
		guiContext.pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserPanel )
	{
		auto & guiContext = guiparse::getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( GUISection::ePanel )

	CU_ImplementAttributeParser( parserPanelTheme )
	{
		castor::String themeName;
		params[0]->get( themeName );
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl( guiContext
			, parsingContext.scene
			, guiContext.controlName
			, themeName
			, guiContext.panel );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPanelEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::finishControl( guiparse::getControlsManager( context ), guiContext, guiContext.panel );
		guiContext.pop();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserControlPixelPosition )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.getTop() )
		{
			castor::Position position;
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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.getTop() )
		{
			castor::Size size;
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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.getTop() )
		{
			castor::Point4ui size;
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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.getTop() )
		{
			castor::Point4d value;
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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.getTop() )
		{
			castor::Point4d value;
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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.getTop() )
		{
			castor::Point4d value;
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
		auto & guiContext = guiparse::getParserContext( context );

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

	CU_ImplementAttributeParser( parserControlBoxLayout )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.getTop() )
		{
			guiContext.layout = castor::makeUniqueDerived< Layout, LayoutBox >( static_cast< LayoutControl & >( *control ) );
		}
		else
		{
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttributePush( GUISection::eBoxLayout )

	CU_ImplementAttributeParser( parserDefaultFont )
	{
		auto & guiContext = guiparse::getParserContext( context );
		auto & cache = guiContext.engine->getFontCache();
		castor::String name;
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
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.buttonStyle = guiContext.theme->createButtonStyle();
		guiContext.style = guiContext.buttonStyle;
	}
	CU_EndAttributePush( GUISection::eButtonStyle )

	CU_ImplementAttributeParser( parserStyleButtonEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.buttonStyle = nullptr;

		if ( guiContext.comboStyle )
		{
			guiContext.style = guiContext.comboStyle;
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserComboBoxStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.comboStyle = guiContext.theme->createComboBoxStyle();
		guiContext.style = guiContext.comboStyle;
	}
	CU_EndAttributePush( GUISection::eComboStyle )

	CU_ImplementAttributeParser( parserComboButtonStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.buttonStyle = &guiContext.comboStyle->getButtonStyle();
		guiContext.style = guiContext.buttonStyle;
	}
	CU_EndAttributePush( GUISection::eButtonStyle )

	CU_ImplementAttributeParser( parserComboListBoxStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.listboxStyle = &guiContext.comboStyle->getListBoxStyle();
		guiContext.style = guiContext.listboxStyle;
	}
	CU_EndAttributePush( GUISection::eListStyle )

	CU_ImplementAttributeParser( parserStyleComboBoxEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.comboStyle = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserEditStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.editStyle = guiContext.theme->createEditStyle();
		guiContext.style = guiContext.editStyle;
	}
	CU_EndAttributePush( GUISection::eEditStyle )

	CU_ImplementAttributeParser( parserStyleEditEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.editStyle = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserListBoxStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.listboxStyle = guiContext.theme->createListBoxStyle();
		guiContext.style = guiContext.listboxStyle;
	}
	CU_EndAttributePush( GUISection::eListStyle )

	CU_ImplementAttributeParser( parserItemStaticStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.staticStyle = &guiContext.listboxStyle->getItemStyle();
		guiContext.style = guiContext.staticStyle;
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserSelItemStaticStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.staticStyle = &guiContext.listboxStyle->getSelectedItemStyle();
		guiContext.style = guiContext.staticStyle;
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserHighItemStaticStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.staticStyle = &guiContext.listboxStyle->getHighlightedItemStyle();
		guiContext.style = guiContext.staticStyle;
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserStyleListBoxEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.listboxStyle = nullptr;

		if ( guiContext.comboStyle )
		{
			guiContext.style = guiContext.comboStyle;
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSliderStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.sliderStyle = guiContext.theme->createSliderStyle();
		guiContext.style = guiContext.sliderStyle;
	}
	CU_EndAttributePush( GUISection::eSliderStyle )

	CU_ImplementAttributeParser( parserStyleSliderEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.sliderStyle = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStaticStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.staticStyle = guiContext.theme->createStaticStyle();
		guiContext.style = guiContext.staticStyle;
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserPanelStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.panelStyle = guiContext.theme->createPanelStyle();
		guiContext.style = guiContext.panelStyle;
	}
	CU_EndAttributePush( GUISection::ePanelStyle )

	CU_ImplementAttributeParser( parserLineStaticStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.staticStyle = &guiContext.sliderStyle->getLineStyle();
		guiContext.style = guiContext.staticStyle;

		if ( guiContext.listboxStyle )
		{
			guiContext.style = guiContext.listboxStyle;
		}
		else if ( guiContext.sliderStyle )
		{
			guiContext.style = guiContext.sliderStyle;
		}
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserTickStaticStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.staticStyle = &guiContext.sliderStyle->getTickStyle();
		guiContext.style = guiContext.staticStyle;
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserStyleStaticEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.staticStyle = nullptr;

		if ( guiContext.listboxStyle )
		{
			guiContext.style = guiContext.listboxStyle;
		}
		else if ( guiContext.sliderStyle )
		{
			guiContext.style = guiContext.sliderStyle;
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStylePanelEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.panelStyle = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserThemeEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.theme = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStyleButtonFont )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			castor::String name;
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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

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

	CU_ImplementAttributeParser( parserStyleButtonDisabledBackgroundMaterial )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

			if ( material )
			{
				style->setDisabledBackgroundMaterial( material );
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

	CU_ImplementAttributeParser( parserStyleButtonDisabledForegroundMaterial )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

			if ( material )
			{
				style->setDisabledForegroundMaterial( material );
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

	CU_ImplementAttributeParser( parserStyleButtonDisabledTextMaterial )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.buttonStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

			if ( material )
			{
				style->setDisabledTextMaterial( material );
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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.editStyle )
		{
			castor::String name;
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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.editStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.staticStyle )
		{
			castor::String name;
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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.staticStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.style )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.style )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

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
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.style )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name ).lock().get();

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

	CU_ImplementAttributeParser( parserLayoutCtrl )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			params[0]->get( guiContext.controlName );
			guiContext.layoutCtrlFlags = {};
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttributePush( GUISection::eLayoutCtrl )

	CU_ImplementAttributeParser( parserLayoutEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			guiContext.layout->getContainer().setLayout( std::move( guiContext.layout ) );
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserBoxLayoutStaticSpacer )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			auto size = params[0]->get< uint32_t >();
			guiContext.layout->addSpacer( size );
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBoxLayoutDynamicSpacer )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			guiContext.layout->addSpacer( Spacer::Dynamic );
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBoxLayoutHorizontal )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			auto & box = static_cast< LayoutBox & >( *guiContext.layout );
			box.setHorizontal( params[0]->get< bool >() );
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLayoutCtrlHAlign )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			auto value = params[0]->get< uint32_t >();
			guiContext.layoutCtrlFlags.align( HAlign( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLayoutCtrlVAlign )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			auto value = params[0]->get< uint32_t >();
			guiContext.layoutCtrlFlags.align( VAlign( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLayoutCtrlStretch )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			auto value = params[0]->get< bool >();
			guiContext.layoutCtrlFlags.stretch( value );
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLayoutCtrlReserveIfHidden )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			auto value = params[0]->get< bool >();
			guiContext.layoutCtrlFlags.reserveSpaceIfHidden( value );
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLayoutCtrlEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			if ( auto parent = guiContext.getTop() )
			{
				guiContext.controlName = parent->getName() + "/" + guiContext.controlName;
			}

			if ( auto control = guiparse::getControlsManager( guiContext ).findControl( guiContext.controlName ) )
			{
				guiContext.layout->addControl( *control
					, std::move( guiContext.layoutCtrlFlags ) );
			}
			else
			{
				CU_ParsingError( cuT( "Control [" ) + guiContext.controlName + cuT( "] was not found." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttributePop()
}
