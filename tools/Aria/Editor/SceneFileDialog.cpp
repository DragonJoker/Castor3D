#include "Aria/Editor/SceneFileDialog.hpp"

#include "Aria/Aui/AuiDockArt.hpp"
#include "Aria/Aui/AuiTabArt.hpp"
#include "Aria/Aui/AuiToolBarArt.hpp"
#include "Aria/Editor/StcTextEditor.hpp"
#include "Aria/Editor/SceneFileEditor.hpp"

#include <CastorUtils/Data/File.hpp>

#include <wx/dialog.h>
#include <wx/filedlg.h>
#include <wx/filename.h>

namespace aria
{
	typedef enum eID
	{
		eID_PAGES,
		eID_MENU_QUIT,
		eID_MENU_OPEN,
		eID_MENU_CLOSE,
		eID_MENU_SAVE,
		eID_MENU_RUN,
	}	eID;

	SceneFileDialog::SceneFileDialog( Config const & config
		, wxString const & filename
		, wxString const & title
		, wxWindow * parent
		, wxPoint const & position
		, const wxSize size )
		: wxFrame{ parent
			, wxID_ANY
			, _( "Scene File" ) + wxT( " - " ) + title
			, position
			, size
			, wxDEFAULT_FRAME_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxFRAME_FLOAT_ON_PARENT }
		, m_config{ config }
		, m_filename{ filename }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
	{
		doLoadLanguage();
		doInitialiseLayout( filename );
		doPopulateMenu();

		Bind( wxEVT_CLOSE_WINDOW
			, [this]( wxCloseEvent & event )
			{
				doCleanup();
				event.Skip();
			} );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				Close();
				event.Skip();
			}
			, eID_MENU_QUIT );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				doCloseFile();
				event.Skip();
			}
			, eID_MENU_CLOSE );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				doOpenFile();
				event.Skip();
			}
			, eID_MENU_OPEN );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				doSaveFile();
				event.Skip();
			}
		, eID_MENU_SAVE );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				wxString command = m_config.viewer;
				command << " " << m_filename
					<< " -l 0"
					<< " -a";
				auto result = wxExecute( command, wxEXEC_SYNC );
				event.Skip();
			}
			, eID_MENU_RUN );
	}

	SceneFileDialog::~SceneFileDialog()
	{
		m_auiManager.UnInit();
	}

	void SceneFileDialog::doLoadLanguage()
	{
		castor::Path pathBin = castor::File::getExecutableDirectory();

		while ( pathBin.getFileName() != cuT( "bin" ) )
		{
			pathBin = pathBin.getPath();
		}

		castor::PathArray arrayFiles;
		castor::File::listDirectoryFiles( pathBin.getPath() / cuT( "share" ) / cuT( "Aria" ), arrayFiles, true );

		for ( auto pathFile : arrayFiles )
		{
			if ( pathFile.getFileName()[0] != cuT( '.' ) && pathFile.getExtension() == cuT( "lang" ) )
			{
				m_stcContext.parseFile( pathFile );
			}
		}
	}

	void SceneFileDialog::doInitialiseLayout( wxString const & filename )
	{
		m_editors = new wxAuiNotebook( this
			, eID_PAGES
			, wxDefaultPosition
			, wxDefaultSize
			, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH | wxAUI_NB_SCROLL_BUTTONS );
		m_editors->SetArtProvider( new AuiTabArt );

		wxSize size = GetClientSize();
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_editors
			, wxAuiPaneInfo()
			.CaptionVisible( false )
			.Name( wxT( "Files" ) )
			.Caption( _( "Files" ) )
			.CenterPane()
			.Dock()
			.MinSize( size )
			.Layer( 1 )
			.PaneBorder( false ) );
		m_auiManager.Update();

		doLoadPage( filename );
	}

	void SceneFileDialog::doPopulateMenu()
	{
		wxMenuBar * menuBar = new wxMenuBar;
		menuBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		menuBar->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		wxMenu * menu = new wxMenu;
		menu->Append( eID_MENU_OPEN, _( "&Open File" ) + wxT( "\tCTRL+O" ) );
		menu->AppendSeparator();
		menu->Append( eID_MENU_CLOSE, _( "Close File" ) + wxT( "\tCTRL+W" ) );
		menu->AppendSeparator();
		menu->Append( eID_MENU_SAVE, _( "&Save" ) + wxT( "\tCTRL+S" ) );
		menu->AppendSeparator();
		menu->Append( eID_MENU_RUN, _( "&Run" ) + wxT( "\tF5" ) );
		menu->AppendSeparator();
		menu->Append( eID_MENU_QUIT, _( "&Quit" ) + wxT( "\tCTRL+Q" ) );
		menuBar->Append( menu, _T( "&File" ) );

		SetMenuBar( menuBar );
	}

	void SceneFileDialog::doCleanup()
	{
		m_auiManager.DetachPane( m_editors );
		m_editors->DeleteAllPages();
	}

	void SceneFileDialog::doOpenFile()
	{
		static const wxString CSCN_WILDCARD = wxT( " (*.cscn)|*.cscn|" );

		wxFileName fileName{ m_filename };
		auto path = fileName.GetPath();
		wxString wildcard;
		wildcard << _( "Castor3D scene file" );
		wildcard << CSCN_WILDCARD;

		wxFileDialog fileDialog{ this, _( "Open a scene" )
			, path
			, wxEmptyString
			, wildcard
			, wxFD_OPEN | wxFD_FILE_MUST_EXIST };

		if ( fileDialog.ShowModal() == wxID_OK )
		{
			doLoadPage( fileDialog.GetPath() );
		}
	}

	void SceneFileDialog::doCloseFile()
	{
		m_editors->DeletePage( m_editors->GetSelection() );

		if ( m_editors->GetPageCount() == 0 )
		{
			Close();
		}
	}

	void SceneFileDialog::doSaveFile()
	{
		auto editor = static_cast< SceneFileEditor * >( m_editors->GetPage( m_editors->GetSelection() ) );

		if ( editor )
		{
			editor->saveFile();
		}
	}

	void SceneFileDialog::doLoadPage( wxString const & filename )
	{
		wxFileName fileName{ filename };
		auto page = new SceneFileEditor{ m_stcContext
			, filename
			, this };
		page->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		page->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_editors->AddPage( page, fileName.GetName(), true );
		auto size = m_editors->GetClientSize();
		page->SetSize( 0, 22, size.x, size.y - 22 );
	}
}
