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

namespace GuiCommon
{
	namespace dialg
	{
		enum class eID
		{
			MenuQuit,
			MenuPrefs,
			MenuLang,
#if GC_HasGLSL
			MenuLangGLSL,
#endif
#if GC_HasHLSL
			MenuLangHLSL,
#endif
			MenuLangSPIRV,
			Pages,
		};
	}

	ShaderDialog::~ShaderDialog()noexcept = default;

	ShaderDialog::ShaderDialog( c3d::Engine * engine
		, ImagesLoader & imagesLoader
		, ShaderSources sources
		, wxString const & title
		, wxWindow * parent
		, wxPoint const & position
		, const wxSize size )
		: wxFrame( parent, wxID_ANY, title + wxT( " - " ) + _( "Shaders" ), position, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX )
		, m_engine{ engine }
		, m_imagesLoader{ imagesLoader }
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
		, m_stcContext( c3d::makeRawUnique< StcContext >() )
		, m_sources( c3d::move( sources ) )
	{
		doInitialiseShaderLanguage();
		doInitialiseLayout();
		doLoadPages();
		doPopulateMenu();
		wxFrame::Maximize();
	}

	void ShaderDialog::doLoadLanguage( ShaderLanguage language )const
	{
		for ( auto page : m_pages )
		{
			page->loadLanguage( language );
		}
	}

	void ShaderDialog::doInitialiseShaderLanguage()
	{
		c3d::PathArray arrayFiles;
		c3d::File::listDirectoryFiles( c3d::Engine::getDataDirectory() / cuT( "Castor3D" ), arrayFiles, true );

		for ( auto const & pathFile : arrayFiles )
		{
			if ( pathFile.getFileName()[0] != cuT( '.' ) && pathFile.getExtension() == cuT( "lang" ) )
			{
				m_stcContext->parseFile( pathFile );
			}
		}
	}

	void ShaderDialog::doInitialiseLayout()
	{
		wxSize size = wxFrame::GetClientSize();
		m_programs = new wxAuiNotebook( this
			, int( dialg::eID::Pages )
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
				, m_imagesLoader
				, true
				, *m_stcContext
				, sources
#if GC_HasGLSL
				, ShaderLanguage::eGLSL
#elif GC_HasHLSL
				, ShaderLanguage::eHLSL
#else
				, ShaderLanguage::eSPIRV
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
		auto menuBar = new wxMenuBar;
		menuBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		menuBar->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		auto menu = new wxMenu;

		menu->Append( int( dialg::eID::MenuQuit ), _( "&Quit\tCTRL+Q" ) );
		menuBar->Append( menu, _T( "&File" ) );
		menu = new wxMenu;
#if GC_HasGLSL
		m_glslRadio = menu->AppendRadioItem( int( dialg::eID::MenuLangGLSL ), wxT( "GLSL" ), _( "Use GLSL to display shaders" ) );
#endif
#if GC_HasHLSL
		m_hlslRadio = menu->AppendRadioItem( int( dialg::eID::MenuLangHLSL ), wxT( "HLSL" ), _( "Use HLSL to display shaders" ) );
#endif
		m_spirvRadio = menu->AppendRadioItem( int( dialg::eID::MenuLangSPIRV ), wxT( "SPIR-V" ), _( "Use SPIR-V to display shaders" ) );
		menu->AppendSeparator();
		menu->Append( int( dialg::eID::MenuPrefs ), _( "&Edit preferences ...\tCTRL+E" ) );
		menuBar->Append( menu, _T( "O&ptions" ) );
		SetMenuBar( menuBar );
	}

	void ShaderDialog::doCleanup()
	{
		m_auiManager.DetachPane( m_programs );
		m_programs->DeleteAllPages();
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( ShaderDialog, wxFrame )
		EVT_CLOSE( ShaderDialog::onClose )
		EVT_MENU( int( dialg::eID::MenuQuit ), ShaderDialog::onMenuClose )
#if GC_HasGLSL
		EVT_MENU( int( dialg::eID::MenuLangGLSL ), ShaderDialog::onMenuLanguageGLSL )
#endif
#if GC_HasHLSL
		EVT_MENU( int( dialg::eID::MenuLangHLSL ), ShaderDialog::onMenuLanguageHLSL )
#endif
		EVT_MENU( int( dialg::eID::MenuLangSPIRV ), ShaderDialog::onMenuLanguageSPIRV )
		EVT_MENU( int( dialg::eID::MenuPrefs ), ShaderDialog::onMenuPreferences )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void ShaderDialog::onClose( wxCloseEvent & event )
	{
		doCleanup();
		m_auiManager.UnInit();
		event.Skip();
	}

	void ShaderDialog::onMenuClose( wxCommandEvent & event )
	{
		Close();
		event.Skip();
	}

#if GC_HasGLSL
	void ShaderDialog::onMenuLanguageGLSL( wxCommandEvent & event )
	{
		if ( !m_glslRadio->IsCheck() )
		{
			m_hlslRadio->Check( false );
			m_spirvRadio->Check( false );
			doLoadLanguage( ShaderLanguage::eGLSL );
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
			doLoadLanguage( ShaderLanguage::eHLSL );
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
			doLoadLanguage( ShaderLanguage::eSPIRV );
		}

		event.Skip();
	}

	void ShaderDialog::onMenuPreferences( wxCommandEvent & event )
	{
		event.Skip();
	}
}
