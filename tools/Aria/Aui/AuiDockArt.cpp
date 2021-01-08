#include "Aria/Aui/AuiDockArt.hpp"

#include <wx/dc.h>

namespace aria
{
	AuiDockArt::AuiDockArt()
		: wxAuiDefaultDockArt()
	{
		wxAuiDefaultDockArt::SetMetric( wxAuiPaneDockArtSetting::wxAUI_DOCKART_PANE_BORDER_SIZE, 0 );
		wxAuiDefaultDockArt::SetMetric( wxAuiPaneDockArtSetting::wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE );
		wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_BACKGROUND_COLOUR, INACTIVE_TAB_COLOUR );
		wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_BORDER_COLOUR, BORDER_COLOUR );
		wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_GRIPPER_COLOUR, wxColour( 127, 127, 127 ) );
		wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_SASH_COLOUR, INACTIVE_TAB_COLOUR );
		wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, INACTIVE_TAB_COLOUR );
		wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, INACTIVE_TAB_COLOUR );
		wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR, ACTIVE_TAB_COLOUR );
		wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR, ACTIVE_TAB_COLOUR );
		wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, INACTIVE_TEXT_COLOUR );
		wxAuiDefaultDockArt::SetColour( wxAuiPaneDockArtSetting::wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR, ACTIVE_TEXT_COLOUR );
	}

	void AuiDockArt::DrawBackground( wxDC & dc
		, wxWindow * window
		, wxRect const & rect )
	{
#if wxCHECK_VERSION( 2, 9, 0 )

		dc.SetPen( wxPen( PANEL_BACKGROUND_COLOUR, 1, wxPENSTYLE_SOLID ) );
		dc.SetBrush( wxBrush( PANEL_BACKGROUND_COLOUR, wxBRUSHSTYLE_SOLID ) );

#else

		dc.SetPen( wxPen( PANEL_BACKGROUND_COLOUR, 1, wxSOLID ) );
		dc.SetBrush( wxBrush( PANEL_BACKGROUND_COLOUR, wxSOLID ) );

#endif

		dc.DrawRectangle( rect );
	}
}
