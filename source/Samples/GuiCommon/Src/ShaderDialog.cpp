#include "ShaderDialog.hpp"

#include "AuiDockArt.hpp"
#include "AuiTabArt.hpp"
#include "AuiToolBarArt.hpp"
#include "StcTextEditor.hpp"
#include "FrameVariablesList.hpp"
#include "PropertiesContainer.hpp"
#include "ShaderEditorPage.hpp"

#include <Engine.hpp>
#include <Material/Pass.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderWindow.hpp>
#include <Scene/Scene.hpp>
#include <Technique/RenderTechnique.hpp>
#include <Technique/RenderTechniquePass.hpp>

using namespace castor3d;
using namespace castor;
namespace GuiCommon
{
	typedef enum eID
	{
		eID_GRID_VERTEX,
		eID_GRID_FRAGMENT,
		eID_GRID_GEOMETRY,
		eID_GRID_HULL,
		eID_GRID_DOMAIN,
		eID_GRID_COMPUTE,
		eID_MENU_QUIT,
		eID_MENU_PREFS,
		eID_PAGES
	}	eID;

	ShaderDialog::ShaderDialog( Engine * engine
		, ShaderSources && sources
		, wxString const & title
		, wxWindow * parent
		, wxPoint const & position
		, const wxSize size )
		: wxFrame( parent, wxID_ANY, title + wxT( " - " ) + _( "Shaders" ), position, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX )
		, m_engine{ engine }
		, m_sources( std::move( sources ) )
		, m_stcContext( std::make_unique< StcContext >() )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
	{
		doInitialiseShaderLanguage();
		doInitialiseLayout();
		doLoadPages();
		doPopulateMenu();
		this->Maximize();
	}

	ShaderDialog::~ShaderDialog()
	{
		m_auiManager.UnInit();
		m_stcContext.reset();
	}

	void ShaderDialog::doInitialiseShaderLanguage()
	{
		PathArray arrayFiles;
		File::listDirectoryFiles( Engine::getDataDirectory() / cuT( "Castor3D" ), arrayFiles, true );

		for ( auto pathFile : arrayFiles )
		{
			if ( pathFile.getFileName()[0] != cuT( '.' ) && pathFile.getExtension() == cuT( "lang" ) )
			{
				m_stcContext->parseFile( pathFile );
			}
		}
	}

	void ShaderDialog::doInitialiseLayout()
	{
		wxSize size = GetClientSize();
		m_editors = new wxAuiNotebook( this
			, eID_PAGES
			, wxDefaultPosition
			, wxDefaultSize
			, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH | wxAUI_NB_SCROLL_BUTTONS );
		m_editors->SetArtProvider( new AuiTabArt );

		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_editors
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.Name( wxT( "Shaders" ) )
				.Caption( _( "Shaders" ) )
				.CenterPane()
				.Dock()
				.MinSize( size )
				.Layer( 1 )
				.PaneBorder( false ) );
		m_auiManager.Update();
	}

	void ShaderDialog::doLoadPages()
	{
		std::map< renderer::ShaderStageFlag, wxString > const texts
		{
			{ renderer::ShaderStageFlag::eVertex, _( "Vertex" ) },
			{ renderer::ShaderStageFlag::eTessellationControl, _( "Tessellation Control" ) },
			{ renderer::ShaderStageFlag::eTessellationEvaluation, _( "Tessellation Evaluation" ) },
			{ renderer::ShaderStageFlag::eGeometry, _( "Geometry" ) },
			{ renderer::ShaderStageFlag::eFragment, _( "Fragment" ) },
			{ renderer::ShaderStageFlag::eCompute, _( "Compute" ) },
		};

		for ( auto & sources : m_sources )
		{
			for ( auto & source : sources.sources )
			{
				wxArrayString arrayChoices;
				arrayChoices.push_back( wxCOMBO_NEW );

				// The editor page
				m_pages.push_back( new ShaderEditorPage( m_engine
					, true
					, *m_stcContext
					, source.first
					, source.second
					, sources.ubos
					, m_editors ) );
				auto & page = *m_pages.back();
				page.SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
				page.SetForegroundColour( PANEL_FOREGROUND_COLOUR );
				m_editors->AddPage( &page, sources.name + cuT( " - " ) + texts.at( source.first ), true );
				page.SetSize( 0, 22, m_editors->GetClientSize().x, m_editors->GetClientSize().y - 22 );
			}
		}
	}

	void ShaderDialog::doPopulateMenu()
	{
		wxMenuBar * menuBar = new wxMenuBar;
		menuBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		menuBar->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxMenu * menu = new wxMenu;

		menu->Append( eID_MENU_QUIT, _( "&Quit\tCTRL+Q" ) );
		menuBar->Append( menu, _T( "&File" ) );
		menu = new wxMenu;
		menu->Append( eID_MENU_PREFS, _( "&Edit preferences ...\tCTRL+E" ) );
		menuBar->Append( menu, _T( "O&ptions" ) );
		SetMenuBar( menuBar );
	}

	void ShaderDialog::doCleanup()
	{
		m_auiManager.DetachPane( m_editors );
		m_editors->DeleteAllPages();
	}

	BEGIN_EVENT_TABLE( ShaderDialog, wxFrame )
		EVT_CLOSE( ShaderDialog::onClose )
		EVT_MENU( eID_MENU_QUIT, ShaderDialog::onMenuClose )
		EVT_AUINOTEBOOK_PAGE_CHANGED( eID_PAGES, ShaderDialog::onPageChanged )
	END_EVENT_TABLE()

	void ShaderDialog::onClose( wxCloseEvent & event )
	{
		doCleanup();
		event.Skip();
	}

	void ShaderDialog::onMenuClose( wxCommandEvent & event )
	{
		Close();
		event.Skip();
	}

	void ShaderDialog::onPageChanged( wxAuiNotebookEvent & event )
	{
		event.Skip();
	}
}
