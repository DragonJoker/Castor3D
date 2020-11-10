#include "CastorTestParser/Aui/AuiToolBarArt.hpp"

#include <wx/dc.h>

namespace test_parser
{
	AuiToolBarArt::AuiToolBarArt()
		: wxAuiDefaultToolBarArt()
	{
	}

	wxAuiToolBarArt * AuiToolBarArt::Clone()
	{
		return new AuiToolBarArt( *this );
	}

	void AuiToolBarArt::DrawBackground( wxDC & dc
		, wxWindow * window
		, wxRect const & rect )
	{
#if wxCHECK_VERSION( 2, 9, 0 )

		dc.SetPen( wxPen( INACTIVE_TAB_COLOUR, 1, wxPENSTYLE_SOLID ) );
		dc.SetBrush( wxBrush( INACTIVE_TAB_COLOUR, wxBRUSHSTYLE_SOLID ) );

#else

		dc.SetPen( wxPen( INACTIVE_TAB_COLOUR, 1, wxSOLID ) );
		dc.SetBrush( wxBrush( INACTIVE_TAB_COLOUR, wxSOLID ) );

#endif

		dc.DrawRectangle( rect );
	}

	void AuiToolBarArt::DrawPlainBackground( wxDC & dc
		, wxWindow * window
		, wxRect const & rect )
	{
#if wxCHECK_VERSION( 2, 9, 0 )

		dc.SetPen( wxPen( INACTIVE_TAB_COLOUR, 1, wxPENSTYLE_SOLID ) );
		dc.SetBrush( wxBrush( INACTIVE_TAB_COLOUR, wxBRUSHSTYLE_SOLID ) );

#else

		dc.SetPen( wxPen( INACTIVE_TAB_COLOUR, 1, wxSOLID ) );
		dc.SetBrush( wxBrush( INACTIVE_TAB_COLOUR, wxSOLID ) );

#endif

		dc.DrawRectangle( rect );
	}

	void AuiToolBarArt::DrawSeparator( wxDC & dc
		, wxWindow * window
		, wxRect const & rect )
	{
#if wxCHECK_VERSION( 2, 9, 0 )

		dc.SetPen( wxPen( BORDER_COLOUR, 1, wxPENSTYLE_SOLID ) );

#else

		dc.SetPen( wxPen( BORDER_COLOUR, 1, wxSOLID ) );

#endif
		dc.DrawLine( ( rect.GetBottomLeft() + rect.GetBottomRight() ) / 2, ( rect.GetTopLeft() + rect.GetTopRight() ) / 2 );
	}
}
