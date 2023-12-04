#include "Castor3D/Gui/Gui_Parsers.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlButton.hpp"
#include "Castor3D/Gui/Controls/CtrlComboBox.hpp"
#include "Castor3D/Gui/Controls/CtrlEdit.hpp"
#include "Castor3D/Gui/Controls/CtrlExpandablePanel.hpp"
#include "Castor3D/Gui/Controls/CtrlFrame.hpp"
#include "Castor3D/Gui/Controls/CtrlListBox.hpp"
#include "Castor3D/Gui/Controls/CtrlPanel.hpp"
#include "Castor3D/Gui/Controls/CtrlProgress.hpp"
#include "Castor3D/Gui/Controls/CtrlSlider.hpp"
#include "Castor3D/Gui/Controls/CtrlStatic.hpp"
#include "Castor3D/Gui/Theme/StylesHolder.hpp"
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
		ControlT * createControl( castor::FileParserContext & context
			, GuiParserContext & guiContext
			, SceneRPtr scene
			, castor::String const & controlName
			, castor::String const & styleName
			, ControlT *& control )
		{
			auto & manager = guiparse::getControlsManager( guiContext );
			auto style = manager.template getStyle< StyleT >( styleName );

			if ( style == nullptr )
			{
				CU_ParsingError( "Style [" + styleName + "] was not found" );
				return nullptr;
			}

			control = manager.registerControlT( castor::makeUnique< ControlT >( scene
				, controlName
				, style
				, guiContext.getTopControl() ) );
			guiContext.parents.push( control );
			return control;
		}

		template< typename ControlT >
		void finishControl( ControlsManager & manager
			, GuiParserContext & context
			, ControlT * control )
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
			result = parents.top();
		}

		return result;
	}

	void GuiParserContext::popControl()
	{
		button = nullptr;
		edit = nullptr;
		listbox = nullptr;
		slider = nullptr;
		staticTxt = nullptr;
		combo = nullptr;
		panel = nullptr;
		progress = nullptr;
		expandablePanel = nullptr;
		scrollable = nullptr;

		if ( !parents.empty() )
		{
			ControlRPtr top;
			top = parents.top();

			switch ( top->getType() )
			{
			case ControlType::eStatic:
				staticTxt = &static_cast< StaticCtrl & >( *top );
				break;
			case ControlType::eEdit:
				edit = &static_cast< EditCtrl & >( *top );
				scrollable = edit;
				break;
			case ControlType::eSlider:
				slider = &static_cast< SliderCtrl & >( *top );
				break;
			case ControlType::eComboBox:
				combo = &static_cast< ComboBoxCtrl & >( *top );
				break;
			case ControlType::eListBox:
				listbox = &static_cast< ListBoxCtrl & >( *top );
				break;
			case ControlType::eButton:
				button = &static_cast< ButtonCtrl & >( *top );
				break;
			case ControlType::ePanel:
				panel = &static_cast< PanelCtrl & >( *top );
				scrollable = panel;
				break;
			case ControlType::eProgress:
				progress = &static_cast< ProgressCtrl & >( *top );
				break;
			case ControlType::eExpandablePanel:
				expandablePanel = &static_cast< ExpandablePanelCtrl & >( *top );
				break;
			case ControlType::eFrame:
				frame = &static_cast< FrameCtrl & >( *top );
				break;
			default:
				CU_Failure( "Unsupported Control Type" );
				break;
			}
		}
	}

	void GuiParserContext::popStylesHolder( StylesHolder const * style )
	{
		if ( style == stylesHolder.top() )
		{
			stylesHolder.pop();
		}
	}

	void GuiParserContext::pushStylesHolder( StylesHolder * style )
	{
		style->setDefaultFont( stylesHolder.top()->getDefaultFont() );
		stylesHolder.push( style );
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
		progressStyle = {};
		expandablePanelStyle = {};
		scrollableStyle = {};

		if ( !styles.empty()
			&& !stylesHolder.empty()
			&& isStylesHolder( *styles.top() ) )
		{
			popStylesHolder( &static_cast< PanelStyle const & >( *styles.top() ) );
		}

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
				scrollableStyle = editStyle;
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
				scrollableStyle = panelStyle;
				break;
			case ControlType::eProgress:
				progressStyle = &static_cast< ProgressStyle & >( *top );
				break;
			case ControlType::eExpandablePanel:
				expandablePanelStyle = &static_cast< ExpandablePanelStyle & >( *top );
				break;
			case ControlType::eFrame:
				frameStyle = &static_cast< FrameStyle & >( *top );
				break;
			case ControlType::eScrollBar:
				scrollBarStyle = &static_cast< ScrollBarStyle & >( *top );
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
		guiContext.theme = ctrlsManager.createTheme( name
			, getSceneParserContext( context ).scene );
		guiContext.pushStylesHolder( guiContext.theme );
	}
	CU_EndAttributePush( GUISection::eTheme )

	CU_ImplementAttributeParser( parserGlobalBoxLayout )
	{
		ControlsManager & ctrlsManager = guiparse::getControlsManager( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.layout = castor::makeUniqueDerived< Layout, LayoutBox >( ctrlsManager );
	}
	CU_EndAttributePush( GUISection::eBoxLayout )

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
			control->setCaption( castor::string::toU32String( text ) );
		}
		else
		{
			CU_ParsingError( cuT( "No button control initialised." ) );
		}
	}
	CU_EndAttribute()

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

		if ( auto combo = guiContext.combo )
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
		guiContext.scrollable = guiContext.edit;
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
		guiContext.scrollable = guiContext.edit;
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserEditCaption )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto edit = guiContext.edit )
		{
			auto text = params[0]->get< castor::String >();
			castor::string::replace( text, "\\?", "\?" );
			castor::string::replace( text, "\\\\", "\\" );
			castor::string::replace( text, "\\\"", "\"" );
			castor::string::replace( text, "\\a", "\a" );
			castor::string::replace( text, "\\b", "\b" );
			castor::string::replace( text, "\\f", "\f" );
			castor::string::replace( text, "\\n", "\n" );
			castor::string::replace( text, "\\r", "\r" );
			castor::string::replace( text, "\\t", "\t" );
			castor::string::replace( text, "\\v", "\v" );
			edit->setCaption( castor::string::toU32String( text ) );
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

		if ( auto edit = guiContext.edit )
		{
			bool value;
			params[0]->get( value );
			edit->addFlag( EditFlag::eMultiline/*
				| EditFlag::eProcessEnter
				| EditFlag::eProcessTab*/ );
		}
		else
		{
			CU_ParsingError( cuT( "No edit control initialised." ) );
		}
	}
	CU_EndAttribute()

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

		if ( auto listbox = guiContext.listbox )
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
			control->setCaption( castor::string::toU32String( text ) );
		}
		else
		{
			CU_ParsingError( cuT( "No static control initialised." ) );
		}
	}
	CU_EndAttribute()

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
		guiContext.scrollable = guiContext.panel;
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
		guiContext.scrollable = guiContext.panel;
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserProgress )
	{
		auto & guiContext = guiparse::getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( GUISection::eProgress )

	CU_ImplementAttributeParser( parserProgressTheme )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< ProgressStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name + "/Progress"
			, guiContext.progress );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserProgressStyle )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< ProgressStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name
			, guiContext.progress );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserProgressContainerBorderSize )
	{
		if ( auto progress = guiparse::getParserContext( context ).progress )
		{
			progress->setContainerBorderSize( params[0]->get< castor::Point4ui >() );
		}
		else
		{
			CU_ParsingError( cuT( "No progress control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserProgressBarBorderSize )
	{
		if ( auto progress = guiparse::getParserContext( context ).progress )
		{
			progress->setBarBorderSize( params[0]->get< castor::Point4ui >() );
		}
		else
		{
			CU_ParsingError( cuT( "No progress control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserProgressLeftToRight )
	{
		if ( auto progress = guiparse::getParserContext( context ).progress )
		{
			progress->setLeftToRight();
		}
		else
		{
			CU_ParsingError( cuT( "No progress control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserProgressRightToLeft )
	{
		if ( auto progress = guiparse::getParserContext( context ).progress )
		{
			progress->setRightToLeft();
		}
		else
		{
			CU_ParsingError( cuT( "No progress control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserProgressTopToBottom )
	{
		if ( auto progress = guiparse::getParserContext( context ).progress )
		{
			progress->setTopToBottom();
		}
		else
		{
			CU_ParsingError( cuT( "No progress control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserProgressBottomToTop )
	{
		if ( auto progress = guiparse::getParserContext( context ).progress )
		{
			progress->setBottomToTop();
		}
		else
		{
			CU_ParsingError( cuT( "No progress control initialised." ) );
		}
	}
	CU_EndAttribute()

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

	CU_ImplementAttributeParser( parserExpandablePanelExpand )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.button = guiContext.expandablePanel->getExpand();
		guiContext.parents.push( guiContext.button );
	}
	CU_EndAttributePush( GUISection::eExpandablePanelExpand )

	CU_ImplementAttributeParser( parserExpandablePanelContent )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.panel = guiContext.expandablePanel->getContent();
		guiContext.scrollable = guiContext.panel;
		guiContext.parents.push( guiContext.panel );
	}
	CU_EndAttributePush( GUISection::eExpandablePanelContent )

	CU_ImplementAttributeParser( parserExpandablePanelHeaderEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.parents.pop();
		guiContext.popControl();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserExpandablePanelExpandCaption )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.expandablePanel->setExpandCaption( castor::string::toU32String( params[0]->get< castor::String >() ) );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserExpandablePanelRetractCaption )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.expandablePanel->setRetractCaption( castor::string::toU32String( params[0]->get< castor::String >() ) );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserExpandablePanelExpandEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.parents.pop();
		guiContext.popControl();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserExpandablePanelContentEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.parents.pop();
		guiContext.popControl();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserFrame )
	{
		auto & guiContext = guiparse::getParserContext( context );
		params[0]->get( guiContext.controlName );
	}
	CU_EndAttributePush( GUISection::eFrame )

	CU_ImplementAttributeParser( parserFrameTheme )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< FrameStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name + "/Frame"
			, guiContext.frame );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFrameStyle )
	{
		auto name = params[0]->get< castor::String >();
		auto & parsingContext = getParserContext( context );
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::createControl< FrameStyle >( context
			, guiContext
			, parsingContext.scene
			, guiContext.controlName
			, name
			, guiContext.frame );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFrameHeaderHAlign )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.frame )
		{
			control->setHeaderHAlign( HAlign( params[0]->get< uint32_t >() ) );
		}
		else
		{
			CU_ParsingError( cuT( "No frame control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFrameHeaderVAlign )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.frame )
		{
			control->setHeaderVAlign( VAlign( params[0]->get< uint32_t >() ) );
		}
		else
		{
			CU_ParsingError( cuT( "No frame control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFrameHeaderCaption )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.frame )
		{
			control->setCaption( castor::string::toU32String( params[0]->get< castor::String >() ) );
		}
		else
		{
			CU_ParsingError( cuT( "No frame control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFrameMinSize )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto control = guiContext.frame )
		{
			control->setMinSize( params[0]->get< castor::Size >() );
		}
		else
		{
			CU_ParsingError( cuT( "No frame control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFrameContent )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.panel = guiContext.frame->getContent();
		guiContext.scrollable = guiContext.panel;
		guiContext.parents.push( guiContext.panel );
	}
	CU_EndAttributePush( GUISection::eFrameContent )

	CU_ImplementAttributeParser( parserFrameContentEnd )
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
			control->setBorderSize( size );
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
			control->setBorderInnerUV( value );
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
			control->setBorderOuterUV( value );
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
			control->setUV( value );
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
				control->addFlag( ControlFlag::eMovable );
			}
			else
			{
				control->removeFlag( ControlFlag::eMovable );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserControlResizable )
	{
		auto & guiContext = guiparse::getParserContext( context );
		auto enable = params[0]->get< bool >();

		if ( auto control = guiContext.getTopControl() )
		{
			if ( enable )
			{
				control->addFlag( ControlFlag::eResizable );
			}
			else
			{
				control->removeFlag( ControlFlag::eResizable );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No control initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserControlEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiparse::finishControl( guiparse::getControlsManager( context ), guiContext, guiContext.getTopControl() );
		guiContext.popControl();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserScrollableVerticalScroll )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( params[0]->get< bool >() )
		{
			if ( guiContext.edit )
			{
				guiContext.edit->addFlag( ScrollBarFlag::eVertical );
			}
			else if ( guiContext.panel )
			{
				guiContext.panel->addFlag( ScrollBarFlag::eVertical );
			}
		}
		else
		{
			if ( guiContext.edit )
			{
				guiContext.edit->removeFlag( ScrollBarFlag::eVertical );
			}
			else if ( guiContext.panel )
			{
				guiContext.panel->removeFlag( ScrollBarFlag::eVertical );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserScrollableHorizontalScroll )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( params[0]->get< bool >() )
		{
			if ( guiContext.edit )
			{
				guiContext.edit->addFlag( ScrollBarFlag::eHorizontal );
			}
			else if ( guiContext.panel )
			{
				guiContext.panel->addFlag( ScrollBarFlag::eHorizontal );
			}
		}
		else
		{
			if ( guiContext.edit )
			{
				guiContext.edit->removeFlag( ScrollBarFlag::eHorizontal );
			}
			else if ( guiContext.panel )
			{
				guiContext.panel->removeFlag( ScrollBarFlag::eHorizontal );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserScrollableVerticalScrollBarStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( !guiContext.scrollable )
		{
			CU_ParsingError( "No ScrollableCtrl initialised" );
		}
		else
		{
			auto styleName = params[0]->get< castor::String >();
			auto style = guiparse::getControlsManager( guiContext ).getScrollBarStyle( styleName );

			if ( style == nullptr )
			{
				CU_ParsingError( "Style [" + styleName + "] was not found" );
			}
			else
			{
				guiContext.scrollable->getStyle().setVerticalStyle( *style );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserScrollableHorizontalScrollBarStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( !guiContext.scrollable )
		{
			CU_ParsingError( "No ScrollableCtrl initialised" );
		}
		else
		{
			auto styleName = params[0]->get< castor::String >();
			auto style = guiparse::getControlsManager( guiContext ).getScrollBarStyle( styleName );

			if ( style == nullptr )
			{
				CU_ParsingError( "Style [" + styleName + "] was not found" );
			}
			else
			{
				guiContext.scrollable->getStyle().setHorizontalStyle( *style );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserThemeEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.popStylesHolder( guiContext.theme );
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
			auto material = guiContext.engine->findMaterial( name );

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
			auto material = guiContext.engine->findMaterial( name );

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
			auto material = guiContext.engine->findMaterial( name );

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
			auto material = guiContext.engine->findMaterial( name );

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
			auto material = guiContext.engine->findMaterial( name );

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
			auto material = guiContext.engine->findMaterial( name );

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
			auto material = guiContext.engine->findMaterial( name );

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
			auto material = guiContext.engine->findMaterial( name );

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
			auto material = guiContext.engine->findMaterial( name );

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
			auto material = guiContext.engine->findMaterial( name );

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

	CU_ImplementAttributeParser( parserStyleComboButton )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.comboStyle->getExpandStyle()
			, guiContext.buttonStyle );
	}
	CU_EndAttributePush( GUISection::eButtonStyle )

	CU_ImplementAttributeParser( parserStyleComboListBox )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.comboStyle->getElementsStyle()
			, guiContext.listboxStyle );
	}
	CU_EndAttributePush( GUISection::eListStyle )

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
			auto material = guiContext.engine->findMaterial( name );

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

	CU_ImplementAttributeParser( parserStyleEditSelectionMaterial )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.editStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name );

			if ( material )
			{
				style->setSelectionMaterial( material );
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
			auto material = guiContext.engine->findMaterial( name );

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

	CU_ImplementAttributeParser( parserStyleProgressTitleFont )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.progressStyle )
		{
			style->setTitleFontName( params[0]->get< castor::String >() );
		}
		else
		{
			CU_ParsingError( cuT( "No progress style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleProgressTitleMaterial )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.progressStyle )
		{
			auto name = params[0]->get< castor::String >();
			auto material = guiContext.engine->findMaterial( name );

			if ( material )
			{
				style->setTitleMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material [" + name + "] not found." ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No progress style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleProgressContainer )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.progressStyle )
		{
			guiContext.pushStyle( &style->getContainerStyle()
				, guiContext.panelStyle );
		}
		else
		{
			CU_ParsingError( cuT( "No progress style initialised." ) );
		}
	}
	CU_EndAttributePush( GUISection::ePanelStyle )

	CU_ImplementAttributeParser( parserStyleProgressProgress )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.progressStyle )
		{
			guiContext.pushStyle( &style->getProgressStyle()
				, guiContext.panelStyle );
		}
		else
		{
			CU_ParsingError( cuT( "No progress style initialised." ) );
		}
	}
	CU_EndAttributePush( GUISection::ePanelStyle )

	CU_ImplementAttributeParser( parserStyleProgressTextFont )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.progressStyle )
		{
			style->setTextFontName( params[0]->get< castor::String >() );
		}
		else
		{
			CU_ParsingError( cuT( "No progress style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleProgressTextMaterial )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.progressStyle )
		{
			auto name = params[0]->get< castor::String >();
			auto material = guiContext.engine->findMaterial( name );

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
			CU_ParsingError( cuT( "No progress style initialised." ) );
		}
	}
	CU_EndAttribute()

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

	CU_ImplementAttributeParser( parserStyleExpandablePanelContent )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.expandablePanelStyle->getContentStyle()
			, guiContext.panelStyle );
	}
	CU_EndAttributePush( GUISection::ePanelStyle )

	CU_ImplementAttributeParser( parserStyleFrameHeaderFont )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.frameStyle )
		{
			castor::String name;
			params[0]->get( name );
			style->setHeaderFont( name );
		}
		else
		{
			CU_ParsingError( cuT( "No static style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleFrameHeaderTextMaterial )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.frameStyle )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name );

			if ( material )
			{
				style->setHeaderTextMaterial( material );
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

	CU_ImplementAttributeParser( parserStyleScrollBarBeginButton )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.scrollBarStyle->getBeginStyle()
			, guiContext.buttonStyle );
	}
	CU_EndAttributePush( GUISection::eButtonStyle )

	CU_ImplementAttributeParser( parserStyleScrollBarEndButton )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.scrollBarStyle->getEndStyle()
			, guiContext.buttonStyle );
	}
	CU_EndAttributePush( GUISection::eButtonStyle )

	CU_ImplementAttributeParser( parserStyleScrollBarBar )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.scrollBarStyle->getBarStyle()
			, guiContext.panelStyle );
	}
	CU_EndAttributePush( GUISection::ePanelStyle )

	CU_ImplementAttributeParser( parserStyleScrollBarThumb )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( &guiContext.scrollBarStyle->getThumbStyle()
			, guiContext.panelStyle );
	}
	CU_EndAttributePush( GUISection::ePanelStyle )

	CU_ImplementAttributeParser( parserStyleBackgroundMaterial )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.getTopStyle() )
		{
			castor::String name;
			params[0]->get( name );
			auto material = guiContext.engine->findMaterial( name );

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
			auto material = guiContext.engine->findMaterial( name );

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

	CU_ImplementAttributeParser( parserStyleBackgroundInvisible )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.getTopStyle() )
		{
			style->setBackgroundInvisible( params[0]->get< bool >() );
		}
		else
		{
			CU_ParsingError( cuT( "No style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleForegroundInvisible )
	{
		auto & guiContext = guiparse::getParserContext( context );

		if ( auto style = guiContext.getTopStyle() )
		{
			style->setForegroundInvisible( params[0]->get< bool >() );
		}
		else
		{
			CU_ParsingError( cuT( "No style initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleDefaultFont )
	{
		auto & guiContext = guiparse::getParserContext( context );
		auto & cache = guiContext.engine->getFontCache();
		castor::String name;
		params[0]->get( name );
		auto font = cache.find( name );

		if ( font )
		{
			guiContext.stylesHolder.top()->setDefaultFont( font );
		}
		else
		{
			CU_ParsingError( cuT( "Unknown font: " ) + name );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserStyleButtonStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.stylesHolder.top()->createButtonStyle( params[0]->get< castor::String >()
				, getSceneParserContext( context ).scene )
			, guiContext.buttonStyle );
	}
	CU_EndAttributePush( GUISection::eButtonStyle )

	CU_ImplementAttributeParser( parserStyleComboBoxStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.stylesHolder.top()->createComboBoxStyle( params[0]->get< castor::String >()
				, getSceneParserContext( context ).scene )
			, guiContext.comboStyle );
	}
	CU_EndAttributePush( GUISection::eComboStyle )

	CU_ImplementAttributeParser( parserStyleEditStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.stylesHolder.top()->createEditStyle( params[0]->get< castor::String >()
				, getSceneParserContext( context ).scene )
			, guiContext.editStyle );
	}
	CU_EndAttributePush( GUISection::eEditStyle )

	CU_ImplementAttributeParser( parserStyleListBoxStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.stylesHolder.top()->createListBoxStyle( params[0]->get< castor::String >()
				, getSceneParserContext( context ).scene )
			, guiContext.listboxStyle );
	}
	CU_EndAttributePush( GUISection::eListStyle )

	CU_ImplementAttributeParser( parserStyleSliderStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.stylesHolder.top()->createSliderStyle( params[0]->get< castor::String >()
				, getSceneParserContext( context ).scene )
			, guiContext.sliderStyle );
	}
	CU_EndAttributePush( GUISection::eSliderStyle )

	CU_ImplementAttributeParser( parserStyleStaticStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.stylesHolder.top()->createStaticStyle( params[0]->get< castor::String >()
				, getSceneParserContext( context ).scene )
			, guiContext.staticStyle );
	}
	CU_EndAttributePush( GUISection::eStaticStyle )

	CU_ImplementAttributeParser( parserStylePanelStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.stylesHolder.top()->createPanelStyle( params[0]->get< castor::String >()
				, getSceneParserContext( context ).scene )
			, guiContext.panelStyle );
	}
	CU_EndAttributePush( GUISection::ePanelStyle )

	CU_ImplementAttributeParser( parserStyleProgressStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.stylesHolder.top()->createProgressStyle( params[0]->get< castor::String >()
				, getSceneParserContext( context ).scene )
			, guiContext.progressStyle );
	}
	CU_EndAttributePush( GUISection::eProgressStyle )

	CU_ImplementAttributeParser( parserStyleExpandablePanelStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.stylesHolder.top()->createExpandablePanelStyle( params[0]->get< castor::String >()
				, getSceneParserContext( context ).scene )
			, guiContext.expandablePanelStyle );
	}
	CU_EndAttributePush( GUISection::eExpandablePanelStyle )

	CU_ImplementAttributeParser( parserStyleFrameStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.stylesHolder.top()->createFrameStyle( params[0]->get< castor::String >()
				, getSceneParserContext( context ).scene )
			, guiContext.frameStyle );
	}
	CU_EndAttributePush( GUISection::eFrameStyle )

	CU_ImplementAttributeParser( parserStyleScrollBarStyle )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.pushStyle( guiContext.stylesHolder.top()->createScrollBarStyle( params[0]->get< castor::String >()
			, getSceneParserContext( context ).scene )
			, guiContext.scrollBarStyle );
	}
	CU_EndAttributePush( GUISection::eScrollBarStyle )

	CU_ImplementAttributeParser( parserStyleEnd )
	{
		auto & guiContext = guiparse::getParserContext( context );
		guiContext.popStyle();
	}
	CU_EndAttributePop()

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
			if ( guiContext.layout->hasContainer() )
			{
				guiContext.layout->getContainer().setLayout( std::move( guiContext.layout ) );
			}
			else if ( guiContext.layout->hasManager() )
			{
				guiContext.layout->getManager().setLayout( std::move( guiContext.layout ) );
			}
			else
			{
			CU_ParsingError( cuT( "Layout has no container." ) );
			}
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
