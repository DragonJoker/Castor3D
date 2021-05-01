#include "Aria/Editor/SceneFileEditor.hpp"

#include "Aria/Aui/AuiDockArt.hpp"
#include "Aria/Editor/StcTextEditor.hpp"

namespace aria
{
	SceneFileEditor::SceneFileEditor( StcContext & stcContext
		, wxString const & filename
		, wxWindow * parent
		, wxPoint const & position
		, const wxSize size )
		: wxPanel( parent, wxID_ANY, position, size )
		, m_stcContext( stcContext )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
	{
		doInitialiseLayout( filename );
		Bind( wxEVT_CLOSE_WINDOW
			, [this]( wxCloseEvent & event )
			{
				doCleanup();
				event.Skip();
			} );
	}

	SceneFileEditor::~SceneFileEditor()
	{
		doCleanup();
		m_auiManager.UnInit();
	}

	bool SceneFileEditor::saveFile()
	{
		return m_editor->saveFile();
	}

	void SceneFileEditor::doInitialiseLayout( wxString const & filename )
	{
		static int constexpr ListWidth = 200;
		wxSize size = GetClientSize();
		// The editor
		m_editor = new StcTextEditor( m_stcContext, this, wxID_ANY, wxDefaultPosition, size );
		m_editor->Show();
#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
		m_editor->AlwaysShowScrollbars( true, true );
#endif
		m_editor->loadFile( filename );
		m_editor->SetReadOnly( false );

		// Put all that in the AUI manager
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_editor
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.CloseButton( false )
				.Name( wxT( "Scene File" ) )
				.Caption( _( "Scene File" ) )
				.CaptionVisible( false )
				.Center()
				.Layer( 0 )
				.Movable( false )
				.PaneBorder( false )
				.Dockable( false ) );
		m_auiManager.Update();
	}

	void SceneFileEditor::doCleanup()
	{
		m_auiManager.DetachPane( m_editor );
	}
}
