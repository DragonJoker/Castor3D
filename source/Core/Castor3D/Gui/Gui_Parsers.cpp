#include "Castor3D/Gui/Gui_Parsers.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlButton.hpp"
#include "Castor3D/Gui/Controls/CtrlComboBox.hpp"
#include "Castor3D/Gui/Controls/CtrlEdit.hpp"
#include "Castor3D/Gui/Controls/CtrlExpandablePanel.hpp"
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

		template< typename StyleT, typename ControlT >
		std::shared_ptr< ControlT > createControl( castor::FileParserContext & context
			, GuiParserContext & guiContext
			, SceneRPtr scene
			, castor::String const & controlName
			, castor::String const & styleName
			, std::shared_ptr< ControlT > & control )
		{
			auto style = guiparse::getControlsManager( guiContext ).template getStyle< StyleT >( styleName );

			if ( style == nullptr )
			{
				CU_ParsingError( "Style [" + styleName + "] was not found" );
				return nullptr;
			}

			control = std::make_shared< ControlT >( scene
				, controlName
				, style
				, guiContext.getTopControl() );
			guiContext.parents.push( control );
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

	ControlRPtr GuiParserContext::getTopControl()const
	{
		ControlRPtr result{};

		if ( !parents.empty() )
		{
			result = parents.top().get();
		}

		return result;
	}

	void GuiParserContext::popControl()
	{
		button.reset();
		edit.reset();
		listbox.reset();
		slider.reset();
		staticTxt.reset();
		combo.reset();
		panel.reset();
		expandablePanel.reset();

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
			case ControlType::eExpandablePanel:
				expandablePanel = std::static_pointer_cast< ExpandablePanelCtrl >( top );
				break;
			default:
				CU_Failure( "Unsupported Control Type" );
				break;
			}
		}
	}

	ControlStyleRPtr GuiParserContext::getTopStyle()const
	{
		ControlStyleRPtr result{};

		if ( !styles.empty() )
		{
			result = styles.top();
		}

		return result;
	}

	void GuiParserContext::popStyle()
	{
		buttonStyle = {};
		comboStyle = {};
		editStyle = {};
		listboxStyle = {};
		sliderStyle = {};
		staticStyle = {};
		panelStyle = {};
		expandablePanelStyle = {};
		styles.pop();

		if ( !styles.empty() )
		{
			auto top = styles.top();

			switch ( top->getType() )
			{
			case ControlType::eStatic:
				staticStyle = &static_cast< StaticStyle & >( *top );
				break;
			case ControlType::eEdit:
				editStyle = &static_cast< EditStyle & >( *top );
				break;
			case ControlType::eSlider:
				sliderStyle = &static_cast< SliderStyle & >( *top );
				break;
			case ControlType::eComboBox:
				comboStyle = &static_cast< ComboBoxStyle & >( *top );
				break;
			case ControlType::eListBox:
				listboxStyle = &static_cast< ListBoxStyle & >( *top );
				break;
			case ControlType::eButton:
				buttonStyle = &static_cast< ButtonStyle & >( *top );
				break;
			case ControlType::ePanel:
				panelStyle = &static_cast< PanelStyle & >( *top );
				break;
			case ControlType::eExpandablePanel:
				expandablePanelStyle = &static_cast< ExpandablePanelStyle & >( *top );
				break;
			default:
				CU_Failure( "Unsupported Style Type" );
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

	CU_ImplementAttributeParser( parserButton )
	{
		auto & guiContext = guiparse::getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( GUISection::eButton )

	CU_ImplementAttributeParser( parserButtonTheme )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< ButtonStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name + "/Button"
			, guiContext.button );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserButtonStyle )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< ButtonStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name
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
		guiContext.popControl();
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
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< ComboBoxStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name + "/ComboBox"
			, guiContext.combo );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserComboBoxStyle )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< ComboBoxStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name
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
		guiContext.popControl();
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
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< EditStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name + "/Edit"
			, guiContext.edit );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserEditStyle )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< EditStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name
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
			guiContext.edit->addFlag( EditFlag::eMultiline/*
				| EditFlag::eProcessEnter
				| EditFlag::eProcessTab*/ );
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
		guiContext.popControl();
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
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< ListBoxStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name + "/ListBox"
			, guiContext.listbox );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserListBoxStyle )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< ListBoxStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name
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
		guiContext.popControl();
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
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< SliderStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name + "/Slider"
			, guiContext.slider );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSliderStyle )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< SliderStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name
			, guiContext.slider );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSliderEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::finishControl( guiparse::getControlsManager( context ), guiContext, guiContext.slider );
		guiContext.popControl();
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
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< StaticStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name + "/Static"
			, guiContext.staticTxt );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStaticStyle )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< StaticStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name
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
		guiContext.popControl();
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
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< PanelStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name + "/Panel"
			, guiContext.panel );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPanelStyle )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< PanelStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name
			, guiContext.panel );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPanelEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::finishControl( guiparse::getControlsManager( context ), guiContext, guiContext.panel );
		guiContext.popControl();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserExpandablePanel )
	{
		auto & guiContext = guiparse::getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( GUISection::eExpandablePanel )

	CU_ImplementAttributeParser( parserExpandablePanelTheme )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< ExpandablePanelStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name + "/ExpandablePanel"
			, guiContext.expandablePanel );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserExpandablePanelStyle )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< ExpandablePanelStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name
			, guiContext.expandablePanel );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserExpandablePanelHeader )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.panel = guiContext.expandablePanel->getHeader();
		guiContext.parents.push( guiContext.panel );
	}
	CU_EndAttributePush( GUISection::eExpandablePanelHeader )

	CU_ImplementAttributeParser( parserExpandablePanelPanel )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.panel = guiContext.expandablePanel->getPanel();
		guiContext.parents.push( guiContext.panel );
	}
	CU_EndAttributePush( GUISection::eExpandablePanelPanel )

	CU_ImplementAttributeParser( parserExpandablePanelEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::finishControl( guiparse::getControlsManager( context ), guiContext, guiContext.expandablePanel );
		guiContext.popControl();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserExpandablePanelHeaderEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.parents.pop();
		guiContext.popControl();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserExpandablePanelPanelEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.parents.pop();
		guiContext.popControl();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserControlPixelPosition )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.getTopControl() )
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

		if ( auto control = guiContext.getTopControl() )
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

		if ( auto control = guiContext.getTopControl() )
		{
			castor::Point4ui size;
			params[0]->get( size );
			control->setBackgroundBorderSize( size );
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

		if ( auto control = guiContext.getTopControl() )
		{
			castor::Point4d value;
			params[0]->get( value );
			control->setBackgroundBorderInnerUV( value );
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

		if ( auto control = guiContext.getTopControl() )
		{
			castor::Point4d value;
			params[0]->get( value );
			control->setBackgroundBorderOuterUV( value );
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

		if ( auto control = guiContext.getTopControl() )
		{
			castor::Point4d value;
			params[0]->get( value );
			control->setBackgroundUV( value );
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

		if ( auto control = guiContext.getTopControl() )
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

		if ( auto control = guiContext.getTopControl() )
		{
			guiContext.layout = castor::makeUniqueDerived< Layout, LayoutBox >( static_cast< LayoutControl & >( *control ) );
		}
		else
		{
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttributePush( GUISection::eBoxLayout )

	CU_ImplementAttributeParser( parserControlMovable )
	{
		auto & guiContext = guiparse::getParserContext( context );
		auto enable = params[0]->get< bool >();

		if ( auto control = guiContext.getTopControl() )
		{
			if ( enable )
			{
				control->addFlag( ControlFlag::eDraggable );
			}
			else
			{
				control->removeFlag( ControlFlag::eDraggable );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttributePush( GUISection::eBoxLayout )

	CU_ImplementAttributeParser( parserThemeDefaultFont )
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

	CU_ImplementAttributeParser( parserThemeButtonStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.theme->createButtonStyle( params[0]->get< castor::String >() )
			, guiContext.buttonStyle );
	}
	CU_EndAttributePush( GUISection::eButtonStyle )

	CU_ImplementAttributeParser( parserThemeComboBoxStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.theme->createComboBoxStyle( params[0]->get< castor::String >() )
			, guiContext.comboStyle );
	}
	CU_EndAttributePush( GUISection::eComboStyle )

	CU_ImplementAttributeParser( parserThemeEditStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.theme->createEditStyle( params[0]->get< castor::String >() )
			, guiContext.editStyle );
	}
	CU_EndAttributePush( GUISection::eEditStyle )

	CU_ImplementAttributeParser( parserThemeListBoxStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.theme->createListBoxStyle( params[0]->get< castor::String >() )
			, guiContext.listboxStyle );
	}
	CU_EndAttributePush( GUISection::eListStyle )

	CU_ImplementAttributeParser( parserThemeSliderStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.theme->createSliderStyle( params[0]->get< castor::String >() )
			, guiContext.sliderStyle );
	}
	CU_EndAttributePush( GUISection::eSliderStyle )

	CU_ImplementAttributeParser( parserThemeStaticStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.theme->createStaticStyle( params[0]->get< castor::String >() )
			, guiContext.staticStyle );
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserThemePanelStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.theme->createPanelStyle( params[0]->get< castor::String >() )
			, guiContext.panelStyle );
	}
	CU_EndAttributePush( GUISection::ePanelStyle )

	CU_ImplementAttributeParser( parserThemeExpandablePanelStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.theme->createExpandablePanelStyle( params[0]->get< castor::String >() )
			, guiContext.expandablePanelStyle );
	}
	CU_EndAttributePush( GUISection::eExpandablePanelStyle )

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

	CU_ImplementAttributeParser( parserStyleButtonEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.popStyle();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStyleComboButton )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.comboStyle->getButtonStyle()
			, guiContext.buttonStyle );
	}
	CU_EndAttributePush( GUISection::eButtonStyle )

	CU_ImplementAttributeParser( parserStyleComboListBox )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.comboStyle->getListBoxStyle()
			, guiContext.listboxStyle );
	}
	CU_EndAttributePush( GUISection::eListStyle )

	CU_ImplementAttributeParser( parserStyleComboBoxEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.popStyle();
	}
	CU_EndAttributePop()

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

	CU_ImplementAttributeParser( parserStyleEditEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.popStyle();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStyleListBoxItemStatic )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.listboxStyle->getItemStyle()
			, guiContext.staticStyle );
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserStyleListBoxSelItemStatic )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.listboxStyle->getSelectedItemStyle()
			, guiContext.staticStyle );
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserStyleListBoxHighItemStatic )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.listboxStyle->getHighlightedItemStyle()
			, guiContext.staticStyle );
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserStyleListBoxEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.popStyle();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStyleSliderLineStatic )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.sliderStyle->getLineStyle()
			, guiContext.staticStyle );
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserStyleSliderTickStatic )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.sliderStyle->getTickStyle()
			, guiContext.staticStyle );
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserStyleSliderEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.popStyle();
	}
	CU_EndAttributePop()

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

	CU_ImplementAttributeParser( parserStyleStaticEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.popStyle();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStylePanelEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.popStyle();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStyleExpandablePanelHeader )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.expandablePanelStyle->getHeaderStyle()
			, guiContext.panelStyle );
	}
	CU_EndAttributePush( GUISection::ePanelStyle )

	CU_ImplementAttributeParser( parserStyleExpandablePanelExpand )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.expandablePanelStyle->getExpandStyle()
			, guiContext.buttonStyle );
	}
	CU_EndAttributePush( GUISection::eButtonStyle )

	CU_ImplementAttributeParser( parserStyleExpandablePanelPanel )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.expandablePanelStyle->getPanelStyle()
			, guiContext.panelStyle );
	}
	CU_EndAttributePush( GUISection::ePanelStyle )

	CU_ImplementAttributeParser( parserStyleExpandablePanelEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.popStyle();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserStyleBackgroundMaterial )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.getTopStyle() )
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

		if ( auto style = guiContext.getTopStyle() )
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

		if ( auto style = guiContext.getTopStyle() )
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

	CU_ImplementAttributeParser( parserStyleButtonStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		auto name = params[0]->get< castor::String >();

		if ( auto style = guiContext.getTopStyle() )
		{
			name = style->getName() + "/" + name;
		}

		guiContext.pushStyle( guiparse::getControlsManager( guiContext ).createButtonStyle( name )
			, guiContext.buttonStyle );
	}
	CU_EndAttributePush( GUISection::eButtonStyle )

	CU_ImplementAttributeParser( parserStyleComboBoxStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		auto name = params[0]->get< castor::String >();

		if ( auto style = guiContext.getTopStyle() )
		{
			name = style->getName() + "/" + name;
		}

		guiContext.pushStyle( guiparse::getControlsManager( guiContext ).createComboBoxStyle( name )
			, guiContext.comboStyle );
	}
	CU_EndAttributePush( GUISection::eComboStyle )

	CU_ImplementAttributeParser( parserStyleEditStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		auto name = params[0]->get< castor::String >();

		if ( auto style = guiContext.getTopStyle() )
		{
			name = style->getName() + "/" + name;
		}

		guiContext.pushStyle( guiparse::getControlsManager( guiContext ).createEditStyle( name )
			, guiContext.editStyle );
	}
	CU_EndAttributePush( GUISection::eEditStyle )

	CU_ImplementAttributeParser( parserStyleListBoxStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		auto name = params[0]->get< castor::String >();

		if ( auto style = guiContext.getTopStyle() )
		{
			name = style->getName() + "/" + name;
		}

		guiContext.pushStyle( guiparse::getControlsManager( guiContext ).createListBoxStyle( name )
			, guiContext.listboxStyle );
	}
	CU_EndAttributePush( GUISection::eListStyle )

	CU_ImplementAttributeParser( parserStyleSliderStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		auto name = params[0]->get< castor::String >();

		if ( auto style = guiContext.getTopStyle() )
		{
			name = style->getName() + "/" + name;
		}

		guiContext.pushStyle( guiparse::getControlsManager( guiContext ).createSliderStyle( name )
			, guiContext.sliderStyle );
	}
	CU_EndAttributePush( GUISection::eSliderStyle )

	CU_ImplementAttributeParser( parserStyleStaticStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		auto name = params[0]->get< castor::String >();

		if ( auto style = guiContext.getTopStyle() )
		{
			name = style->getName() + "/" + name;
		}

		guiContext.pushStyle( guiparse::getControlsManager( guiContext ).createStaticStyle( name )
			, guiContext.staticStyle );
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserStylePanelStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		auto name = params[0]->get< castor::String >();

		if ( auto style = guiContext.getTopStyle() )
		{
			name = style->getName() + "/" + name;
		}

		guiContext.pushStyle( guiparse::getControlsManager( guiContext ).createPanelStyle( name )
			, guiContext.panelStyle );
	}
	CU_EndAttributePush( GUISection::ePanelStyle )

	CU_ImplementAttributeParser( parserStyleExpandablePanelStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		auto name = params[0]->get< castor::String >();

		if ( auto style = guiContext.getTopStyle() )
		{
			name = style->getName() + "/" + name;
		}

		guiContext.pushStyle( guiparse::getControlsManager( guiContext ).createExpandablePanelStyle( name )
			, guiContext.expandablePanelStyle );
	}
	CU_EndAttributePush( GUISection::eExpandablePanelStyle )

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

	CU_ImplementAttributeParser( parserLayoutCtrlPadding )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			guiContext.layoutCtrlFlags.padding( params[0]->get< castor::Point4ui >() );
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLayoutCtrlPadLeft )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			guiContext.layoutCtrlFlags.padLeft( params[0]->get< uint32_t >() );
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLayoutCtrlPadTop )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			guiContext.layoutCtrlFlags.padTop( params[0]->get< uint32_t >() );
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLayoutCtrlPadRight )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			guiContext.layoutCtrlFlags.padRight( params[0]->get< uint32_t >() );
		}
		else
		{
			CU_ParsingError( cuT( "No layout initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLayoutCtrlPadBottom )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( guiContext.layout )
		{
			guiContext.layoutCtrlFlags.padBottom( params[0]->get< uint32_t >() );
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
			if ( auto parent = guiContext.getTopControl() )
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
