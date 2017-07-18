#include "ShaderDialog.hpp"

#include "AuiDockArt.hpp"
#include "AuiTabArt.hpp"
#include "AuiToolBarArt.hpp"
#include "StcTextEditor.hpp"
#include "FrameVariablesList.hpp"
#include "PropertiesHolder.hpp"
#include "ShaderEditorPage.hpp"

#include <Engine.hpp>
#include <Material/Pass.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Scene/Scene.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Technique/RenderTechnique.hpp>
#include <Technique/RenderTechniquePass.hpp>

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

	ShaderDialog::ShaderDialog( Scene & p_scene
		, bool p_bCanEdit
		, wxWindow * p_parent
		, Pass & p_pass
		, wxPoint const & p_position
		, const wxSize p_size )
		: wxFrame( p_parent, wxID_ANY, _( "Shaders" ), p_position, p_size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX )
		, m_pass( p_pass )
		, m_bCompiled( true )
		, m_bOwnShader( true )
		, m_pStcContext( std::make_unique< StcContext >() )
#if defined( NDEBUG )
		, m_bCanEdit( p_bCanEdit )
#else
		, m_bCanEdit( p_bCanEdit || true )
#endif
		, m_scene( p_scene )
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
		if ( m_shaderProgram.lock() )
		{
			m_bOwnShader = false;
		}
		else
		{
			auto lock = Castor::make_unique_lock( m_scene.GetEngine()->GetRenderWindowCache() );
			auto it = m_scene.GetEngine()->GetRenderWindowCache().begin();

			if ( it != m_scene.GetEngine()->GetRenderWindowCache().end() && it->second->GetRenderTarget() )
			{
				RenderTechniqueSPtr technique = it->second->GetRenderTarget()->GetTechnique();

				if ( technique )
				{
					m_shaderProgram = m_scene.GetEngine()->GetShaderProgramCache().GetAutomaticProgram( technique->GetOpaquePass()
						, m_pass.GetTextureFlags()
						, m_pass.GetProgramFlags()
						, m_scene.GetFlags()
						, m_pass.GetAlphaFunc()
						, false );
					m_bOwnShader = true;
				}
			}
		}

		PathArray arrayFiles;
		File::ListDirectoryFiles( Engine::GetDataDirectory() / cuT( "Castor3D" ), arrayFiles, true );

		for ( auto pathFile : arrayFiles )
		{
			if ( pathFile.GetFileName()[0] != cuT( '.' ) && pathFile.GetExtension() == cuT( "lang" ) )
			{
				m_pStcContext->ParseFile( pathFile );
			}
		}
	}

	void ShaderDialog::DoInitialiseLayout()
	{
		wxSize size = GetClientSize();
		m_pNotebookEditors = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_pNotebookEditors->SetArtProvider( new AuiTabArt );

		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_pNotebookEditors, wxAuiPaneInfo().CaptionVisible( false ).Name( wxT( "Shaders" ) ).Caption( _( "Shaders" ) ).CenterPane().Dock().MinSize( size ).Layer( 1 ).PaneBorder( false ) );
		m_auiManager.Update();
	}

	void ShaderDialog::DoLoadPages()
	{
		ShaderProgramSPtr program = m_shaderProgram.lock();
		wxArrayString arrayTexts;
		arrayTexts.push_back( _( "Vertex" ) );
		arrayTexts.push_back( _( "Hull" ) );
		arrayTexts.push_back( _( "Domain" ) );
		arrayTexts.push_back( _( "Geometry" ) );
		arrayTexts.push_back( _( "Pixel" ) );
		arrayTexts.push_back( _( "Compute" ) );

		for ( uint8_t i = 0; i < uint8_t( ShaderType::eCount ); i++ )
		{
			if ( program->HasObject( ShaderType( i ) ) )
			{
				wxArrayString arrayChoices;
				arrayChoices.push_back( wxCOMBO_NEW );

				// The editor page
				m_pEditorPages[i] = new ShaderEditorPage( m_bCanEdit
					, *m_pStcContext
					, program
					, ShaderType( i )
					, m_pass
					, m_scene
					, m_pNotebookEditors );
				m_pEditorPages[i]->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
				m_pEditorPages[i]->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
				m_pNotebookEditors->AddPage( m_pEditorPages[i], arrayTexts[i], true );
				m_pEditorPages[i]->SetSize( 0, 22, m_pNotebookEditors->GetClientSize().x, m_pNotebookEditors->GetClientSize().y - 22 );
			}
		}
	}

	void ShaderDialog::DoPopulateMenu()
	{
		wxMenuBar * pMenuBar = new wxMenuBar;
		pMenuBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		pMenuBar->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxMenu * pMenu = new wxMenu;

		if ( m_bCanEdit )
		{
			pMenu->Append( eID_MENU_OPEN, _( "&Open file\tCTRL+O" ) );
			pMenu->Append( eID_MENU_SAVE_ONE, _( "Save &current file\tCTRL+S" ) );
			pMenu->Append( eID_MENU_SAVE_ALL, _( "Save &all files\tCTRL+SHIFT+S" ) );
			pMenu->AppendSeparator();
		}

		pMenu->Append( eID_MENU_QUIT, _( "&Quit\tCTRL+Q" ) );
		pMenuBar->Append( pMenu, _T( "&File" ) );
		pMenu = new wxMenu;
		pMenu->Append( eID_MENU_PREFS, _( "&Edit preferences ...\tCTRL+E" ) );
		pMenuBar->Append( pMenu, _T( "O&ptions" ) );
		SetMenuBar( pMenuBar );
	}

	void ShaderDialog::DoCleanup()
	{
		m_auiManager.DetachPane( m_pNotebookEditors );
		m_pNotebookEditors->DeleteAllPages();

		if ( m_bOwnShader && !m_shaderProgram.expired() )
		{
			m_shaderProgram.reset();
		}
	}

	void ShaderDialog::DoLoadShader()
	{
		bool cont = true;

		if ( cont && m_pEditorPages[size_t( ShaderType::eVertex )]->GetShaderFile().empty() )
		{
			wxMessageBox( _( "Fill the vertex shader file name" ), _( "ERROR" ) );
			cont = false;
		}

		if ( cont && m_pEditorPages[size_t( ShaderType::ePixel )]->GetShaderFile().empty() )
		{
			wxMessageBox( _( "Fill the fragment file name" ), _( "ERROR" ) );
			cont = false;
		}

		if ( cont )
		{
			if ( m_shaderProgram.expired() )
			{
				m_shaderProgram = m_scene.GetEngine()->GetShaderProgramCache().GetNewProgram( false );
			}

			for ( uint8_t i = uint8_t( ShaderType::eVertex ); i < uint8_t( ShaderType::eCount ); i++ )
			{
				wxString file = m_pEditorPages[i]->GetShaderFile();

				if ( file.empty() )
				{
					m_pEditorPages[i]->SaveFile( false );
					m_shaderProgram.lock()->CreateObject( ShaderType( i ) );
					m_shaderProgram.lock()->SetFile( ShaderType( i ), Path{ ( wxChar const * )file.c_str() } );
				}
			}

			m_bCompiled = true;
		}
	}

	void ShaderDialog::DoFolder( ShaderType p_type )
	{
		wxFileDialog dialog( this, _( "Select a shader program file" ), wxEmptyString, wxEmptyString, wxEmptyString );

		if ( dialog.ShowModal() == wxID_OK )
		{
			m_pEditorPages[size_t( p_type )]->LoadFile( dialog.GetPath() );
			m_bCompiled = false;
		}
	}

	void ShaderDialog::DoSave( Castor3D::ShaderType p_type, bool p_createIfNone )
	{
		m_pEditorPages[size_t( p_type )]->SaveFile( p_createIfNone );
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
		ShaderType type = ShaderType::eCount;

		for ( uint8_t i = 0; i < uint8_t( ShaderType::eCount ) && type == ShaderType::eCount; i++ )
		{
			if ( m_pNotebookEditors->GetPage( i ) == m_pNotebookEditors->GetCurrentPage() )
			{
				type = ShaderType( i );
			}
		}

		if ( type != ShaderType::eCount )
		{
			DoFolder( type );
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
		ShaderType type = ShaderType::eCount;

		for ( uint8_t i = 0; i < uint8_t( ShaderType::eCount ) && type == ShaderType::eCount; i++ )
		{
			if ( m_pNotebookEditors->GetPage( i ) == m_pNotebookEditors->GetCurrentPage() )
			{
				type = ShaderType( i );
			}
		}

		if ( type != ShaderType::eCount )
		{
			DoSave( type, true );
		}

		p_event.Skip();
	}

	void ShaderDialog::OnSaveAll( wxCommandEvent & p_event )
	{
		for ( uint8_t i = 0; i < uint8_t( ShaderType::eCount ); i++ )
		{
			DoSave( ShaderType( i ), false );
		}

		p_event.Skip();
	}

	void ShaderDialog::OnMenuClose( wxCommandEvent & p_event )
	{
		Close();
		p_event.Skip();
	}
}
