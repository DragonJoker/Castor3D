#include "ShaderDialog.hpp"

#include "AuiDockArt.hpp"
#include "AuiTabArt.hpp"
#include "AuiToolBarArt.hpp"
#include "StcTextEditor.hpp"
#include "Parameter.hpp"
#include "FrameVariablesList.hpp"
#include "PropertiesHolder.hpp"
#include "ShaderEditorPage.hpp"

#include <Engine.hpp>
#include <RenderSystem.hpp>
#include <Pass.hpp>
#include <RenderWindow.hpp>
#include <RenderTarget.hpp>
#include <ShaderManager.hpp>
#include <ShaderProgram.hpp>
#include <WindowManager.hpp>

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

	ShaderDialog::ShaderDialog( Engine * p_engine, bool p_bCanEdit, wxWindow * p_parent, PassSPtr p_pPass, wxPoint const & p_ptPosition, const wxSize p_ptSize )
		: wxFrame( p_parent, wxID_ANY, _( "Shaders" ), p_ptPosition, p_ptSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX )
		, m_pPass( p_pPass )
		, m_bCompiled( true )
		, m_bOwnShader( true )
		, m_pStcContext( std::make_unique< StcContext >() )
#if defined( NDEBUG )
		, m_bCanEdit( p_bCanEdit )
#else
		, m_bCanEdit( p_bCanEdit || true )
#endif
		, m_engine( p_engine )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
	{
		DoInitialiseShaderLanguage();
		DoInitialiseLayout();
		DoLoadPages();
		DoPopulateMenu();
		this->Maximize();
	}

	ShaderDialog::~ShaderDialog()
	{
		m_auiManager.UnInit();
		m_pStcContext.reset();
	}

	void ShaderDialog::DoInitialiseShaderLanguage()
	{
		m_shaderProgram = m_pPass.lock()->GetShader< ShaderProgramBase >();

		if ( m_shaderProgram.lock() )
		{
			m_bOwnShader = false;
		}
		else
		{
			PassSPtr l_pass = m_pPass.lock();
			Engine * l_engine = l_pass->GetOwner();
			auto l_lock = Castor::make_unique_lock( l_engine->GetWindowManager() );
			auto && l_it = l_engine->GetWindowManager().begin();

			if ( l_it != l_engine->GetWindowManager().end() && l_it->second->GetRenderTarget() )
			{
				RenderTechniqueBaseSPtr l_technique = l_it->second->GetRenderTarget()->GetTechnique();

				if ( l_technique )
				{
					m_shaderProgram = m_engine->GetShaderManager().GetAutomaticProgram( *l_technique, l_pass->GetTextureFlags(), 0 );
					m_bOwnShader = true;
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

	void ShaderDialog::DoLoadPages()
	{
		ShaderProgramBaseSPtr l_program = m_shaderProgram.lock();
		wxArrayString l_arrayTexts;
		l_arrayTexts.push_back( _( "Vertex" ) );
		l_arrayTexts.push_back( _( "Hull" ) );
		l_arrayTexts.push_back( _( "Domain" ) );
		l_arrayTexts.push_back( _( "Geometry" ) );
		l_arrayTexts.push_back( _( "Pixel" ) );
		l_arrayTexts.push_back( _( "Compute" ) );

		for ( int i = 0; i < eSHADER_TYPE_COUNT; i++ )
		{
			wxArrayString l_arrayChoices;
			l_arrayChoices.push_back( wxCOMBO_NEW );

			// The editor page
			m_pEditorPages[i] = new ShaderEditorPage( m_bCanEdit, *m_pStcContext, l_program, eSHADER_TYPE( i ), m_pNotebookEditors );
			m_pEditorPages[i]->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			m_pEditorPages[i]->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			m_pNotebookEditors->AddPage( m_pEditorPages[i], l_arrayTexts[i], true );
			m_pEditorPages[i]->SetSize( 0, 22, m_pNotebookEditors->GetClientSize().x, m_pNotebookEditors->GetClientSize().y - 22 );
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

		if ( m_bOwnShader && !m_shaderProgram.expired() )
		{
			m_pPass.lock()->SetShader( nullptr );
			//m_engine->GetShaderManager().RemoveProgram( m_shaderProgram.lock() );
			m_shaderProgram.reset();
		}
	}

	void ShaderDialog::DoLoadShader()
	{
		bool l_continue = true;

		if ( l_continue && m_pEditorPages[eSHADER_TYPE_VERTEX]->GetShaderFile().empty() )
		{
			wxMessageBox( _( "Fill the vertex shader file name" ), _( "ERROR" ) );
			l_continue = false;
		}

		if ( l_continue && m_pEditorPages[eSHADER_TYPE_PIXEL]->GetShaderFile().empty() )
		{
			wxMessageBox( _( "Fill the fragment file name" ), _( "ERROR" ) );
			l_continue = false;
		}

		if ( l_continue )
		{
			if ( m_shaderProgram.expired() )
			{
				m_shaderProgram = m_engine->GetShaderManager().GetNewProgram( eSHADER_LANGUAGE_GLSL );
			}

			for ( int i = eSHADER_TYPE_VERTEX; i < eSHADER_TYPE_COUNT; i++ )
			{
				wxString l_file = m_pEditorPages[i]->GetShaderFile();

				if ( l_file.empty() )
				{
					m_pEditorPages[i]->SaveFile( false );
					m_shaderProgram.lock()->CreateObject( eSHADER_TYPE( i ) );
					m_shaderProgram.lock()->SetFile( eSHADER_TYPE( i ), m_pEditorPages[i]->GetShaderModel(), ( wxChar const * )l_file.c_str() );
				}
			}

			m_pPass.lock()->SetShader( m_shaderProgram.lock() );
			m_bCompiled = true;
		}
	}

	void ShaderDialog::DoFolder( eSHADER_TYPE p_type )
	{
		wxFileDialog l_dialog( this, _( "Select a shader program file" ), wxEmptyString, wxEmptyString, wxEmptyString );

		if ( l_dialog.ShowModal() == wxID_OK )
		{
			m_pEditorPages[p_type]->LoadFile( l_dialog.GetPath() );
			m_bCompiled = false;
		}
	}

	void ShaderDialog::DoSave( Castor3D::eSHADER_TYPE p_type, bool p_createIfNone )
	{
		m_pEditorPages[p_type]->SaveFile( p_createIfNone );
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
		eSHADER_TYPE l_type = eSHADER_TYPE_COUNT;

		for ( int i = 0; i < eSHADER_TYPE_COUNT && l_type == eSHADER_TYPE_COUNT; i++ )
		{
			if ( m_pNotebookEditors->GetPage( i ) == m_pNotebookEditors->GetCurrentPage() )
			{
				l_type = eSHADER_TYPE( i );
			}
		}

		if ( l_type != eSHADER_TYPE_COUNT )
		{
			DoFolder( l_type );
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
		eSHADER_TYPE l_type = eSHADER_TYPE_COUNT;

		for ( int i = 0; i < eSHADER_TYPE_COUNT && l_type == eSHADER_TYPE_COUNT; i++ )
		{
			if ( m_pNotebookEditors->GetPage( i ) == m_pNotebookEditors->GetCurrentPage() )
			{
				l_type = eSHADER_TYPE( i );
			}
		}

		if ( l_type != eSHADER_TYPE_COUNT )
		{
			DoSave( l_type, true );
		}

		p_event.Skip();
	}

	void ShaderDialog::OnSaveAll( wxCommandEvent & p_event )
	{
		eSHADER_TYPE l_type = eSHADER_TYPE_COUNT;

		for ( int i = 0; i < eSHADER_TYPE_COUNT && l_type == eSHADER_TYPE_COUNT; i++ )
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
