#include "GuiCommon/Aui/AuiToolBarArt.hpp"

namespace GuiCommon
{
	AuiToolBarArt::AuiToolBarArt()
		: wxAuiDefaultToolBarArt()
	{
	}

	wxAuiToolBarArt * AuiToolBarArt::Clone()
	{
		return new AuiToolBarArt( *this );
	}

	void AuiToolBarArt::DrawBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect )
	{
#if wxCHECK_VERSION( 2, 9, 0 )

		p_dc.SetPen( wxPen( INACTIVE_TAB_COLOUR, 1, wxPENSTYLE_SOLID ) );
		p_dc.SetBrush( wxBrush( INACTIVE_TAB_COLOUR, wxBRUSHSTYLE_SOLID ) );

#else

		p_dc.SetPen( wxPen( INACTIVE_TAB_COLOUR, 1, wxSOLID ) );
		p_dc.SetBrush( wxBrush( INACTIVE_TAB_COLOUR, wxSOLID ) );

#endif

		p_dc.DrawRectangle( p_rect );
	}

	void AuiToolBarArt::DrawPlainBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect )
	{
#if wxCHECK_VERSION( 2, 9, 0 )

		p_dc.SetPen( wxPen( INACTIVE_TAB_COLOUR, 1, wxPENSTYLE_SOLID ) );
		p_dc.SetBrush( wxBrush( INACTIVE_TAB_COLOUR, wxBRUSHSTYLE_SOLID ) );

#else

		p_dc.SetPen( wxPen( INACTIVE_TAB_COLOUR, 1, wxSOLID ) );
		p_dc.SetBrush( wxBrush( INACTIVE_TAB_COLOUR, wxSOLID ) );

#endif

		p_dc.DrawRectangle( p_rect );
	}

	void AuiToolBarArt::DrawSeparator( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect )
	{
#if wxCHECK_VERSION( 2, 9, 0 )

		p_dc.SetPen( wxPen( BORDER_COLOUR, 1, wxPENSTYLE_SOLID ) );

#else

		p_dc.SetPen( wxPen( BORDER_COLOUR, 1, wxSOLID ) );

#endif
		p_dc.DrawLine( ( p_rect.GetBottomLeft() + p_rect.GetBottomRight() ) / 2, ( p_rect.GetTopLeft() + p_rect.GetTopRight() ) / 2 );
	}
}
