#include "ShaderDialog.hpp"

#include "AuiDockArt.hpp"
#include "AuiTabArt.hpp"
#include "AuiToolBarArt.hpp"
#include "FrameVariableDialog.hpp"
#include "StcTextEditor.hpp"
#include "Parameter.hpp"
#include "FrameVariablesList.hpp"
#include "PropertiesHolder.hpp"

#include <Engine.hpp>
#include <RenderSystem.hpp>
#include <Pass.hpp>
#include <RenderWindow.hpp>
#include <RenderTarget.hpp>
#include <ShaderManager.hpp>
#include <ShaderProgram.hpp>

using namespace Castor3D;
using namespace Castor;
namespace GuiCommon
{
	typedef enum eID
	{
		eID_GRID_VERTEX,
		eID_GRID_FRAGMENT,
		eID_GRID_GEOMETRY,
		eID_GRID_HULL,
		eID_GRID_DOMAIN,
		eID_MENU_OPEN,
		eID_MENU_SAVE_ONE,
		eID_MENU_SAVE_ALL,
		eID_MENU_QUIT,
		eID_MENU_PREFS,
	}	eID;

	ShaderDialog::ShaderDialog( Engine * p_pEngine, bool p_bCanEdit, wxWindow * p_pParent, PassSPtr p_pPass, wxPoint const & p_ptPosition, const wxSize p_ptSize )
		: wxFrame( p_pParent, wxID_ANY, _( "Shaders" ), p_ptPosition, p_ptSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX )
		, m_pPass( p_pPass )
		, m_bCompiled( true )
		, m_bOwnShader( true )
		, m_pStcContext( std::make_unique< StcContext >() )
#if defined( NDEBUG )
		, m_bCanEdit( p_bCanEdit )
#else
		, m_bCanEdit( p_bCanEdit || true )
#endif
		, m_pEngine( p_pEngine )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
	{
		eSHADER_MODEL l_maxShaderModel = DoInitialiseShaderLanguage();

		if ( l_maxShaderModel != eSHADER_MODEL_COUNT )
		{
			DoInitialiseLayout();
			DoLoadPages( l_maxShaderModel );
			DoPopulateMenu();
			this->Maximize();
		}
	}

	ShaderDialog::~ShaderDialog()
	{
		m_auiManager.UnInit();
		m_pStcContext.reset();
	}

	eSHADER_MODEL ShaderDialog::DoInitialiseShaderLanguage()
	{
		eSHADER_MODEL l_eMaxShaderModel = eSHADER_MODEL_COUNT;
		RenderSystem * l_pRenderSystem = m_pEngine->GetRenderSystem();

		if ( l_pRenderSystem->CheckSupport( eSHADER_MODEL_5 ) )
		{
			l_eMaxShaderModel = eSHADER_MODEL_5;
		}
		else if ( l_pRenderSystem->CheckSupport( eSHADER_MODEL_4 ) )
		{
			l_eMaxShaderModel = eSHADER_MODEL_4;
		}
		else if ( l_pRenderSystem->CheckSupport( eSHADER_MODEL_3 ) )
		{
			l_eMaxShaderModel = eSHADER_MODEL_3;
		}
		else if ( l_pRenderSystem->CheckSupport( eSHADER_MODEL_2 ) )
		{
			l_eMaxShaderModel = eSHADER_MODEL_2;
		}
		else if ( l_pRenderSystem->CheckSupport( eSHADER_MODEL_1 ) )
		{
			l_eMaxShaderModel = eSHADER_MODEL_1;
		}

		if ( l_eMaxShaderModel != eSHADER_MODEL_COUNT )
		{
			m_pShaderProgram = m_pPass.lock()->GetShader< ShaderProgramBase >();

			if ( m_pShaderProgram.lock() )
			{
				m_bOwnShader = false;
			}
			else
			{
				PassSPtr l_pass = m_pPass.lock();
				Engine * l_engine = l_pass->GetEngine();
				auto && l_it = l_engine->RenderWindowsBegin();

				if ( l_it != l_engine->RenderWindowsEnd() && l_it->second->GetRenderTarget() )
				{
					RenderTechniqueBaseSPtr l_technique = l_it->second->GetRenderTarget()->GetTechnique();

					if ( l_technique )
					{
						m_pShaderProgram = l_pRenderSystem->GetEngine()->GetShaderManager().GetAutomaticProgram( *l_technique, l_pass->GetTextureFlags(), 0 );
						m_bOwnShader = true;
					}
				}
			}
		}

		PathArray l_arrayFiles;
		File::ListDirectoryFiles( Engine::GetDataDirectory() / cuT( "Castor3D" ), l_arrayFiles, true );

		for( auto && l_pathFile : l_arrayFiles )
		{
			if ( l_pathFile.GetFileName()[0] != cuT( '.' ) && l_pathFile.GetExtension() == cuT( "lang" ) )
			{
				m_pStcContext->ParseFile( l_pathFile );
			}
		}

		return l_eMaxShaderModel;
	}

	void ShaderDialog::DoInitialiseLayout()
	{
		wxSize l_size = GetClientSize();
		m_pNotebookEditors = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_pNotebookEditors->SetArtProvider( new AuiTabArt );

		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_pNotebookEditors, wxAuiPaneInfo().CaptionVisible( false ).Name( wxT( "Shaders" ) ).Caption( _( "Shaders" ) ).CenterPane().Dock().MinSize( l_size ).Layer( 1 ).PaneBorder( false ) );
		m_auiManager.Update();
	}

	void ShaderDialog::DoLoadPages( eSHADER_MODEL p_maxModel )
	{
		int l_iListWidth = 200;
		wxPoint l_ptButtonPosition = wxPoint( 10, 10 );
		ShaderProgramBaseSPtr l_program = m_pShaderProgram.lock();
		wxArrayString l_arrayTexts;
		l_arrayTexts.push_back( _( "Vertex" ) );
		l_arrayTexts.push_back( _( "Hull" ) );
		l_arrayTexts.push_back( _( "Domain" ) );
		l_arrayTexts.push_back( _( "Geometry" ) );
		l_arrayTexts.push_back( _( "Pixel" ) );
		l_arrayTexts.push_back( _( "Compute" ) );
		wxString l_strExtension;

		switch ( l_program->GetLanguage() )
		{
		case Castor3D::eSHADER_LANGUAGE_GLSL:
			l_strExtension = wxT( ".glsl" );
			break;

		case Castor3D::eSHADER_LANGUAGE_HLSL:
			l_strExtension = wxT( ".hlsl" );
			break;
		}

		for ( int i = 0; i < eSHADER_TYPE_COUNT; i++ )
		{
			wxArrayString l_arrayChoices;
			l_arrayChoices.push_back( wxCOMBO_NEW );

			if ( l_program->HasProgram( eSHADER_TYPE( i ) ) )
			{
				// Add shader specific frame variables to the list
				int l_iCount = 0;

				for ( auto l_it : l_program->GetFrameVariables( eSHADER_TYPE( i ) ) )
				{
					l_arrayChoices.push_back( l_it->GetName() );
					m_mapFrameVariables[i].insert( std::make_pair( l_iCount++, l_it ) );
				}

				// Load the shader source file/text
				int l_shaderModel = p_maxModel;

				while ( m_strShaderSources[i].empty() && m_strShaderFiles[i].empty() && l_shaderModel >= eSHADER_MODEL_1 )
				{
					m_strShaderSources[i] = l_program->GetSource( eSHADER_TYPE( i ), eSHADER_MODEL( l_shaderModel ) );
					m_strShaderFiles[i] = l_program->GetFile( eSHADER_TYPE( i ), eSHADER_MODEL( l_shaderModel ) );

					if ( !m_strShaderSources[i].empty() || !m_strShaderFiles[i].empty() )
					{
						// Stop the loop as soon as we've got one of source or file
						l_shaderModel = eSHADER_MODEL_1;
					}

					--l_shaderModel;
				}
			}

			// The panel that holds the editor + the shader specific variables list
			wxPanel * l_panel = new wxPanel( m_pNotebookEditors, wxID_ANY, wxPoint( 0, 0 ) );
			m_pNotebookEditors->AddPage( l_panel, l_arrayTexts[i], true );
			l_panel->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			l_panel->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			l_panel->SetSize( 0, 22, m_pNotebookEditors->GetClientSize().x, m_pNotebookEditors->GetClientSize().y - 22 );

			// The editor
			m_pStcEditors[i] = new StcTextEditor( *m_pStcContext, l_panel, wxID_ANY, wxPoint( l_iListWidth, 0 ), l_panel->GetClientSize() - wxSize( l_iListWidth, 0 ) );
			m_pStcEditors[i]->Show();
#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
			m_pStcEditors[i]->AlwaysShowScrollbars( true, true );
#endif
			// Load the shader
			if ( !m_strShaderFiles[i].empty() )
			{
				// file
				m_pStcEditors[i]->LoadFile( m_strShaderFiles[i] );
				m_pStcEditors[i]->SetReadOnly( !m_bCanEdit );
			}
			else if ( !m_strShaderSources[i].empty() )
			{
				// or source (read only, then)
				m_pStcEditors[i]->SetText( m_strShaderSources[i] );
				m_pStcEditors[i]->SetReadOnly( true );
			}
			// Initialise the editor
			m_pStcEditors[i]->InitializePrefs( m_pStcEditors[i]->DeterminePrefs( l_strExtension ) );

			// The frame variable properties holder
			m_pPropsFrameVariables[i] = new PropertiesHolder( m_bCanEdit, l_panel, wxDefaultPosition, wxSize( l_iListWidth, 0 ) );
			m_pPropsFrameVariables[i]->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			m_pPropsFrameVariables[i]->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			m_pPropsFrameVariables[i]->SetCaptionBackgroundColour( PANEL_BACKGROUND_COLOUR );
			m_pPropsFrameVariables[i]->SetCaptionTextColour( PANEL_FOREGROUND_COLOUR );
			m_pPropsFrameVariables[i]->SetSelectionBackgroundColour( ACTIVE_TAB_COLOUR );
			m_pPropsFrameVariables[i]->SetSelectionTextColour( ACTIVE_TEXT_COLOUR );
			m_pPropsFrameVariables[i]->SetCellBackgroundColour( INACTIVE_TAB_COLOUR );
			m_pPropsFrameVariables[i]->SetCellTextColour( INACTIVE_TEXT_COLOUR );
			m_pPropsFrameVariables[i]->SetLineColour( BORDER_COLOUR );
			m_pPropsFrameVariables[i]->SetMarginColour( BORDER_COLOUR );

			// The frame variables list
			wxStaticText * l_pTmpStatic = new wxStaticText( l_panel, wxID_ANY, _( "Frame variables" ), wxPoint( 0, 0 ), wxSize( l_iListWidth, 25 ), wxALIGN_CENTRE );
			m_pListFrameVariables[i] = new FrameVariablesList( m_pPropsFrameVariables[i], l_panel, wxPoint( 0, 25 ), wxSize( l_iListWidth, 0 ) );
			m_pListFrameVariables[i]->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			m_pListFrameVariables[i]->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			m_pListFrameVariables[i]->LoadVariables( eSHADER_TYPE( i ), l_program );
			m_pListFrameVariables[i]->Enable( m_bCanEdit );
			l_ptButtonPosition.y += 20;

			// Set a sizer for this couple
			wxBoxSizer * l_pSizerTabList = new wxBoxSizer( wxVERTICAL );
			l_pSizerTabList->Add( l_pTmpStatic, wxSizerFlags( 0 ) );
			l_pSizerTabList->Add( m_pListFrameVariables[i], wxSizerFlags( 1 ).Expand() );
			l_pSizerTabList->Add( m_pPropsFrameVariables[i], wxSizerFlags( 1 ).Expand() );
			wxBoxSizer * l_pSizerTab = new wxBoxSizer( wxHORIZONTAL );
			l_pSizerTab->Add( m_pStcEditors[i],	wxSizerFlags( 1 ).Expand() );
			l_pSizerTab->Add( l_pSizerTabList,	wxSizerFlags( 0 ).Expand() );
			l_panel->SetSizer( l_pSizerTab );
			l_pSizerTab->SetSizeHints( l_panel );
		}
	}

	void ShaderDialog::DoPopulateMenu()
	{
		wxMenuBar * l_pMenuBar = new wxMenuBar;
		l_pMenuBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		l_pMenuBar->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxMenu * l_pMenu = new wxMenu;

		if ( m_bCanEdit )
		{
			l_pMenu->Append( eID_MENU_OPEN, _( "&Open file\tCTRL+O" ) );
			l_pMenu->Append( eID_MENU_SAVE_ONE, _( "Save &current file\tCTRL+S" ) );
			l_pMenu->Append( eID_MENU_SAVE_ALL, _( "Save &all files\tCTRL+SHIFT+S" ) );
			l_pMenu->AppendSeparator();
		}

		l_pMenu->Append( eID_MENU_QUIT, _( "&Quit\tCTRL+Q" ) );
		l_pMenuBar->Append( l_pMenu, _T( "&File" ) );
		l_pMenu = new wxMenu;
		l_pMenu->Append( eID_MENU_PREFS, _( "&Edit preferences ...\tCTRL+E" ) );
		l_pMenuBar->Append( l_pMenu, _T( "O&ptions" ) );
		SetMenuBar( l_pMenuBar );
	}

	void ShaderDialog::DoCleanup()
	{
		m_auiManager.DetachPane( m_pNotebookEditors );

		if ( m_bOwnShader && !m_pShaderProgram.expired() )
		{
			m_pPass.lock()->SetShader( nullptr );
			//m_pEngine->GetShaderManager().RemoveProgram( m_pShaderProgram.lock() );
			m_pShaderProgram.reset();
		}
	}

	void ShaderDialog::DoLoadShader()
	{
		if ( m_strShaderFiles[eSHADER_TYPE_VERTEX].empty() )
		{
			wxMessageBox( _( "Fill the vertex shader file name" ), _( "ERROR" ) );
		}
		else
		{
			if ( m_strShaderFiles[eSHADER_TYPE_PIXEL].empty() )
			{
				wxMessageBox( _( "Fill the fragment file name" ), _( "ERROR" ) );
			}
			else
			{
				if ( m_pShaderProgram.expired() )
				{
					m_pShaderProgram = m_pEngine->GetShaderManager().GetNewProgram( eSHADER_LANGUAGE_GLSL );
				}

				for ( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++ )
				{
					if ( !m_strShaderFiles[i].empty() )
					{
						m_pStcEditors[i]->SaveFile( m_strShaderFiles[i] );
						m_pShaderProgram.lock()->CreateObject( eSHADER_TYPE( i ) );
						m_pShaderProgram.lock()->SetFile( eSHADER_TYPE( i ), eSHADER_MODEL_3, ( wxChar const * )m_strShaderFiles[i].c_str() );
					}
				}

				m_pPass.lock()->SetShader( m_pShaderProgram.lock() );
				m_bCompiled = true;
			}
		}
	}

	void ShaderDialog::DoFolder( eSHADER_TYPE p_eType )
	{
		wxFileDialog l_dialog( this, _( "Select a shader program file" ), wxEmptyString, wxEmptyString, wxEmptyString );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			m_strShaderFiles[p_eType] = l_dialog.GetPath();
			m_pStcEditors[p_eType]->LoadFile( m_strShaderFiles[p_eType] );
			m_bCompiled = false;
		}
	}

	void ShaderDialog::DoSave( Castor3D::eSHADER_TYPE p_eType, bool p_bTell )
	{
		if ( m_strShaderFiles[p_eType].empty() && p_bTell )
		{
			wxString l_wildcard;

			switch ( m_pShaderProgram.lock()->GetLanguage() )
			{
			case Castor3D::eSHADER_LANGUAGE_GLSL:
				l_wildcard = _( "GLSL Files" );
				l_wildcard += wxT( " (*.glsl;*.frag;*.vert;*.geom;*.ctrl;*.eval)|*.glsl;*.frag;*.vert;*.geom;*.ctrl;*.eval" );
				break;

			case Castor3D::eSHADER_LANGUAGE_HLSL:
				l_wildcard = _( "HLSL Files" );
				l_wildcard += wxT( " (*.hlsl)|*.hlsl" );
				break;
			}

			wxFileDialog l_dialog( this, _( "Save Shader file " ), wxEmptyString, wxEmptyString, l_wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

			if ( l_dialog.ShowModal() == wxID_OK )
			{
				m_strShaderFiles[p_eType] = l_dialog.GetPath();
			}
		}

		if ( !m_strShaderFiles[p_eType].empty() )
		{
			m_pStcEditors[p_eType]->SaveFile( m_strShaderFiles[p_eType] );
			m_pStcEditors[p_eType]->SetReadOnly( !m_bCanEdit );
		}
	}

	BEGIN_EVENT_TABLE( ShaderDialog, wxFrame )
		EVT_CLOSE( ShaderDialog::OnClose )
		EVT_MENU( eID_MENU_OPEN, ShaderDialog::OnOpenFile )
		EVT_MENU( eID_MENU_SAVE_ONE, ShaderDialog::OnSaveFile )
		EVT_MENU( eID_MENU_SAVE_ALL, ShaderDialog::OnSaveAll )
		EVT_MENU( eID_MENU_QUIT, ShaderDialog::OnMenuClose )
	END_EVENT_TABLE()

	void ShaderDialog::OnOpenFile( wxCommandEvent & p_event )
	{
		eSHADER_TYPE l_eType = eSHADER_TYPE_COUNT;

		for ( int i = 0; i < eSHADER_TYPE_COUNT && l_eType == eSHADER_TYPE_COUNT; i++ )
		{
			if ( m_pNotebookEditors->GetPage( i ) == m_pNotebookEditors->GetCurrentPage() )
			{
				l_eType = eSHADER_TYPE( i );
			}
		}

		if ( l_eType != eSHADER_TYPE_COUNT )
		{
			DoFolder( l_eType );
		}

		p_event.Skip();
	}

	void ShaderDialog::OnLoadShader( wxCommandEvent & p_event )
	{
		DoLoadShader();
		p_event.Skip();
	}

	void ShaderDialog::OnClose( wxCloseEvent & p_event )
	{
		DoCleanup();
		p_event.Skip();
	}

	void ShaderDialog::OnSaveFile( wxCommandEvent & p_event )
	{
		eSHADER_TYPE l_eType = eSHADER_TYPE_COUNT;

		for ( int i = 0; i < eSHADER_TYPE_COUNT && l_eType == eSHADER_TYPE_COUNT; i++ )
		{
			if ( m_pNotebookEditors->GetPage( i ) == m_pNotebookEditors->GetCurrentPage() )
			{
				l_eType = eSHADER_TYPE( i );
			}
		}

		if ( l_eType != eSHADER_TYPE_COUNT )
		{
			DoSave( l_eType, true );
		}

		p_event.Skip();
	}

	void ShaderDialog::OnSaveAll( wxCommandEvent & p_event )
	{
		eSHADER_TYPE l_eType = eSHADER_TYPE_COUNT;

		for ( int i = 0; i < eSHADER_TYPE_COUNT && l_eType == eSHADER_TYPE_COUNT; i++ )
		{
			DoSave( eSHADER_TYPE( i ), false );
		}

		p_event.Skip();
	}

	void ShaderDialog::OnMenuClose( wxCommandEvent & p_event )
	{
		Close();
		p_event.Skip();
	}
}
