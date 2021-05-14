#include "GuiCommon/Properties/PropertiesHolder.hpp"

#include "GuiCommon/Aui/AuiDockArt.hpp"

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	PropertiesHolder::PropertiesHolder( wxWindow * parent
		, wxPoint const & position
		, wxSize const & size )
		: wxPanel( parent, wxID_ANY, position, size )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
	{
		m_auiManager.SetArtProvider( new AuiDockArt );
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_auiManager.Update();
	}

	PropertiesHolder::~PropertiesHolder()
	{
		m_auiManager.UnInit();
	}

	void PropertiesHolder::setGrid( wxPropertyGrid * grid )
	{
		if ( m_grid )
		{
			m_auiManager.DetachPane( m_grid );
		}

		m_grid = grid;

		if ( m_grid )
		{
			m_auiManager.AddPane( m_grid
				, wxAuiPaneInfo().Name( wxT( "Grid" ) )
				.CaptionVisible( false )
				.Center()
				.CloseButton( false )
				.Name( wxT( "Render" ) )
				.Layer( 0 )
				.Movable( false )
				.PaneBorder( false )
				.Dockable( false ) );
		}

		m_auiManager.Update();
	}
}
