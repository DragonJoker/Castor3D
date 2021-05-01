#include "Aria/Editor/SceneFileDialog.hpp"

#include "Aria/Aui/AuiDockArt.hpp"
#include "Aria/Aui/AuiTabArt.hpp"
#include "Aria/Aui/AuiToolBarArt.hpp"
#include "Aria/Editor/StcTextEditor.hpp"
#include "Aria/Editor/SceneFileEditor.hpp"

#include <CastorUtils/Data/File.hpp>

#include <wx/dialog.h>

namespace aria
{
	typedef enum eID
	{
		eID_MENU_QUIT,
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
				m_editor->saveFile();
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
		m_editor = new SceneFileEditor{ m_stcContext
			, filename
			, this };
		m_editor->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_editor->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		wxSize size = GetClientSize();
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_editor
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.Name( wxT( "Editor" ) )
				.Caption( _( "Editor" ) )
				.CenterPane()
				.CaptionVisible( false )
				.Dock()
				.MinSize( size )
				.Layer( 1 )
				.PaneBorder( false ) );
		m_auiManager.Update();
	}

	void SceneFileDialog::doPopulateMenu()
	{
		wxMenuBar * menuBar = new wxMenuBar;
		menuBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		menuBar->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		wxMenu * menu = new wxMenu;
		menu->Append( eID_MENU_SAVE, _( "&Save\tCTRL+S" ) );
		menu->AppendSeparator();
		menu->Append( eID_MENU_RUN, _( "&Run\tF5" ) );
		menu->AppendSeparator();
		menu->Append( eID_MENU_QUIT, _( "&Quit\tCTRL+Q" ) );
		menuBar->Append( menu, _T( "&File" ) );

		SetMenuBar( menuBar );
	}

	void SceneFileDialog::doCleanup()
	{
		m_auiManager.DetachPane( m_editor );
	}
}
