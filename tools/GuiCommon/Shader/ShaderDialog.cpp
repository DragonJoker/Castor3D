#include "GuiCommon/Shader/ShaderDialog.hpp"

#include "GuiCommon/Aui/AuiDockArt.hpp"
#include "GuiCommon/Aui/AuiTabArt.hpp"
#include "GuiCommon/Aui/AuiToolBarArt.hpp"
#include "GuiCommon/Shader/StcTextEditor.hpp"
#include "GuiCommon/Shader/FrameVariablesList.hpp"
#include "GuiCommon/Shader/ShaderEditor.hpp"
#include "GuiCommon/Shader/ShaderProgramPage.hpp"
#include "GuiCommon/Properties/PropertiesContainer.hpp"

#include <Castor3D/Engine.hpp>

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	typedef enum eID
	{
		eID_MENU_QUIT,
		eID_MENU_PREFS,
		eID_MENU_LANG,
#if C3D_HasGLSL
		eID_MENU_LANG_GLSL,
#endif
#if GC_HasHLSL
		eID_MENU_LANG_HLSL,
#endif
		eID_MENU_LANG_SPIRV,
		eID_PAGES,
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
	}

	void ShaderDialog::doLoadLanguage( ShaderLanguage language )
	{
		for ( auto page : m_pages )
		{
			page->loadLanguage( language );
		}
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
		m_programs = new wxAuiNotebook( this
			, eID_PAGES
			, wxDefaultPosition
			, wxDefaultSize
			, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH | wxAUI_NB_SCROLL_BUTTONS );
		m_programs->SetArtProvider( new AuiTabArt );

		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_programs
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.Name( wxT( "Programs" ) )
				.Caption( _( "Programs" ) )
				.CenterPane()
				.Dock()
				.MinSize( size )
				.Layer( 1 )
				.PaneBorder( false ) );
		m_auiManager.Update();
	}

	void ShaderDialog::doLoadPages()
	{
		for ( auto & sources : m_sources )
		{
			m_pages.push_back( new ShaderProgramPage( m_engine
				, true
				, *m_stcContext
				, sources
#if C3D_HasGLSL
				, ShaderLanguage::GLSL
#elif GC_HasHLSL
				, ShaderLanguage::HLSL
#else
				, ShaderLanguage::SPIRV
#endif
				, m_programs ) );
			auto & page = *m_pages.back();
			page.SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			page.SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			m_programs->AddPage( &page, sources.name, true );
			page.SetSize( 0, 22, m_programs->GetClientSize().x, m_programs->GetClientSize().y - 22 );
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
#if C3D_HasGLSL
		m_glslRadio = menu->AppendRadioItem( eID_MENU_LANG_GLSL, wxT( "GLSL" ), _( "Use GLSL to display shaders" ) );
#endif
#if GC_HasHLSL
		m_hlslRadio = menu->AppendRadioItem( eID_MENU_LANG_HLSL, wxT( "HLSL" ), _( "Use HLSL to display shaders" ) );
#endif
		m_spirvRadio = menu->AppendRadioItem( eID_MENU_LANG_SPIRV, wxT( "SPIR-V" ), _( "Use SPIR-V to display shaders" ) );
		menu->AppendSeparator();
		menu->Append( eID_MENU_PREFS, _( "&Edit preferences ...\tCTRL+E" ) );
		menuBar->Append( menu, _T( "O&ptions" ) );
		SetMenuBar( menuBar );
	}

	void ShaderDialog::doCleanup()
	{
		m_auiManager.DetachPane( m_programs );
		m_programs->DeleteAllPages();
	}

	BEGIN_EVENT_TABLE( ShaderDialog, wxFrame )
		EVT_CLOSE( ShaderDialog::onClose )
		EVT_MENU( eID_MENU_QUIT, ShaderDialog::onMenuClose )
#if C3D_HasGLSL
		EVT_MENU( eID_MENU_LANG_GLSL, ShaderDialog::onMenuLanguageGLSL )
#endif
#if GC_HasHLSL
		EVT_MENU( eID_MENU_LANG_HLSL, ShaderDialog::onMenuLanguageHLSL )
#endif
		EVT_MENU( eID_MENU_LANG_SPIRV, ShaderDialog::onMenuLanguageSPIRV )
		EVT_MENU( eID_MENU_PREFS, ShaderDialog::onMenuPreferences )
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

#if C3D_HasGLSL
	void ShaderDialog::onMenuLanguageGLSL( wxCommandEvent & event )
	{
		if ( !m_glslRadio->IsCheck() )
		{
			m_hlslRadio->Check( false );
			m_spirvRadio->Check( false );
			doLoadLanguage( ShaderLanguage::GLSL );
		}

		event.Skip();
	}
#endif
#if GC_HasHLSL
	void ShaderDialog::onMenuLanguageHLSL( wxCommandEvent & event )
	{
		if ( !m_hlslRadio->IsCheck() )
		{
			m_glslRadio->Check( false );
			m_spirvRadio->Check( false );
			doLoadLanguage( ShaderLanguage::HLSL );
		}

		event.Skip();
	}
#endif
	void ShaderDialog::onMenuLanguageSPIRV( wxCommandEvent & event )
	{
		if ( !m_spirvRadio->IsCheck() )
		{
			m_glslRadio->Check( false );
			m_hlslRadio->Check( false );
			doLoadLanguage( ShaderLanguage::SPIRV );
		}

		event.Skip();
	}

	void ShaderDialog::onMenuPreferences( wxCommandEvent & event )
	{
		event.Skip();
	}
}
