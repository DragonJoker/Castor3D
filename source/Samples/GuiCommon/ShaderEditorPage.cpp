#include "GuiCommon/ShaderEditorPage.hpp"

#include "GuiCommon/AuiDockArt.hpp"
#include "GuiCommon/StcTextEditor.hpp"
#include "GuiCommon/FrameVariablesList.hpp"
#include "GuiCommon/PropertiesContainer.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/WindowCache.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Material/Pass.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Technique/RenderTechnique.hpp>
#include <Castor3D/Technique/RenderTechniquePass.hpp>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	ShaderEditorPage::ShaderEditorPage( Engine * engine
		, bool canEdit
		, StcContext & stcContext
		, ashes::ShaderStageFlag type
		, castor::String const & source
		, std::vector< UniformBufferValues > & ubos
		, wxWindow * parent
		, wxPoint const & position
		, const wxSize size )
		: wxPanel( parent, wxID_ANY, position, size )
		, m_stage( type )
		, m_source( make_wxString( source ) )
		, m_ubos{ ubos }
		, m_stcContext( stcContext )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
		, m_canEdit( canEdit )
	{
		doInitialiseShaderLanguage();
		doInitialiseLayout( engine );
		doLoadPage();
	}

	ShaderEditorPage::~ShaderEditorPage()
	{
		doCleanup();
		m_auiManager.UnInit();
	}

	void ShaderEditorPage::doInitialiseShaderLanguage()
	{
	}

	void ShaderEditorPage::doInitialiseLayout( Engine * engine )
	{
		static int constexpr ListWidth = 200;
		wxSize size = GetClientSize();
		// The editor
		m_editor = new StcTextEditor( m_stcContext, this, wxID_ANY, wxDefaultPosition, size );
		m_editor->Show();
#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
		m_editor->AlwaysShowScrollbars( true, true );
#endif

		// The frame variable properties holder
		m_frameVariablesProperties = new PropertiesContainer( m_canEdit, this, wxDefaultPosition, wxSize( ListWidth, 0 ) );
		m_frameVariablesProperties->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_frameVariablesProperties->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_frameVariablesProperties->SetCaptionBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_frameVariablesProperties->SetCaptionTextColour( PANEL_FOREGROUND_COLOUR );
		m_frameVariablesProperties->SetSelectionBackgroundColour( ACTIVE_TAB_COLOUR );
		m_frameVariablesProperties->SetSelectionTextColour( ACTIVE_TEXT_COLOUR );
		m_frameVariablesProperties->SetCellBackgroundColour( INACTIVE_TAB_COLOUR );
		m_frameVariablesProperties->SetCellTextColour( INACTIVE_TEXT_COLOUR );
		m_frameVariablesProperties->SetLineColour( BORDER_COLOUR );
		m_frameVariablesProperties->SetMarginColour( BORDER_COLOUR );
//
		// The frame variables list
		m_frameVariablesList = new FrameVariablesList( engine, m_frameVariablesProperties, this, wxPoint( 0, 25 ), wxSize( ListWidth, 0 ) );
		m_frameVariablesList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_frameVariablesList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_frameVariablesList->Enable( m_canEdit );

		// Put all that in the AUI manager
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_editor
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.CloseButton( false )
				.Name( wxT( "Shaders" ) )
				.Caption( _( "Shaders" ) )
				.Center()
				.Layer( 0 )
				.Movable( false )
				.PaneBorder( false )
				.Dockable( false ) );
		m_auiManager.AddPane( m_frameVariablesList
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.CloseButton( false )
				.Name( wxT( "FrameVariablesList" ) )
				.Caption( _( "Frame variables" ) )
				.Right()
				.Dock()
				.LeftDockable()
				.RightDockable()
				.Movable()
				.PinButton()
				.Layer( 2 )
				.PaneBorder( false )
				.MinSize( ListWidth, 0 ) );
		m_auiManager.AddPane( m_frameVariablesProperties
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.CloseButton( false )
				.Name( wxT( "Properties" ) )
				.Caption( _( "Properties" ) )
				.Right()
				.Dock()
				.LeftDockable()
				.RightDockable()
				.Movable()
				.PinButton()
				.Layer( 2 )
				.PaneBorder( false )
				.MinSize( ListWidth, 0 ) );
		m_auiManager.Update();
	}

	void ShaderEditorPage::doLoadPage()
	{
		wxString extension = wxT( ".glsl" );
		wxArrayString arrayChoices;
		arrayChoices.push_back( wxCOMBO_NEW );
		m_editor->setText( m_source );
		m_editor->SetReadOnly( !m_canEdit );
		m_editor->initializePrefs( m_editor->determinePrefs( extension ) );

		// Load frame variables list
		m_frameVariablesList->loadVariables( m_stage, m_ubos );
	}

	void ShaderEditorPage::doCleanup()
	{
		m_auiManager.DetachPane( m_editor );
	}

	BEGIN_EVENT_TABLE( ShaderEditorPage, wxPanel )
		EVT_CLOSE( ShaderEditorPage::onClose )
	END_EVENT_TABLE()

	void ShaderEditorPage::onClose( wxCloseEvent & p_event )
	{
		doCleanup();
		p_event.Skip();
	}
}
