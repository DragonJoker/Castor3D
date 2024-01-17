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
#include "Castor3D/Gui/Layout/LayoutBox.hpp"
#include "Castor3D/Gui/Theme/StylesHolder.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParserContext.hpp>

#include <stack>

namespace castor3d
{
	namespace guiparse
	{
		//*********************************************************************************************

		template< typename StyleT, typename ControlT >
		ControlT * createControl( castor::FileParserContext & context
			, GuiContext & blockContext
			, SceneRPtr scene
			, castor::String const & controlName
			, castor::String const & styleName
			, ControlT *& control )
		{
			auto style = blockContext.controls->template getStyle< StyleT >( styleName );

			if ( style == nullptr )
			{
				CU_ParsingError( "Style [" + styleName + "] was not found" );
				return nullptr;
			}

			control = blockContext.controls->registerControlT( castor::makeUnique< ControlT >( scene
				, controlName
				, style
				, blockContext.getTopControl() ) );
			blockContext.parents.push( control );
			return control;
		}

		template< typename ControlT >
		void finishControl( ControlsManager & manager
			, GuiContext & context
			, ControlT * control )
		{
			if ( control )
			{
				manager.create( control );
				context.parents.pop();
			}
		}

		GuiContext * getGuiContext( RootContext const * context )
		{
			return context->gui.get();
		}

		GuiContext * getGuiContext( SceneContext const * context )
		{
			return context->root->gui.get();
		}

		GuiContext * getGuiContext( GuiContext * context )
		{
			return context;
		}

		template< typename ContextT >
		Scene * getScene( ContextT * context )
		{
			auto gui = getGuiContext( context );
			return gui->scene ? gui->scene->scene : nullptr;
		}

		//*********************************************************************************************

		static CU_ImplementAttributeParserBlock( parserRootGui, RootContext )
		{
			blockContext->gui->scene = {};
		}
		CU_EndAttributePushBlock( GUISection::eGUI, blockContext->gui.get() )

		static CU_ImplementAttributeParserBlock( parserSceneGui, SceneContext )
		{
			blockContext->root->gui->scene = blockContext;
		}
		CU_EndAttributePushBlock( GUISection::eGUI, blockContext->root->gui.get() )

		static CU_ImplementAttributeParserBlock( parserRootTheme, RootContext )
		{
			blockContext->gui->scene = {};
			blockContext->gui->theme = blockContext->gui->controls->createTheme( params[0]->get< castor::String >()
				, getScene( blockContext ) );
			blockContext->gui->pushStylesHolder( blockContext->gui->theme );
		}
		CU_EndAttributePushBlock( GUISection::eTheme, blockContext->gui.get() )

		static CU_ImplementAttributeParserBlock( parserSceneTheme, SceneContext )
		{
			blockContext->root->gui->scene = blockContext;
			blockContext->root->gui->theme = blockContext->root->gui->controls->createTheme( params[0]->get< castor::String >()
				, blockContext->scene );
			blockContext->root->gui->pushStylesHolder( blockContext->root->gui->theme );
		}
		CU_EndAttributePushBlock( GUISection::eTheme, blockContext->root->gui.get() )

		static CU_ImplementAttributeParserBlock( parserGuiTheme, GuiContext )
		{
			blockContext->theme = blockContext->controls->createTheme( params[0]->get< castor::String >()
				, getScene( blockContext ) );
			blockContext->pushStylesHolder( blockContext->theme );
		}
		CU_EndAttributePushBlock( GUISection::eTheme, blockContext )

		static CU_ImplementAttributeParserBlock( parserRootGlobalBoxLayout, RootContext )
		{
			blockContext->gui->scene = {};
			blockContext->gui->layout = castor::makeUniqueDerived< Layout, LayoutBox >( *blockContext->gui->controls );
		}
		CU_EndAttributePushBlock( GUISection::eBoxLayout, blockContext->gui.get() )

		static CU_ImplementAttributeParserBlock( parserSceneGlobalBoxLayout, SceneContext )
		{
			blockContext->root->gui->scene = blockContext;
			blockContext->root->gui->layout = castor::makeUniqueDerived< Layout, LayoutBox >( *blockContext->root->gui->controls );
		}
		CU_EndAttributePushBlock( GUISection::eBoxLayout, blockContext->root->gui.get() )

		static CU_ImplementAttributeParserBlock( parserGuiGlobalBoxLayout, GuiContext )
		{
			blockContext->layout = castor::makeUniqueDerived< Layout, LayoutBox >( *blockContext->controls );
		}
		CU_EndAttributePushBlock( GUISection::eBoxLayout, blockContext )

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserButton, ContextT )
		{
			params[0]->get( getGuiContext( blockContext )->controlName );
		}
		CU_EndAttributePushBlock( GUISection::eButton, getGuiContext( blockContext ) )

		static CU_ImplementAttributeParserBlock( parserButtonTheme, GuiContext )
		{
			guiparse::createControl< ButtonStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >() + "/Button"
				, blockContext->button );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserButtonStyle, GuiContext )
		{
			guiparse::createControl< ButtonStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >()
				, blockContext->button );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserButtonHAlign, GuiContext )
		{
			if ( auto control = blockContext->button )
			{
				control->setHAlign( HAlign( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No button control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserButtonVAlign, GuiContext )
		{
			if ( auto control = blockContext->button )
			{
				control->setVAlign( VAlign( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No button control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserButtonCaption, GuiContext )
		{
			if ( auto control = blockContext->button )
			{
				control->setCaption( castor::string::toU32String( params[0]->get< castor::String >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No button control initialised." ) );
			}
		}
		CU_EndAttribute()

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserComboBox, ContextT )
		{
			params[0]->get( getGuiContext( blockContext )->controlName );
		}
		CU_EndAttributePushBlock( GUISection::eComboBox, getGuiContext( blockContext ) )

		static CU_ImplementAttributeParserBlock( parserComboBoxTheme, GuiContext )
		{
			guiparse::createControl< ComboBoxStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >() + "/ComboBox"
				, blockContext->combo );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserComboBoxStyle, GuiContext )
		{
			guiparse::createControl< ComboBoxStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >()
				, blockContext->combo );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserComboBoxItem, GuiContext )
		{
			if ( auto combo = blockContext->combo )
			{
				combo->appendItem( params[0]->get< castor::String >() );
			}
			else
			{
				CU_ParsingError( cuT( "No combobox control initialised." ) );
			}
		}
		CU_EndAttribute()

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserEdit, ContextT )
		{
			params[0]->get( getGuiContext( blockContext )->controlName );
		}
		CU_EndAttributePushBlock( GUISection::eEdit, getGuiContext( blockContext ) )

		static CU_ImplementAttributeParserBlock( parserEditTheme, GuiContext )
		{
			guiparse::createControl< EditStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >() + "/Edit"
				, blockContext->edit );
			blockContext->scrollable = blockContext->edit;
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEditStyle, GuiContext )
		{
			guiparse::createControl< EditStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >()
				, blockContext->edit );
			blockContext->scrollable = blockContext->edit;
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEditCaption, GuiContext )
		{
			if ( auto edit = blockContext->edit )
			{
				auto text = params[0]->get< castor::String >();
				castor::string::replace( text, R"(\?)", "\?" );
				castor::string::replace( text, R"(\\)", "\\" );
				castor::string::replace( text, R"(\")", "\"" );
				castor::string::replace( text, R"(\a)", "\a" );
				castor::string::replace( text, R"(\b)", "\b" );
				castor::string::replace( text, R"(\f)", "\f" );
				castor::string::replace( text, R"(\n)", "\n" );
				castor::string::replace( text, R"(\r)", "\r" );
				castor::string::replace( text, R"(\t)", "\t" );
				castor::string::replace( text, R"(\v)", "\v" );
				edit->setCaption( castor::string::toU32String( text ) );
			}
			else
			{
				CU_ParsingError( cuT( "No edit control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEditMultiLine, GuiContext )
		{
			if ( auto edit = blockContext->edit )
			{
				if ( params[0]->get< bool >() )
				{
					edit->addFlag( EditFlag::eMultiline );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No edit control initialised." ) );
			}
		}
		CU_EndAttribute()

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserListBox, ContextT )
		{
			params[0]->get( getGuiContext( blockContext )->controlName );
		}
		CU_EndAttributePushBlock( GUISection::eListBox, getGuiContext( blockContext ) )

		static CU_ImplementAttributeParserBlock( parserListBoxTheme, GuiContext )
		{
			guiparse::createControl< ListBoxStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >() + "/ListBox"
				, blockContext->listbox );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserListBoxStyle, GuiContext )
		{
			guiparse::createControl< ListBoxStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >()
				, blockContext->listbox );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserListBoxItem, GuiContext )
		{
			if ( auto listbox = blockContext->listbox )
			{
				listbox->appendItem( params[0]->get< castor::String >() );
			}
			else
			{
				CU_ParsingError( cuT( "No listbox control initialised." ) );
			}
		}
		CU_EndAttribute()

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserSlider, ContextT )
		{
			params[0]->get( getGuiContext( blockContext )->controlName );
		}
		CU_EndAttributePushBlock( GUISection::eSlider, getGuiContext( blockContext ) )

		static CU_ImplementAttributeParserBlock( parserSliderTheme, GuiContext )
		{
			guiparse::createControl< SliderStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >() + "/Slider"
				, blockContext->slider );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSliderStyle, GuiContext )
		{
			guiparse::createControl< SliderStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >()
				, blockContext->slider );
		}
		CU_EndAttribute()

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserStatic, ContextT )
		{
			params[0]->get( getGuiContext( blockContext )->controlName );
		}
		CU_EndAttributePushBlock( GUISection::eStatic, getGuiContext( blockContext ) )

		static CU_ImplementAttributeParserBlock( parserStaticTheme, GuiContext )
		{
			guiparse::createControl< StaticStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >() + "/Static"
				, blockContext->staticTxt );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserStaticStyle, GuiContext )
		{
			guiparse::createControl< StaticStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >()
				, blockContext->staticTxt );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserStaticHAlign, GuiContext )
		{
			if ( auto control = blockContext->staticTxt )
			{
				control->setHAlign( HAlign( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No static control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserStaticVAlign, GuiContext )
		{
			if ( auto control = blockContext->staticTxt )
			{
				control->setVAlign( VAlign( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No static control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserStaticCaption, GuiContext )
		{
			if ( auto control = blockContext->staticTxt )
			{
				control->setCaption( castor::string::toU32String( params[0]->get< castor::String >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No static control initialised." ) );
			}
		}
		CU_EndAttribute()

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserPanel, ContextT )
		{
			params[0]->get( getGuiContext( blockContext )->controlName );
		}
		CU_EndAttributePushBlock( GUISection::ePanel, getGuiContext( blockContext ) )

		static CU_ImplementAttributeParserBlock( parserPanelTheme, GuiContext )
		{
			guiparse::createControl< PanelStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >() + "/Panel"
				, blockContext->panel );
			blockContext->scrollable = blockContext->panel;
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPanelStyle, GuiContext )
		{
			guiparse::createControl< PanelStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >()
				, blockContext->panel );
			blockContext->scrollable = blockContext->panel;
		}
		CU_EndAttribute()

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserProgress, ContextT )
		{
			params[0]->get( getGuiContext( blockContext )->controlName );
		}
		CU_EndAttributePushBlock( GUISection::eProgress, getGuiContext( blockContext ) )

		static CU_ImplementAttributeParserBlock( parserProgressTheme, GuiContext )
		{
			guiparse::createControl< ProgressStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >() + "/Progress"
				, blockContext->progress );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserProgressStyle, GuiContext )
		{
			guiparse::createControl< ProgressStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >()
				, blockContext->progress );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserProgressContainerBorderSize, GuiContext )
		{
			if ( auto progress = blockContext->progress )
			{
				progress->setContainerBorderSize( params[0]->get< castor::Point4ui >() );
			}
			else
			{
				CU_ParsingError( cuT( "No progress control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserProgressBarBorderSize, GuiContext )
		{
			if ( auto progress = blockContext->progress )
			{
				progress->setBarBorderSize( params[0]->get< castor::Point4ui >() );
			}
			else
			{
				CU_ParsingError( cuT( "No progress control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserProgressLeftToRight, GuiContext )
		{
			if ( auto progress = blockContext->progress )
			{
				progress->setLeftToRight();
			}
			else
			{
				CU_ParsingError( cuT( "No progress control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserProgressRightToLeft, GuiContext )
		{
			if ( auto progress = blockContext->progress )
			{
				progress->setRightToLeft();
			}
			else
			{
				CU_ParsingError( cuT( "No progress control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserProgressTopToBottom, GuiContext )
		{
			if ( auto progress = blockContext->progress )
			{
				progress->setTopToBottom();
			}
			else
			{
				CU_ParsingError( cuT( "No progress control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserProgressBottomToTop, GuiContext )
		{
			if ( auto progress = blockContext->progress )
			{
				progress->setBottomToTop();
			}
			else
			{
				CU_ParsingError( cuT( "No progress control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserProgressHideTitle, GuiContext )
		{
			if ( auto progress = blockContext->progress )
			{
				progress->showTitle( !params[0]->get< bool >() );
			}
			else
			{
				CU_ParsingError( cuT( "No progress control initialised." ) );
			}
		}
		CU_EndAttribute()

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserExpandablePanel, ContextT )
		{
			params[0]->get( getGuiContext( blockContext )->controlName );
		}
		CU_EndAttributePushBlock( GUISection::eExpandablePanel, getGuiContext( blockContext ) )

		static CU_ImplementAttributeParserBlock( parserExpandablePanelTheme, GuiContext )
		{
			guiparse::createControl< ExpandablePanelStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >() + "/ExpandablePanel"
				, blockContext->expandablePanel );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserExpandablePanelStyle, GuiContext )
		{
			guiparse::createControl< ExpandablePanelStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >()
				, blockContext->expandablePanel );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserExpandablePanelHeader, GuiContext )
		{
			blockContext->panel = blockContext->expandablePanel->getHeader();
			blockContext->parents.push( blockContext->panel );
		}
		CU_EndAttributePushBlock( GUISection::eExpandablePanelHeader, blockContext )

		static CU_ImplementAttributeParserBlock( parserExpandablePanelExpand, GuiContext )
		{
			blockContext->button = blockContext->expandablePanel->getExpand();
			blockContext->parents.push( blockContext->button );
		}
		CU_EndAttributePushBlock( GUISection::eExpandablePanelExpand, blockContext )

		static CU_ImplementAttributeParserBlock( parserExpandablePanelContent, GuiContext )
		{
			blockContext->panel = blockContext->expandablePanel->getContent();
			blockContext->scrollable = blockContext->panel;
			blockContext->parents.push( blockContext->panel );
		}
		CU_EndAttributePushBlock( GUISection::eExpandablePanelContent, blockContext )

		static CU_ImplementAttributeParserBlock( parserExpandablePanelHeaderEnd, GuiContext )
		{
			blockContext->parents.pop();
			blockContext->popControl();
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserExpandablePanelExpandCaption, GuiContext )
		{
			blockContext->expandablePanel->setExpandCaption( castor::string::toU32String( params[0]->get< castor::String >() ) );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserExpandablePanelRetractCaption, GuiContext )
		{
			blockContext->expandablePanel->setRetractCaption( castor::string::toU32String( params[0]->get< castor::String >() ) );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserExpandablePanelExpandEnd, GuiContext )
		{
			blockContext->parents.pop();
			blockContext->popControl();
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserExpandablePanelContentEnd, GuiContext )
		{
			blockContext->parents.pop();
			blockContext->popControl();
		}
		CU_EndAttributePop()

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserFrame, ContextT )
		{
			params[0]->get( getGuiContext( blockContext )->controlName );
		}
		CU_EndAttributePushBlock( GUISection::eFrame, getGuiContext( blockContext ) )

		static CU_ImplementAttributeParserBlock( parserFrameTheme, GuiContext )
		{
			guiparse::createControl< FrameStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >() + "/Frame"
				, blockContext->frame );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFrameStyle, GuiContext )
		{
			guiparse::createControl< FrameStyle >( context
				, *blockContext
				, getScene( blockContext )
				, blockContext->controlName
				, params[0]->get< castor::String >()
				, blockContext->frame );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFrameHeaderHAlign, GuiContext )
		{
			if ( auto control = blockContext->frame )
			{
				control->setHeaderHAlign( HAlign( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No frame control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFrameHeaderVAlign, GuiContext )
		{
			if ( auto control = blockContext->frame )
			{
				control->setHeaderVAlign( VAlign( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No frame control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFrameHeaderCaption, GuiContext )
		{
			if ( auto control = blockContext->frame )
			{
				control->setCaption( castor::string::toU32String( params[0]->get< castor::String >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No frame control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFrameMinSize, GuiContext )
		{
			if ( auto control = blockContext->frame )
			{
				control->setMinSize( params[0]->get< castor::Size >() );
			}
			else
			{
				CU_ParsingError( cuT( "No frame control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFrameContent, GuiContext )
		{
			blockContext->panel = blockContext->frame->getContent();
			blockContext->scrollable = blockContext->panel;
			blockContext->parents.push( blockContext->panel );
		}
		CU_EndAttributePushBlock( GUISection::eFrameContent, blockContext )

		static CU_ImplementAttributeParserBlock( parserFrameContentEnd, GuiContext )
		{
			blockContext->parents.pop();
			blockContext->popControl();
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserControlPixelPosition, GuiContext )
		{
			if ( auto control = blockContext->getTopControl() )
			{
				control->setPosition( params[0]->get< castor::Position >() );
			}
			else
			{
				CU_ParsingError( cuT( "No control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserControlPixelSize, GuiContext )
		{
			if ( auto control = blockContext->getTopControl() )
			{
				control->setSize( params[0]->get< castor::Size >() );
			}
			else
			{
				CU_ParsingError( cuT( "No control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserControlPixelBorderSize, GuiContext )
		{
			if ( auto control = blockContext->getTopControl() )
			{
				control->setBorderSize( params[0]->get< castor::Point4ui >() );
			}
			else
			{
				CU_ParsingError( cuT( "No control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserControlBorderInnerUv, GuiContext )
		{
			if ( auto control = blockContext->getTopControl() )
			{
				control->setBorderInnerUV( params[0]->get< castor::Point4d >() );
			}
			else
			{
				CU_ParsingError( cuT( "No control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserControlBorderOuterUv, GuiContext )
		{
			if ( auto control = blockContext->getTopControl() )
			{
				control->setBorderOuterUV( params[0]->get< castor::Point4d >() );
			}
			else
			{
				CU_ParsingError( cuT( "No control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserControlCenterUv, GuiContext )
		{
			if ( auto control = blockContext->getTopControl() )
			{
				control->setUV( params[0]->get< castor::Point4d >() );
			}
			else
			{
				CU_ParsingError( cuT( "No control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserControlVisible, GuiContext )
		{
			if ( auto control = blockContext->getTopControl() )
			{
				control->setVisible( params[0]->get< bool >() );
			}
			else
			{
				CU_ParsingError( cuT( "No control initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserControlBoxLayout, GuiContext )
		{
			if ( auto control = blockContext->getTopControl() )
			{
				blockContext->layout = castor::makeUniqueDerived< Layout, LayoutBox >( static_cast< LayoutControl & >( *control ) );
			}
			else
			{
				CU_ParsingError( cuT( "No control initialised." ) );
			}
		}
		CU_EndAttributePushBlock( GUISection::eBoxLayout, blockContext )

		static CU_ImplementAttributeParserBlock( parserControlMovable, GuiContext )
		{
			if ( auto control = blockContext->getTopControl() )
			{
				if ( params[0]->get< bool >() )
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

		static CU_ImplementAttributeParserBlock( parserControlResizable, GuiContext )
		{
			if ( auto control = blockContext->getTopControl() )
			{
				if ( params[0]->get< bool >() )
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

		static CU_ImplementAttributeParserBlock( parserControlEnd, GuiContext )
		{
			guiparse::finishControl( *blockContext->controls, *blockContext, blockContext->getTopControl() );
			blockContext->popControl();
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserScrollableVerticalScroll, GuiContext )
		{
			if ( params[0]->get< bool >() )
			{
				if ( blockContext->edit )
				{
					blockContext->edit->addFlag( ScrollBarFlag::eVertical );
				}
				else if ( blockContext->panel )
				{
					blockContext->panel->addFlag( ScrollBarFlag::eVertical );
				}
			}
			else
			{
				if ( blockContext->edit )
				{
					blockContext->edit->removeFlag( ScrollBarFlag::eVertical );
				}
				else if ( blockContext->panel )
				{
					blockContext->panel->removeFlag( ScrollBarFlag::eVertical );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserScrollableHorizontalScroll, GuiContext )
		{
			if ( params[0]->get< bool >() )
			{
				if ( blockContext->edit )
				{
					blockContext->edit->addFlag( ScrollBarFlag::eHorizontal );
				}
				else if ( blockContext->panel )
				{
					blockContext->panel->addFlag( ScrollBarFlag::eHorizontal );
				}
			}
			else
			{
				if ( blockContext->edit )
				{
					blockContext->edit->removeFlag( ScrollBarFlag::eHorizontal );
				}
				else if ( blockContext->panel )
				{
					blockContext->panel->removeFlag( ScrollBarFlag::eHorizontal );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserScrollableVerticalScrollBarStyle, GuiContext )
		{
			if ( !blockContext->scrollable )
			{
				CU_ParsingError( "No ScrollableCtrl initialised" );
			}
			else
			{
				auto styleName = params[0]->get< castor::String >();
				auto style = blockContext->controls->getScrollBarStyle( styleName );

				if ( style == nullptr )
				{
					CU_ParsingError( "Style [" + styleName + "] was not found" );
				}
				else
				{
					blockContext->scrollable->getStyle().setVerticalStyle( *style );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserScrollableHorizontalScrollBarStyle, GuiContext )
		{
			if ( !blockContext->scrollable )
			{
				CU_ParsingError( "No ScrollableCtrl initialised" );
			}
			else
			{
				auto styleName = params[0]->get< castor::String >();
				auto style = blockContext->controls->getScrollBarStyle( styleName );

				if ( style == nullptr )
				{
					CU_ParsingError( "Style [" + styleName + "] was not found" );
				}
				else
				{
					blockContext->scrollable->getStyle().setHorizontalStyle( *style );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserThemeEnd, GuiContext )
		{
			blockContext->popStylesHolder( blockContext->theme );
			blockContext->theme = nullptr;
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserStyleButtonFont, GuiContext )
		{
			if ( auto style = blockContext->buttonStyle )
			{
				style->setFont( params[0]->get< castor::String >() );
			}
			else
			{
				CU_ParsingError( cuT( "No button style initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserStyleButtonTextMaterial, GuiContext )
		{
			if ( auto style = blockContext->buttonStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleButtonHighlightedBackgroundMaterial, GuiContext )
		{
			if ( auto style = blockContext->buttonStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleButtonHighlightedForegroundMaterial, GuiContext )
		{
			if ( auto style = blockContext->buttonStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleButtonHighlightedTextMaterial, GuiContext )
		{
			if ( auto style = blockContext->buttonStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleButtonPushedBackgroundMaterial, GuiContext )
		{
			if ( auto style = blockContext->buttonStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleButtonPushedForegroundMaterial, GuiContext )
		{
			if ( auto style = blockContext->buttonStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleButtonPushedTextMaterial, GuiContext )
		{
			if ( auto style = blockContext->buttonStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleButtonDisabledBackgroundMaterial, GuiContext )
		{
			if ( auto style = blockContext->buttonStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleButtonDisabledForegroundMaterial, GuiContext )
		{
			if ( auto style = blockContext->buttonStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleButtonDisabledTextMaterial, GuiContext )
		{
			if ( auto style = blockContext->buttonStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleComboButton, GuiContext )
		{
			blockContext->pushStyle( &blockContext->comboStyle->getExpandStyle()
				, blockContext->buttonStyle );
		}
		CU_EndAttributePushBlock( GUISection::eButtonStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleComboListBox, GuiContext )
		{
			blockContext->pushStyle( &blockContext->comboStyle->getElementsStyle()
				, blockContext->listboxStyle );
		}
		CU_EndAttributePushBlock( GUISection::eListStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleEditFont, GuiContext )
		{
			if ( auto style = blockContext->editStyle )
			{
				style->setFont( params[0]->get< castor::String >() );
			}
			else
			{
				CU_ParsingError( cuT( "No edit style initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserStyleEditTextMaterial, GuiContext )
		{
			if ( auto style = blockContext->editStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleEditSelectionMaterial, GuiContext )
		{
			if ( auto style = blockContext->editStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleListBoxItemStatic, GuiContext )
		{
			blockContext->pushStyle( &blockContext->listboxStyle->getItemStyle()
				, blockContext->staticStyle );
		}
		CU_EndAttributePushBlock( GUISection::eStaticStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleListBoxSelItemStatic, GuiContext )
		{
			blockContext->pushStyle( &blockContext->listboxStyle->getSelectedItemStyle()
				, blockContext->staticStyle );
		}
		CU_EndAttributePushBlock( GUISection::eStaticStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleListBoxHighItemStatic, GuiContext )
		{
			blockContext->pushStyle( &blockContext->listboxStyle->getHighlightedItemStyle()
				, blockContext->staticStyle );
		}
		CU_EndAttributePushBlock( GUISection::eStaticStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleSliderLineStatic, GuiContext )
		{
			blockContext->pushStyle( &blockContext->sliderStyle->getLineStyle()
				, blockContext->staticStyle );
		}
		CU_EndAttributePushBlock( GUISection::eStaticStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleSliderTickStatic, GuiContext )
		{
			blockContext->pushStyle( &blockContext->sliderStyle->getTickStyle()
				, blockContext->staticStyle );
		}
		CU_EndAttributePushBlock( GUISection::eStaticStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleStaticFont, GuiContext )
		{
			if ( auto style = blockContext->staticStyle )
			{
				style->setFont( params[0]->get< castor::String >() );
			}
			else
			{
				CU_ParsingError( cuT( "No static style initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserStyleStaticTextMaterial, GuiContext )
		{
			if ( auto style = blockContext->staticStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleProgressTitleFont, GuiContext )
		{
			if ( auto style = blockContext->progressStyle )
			{
				style->setTitleFontName( params[0]->get< castor::String >() );
			}
			else
			{
				CU_ParsingError( cuT( "No progress style initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserStyleProgressTitleMaterial, GuiContext )
		{
			if ( auto style = blockContext->progressStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleProgressContainer, GuiContext )
		{
			if ( auto style = blockContext->progressStyle )
			{
				blockContext->pushStyle( &style->getContainerStyle()
					, blockContext->panelStyle );
			}
			else
			{
				CU_ParsingError( cuT( "No progress style initialised." ) );
			}
		}
		CU_EndAttributePushBlock( GUISection::ePanelStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleProgressProgress, GuiContext )
		{
			if ( auto style = blockContext->progressStyle )
			{
				blockContext->pushStyle( &style->getProgressStyle()
					, blockContext->panelStyle );
			}
			else
			{
				CU_ParsingError( cuT( "No progress style initialised." ) );
			}
		}
		CU_EndAttributePushBlock( GUISection::ePanelStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleProgressTextFont, GuiContext )
		{
			if ( auto style = blockContext->progressStyle )
			{
				style->setTextFontName( params[0]->get< castor::String >() );
			}
			else
			{
				CU_ParsingError( cuT( "No progress style initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserStyleProgressTextMaterial, GuiContext )
		{
			if ( auto style = blockContext->progressStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleExpandablePanelHeader, GuiContext )
		{
			blockContext->pushStyle( &blockContext->expandablePanelStyle->getHeaderStyle()
				, blockContext->panelStyle );
		}
		CU_EndAttributePushBlock( GUISection::ePanelStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleExpandablePanelExpand, GuiContext )
		{
			blockContext->pushStyle( &blockContext->expandablePanelStyle->getExpandStyle()
				, blockContext->buttonStyle );
		}
		CU_EndAttributePushBlock( GUISection::eButtonStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleExpandablePanelContent, GuiContext )
		{
			blockContext->pushStyle( &blockContext->expandablePanelStyle->getContentStyle()
				, blockContext->panelStyle );
		}
		CU_EndAttributePushBlock( GUISection::ePanelStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleFrameHeaderFont, GuiContext )
		{
			if ( auto style = blockContext->frameStyle )
			{
				style->setHeaderFont( params[0]->get< castor::String >() );
			}
			else
			{
				CU_ParsingError( cuT( "No static style initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserStyleFrameHeaderTextMaterial, GuiContext )
		{
			if ( auto style = blockContext->frameStyle )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleScrollBarBeginButton, GuiContext )
		{
			blockContext->pushStyle( &blockContext->scrollBarStyle->getBeginStyle()
				, blockContext->buttonStyle );
		}
		CU_EndAttributePushBlock( GUISection::eButtonStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleScrollBarEndButton, GuiContext )
		{
			blockContext->pushStyle( &blockContext->scrollBarStyle->getEndStyle()
				, blockContext->buttonStyle );
		}
		CU_EndAttributePushBlock( GUISection::eButtonStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleScrollBarBar, GuiContext )
		{
			blockContext->pushStyle( &blockContext->scrollBarStyle->getBarStyle()
				, blockContext->panelStyle );
		}
		CU_EndAttributePushBlock( GUISection::ePanelStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleScrollBarThumb, GuiContext )
		{
			blockContext->pushStyle( &blockContext->scrollBarStyle->getThumbStyle()
				, blockContext->panelStyle );
		}
		CU_EndAttributePushBlock( GUISection::ePanelStyle, blockContext )

		static CU_ImplementAttributeParserBlock( parserStyleBackgroundMaterial, GuiContext )
		{
			if ( auto style = blockContext->getTopStyle() )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleForegroundMaterial, GuiContext )
		{
			if ( auto style = blockContext->getTopStyle() )
			{
				auto name = params[0]->get< castor::String >();
				auto material = blockContext->controls->getEngine()->findMaterial( name );

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

		static CU_ImplementAttributeParserBlock( parserStyleBackgroundInvisible, GuiContext )
		{
			if ( auto style = blockContext->getTopStyle() )
			{
				style->setBackgroundInvisible( params[0]->get< bool >() );
			}
			else
			{
				CU_ParsingError( cuT( "No style initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserStyleForegroundInvisible, GuiContext )
		{
			if ( auto style = blockContext->getTopStyle() )
			{
				style->setForegroundInvisible( params[0]->get< bool >() );
			}
			else
			{
				CU_ParsingError( cuT( "No style initialised." ) );
			}
		}
		CU_EndAttribute()

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserStyleDefaultFont, ContextT )
		{
			auto guiContext = getGuiContext( blockContext );
			auto & cache = guiContext->controls->getEngine()->getFontCache();
			auto name = params[0]->get< castor::String >();
			auto font = cache.find( name );

			if ( font )
			{
				guiContext->stylesHolder.top()->setDefaultFont( font );
			}
			else
			{
				CU_ParsingError( cuT( "Unknown font: " ) + name );
			}
		}
		CU_EndAttribute()

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserStyleButtonStyle, ContextT )
		{
			auto guiContext = getGuiContext( blockContext );
			guiContext->pushStyle( guiContext->stylesHolder.top()->createButtonStyle( params[0]->get< castor::String >()
					, getScene( guiContext ) )
				, guiContext->buttonStyle );
		}
		CU_EndAttributePushBlock( GUISection::eButtonStyle, getGuiContext( blockContext ) )

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserStyleComboBoxStyle, ContextT )
		{
			auto guiContext = getGuiContext( blockContext );
			guiContext->pushStyle( guiContext->stylesHolder.top()->createComboBoxStyle( params[0]->get< castor::String >()
					, getScene( guiContext ) )
				, guiContext->comboStyle );
		}
		CU_EndAttributePushBlock( GUISection::eComboStyle, getGuiContext( blockContext ) )

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserStyleEditStyle, ContextT )
		{
			auto guiContext = getGuiContext( blockContext );
			guiContext->pushStyle( guiContext->stylesHolder.top()->createEditStyle( params[0]->get< castor::String >()
					, getScene( guiContext ) )
				, guiContext->editStyle );
		}
		CU_EndAttributePushBlock( GUISection::eEditStyle, getGuiContext( blockContext ) )

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserStyleListBoxStyle, ContextT )
		{
			auto guiContext = getGuiContext( blockContext );
			guiContext->pushStyle( guiContext->stylesHolder.top()->createListBoxStyle( params[0]->get< castor::String >()
					, getScene( guiContext ) )
				, guiContext->listboxStyle );
		}
		CU_EndAttributePushBlock( GUISection::eListStyle, getGuiContext( blockContext ) )

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserStyleSliderStyle, ContextT )
		{
			auto guiContext = getGuiContext( blockContext );
			guiContext->pushStyle( guiContext->stylesHolder.top()->createSliderStyle( params[0]->get< castor::String >()
					, getScene( guiContext ) )
				, guiContext->sliderStyle );
		}
		CU_EndAttributePushBlock( GUISection::eSliderStyle, getGuiContext( blockContext ) )

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserStyleStaticStyle, ContextT )
		{
			auto guiContext = getGuiContext( blockContext );
			guiContext->pushStyle( guiContext->stylesHolder.top()->createStaticStyle( params[0]->get< castor::String >()
					, getScene( guiContext ) )
				, guiContext->staticStyle );
		}
		CU_EndAttributePushBlock( GUISection::eStaticStyle, getGuiContext( blockContext ) )

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserStylePanelStyle, ContextT )
		{
			auto guiContext = getGuiContext( blockContext );
			guiContext->pushStyle( guiContext->stylesHolder.top()->createPanelStyle( params[0]->get< castor::String >()
					, getScene( guiContext ) )
				, guiContext->panelStyle );
		}
		CU_EndAttributePushBlock( GUISection::ePanelStyle, getGuiContext( blockContext ) )

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserStyleProgressStyle, ContextT )
		{
			auto guiContext = getGuiContext( blockContext );
			guiContext->pushStyle( guiContext->stylesHolder.top()->createProgressStyle( params[0]->get< castor::String >()
					, getScene( guiContext ) )
				, guiContext->progressStyle );
		}
		CU_EndAttributePushBlock( GUISection::eProgressStyle, getGuiContext( blockContext ) )

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserStyleExpandablePanelStyle, ContextT )
		{
			auto guiContext = getGuiContext( blockContext );
			guiContext->pushStyle( guiContext->stylesHolder.top()->createExpandablePanelStyle( params[0]->get< castor::String >()
					, getScene( guiContext ) )
				, guiContext->expandablePanelStyle );
		}
		CU_EndAttributePushBlock( GUISection::eExpandablePanelStyle, getGuiContext( blockContext ) )

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserStyleFrameStyle, ContextT )
		{
			auto guiContext = getGuiContext( blockContext );
			guiContext->pushStyle( guiContext->stylesHolder.top()->createFrameStyle( params[0]->get< castor::String >()
					, getScene( guiContext ) )
				, guiContext->frameStyle );
		}
		CU_EndAttributePushBlock( GUISection::eFrameStyle, getGuiContext( blockContext ) )

		template< typename ContextT >
		static CU_ImplementAttributeParserBlock( parserStyleScrollBarStyle, ContextT )
		{
			auto guiContext = getGuiContext( blockContext );
			guiContext->pushStyle( guiContext->stylesHolder.top()->createScrollBarStyle( params[0]->get< castor::String >()
				, getScene( guiContext ) )
				, guiContext->scrollBarStyle );
		}
		CU_EndAttributePushBlock( GUISection::eScrollBarStyle, getGuiContext( blockContext ) )

		static CU_ImplementAttributeParserBlock( parserStyleEnd, GuiContext )
		{
			blockContext->popStyle();
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserLayoutCtrl, GuiContext )
		{
			if ( blockContext->layout )
			{
				params[0]->get( blockContext->controlName );
				blockContext->layoutCtrlFlags = {};
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttributePushBlock( GUISection::eLayoutCtrl, blockContext )

		static CU_ImplementAttributeParserBlock( parserLayoutEnd, GuiContext )
		{
			if ( blockContext->layout )
			{
				if ( blockContext->layout->hasContainer() )
				{
					blockContext->layout->getContainer().setLayout( std::move( blockContext->layout ) );
				}
				else if ( blockContext->layout->hasManager() )
				{
					blockContext->layout->getManager().setLayout( std::move( blockContext->layout ) );
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

		static CU_ImplementAttributeParserBlock( parserBoxLayoutStaticSpacer, GuiContext )
		{
			if ( blockContext->layout )
			{
				blockContext->layout->addSpacer( params[0]->get< uint32_t >() );
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBoxLayoutDynamicSpacer, GuiContext )
		{
			if ( blockContext->layout )
			{
				blockContext->layout->addSpacer( Spacer::Dynamic );
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBoxLayoutHorizontal, GuiContext )
		{
			if ( blockContext->layout )
			{
				auto & box = static_cast< LayoutBox & >( *blockContext->layout );
				box.setHorizontal( params[0]->get< bool >() );
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLayoutCtrlHAlign, GuiContext )
		{
			if ( blockContext->layout )
			{
				blockContext->layoutCtrlFlags.align( HAlign( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLayoutCtrlVAlign, GuiContext )
		{
			if ( blockContext->layout )
			{
				blockContext->layoutCtrlFlags.align( VAlign( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLayoutCtrlStretch, GuiContext )
		{
			if ( blockContext->layout )
			{
				blockContext->layoutCtrlFlags.stretch( params[0]->get< bool >() );
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLayoutCtrlReserveIfHidden, GuiContext )
		{
			if ( blockContext->layout )
			{
				blockContext->layoutCtrlFlags.reserveSpaceIfHidden( params[0]->get< bool >() );
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLayoutCtrlPadding, GuiContext )
		{
			if ( blockContext->layout )
			{
				blockContext->layoutCtrlFlags.padding( params[0]->get< castor::Point4ui >() );
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLayoutCtrlPadLeft, GuiContext )
		{
			if ( blockContext->layout )
			{
				blockContext->layoutCtrlFlags.padLeft( params[0]->get< uint32_t >() );
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLayoutCtrlPadTop, GuiContext )
		{
			if ( blockContext->layout )
			{
				blockContext->layoutCtrlFlags.padTop( params[0]->get< uint32_t >() );
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLayoutCtrlPadRight, GuiContext )
		{
			if ( blockContext->layout )
			{
				blockContext->layoutCtrlFlags.padRight( params[0]->get< uint32_t >() );
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLayoutCtrlPadBottom, GuiContext )
		{
			if ( blockContext->layout )
			{
				blockContext->layoutCtrlFlags.padBottom( params[0]->get< uint32_t >() );
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLayoutCtrlEnd, GuiContext )
		{
			if ( blockContext->layout )
			{
				if ( auto parent = blockContext->getTopControl() )
				{
					blockContext->controlName = parent->getName() + "/" + blockContext->controlName;
				}

				if ( auto control = blockContext->controls->findControl( blockContext->controlName ) )
				{
					blockContext->layout->addControl( *control
						, std::move( blockContext->layoutCtrlFlags ) );
				}
				else
				{
					CU_ParsingError( cuT( "Control [" ) + blockContext->controlName + cuT( "] was not found." ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No layout initialised." ) );
			}
		}
		CU_EndAttributePop()

		template< typename SectionT >
		static void createDefaultParsers( castor::AttributeParsers & parsers
			, GUISection newSection
			, SectionT oldSection
			, castor::RawParserFunctionT< GuiContext > themeFunction
			, castor::RawParserFunctionT< GuiContext > styleFunction
			, castor::RawParserFunctionT< GuiContext > endFunction
			, bool hasBorders = true )
		{
			using namespace castor;

			if ( themeFunction )
			{
				addParserT( parsers, newSection, cuT( "theme" ), themeFunction, { makeParameter< ParameterType::eName >() } );
			}

			if ( styleFunction )
			{
				addParserT( parsers, newSection, cuT( "style" ), styleFunction, { makeParameter< ParameterType::eName >() } );
			}

			if ( themeFunction || styleFunction )
			{
				addParserT( parsers, newSection, cuT( "visible" ), &parserControlVisible, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
				addParserT( parsers, newSection, cuT( "pixel_position" ), &parserControlPixelPosition, { makeParameter< ParameterType::ePosition >() } );
				addParserT( parsers, newSection, cuT( "pixel_size" ), &parserControlPixelSize, { makeParameter< ParameterType::eSize >() } );
				addParserT( parsers, newSection, cuT( "movable" ), &parserControlMovable, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
				addParserT( parsers, newSection, cuT( "resizable" ), &parserControlResizable, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			}

			if ( hasBorders )
			{
				addParserT( parsers, newSection, cuT( "pixel_border_size" ), &parserControlPixelBorderSize, { makeParameter< ParameterType::ePoint4U >() } );
				addParserT( parsers, newSection, cuT( "border_inner_uv" ), &parserControlBorderInnerUv, { makeParameter< ParameterType::ePoint4D >() } );
				addParserT( parsers, newSection, cuT( "border_outer_uv" ), &parserControlBorderOuterUv, { makeParameter< ParameterType::ePoint4D >() } );
				addParserT( parsers, newSection, cuT( "center_uv" ), &parserControlCenterUv, { makeParameter< ParameterType::ePoint4D >() } );
			}

			if ( endFunction )
			{
				addParserT( parsers, newSection, oldSection, cuT( "}" ), endFunction );
			}
			else
			{
				addParserT( parsers, newSection, oldSection, cuT( "}" ), parserDefaultEnd );
			}
		}

		static void createScrollableParsers( castor::AttributeParsers & result
			, GUISection section )
		{
			using namespace castor;
			addParserT( result, section, cuT( "vertical_scrollbar" ), &parserScrollableVerticalScroll, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, section, cuT( "horizontal_scrollbar" ), &parserScrollableHorizontalScroll, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, section, cuT( "vertical_scrollbar_style" ), &parserScrollableVerticalScrollBarStyle, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, cuT( "horizontal_scrollbar_style" ), &parserScrollableHorizontalScrollBarStyle, { makeParameter< ParameterType::eName >() } );
		}

		template< typename SectionT >
		static void createDefaultStyleParsers( castor::AttributeParsers & parsers
			, GUISection newSection
			, SectionT oldSection )
		{
			using namespace castor;
			addParserT( parsers, newSection, cuT( "background_invisible" ), &parserStyleBackgroundInvisible, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( parsers, newSection, cuT( "foreground_invisible" ), &parserStyleForegroundInvisible, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( parsers, newSection, cuT( "border_invisible" ), &parserStyleForegroundInvisible, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( parsers, newSection, cuT( "background_material" ), &parserStyleBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, newSection, cuT( "foreground_material" ), &parserStyleForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, newSection, cuT( "border_material" ), &parserStyleForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, newSection, oldSection, cuT( "}" ), &parserStyleEnd );
		}

		static void createDefaultLayoutParsers( castor::AttributeParsers & parsers
			, GUISection section )
		{
			using namespace castor;
			addParserT( parsers, section, GUISection::eBoxLayout, cuT( "box_layout" ), &parserControlBoxLayout );
		}

		template< typename SectionT, typename ContextT >
		static void createControlsParsers( castor::AttributeParsers & parsers
			, SectionT section )
		{
			using namespace castor;
			addParserT( parsers, section, GUISection::eButton, cuT( "button" ), &parserButton< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, GUISection::eStatic, cuT( "static" ), &parserStatic< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, GUISection::eSlider, cuT( "slider" ), &parserSlider< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, GUISection::eComboBox, cuT( "combobox" ), &parserComboBox< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, GUISection::eListBox, cuT( "listbox" ), &parserListBox< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, GUISection::eEdit, cuT( "edit" ), &parserEdit< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, GUISection::ePanel, cuT( "panel" ), &parserPanel< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, GUISection::eProgress, cuT( "progress" ), &parserProgress< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, GUISection::eExpandablePanel, cuT( "expandable_panel" ), &parserExpandablePanel< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( parsers, section, GUISection::eFrame, cuT( "frame" ), &parserFrame< ContextT >, { makeParameter< ParameterType::eName >() } );
		}

		template< typename SectionT, typename ContextT >
		static void createStylesParsers( castor::AttributeParsers & result
			, SectionT section )
		{
			using namespace castor;
			addParserT( result, section, cuT( "default_font" ), &parserStyleDefaultFont< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, GUISection::eButtonStyle, cuT( "button_style" ), &parserStyleButtonStyle< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, GUISection::eStaticStyle, cuT( "static_style" ), &parserStyleStaticStyle< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, GUISection::eSliderStyle, cuT( "slider_style" ), &parserStyleSliderStyle< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, GUISection::eComboStyle, cuT( "combobox_style" ), &parserStyleComboBoxStyle< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, GUISection::eListStyle, cuT( "listbox_style" ), &parserStyleListBoxStyle< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, GUISection::eEditStyle, cuT( "edit_style" ), &parserStyleEditStyle< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, GUISection::ePanelStyle, cuT( "panel_style" ), &parserStylePanelStyle< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, GUISection::eProgressStyle, cuT( "progress_style" ), &parserStyleProgressStyle< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, GUISection::eExpandablePanelStyle, cuT( "expandable_panel_style" ), &parserStyleExpandablePanelStyle< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, GUISection::eFrameStyle, cuT( "frame_style" ), &parserStyleFrameStyle< ContextT >, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, GUISection::eScrollBarStyle, cuT( "scrollbar_style" ), &parserStyleScrollBarStyle< ContextT >, { makeParameter< ParameterType::eName >() } );
		}

		template< typename SectionT, typename ContextT >
		static void createGuiParsers( castor::AttributeParsers & result
			, SectionT section
			, castor::RawParserFunctionT< ContextT > themeFunction
			, castor::RawParserFunctionT< ContextT > layoutFunction )
		{
			using namespace castor;
			createControlsParsers< SectionT, ContextT >( result, section );
			createStylesParsers< SectionT, ContextT >( result, section );
			addParserT( result, section, GUISection::eTheme, cuT( "theme" ), themeFunction, { makeParameter< ParameterType::eName >() } );
			addParserT( result, section, GUISection::eBoxLayout, cuT( "box_layout" ), layoutFunction );
		}

		static void createButtonParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eButton
				, castor::PreviousSection
				, &parserButtonTheme
				, &parserButtonStyle
				, &parserControlEnd );
			addParserT( result, GUISection::eButton, cuT( "caption" ), &parserButtonCaption, { makeParameter< ParameterType::eText >() } );
			addParserT( result, GUISection::eButton, cuT( "horizontal_align" ), &parserButtonHAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
			addParserT( result, GUISection::eButton, cuT( "vertical_align" ), &parserButtonVAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );

			createDefaultStyleParsers( result, GUISection::eButtonStyle, castor::PreviousSection );
			addParserT( result, GUISection::eButtonStyle, cuT( "text_material" ), &parserStyleButtonTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "highlighted_background_material" ), &parserStyleButtonHighlightedBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "highlighted_foreground_material" ), &parserStyleButtonHighlightedForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "highlighted_text_material" ), &parserStyleButtonHighlightedTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "pushed_background_material" ), &parserStyleButtonPushedBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "pushed_foreground_material" ), &parserStyleButtonPushedForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "pushed_text_material" ), &parserStyleButtonPushedTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "disabled_background_material" ), &parserStyleButtonDisabledBackgroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "disabled_foreground_material" ), &parserStyleButtonDisabledForegroundMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "disabled_text_material" ), &parserStyleButtonDisabledTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eButtonStyle, cuT( "font" ), &parserStyleButtonFont, { makeParameter< ParameterType::eName >() } );
		}

		static void createComboBoxParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eComboBox
				, castor::PreviousSection
				, &parserComboBoxTheme
				, &parserComboBoxStyle
				, &parserControlEnd );
			addParserT( result, GUISection::eComboBox, cuT( "item" ), &parserComboBoxItem, { makeParameter< ParameterType::eText >() } );

			createDefaultStyleParsers( result, GUISection::eComboStyle, castor::PreviousSection );
			addParserT( result, GUISection::eComboStyle, GUISection::eButtonStyle, cuT( "expand_style" ), &parserStyleComboButton );
			addParserT( result, GUISection::eComboStyle, GUISection::eListStyle, cuT( "elements_style" ), &parserStyleComboListBox );
		}

		static void createEditParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eEdit
				, castor::PreviousSection
				, &parserEditTheme
				, &parserEditStyle
				, &parserControlEnd );
			createScrollableParsers( result, GUISection::eEdit );
			addParserT( result, GUISection::eEdit, cuT( "multiline" ), &parserEditMultiLine, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eEdit, cuT( "caption" ), &parserEditCaption, { makeParameter< ParameterType::eText >() } );

			createDefaultStyleParsers( result, GUISection::eEditStyle, castor::PreviousSection );
			addParserT( result, GUISection::eEditStyle, cuT( "font" ), &parserStyleEditFont, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eEditStyle, cuT( "text_material" ), &parserStyleEditTextMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eEditStyle, cuT( "selection_material" ), &parserStyleEditSelectionMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eEditStyle, GUISection::eScrollBarStyle, cuT( "scrollbar_style" ), &parserStyleScrollBarStyle< GuiContext >, { makeParameter< ParameterType::eName >() } );
		}

		static void createExpandablePanelHeaderParsers( castor::AttributeParsers & result )
		{
			createDefaultParsers( result
				, GUISection::eExpandablePanelHeader
				, GUISection::eExpandablePanel
				, nullptr
				, nullptr
				, &parserExpandablePanelHeaderEnd );
			createControlsParsers< GUISection, GuiContext >( result, GUISection::eExpandablePanelHeader );
			createDefaultLayoutParsers( result, GUISection::eExpandablePanelHeader );
		}

		static void createExpandablePanelExpandParsers( castor::AttributeParsers & result )
		{
			createDefaultParsers( result
				, GUISection::eExpandablePanelExpand
				, GUISection::eExpandablePanel
				, nullptr
				, nullptr
				, &parserExpandablePanelExpandEnd );
		}

		static void createExpandablePanelContentParsers( castor::AttributeParsers & result )
		{
			createDefaultParsers( result
				, GUISection::eExpandablePanelContent
				, GUISection::eExpandablePanel
				, nullptr
				, nullptr
				, &parserExpandablePanelContentEnd );
			createControlsParsers< GUISection, GuiContext >( result, GUISection::eExpandablePanelContent );
			createDefaultLayoutParsers( result, GUISection::eExpandablePanelContent );
			createScrollableParsers( result, GUISection::eExpandablePanelContent );
		}

		static void createExpandablePanelParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eExpandablePanel
				, castor::PreviousSection
				, &parserExpandablePanelTheme
				, &parserExpandablePanelStyle
				, &parserControlEnd
				, false );
			addParserT( result, GUISection::eExpandablePanel, GUISection::eExpandablePanelHeader, cuT( "header" ), &parserExpandablePanelHeader );
			addParserT( result, GUISection::eExpandablePanel, GUISection::eExpandablePanelExpand, cuT( "expand" ), &parserExpandablePanelExpand );
			addParserT( result, GUISection::eExpandablePanel, GUISection::eExpandablePanelContent, cuT( "content" ), &parserExpandablePanelContent );
			addParserT( result, GUISection::eExpandablePanel, cuT( "expand_caption" ), &parserExpandablePanelExpandCaption, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eExpandablePanel, cuT( "retract_caption" ), &parserExpandablePanelRetractCaption, { makeParameter< ParameterType::eName >() } );

			createDefaultStyleParsers( result, GUISection::eExpandablePanelStyle, castor::PreviousSection );
			addParserT( result, GUISection::eExpandablePanelStyle, GUISection::ePanelStyle, cuT( "header_style" ), &parserStyleExpandablePanelHeader );
			addParserT( result, GUISection::eExpandablePanelStyle, GUISection::eButtonStyle, cuT( "expand_style" ), &parserStyleExpandablePanelExpand );
			addParserT( result, GUISection::eExpandablePanelStyle, GUISection::ePanelStyle, cuT( "content_style" ), &parserStyleExpandablePanelContent );

			createExpandablePanelHeaderParsers( result );
			createExpandablePanelExpandParsers( result );
			createExpandablePanelContentParsers( result );
		}

		static void createFrameContentParsers( castor::AttributeParsers & result )
		{
			createDefaultParsers( result
				, GUISection::eFrameContent
				, GUISection::eFrame
				, nullptr
				, nullptr
				, &parserFrameContentEnd );
			createControlsParsers< GUISection, GuiContext >( result, GUISection::eFrameContent );
			createScrollableParsers( result, GUISection::eFrameContent );
			createDefaultLayoutParsers( result, GUISection::eFrameContent );
		}

		static void createFrameParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eFrame
				, castor::PreviousSection
				, &parserFrameTheme
				, &parserFrameStyle
				, &parserControlEnd );
			addParserT( result, GUISection::eFrame, cuT( "header_horizontal_align" ), &parserFrameHeaderHAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
			addParserT( result, GUISection::eFrame, cuT( "header_vertical_align" ), &parserFrameHeaderVAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );
			addParserT( result, GUISection::eFrame, cuT( "header_caption" ), &parserFrameHeaderCaption, { makeParameter< ParameterType::eText >() } );
			addParserT( result, GUISection::eFrame, cuT( "min_size" ), &parserFrameMinSize, { makeParameter< ParameterType::eSize >() } );
			addParserT( result, GUISection::eFrame, GUISection::eFrameContent, cuT( "content" ), &parserFrameContent );

			createDefaultStyleParsers( result, GUISection::eFrameStyle, castor::PreviousSection );
			addParserT( result, GUISection::eFrameStyle, cuT( "header_font" ), &parserStyleFrameHeaderFont, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eFrameStyle, cuT( "header_text_material" ), &parserStyleFrameHeaderTextMaterial, { makeParameter< ParameterType::eName >() } );

			createFrameContentParsers( result );
		}

		static void createListBoxParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eListBox
				, castor::PreviousSection
				, &parserListBoxTheme
				, &parserListBoxStyle
				, &parserControlEnd );
			addParserT( result, GUISection::eListBox, cuT( "item" ), &parserListBoxItem, { makeParameter< ParameterType::eText >() } );

			createDefaultStyleParsers( result, GUISection::eListStyle, castor::PreviousSection );
			addParserT( result, GUISection::eListStyle, GUISection::eStaticStyle, cuT( "item_style" ), &parserStyleListBoxItemStatic );
			addParserT( result, GUISection::eListStyle, GUISection::eStaticStyle, cuT( "selected_item_style" ), &parserStyleListBoxSelItemStatic );
			addParserT( result, GUISection::eListStyle, GUISection::eStaticStyle, cuT( "highlighted_item_style" ), &parserStyleListBoxHighItemStatic );
		}

		static void createPanelParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::ePanel
				, castor::PreviousSection
				, &parserPanelTheme
				, &parserPanelStyle
				, &parserControlEnd );
			createControlsParsers< GUISection, GuiContext >( result, GUISection::ePanel );
			createScrollableParsers( result, GUISection::ePanel );
			createDefaultLayoutParsers( result, GUISection::ePanel );

			createStylesParsers< GUISection, GuiContext >( result, GUISection::ePanelStyle );
			createDefaultStyleParsers( result, GUISection::ePanelStyle, castor::PreviousSection );
		}

		static void createProgressParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eProgress
				, castor::PreviousSection
				, &parserProgressTheme
				, &parserProgressStyle
				, &parserControlEnd );
			addParserT( result, GUISection::eProgress, cuT( "container_border_size" ), &parserProgressContainerBorderSize, { makeParameter< ParameterType::ePoint4U >() } );
			addParserT( result, GUISection::eProgress, cuT( "bar_border_size" ), &parserProgressBarBorderSize, { makeParameter< ParameterType::ePoint4U >() } );
			addParserT( result, GUISection::eProgress, cuT( "left_to_right" ), &parserProgressLeftToRight, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eProgress, cuT( "right_to_left" ), &parserProgressRightToLeft, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eProgress, cuT( "top_to_bottom" ), &parserProgressTopToBottom, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eProgress, cuT( "bottom_to_top" ), &parserProgressBottomToTop, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eProgress, cuT( "hide_title" ), &parserProgressHideTitle, { makeDefaultedParameter< ParameterType::eBool >( true ) } );

			createDefaultStyleParsers( result, GUISection::eProgressStyle, castor::PreviousSection );
			addParserT( result, GUISection::eProgressStyle, cuT( "title_font" ), &parserStyleProgressTitleFont, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eProgressStyle, cuT( "title_material" ), &parserStyleProgressTitleMaterial, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eProgressStyle, GUISection::ePanelStyle, cuT( "container_style" ), &parserStyleProgressContainer );
			addParserT( result, GUISection::eProgressStyle, GUISection::ePanelStyle, cuT( "bar_style" ), &parserStyleProgressProgress );
			addParserT( result, GUISection::eProgressStyle, cuT( "text_font" ), &parserStyleProgressTextFont, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eProgressStyle, cuT( "text_material" ), &parserStyleProgressTextMaterial, { makeParameter< ParameterType::eName >() } );
		}

		static void createScrollBarParsers( castor::AttributeParsers & result )
		{
			addParserT( result, GUISection::eScrollBarStyle, GUISection::eButtonStyle, cuT( "begin_style" ), &parserStyleScrollBarBeginButton );
			addParserT( result, GUISection::eScrollBarStyle, GUISection::eButtonStyle, cuT( "end_style" ), &parserStyleScrollBarEndButton );
			addParserT( result, GUISection::eScrollBarStyle, GUISection::ePanelStyle, cuT( "bar_style" ), &parserStyleScrollBarBar );
			addParserT( result, GUISection::eScrollBarStyle, GUISection::ePanelStyle, cuT( "thumb_style" ), &parserStyleScrollBarThumb );
			addParserT( result, GUISection::eScrollBarStyle, castor::PreviousSection, cuT( "}" ), &parserStyleEnd );
		}

		static void createSliderParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eSlider
				, castor::PreviousSection
				, &parserSliderTheme
				, &parserSliderStyle
				, &parserControlEnd );

			createDefaultStyleParsers( result, GUISection::eSliderStyle, castor::PreviousSection );
			addParserT( result, GUISection::eSliderStyle, GUISection::eStaticStyle, cuT( "line_style" ), &parserStyleSliderLineStatic );
			addParserT( result, GUISection::eSliderStyle, GUISection::eStaticStyle, cuT( "tick_style" ), &parserStyleSliderTickStatic );
		}

		static void createStaticParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			createDefaultParsers( result
				, GUISection::eStatic
				, castor::PreviousSection
				, &parserStaticTheme
				, &parserStaticStyle
				, &parserControlEnd );
			addParserT( result, GUISection::eStatic, cuT( "horizontal_align" ), &parserStaticHAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
			addParserT( result, GUISection::eStatic, cuT( "vertical_align" ), &parserStaticVAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );
			addParserT( result, GUISection::eStatic, cuT( "caption" ), &parserStaticCaption, { makeParameter< ParameterType::eText >() } );

			createDefaultStyleParsers( result, GUISection::eStaticStyle, castor::PreviousSection );
			addParserT( result, GUISection::eStaticStyle, cuT( "font" ), &parserStyleStaticFont, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eStaticStyle, cuT( "text_material" ), &parserStyleStaticTextMaterial, { makeParameter< ParameterType::eName >() } );
		}

		static void createThemeParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParserT( result, GUISection::eTheme, cuT( "default_font" ), &parserStyleDefaultFont< GuiContext >, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eTheme, GUISection::eButtonStyle, cuT( "button_style" ), &parserStyleButtonStyle< GuiContext >, { makeDefaultedParameter< ParameterType::eName >( "Button" ) } );
			addParserT( result, GUISection::eTheme, GUISection::eStaticStyle, cuT( "static_style" ), &parserStyleStaticStyle< GuiContext >, { makeDefaultedParameter< ParameterType::eName >( "Static" ) } );
			addParserT( result, GUISection::eTheme, GUISection::eSliderStyle, cuT( "slider_style" ), &parserStyleSliderStyle< GuiContext >, { makeDefaultedParameter< ParameterType::eName >( "Slider" ) } );
			addParserT( result, GUISection::eTheme, GUISection::eComboStyle, cuT( "combobox_style" ), &parserStyleComboBoxStyle< GuiContext >, { makeDefaultedParameter< ParameterType::eName >( "ComboBox" ) } );
			addParserT( result, GUISection::eTheme, GUISection::eListStyle, cuT( "listbox_style" ), &parserStyleListBoxStyle< GuiContext >, { makeDefaultedParameter< ParameterType::eName >( "ListBox" ) } );
			addParserT( result, GUISection::eTheme, GUISection::eEditStyle, cuT( "edit_style" ), &parserStyleEditStyle< GuiContext >, { makeDefaultedParameter< ParameterType::eName >( "Edit" ) } );
			addParserT( result, GUISection::eTheme, GUISection::ePanelStyle, cuT( "panel_style" ), &parserStylePanelStyle< GuiContext >, { makeDefaultedParameter< ParameterType::eName >( "Panel" ) } );
			addParserT( result, GUISection::eTheme, GUISection::eProgressStyle, cuT( "progress_style" ), &parserStyleProgressStyle< GuiContext >, { makeDefaultedParameter< ParameterType::eName >( "Progress" ) } );
			addParserT( result, GUISection::eTheme, GUISection::eExpandablePanelStyle, cuT( "expandable_panel_style" ), &parserStyleExpandablePanelStyle< GuiContext >, { makeDefaultedParameter< ParameterType::eName >( "ExpandablePanel" ) } );
			addParserT( result, GUISection::eTheme, GUISection::eFrameStyle, cuT( "frame_style" ), &parserStyleFrameStyle< GuiContext >, { makeDefaultedParameter< ParameterType::eName >( "Frame" ) } );
			addParserT( result, GUISection::eTheme, GUISection::eScrollBarStyle, cuT( "scrollbar_style" ), &parserStyleScrollBarStyle< GuiContext >, { makeDefaultedParameter< ParameterType::eName >( "ScrollBar" ) } );
			addParserT( result, GUISection::eTheme, castor::PreviousSection, cuT( "}" ), &parserThemeEnd );
		}

		static void createBoxLayoutParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParserT( result, GUISection::eBoxLayout, GUISection::eLayoutCtrl, cuT( "layout_ctrl" ), &parserLayoutCtrl, { makeParameter< ParameterType::eName >() } );
			addParserT( result, GUISection::eBoxLayout, cuT( "layout_staspace" ), &parserBoxLayoutStaticSpacer, { makeParameter< ParameterType::eUInt32 >() } );
			addParserT( result, GUISection::eBoxLayout, cuT( "layout_dynspace" ), &parserBoxLayoutDynamicSpacer );
			addParserT( result, GUISection::eBoxLayout, cuT( "horizontal" ), &parserBoxLayoutHorizontal, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eBoxLayout, castor::PreviousSection, cuT( "}" ), &parserLayoutEnd );
		}

		static void createLayoutCtrlParsers( castor::AttributeParsers & result )
		{
			using namespace castor;
			addParserT( result, GUISection::eLayoutCtrl, cuT( "horizontal_align" ), &parserLayoutCtrlHAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "vertical_align" ), &parserLayoutCtrlVAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "stretch" ), &parserLayoutCtrlStretch, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "reserve_if_hidden" ), &parserLayoutCtrlReserveIfHidden, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "padding" ), &parserLayoutCtrlPadding, { makeParameter< ParameterType::ePoint4U >() } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "pad_left" ), &parserLayoutCtrlPadLeft, { makeParameter< ParameterType::eUInt32 >() } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "pad_top" ), &parserLayoutCtrlPadTop, { makeParameter< ParameterType::eUInt32 >() } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "pad_right" ), &parserLayoutCtrlPadRight, { makeParameter< ParameterType::eUInt32 >() } );
			addParserT( result, GUISection::eLayoutCtrl, cuT( "pad_bottom" ), &parserLayoutCtrlPadBottom, { makeParameter< ParameterType::eUInt32 >() } );
			addParserT( result, GUISection::eLayoutCtrl, castor::PreviousSection, cuT( "}" ), &parserLayoutCtrlEnd );
		}
	}

	//*********************************************************************************************

	ControlRPtr GuiContext::getTopControl()const
	{
		ControlRPtr result{};

		if ( !parents.empty() )
		{
			result = parents.top();
		}

		return result;
	}

	void GuiContext::popControl()
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

	void GuiContext::pushStylesHolder( StylesHolder * style )
	{
		style->setDefaultFont( stylesHolder.top()->getDefaultFont() );
		stylesHolder.push( style );
	}

	void GuiContext::popStylesHolder( StylesHolder const * style )
	{
		if ( style == stylesHolder.top() )
		{
			stylesHolder.pop();
		}
	}

	ControlStyleRPtr GuiContext::getTopStyle()const
	{
		ControlStyleRPtr result{};

		if ( !styles.empty() )
		{
			result = styles.top();
		}

		return result;
	}

	void GuiContext::popStyle()
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

	castor::AttributeParsers createGuiParsers()
	{
		castor::AttributeParsers result;

		addParserT( result, CSCNSection::eRoot, GUISection::eGUI, cuT( "gui" ), &guiparse::parserRootGui );
		addParserT( result, CSCNSection::eScene, GUISection::eGUI, cuT( "gui" ), &guiparse::parserSceneGui );

		guiparse::createGuiParsers( result, CSCNSection::eRoot, &guiparse::parserRootTheme, &guiparse::parserRootGlobalBoxLayout );
		guiparse::createGuiParsers( result, CSCNSection::eScene, &guiparse::parserSceneTheme, &guiparse::parserSceneGlobalBoxLayout );
		guiparse::createGuiParsers( result, GUISection::eGUI, &guiparse::parserGuiTheme, &guiparse::parserGuiGlobalBoxLayout );
		guiparse::createThemeParsers( result );
		guiparse::createButtonParsers( result );
		guiparse::createComboBoxParsers( result );
		guiparse::createEditParsers( result );
		guiparse::createListBoxParsers( result );
		guiparse::createScrollBarParsers( result );
		guiparse::createSliderParsers( result );
		guiparse::createStaticParsers( result );
		guiparse::createPanelParsers( result );
		guiparse::createProgressParsers( result );
		guiparse::createExpandablePanelParsers( result );
		guiparse::createFrameParsers( result );
		guiparse::createBoxLayoutParsers( result );
		guiparse::createLayoutCtrlParsers( result );

		return result;
	}

	castor::StrUInt32Map createGuiSections()
	{
		return
		{
			{ uint32_t( GUISection::eGUI ), cuT( "gui" ) },
			{ uint32_t( GUISection::eTheme ), cuT( "theme" ) },
			{ uint32_t( GUISection::eButtonStyle ), cuT( "button_style" ) },
			{ uint32_t( GUISection::eEditStyle ), cuT( "edit_style" ) },
			{ uint32_t( GUISection::eComboStyle ), cuT( "combobox_style" ) },
			{ uint32_t( GUISection::eListStyle ), cuT( "listbox_style" ) },
			{ uint32_t( GUISection::eSliderStyle ), cuT( "slider_style" ) },
			{ uint32_t( GUISection::eStaticStyle ), cuT( "static_style" ) },
			{ uint32_t( GUISection::ePanelStyle ), cuT( "panel_style" ) },
			{ uint32_t( GUISection::eProgressStyle ), cuT( "progress_style" ) },
			{ uint32_t( GUISection::eExpandablePanelStyle ), cuT( "expandable_panel_style" ) },
			{ uint32_t( GUISection::eFrameStyle ), cuT( "frame_style" ) },
			{ uint32_t( GUISection::eScrollBarStyle ), cuT( "scrollbar_style" ) },
			{ uint32_t( GUISection::eButton ), cuT( "button" ) },
			{ uint32_t( GUISection::eStatic ), cuT( "static" ) },
			{ uint32_t( GUISection::eSlider ), cuT( "slider" ) },
			{ uint32_t( GUISection::eComboBox ), cuT( "combobox" ) },
			{ uint32_t( GUISection::eListBox ), cuT( "listbox" ) },
			{ uint32_t( GUISection::eEdit ), cuT( "edit" ) },
			{ uint32_t( GUISection::ePanel ), cuT( "panel" ) },
			{ uint32_t( GUISection::eProgress ), cuT( "progress" ) },
			{ uint32_t( GUISection::eExpandablePanel ), cuT( "expandable_panel" ) },
			{ uint32_t( GUISection::eExpandablePanelHeader ), cuT( "header" ) },
			{ uint32_t( GUISection::eExpandablePanelExpand ), cuT( "expand" ) },
			{ uint32_t( GUISection::eExpandablePanelContent ), cuT( "content" ) },
			{ uint32_t( GUISection::eFrame ), cuT( "frame" ) },
			{ uint32_t( GUISection::eFrameContent ), cuT( "content" ) },
			{ uint32_t( GUISection::eBoxLayout ), cuT( "box_layout" ) },
			{ uint32_t( GUISection::eLayoutCtrl ), cuT( "layout_ctrl" ) },
		};
	}
}
