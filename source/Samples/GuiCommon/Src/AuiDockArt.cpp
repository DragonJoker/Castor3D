#include "AuiDockArt.hpp"

namespace GuiCommon
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

	void AuiDockArt::DrawBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect )
	{
		p_dc.SetPen( wxPen( PANEL_BACKGROUND_COLOUR, 1, wxSOLID ) );
		p_dc.SetBrush( wxBrush( PANEL_BACKGROUND_COLOUR, wxSOLID ) );
		p_dc.DrawRectangle( p_rect );
	}
}
