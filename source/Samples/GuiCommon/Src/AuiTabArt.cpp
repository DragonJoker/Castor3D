#include "AuiTabArt.hpp"

#include <wx/aui/auibook.h>
#include <wx/renderer.h>

namespace GuiCommon
{
	namespace
	{
		void IndentPressedBitmap( wxRect * rect, int button_state )
		{
			if ( button_state == wxAUI_BUTTON_STATE_PRESSED )
			{
				rect->x++;
				rect->y++;
			}
		}

		static wxString AuiChopText( wxDC & dc, const wxString & text, int max_size )
		{
			wxCoord x, y;
			// first check if the text fits with no problems
			dc.GetTextExtent( text, &x, &y );

			if ( x <= max_size )
			{
				return text;
			}

			size_t i, len = text.Length();
			size_t last_good_length = 0;

			for ( i = 0; i < len; ++i )
			{
				wxString s = text.Left( i );
				s += wxT( "..." );
				dc.GetTextExtent( s, &x, &y );

				if ( x > max_size )
				{
					break;
				}

				last_good_length = i;
			}

			wxString ret = text.Left( last_good_length );
			ret += wxT( "..." );
			return ret;
		}
	}

	AuiTabArt::AuiTabArt()
	{
		wxAuiDefaultTabArt::SetColour( INACTIVE_TAB_COLOUR );
		wxAuiDefaultTabArt::SetActiveColour( ACTIVE_TAB_COLOUR );
		wxAuiDefaultTabArt::SetMeasuringFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false ) );
		wxAuiDefaultTabArt::SetNormalFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false ) );
		wxAuiDefaultTabArt::SetSelectedFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false ) );
	}

	wxAuiTabArt * AuiTabArt::Clone()
	{
		return new AuiTabArt( *this );
	}

	void AuiTabArt::DrawBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect )
	{
		p_dc.SetPen( m_baseColourPen );
		p_dc.SetBrush( m_baseColourBrush );
		p_dc.DrawRectangle( p_rect );
	}

	// DrawTab() draws an individual tab.
	//
	// dc       - output dc
	// in_rect  - rectangle the tab should be confined to
	// caption  - tab's caption
	// active   - whether or not the tab is active
	// out_rect - actual output rectangle
	// x_extent - the advance x; where the next tab should start
	void AuiTabArt::DrawTab( wxDC & dc, wxWindow * wnd, const wxAuiNotebookPage & page, const wxRect & in_rect, int close_button_state, wxRect * out_tab_rect, wxRect * out_button_rect, int * x_extent )
	{
		wxCoord normal_textx, normal_texty;
		wxCoord selected_textx, selected_texty;
		wxCoord texty;

		// if the caption is empty, measure some temporary text
		wxString caption = page.caption;

		if ( caption.empty() )
		{
			caption = wxT( "Xj" );
		}

		dc.SetFont( m_selectedFont );
		dc.GetTextExtent( caption, &selected_textx, &selected_texty );

		dc.SetFont( m_normalFont );
		dc.GetTextExtent( caption, &normal_textx, &normal_texty );

		// figure out the size of the tab
		wxSize tab_size = GetTabSize( dc, wnd, page.caption, page.bitmap, page.active, close_button_state, x_extent );

		wxCoord tab_height = m_tabCtrlHeight - 3;
		wxCoord tab_width = tab_size.x;
		wxCoord tab_x = in_rect.x;
		wxCoord tab_y = in_rect.y + in_rect.height - tab_height;

		caption = page.caption;

		// select pen, brush and font for the tab to be drawn
		if ( page.active )
		{
			dc.SetFont( m_selectedFont );
			texty = selected_texty;
		}
		else
		{
			dc.SetFont( m_normalFont );
			texty = normal_texty;
		}


		// create points that will make the tab outline

		int clip_width = tab_width;

		if ( tab_x + clip_width > in_rect.x + in_rect.width )
		{
			clip_width = ( in_rect.x + in_rect.width ) - tab_x;
		}

		wxPoint clip_points[6];
		clip_points[0] = wxPoint( tab_x,              tab_y + tab_height - 3 );
		clip_points[1] = wxPoint( tab_x,              tab_y + 2 );
		clip_points[2] = wxPoint( tab_x + 2,            tab_y );
		clip_points[3] = wxPoint( tab_x + clip_width - 1, tab_y );
		clip_points[4] = wxPoint( tab_x + clip_width + 1, tab_y + 2 );
		clip_points[5] = wxPoint( tab_x + clip_width + 1, tab_y + tab_height - 3 );

#if !defined(__WXDFB__) && !defined(__WXCOCOA__)
		// since the above code above doesn't play well with WXDFB or WXCOCOA,
		// we'll just use a rectangle for the clipping region for now --
		dc.SetClippingRegion( tab_x, tab_y, clip_width + 1, tab_height - 3 );
#endif

		wxPoint border_points[6];

		if ( m_flags & wxAUI_NB_BOTTOM )
		{
			border_points[0] = wxPoint( tab_x,             tab_y );
			border_points[1] = wxPoint( tab_x,             tab_y + tab_height - 6 );
			border_points[2] = wxPoint( tab_x + 2,           tab_y + tab_height - 4 );
			border_points[3] = wxPoint( tab_x + tab_width - 2, tab_y + tab_height - 4 );
			border_points[4] = wxPoint( tab_x + tab_width,   tab_y + tab_height - 6 );
			border_points[5] = wxPoint( tab_x + tab_width,   tab_y );
		}
		else //if (m_flags & wxAUI_NB_TOP) {}
		{
			border_points[0] = wxPoint( tab_x,             tab_y + tab_height - 4 );
			border_points[1] = wxPoint( tab_x,             tab_y + 2 );
			border_points[2] = wxPoint( tab_x + 2,           tab_y );
			border_points[3] = wxPoint( tab_x + tab_width - 2, tab_y );
			border_points[4] = wxPoint( tab_x + tab_width,   tab_y + 2 );
			border_points[5] = wxPoint( tab_x + tab_width,   tab_y + tab_height - 4 );
		}

		// TODO: else if (m_flags &wxAUI_NB_LEFT) {}
		// TODO: else if (m_flags &wxAUI_NB_RIGHT) {}

		int drawn_tab_yoff = border_points[1].y;
		int drawn_tab_height = border_points[0].y - border_points[1].y;

		if ( page.active )
		{
			// draw active tab

			// draw base background color
			wxRect r( tab_x, tab_y, tab_width, tab_height );
			dc.SetPen( wxPen( m_activeColour ) );
			dc.SetBrush( wxBrush( m_activeColour ) );
			dc.DrawRectangle( r.x + 1, r.y + 1, r.width - 1, r.height - 4 );

			// these two points help the rounded corners appear more antialiased
			dc.SetPen( wxPen( m_activeColour ) );
			dc.DrawPoint( r.x + 2, r.y + 1 );
			dc.DrawPoint( r.x + r.width - 2, r.y + 1 );
		}
		else
		{
			// draw inactive tab

			// draw base background color
			wxRect r( tab_x, tab_y, tab_width, tab_height );
			dc.SetPen( wxPen( m_baseColour ) );
			dc.SetBrush( wxBrush( m_baseColour ) );
			dc.DrawRectangle( r.x + 1, r.y + 1, r.width - 1, r.height - 4 );
		}

		//// draw tab outline
		//dc.SetPen( m_borderPen );
		//dc.SetBrush( *wxTRANSPARENT_BRUSH );
		//dc.DrawPolygon( WXSIZEOF( border_points ), border_points );

		// there are two horizontal grey lines at the bottom of the tab control,
		// this gets rid of the top one of those lines in the tab control
		if ( page.active )
		{
			if ( m_flags & wxAUI_NB_BOTTOM )
			{
				dc.SetPen( wxPen( m_activeColour.ChangeLightness( 170 ) ) );
			}
			// TODO: else if (m_flags &wxAUI_NB_LEFT) {}
			// TODO: else if (m_flags &wxAUI_NB_RIGHT) {}
			else //for wxAUI_NB_TOP
			{
				dc.SetPen( m_activeColour );
			}

			dc.DrawLine( border_points[0].x + 1,
						 border_points[0].y,
						 border_points[5].x,
						 border_points[5].y );
		}


		int text_offset = tab_x + 8;
		int close_button_width = 0;

		if ( close_button_state != wxAUI_BUTTON_STATE_HIDDEN )
		{
			close_button_width = m_activeCloseBmp.GetWidth();
		}

		int bitmap_offset = 0;
		text_offset = tab_x + 8;

		wxString draw_text = AuiChopText( dc, caption, tab_width - ( text_offset - tab_x ) - close_button_width );

		// draw tab text
		if ( page.active )
		{
			dc.SetTextForeground( ACTIVE_TEXT_COLOUR );
		}
		else
		{
			dc.SetTextForeground( INACTIVE_TEXT_COLOUR );
		}

		dc.SetFont( m_normalFont );
		dc.DrawText( draw_text, text_offset, drawn_tab_yoff + ( drawn_tab_height ) / 2 - ( texty / 2 ) );

		// draw focus rectangle
		if ( page.active && ( wnd->FindFocus() == wnd ) )
		{
			wxRect focusRectText( text_offset, ( drawn_tab_yoff + ( drawn_tab_height ) / 2 - ( texty / 2 ) - 1 ), selected_textx, selected_texty );

			wxRect focusRect;
			wxRect focusRectBitmap;

			if ( page.bitmap.IsOk() )
			{
				focusRectBitmap = wxRect( bitmap_offset, drawn_tab_yoff + ( drawn_tab_height / 2 ) - ( page.bitmap.GetHeight() / 2 ), page.bitmap.GetWidth(), page.bitmap.GetHeight() );
			}

			if ( page.bitmap.IsOk() && draw_text.IsEmpty() )
			{
				focusRect = focusRectBitmap;
			}
			else if ( !page.bitmap.IsOk() && !draw_text.IsEmpty() )
			{
				focusRect = focusRectText;
			}
			else if ( page.bitmap.IsOk() && !draw_text.IsEmpty() )
			{
				focusRect = focusRectText.Union( focusRectBitmap );
			}

			focusRect.Inflate( 2, 2 );

			wxRendererNative::Get().DrawFocusRect( wnd, dc, focusRect, 0 );
		}

		// draw close button if necessary
		if ( close_button_state != wxAUI_BUTTON_STATE_HIDDEN )
		{
			wxBitmap bmp = m_disabledCloseBmp;

			if ( close_button_state == wxAUI_BUTTON_STATE_HOVER || close_button_state == wxAUI_BUTTON_STATE_PRESSED )
			{
				bmp = m_activeCloseBmp;
			}

			int offsetY = tab_y - 1;

			if ( m_flags & wxAUI_NB_BOTTOM )
			{
				offsetY = 1;
			}

			wxRect rect( tab_x + tab_width - close_button_width - 1,
						 offsetY + ( tab_height / 2 ) - ( bmp.GetHeight() / 2 ),
						 close_button_width,
						 tab_height );

			IndentPressedBitmap( &rect, close_button_state );
			dc.DrawBitmap( bmp, rect.x, rect.y, true );

			*out_button_rect = rect;
		}

		*out_tab_rect = wxRect( tab_x, tab_y, tab_width, tab_height );

		dc.DestroyClippingRegion();
	}
}
