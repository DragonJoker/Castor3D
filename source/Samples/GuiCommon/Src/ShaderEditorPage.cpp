#include "ShaderEditorPage.hpp"

#include "AuiDockArt.hpp"
#include "StcTextEditor.hpp"
#include "FrameVariablesList.hpp"
#include "PropertiesHolder.hpp"

#include <Render/RenderSystem.hpp>
#include <Shader/ShaderProgram.hpp>

using namespace Castor3D;
using namespace Castor;
namespace GuiCommon
{
	ShaderEditorPage::ShaderEditorPage( bool p_bCanEdit, StcContext & p_stcContext, Castor3D::ShaderProgramSPtr p_shader, Castor3D::ShaderType p_type, wxWindow * p_parent, wxPoint const & p_position, const wxSize p_size )
		: wxPanel( p_parent, wxID_ANY, p_position, p_size )
		, m_shaderProgram( p_shader )
		, m_stcContext( p_stcContext )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
		, m_shaderType( p_type )
		, m_shaderModel( ShaderModel::Count )
#if defined( NDEBUG )
		, m_canEdit( p_bCanEdit )
#else
		, m_canEdit( p_bCanEdit || true )
#endif
	{
		DoInitialiseShaderLanguage();

		if ( m_shaderModel != ShaderModel::Count )
		{
			DoInitialiseLayout();
			DoLoadPage();
		}
	}

	ShaderEditorPage::~ShaderEditorPage()
	{
		m_auiManager.UnInit();
	}

	bool ShaderEditorPage::LoadFile( wxString const & p_file )
	{
		m_shaderFile = p_file;
		return m_editor->LoadFile( m_shaderFile );
	}

	void ShaderEditorPage::SaveFile( bool p_createIfNone )
	{
		ShaderProgramSPtr l_program = m_shaderProgram.lock();

		if ( m_shaderFile.empty() && p_createIfNone )
		{
			wxString l_wildcard;
			l_wildcard = _( "GLSL Files" );
			l_wildcard += wxT( " (*.glsl;*.frag;*.vert;*.geom;*.ctrl;*.eval)|*.glsl;*.frag;*.vert;*.geom;*.ctrl;*.eval" );

			wxFileDialog l_dialog( this, _( "Save Shader file " ), wxEmptyString, wxEmptyString, l_wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

			if ( l_dialog.ShowModal() == wxID_OK )
			{
				m_shaderFile = l_dialog.GetPath();
			}
		}

		if ( !m_shaderFile.empty() )
		{
			m_editor->SaveFile( m_shaderFile );
			m_editor->SetReadOnly( !m_canEdit );
		}
	}

	void ShaderEditorPage::DoInitialiseShaderLanguage()
	{
		m_shaderModel = ShaderModel::Count;
		ShaderProgramSPtr l_program = m_shaderProgram.lock();
		RenderSystem * l_renderSystem = l_program->GetRenderSystem();
		m_shaderModel = l_program->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
	}

	void ShaderEditorPage::DoInitialiseLayout()
	{
		const int l_iListWidth = 200;
		wxSize l_size = GetClientSize();
		// The editor
		m_editor = new StcTextEditor( m_stcContext, this, wxID_ANY, wxPoint( l_iListWidth, 0 ), l_size - wxSize( l_iListWidth, 0 ) );
		m_editor->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_editor->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_editor->Show();
#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
		m_editor->AlwaysShowScrollbars( true, true );
#endif

		// The frame variable properties holder
		m_frameVariablesProperties = new PropertiesHolder( m_canEdit, this, wxDefaultPosition, wxSize( l_iListWidth, 0 ) );
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

		// The frame variables list
		m_frameVariablesList = new FrameVariablesList( m_frameVariablesProperties, this, wxPoint( 0, 25 ), wxSize( l_iListWidth, 0 ) );
		m_frameVariablesList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_frameVariablesList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_frameVariablesList->Enable( m_canEdit );

		// Put all that in the AUI manager
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_editor, wxAuiPaneInfo().CaptionVisible( false ).CloseButton( false ).Name( wxT( "Shaders" ) ).Caption( _( "Shaders" ) ).Center().Layer( 0 ).Movable( false ).PaneBorder( false ).Dockable( false ) );
		m_auiManager.AddPane( m_frameVariablesList, wxAuiPaneInfo().CaptionVisible( false ).CloseButton( false ).Name( wxT( "FrameVariablesList" ) ).Caption( _( "Frame variables" ) ).Right().Dock().LeftDockable().RightDockable().Movable().PinButton().Layer( 2 ).PaneBorder( false ).MinSize( l_iListWidth, 0 ) );
		m_auiManager.AddPane( m_frameVariablesProperties, wxAuiPaneInfo().CaptionVisible( false ).CloseButton( false ).Name( wxT( "Properties" ) ).Caption( _( "Properties" ) ).Right().Dock().LeftDockable().RightDockable().Movable().PinButton().Layer( 2 ).PaneBorder( false ).MinSize( l_iListWidth, 0 ) );
		m_auiManager.Update();
	}

	void ShaderEditorPage::DoLoadPage()
	{
		ShaderProgramSPtr l_program = m_shaderProgram.lock();
		wxString l_extension = wxT( ".glsl" );

		wxArrayString l_arrayChoices;
		l_arrayChoices.push_back( wxCOMBO_NEW );

		if ( l_program->GetObjectStatus( m_shaderType ) != ShaderStatus::DontExist )
		{
			// Load the shader source file/text
			uint8_t l_shaderModel = uint8_t( m_shaderModel );

			while ( m_shaderSource.empty() && m_shaderFile.empty() && ShaderModel( l_shaderModel  )>= ShaderModel::Model1 )
			{
				m_shaderSource = l_program->GetSource( m_shaderType, ShaderModel( l_shaderModel ) );
				m_shaderFile = l_program->GetFile( m_shaderType, ShaderModel( l_shaderModel ) );

				if ( !m_shaderSource.empty() || !m_shaderFile.empty() )
				{
					// Stop the loop as soon as we've got one of source or file
					l_shaderModel = uint8_t( ShaderModel::Model1 );
				}

				--l_shaderModel;
			}
		}

		// Load the shader in the editor
		if ( !m_shaderFile.empty() )
		{
			// file
			m_editor->LoadFile( m_shaderFile );
			m_editor->SetReadOnly( !m_canEdit );
		}
		else if ( !m_shaderSource.empty() )
		{
			// or source (read only, then)
			m_editor->SetText( m_shaderSource );
			m_editor->SetReadOnly( true );
		}

		// Initialise the editor
		m_editor->InitializePrefs( m_editor->DeterminePrefs( l_extension ) );

		// Load frame variables list
		m_frameVariablesList->LoadVariables( m_shaderType, l_program );
	}

	BEGIN_EVENT_TABLE( ShaderEditorPage, wxPanel )
		EVT_CLOSE( ShaderEditorPage::OnClose )
	END_EVENT_TABLE()

	void ShaderEditorPage::OnClose( wxCloseEvent & p_event )
	{
		m_auiManager.DetachPane( m_editor );
		m_auiManager.DetachPane( m_frameVariablesList );
		m_auiManager.DetachPane( m_frameVariablesProperties );
		p_event.Skip();
	}
}
