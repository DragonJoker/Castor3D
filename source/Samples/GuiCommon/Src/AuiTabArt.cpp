#include "AuiTabArt.hpp"

#include <wx/aui/auibook.h>
#include <wx/renderer.h>

namespace GuiCommon
{
	namespace
	{
		void indentPressedBitmap( wxRect * rect
			, int buttonState )
		{
			if ( buttonState == wxAUI_BUTTON_STATE_PRESSED )
			{
				rect->x++;
				rect->y++;
			}
		}

		static wxString auiChopText( wxDC & dc
			, const wxString & text
			, int maxSize )
		{
			wxCoord x, y;
			// first check if the text fits with no problems
			dc.GetTextExtent( text, &x, &y );

			if ( x <= maxSize )
			{
				return text;
			}

			size_t i, len = text.Length();
			size_t lastGoodLength = 0;

			for ( i = 0; i < len; ++i )
			{
				wxString s = text.Left( i );
				s += wxT( "..." );
				dc.GetTextExtent( s, &x, &y );

				if ( x > maxSize )
				{
					break;
				}

				lastGoodLength = i;
			}

			wxString ret = text.Left( lastGoodLength );
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

	void AuiTabArt::DrawBackground( wxDC & dc
		, wxWindow * window
		, wxRect const & rect )
	{
		dc.SetPen( m_baseColourPen );
		dc.SetBrush( m_baseColourBrush );
		dc.DrawRectangle( rect );
	}

	// DrawTab() draws an individual tab.
	//
	// dc       - output dc
	// inRect  - rectangle the tab should be confined to
	// caption  - tab's caption
	// active   - whether or not the tab is active
	// out_rect - actual output rectangle
	// xExtent - the advance x; where the next tab should start
	void AuiTabArt::DrawTab( wxDC & dc
		, wxWindow * wnd
		, const wxAuiNotebookPage & pane
		, const wxRect & inRect
		, int closeButtonState
		, wxRect * outTabRect
		, wxRect * outButtonRect
		, int * xExtent )
	{
		wxCoord normalTextX, normalTextY;
		wxCoord selectedTextX, selectedTextY;
		wxCoord textY;

		// if the caption is empty, measure some temporary text
		wxString caption = pane.caption;

		if ( caption.empty() )
		{
			caption = wxT( "Xj" );
		}

		dc.SetFont( m_selectedFont );
		dc.GetTextExtent( caption, &selectedTextX, &selectedTextY );

		dc.SetFont( m_normalFont );
		dc.GetTextExtent( caption, &normalTextX, &normalTextY );

		// figure out the size of the tab
		wxSize tabSize = GetTabSize( dc, wnd, pane.caption, pane.bitmap, pane.active, closeButtonState, xExtent );

		wxCoord tabHeight = m_tabCtrlHeight - 3;
		wxCoord tabWidth = tabSize.x;
		wxCoord tabX = inRect.x;
		wxCoord tabY = inRect.y + inRect.height - tabHeight;

		caption = pane.caption;

		// select pen, brush and font for the tab to be drawn
		if ( pane.active )
		{
			dc.SetFont( m_selectedFont );
			textY = selectedTextY;
		}
		else
		{
			dc.SetFont( m_normalFont );
			textY = normalTextY;
		}


		// create points that will make the tab outline

		int clip_width = tabWidth;

		if ( tabX + clip_width > inRect.x + inRect.width )
		{
			clip_width = ( inRect.x + inRect.width ) - tabX;
		}

		wxPoint clipPoints[6];
		clipPoints[0] = wxPoint( tabX, tabY + tabHeight - 3 );
		clipPoints[1] = wxPoint( tabX, tabY + 2 );
		clipPoints[2] = wxPoint( tabX + 2, tabY );
		clipPoints[3] = wxPoint( tabX + clip_width - 1, tabY );
		clipPoints[4] = wxPoint( tabX + clip_width + 1, tabY + 2 );
		clipPoints[5] = wxPoint( tabX + clip_width + 1, tabY + tabHeight - 3 );

#if !defined(__WXDFB__) && !defined(__WXCOCOA__)
		// since the above code above doesn't play well with WXDFB or WXCOCOA,
		// we'll just use a rectangle for the clipping region for now --
		dc.SetClippingRegion( tabX, tabY, clip_width + 1, tabHeight - 3 );
#endif

		wxPoint borderPoints[6];

		if ( m_flags & wxAUI_NB_BOTTOM )
		{
			borderPoints[0] = wxPoint( tabX, tabY );
			borderPoints[1] = wxPoint( tabX, tabY + tabHeight - 6 );
			borderPoints[2] = wxPoint( tabX + 2, tabY + tabHeight - 4 );
			borderPoints[3] = wxPoint( tabX + tabWidth - 2, tabY + tabHeight - 4 );
			borderPoints[4] = wxPoint( tabX + tabWidth, tabY + tabHeight - 6 );
			borderPoints[5] = wxPoint( tabX + tabWidth, tabY );
		}
		else //if (m_flags & wxAUI_NB_TOP) {}
		{
			borderPoints[0] = wxPoint( tabX, tabY + tabHeight - 4 );
			borderPoints[1] = wxPoint( tabX, tabY + 2 );
			borderPoints[2] = wxPoint( tabX + 2, tabY );
			borderPoints[3] = wxPoint( tabX + tabWidth - 2, tabY );
			borderPoints[4] = wxPoint( tabX + tabWidth, tabY + 2 );
			borderPoints[5] = wxPoint( tabX + tabWidth, tabY + tabHeight - 4 );
		}

		// TODO: else if (m_flags &wxAUI_NB_LEFT) {}
		// TODO: else if (m_flags &wxAUI_NB_RIGHT) {}

		int drawnTabYOffset = borderPoints[1].y;
		int drawnTabHeight = borderPoints[0].y - borderPoints[1].y;

		if ( pane.active )
		{
			// draw active tab

			// draw base background color
			wxRect r( tabX, tabY, tabWidth, tabHeight );
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
			wxRect r( tabX, tabY, tabWidth, tabHeight );
			dc.SetPen( wxPen( m_baseColour ) );
			dc.SetBrush( wxBrush( m_baseColour ) );
			dc.DrawRectangle( r.x + 1, r.y + 1, r.width - 1, r.height - 4 );
		}

		//// draw tab outline
		//dc.SetPen( m_borderPen );
		//dc.SetBrush( *wxTRANSPARENT_BRUSH );
		//dc.DrawPolygon( WXSIZEOF( borderPoints ), borderPoints );

		// there are two horizontal grey lines at the bottom of the tab control,
		// this gets rid of the top one of those lines in the tab control
		if ( pane.active )
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

			dc.DrawLine( borderPoints[0].x + 1,
				borderPoints[0].y,
				borderPoints[5].x,
				borderPoints[5].y );
		}


		int textOffset = tabX + 8;
		int closeButtonWidth = 0;

		if ( closeButtonState != wxAUI_BUTTON_STATE_HIDDEN )
		{
			closeButtonWidth = m_activeCloseBmp.GetWidth();
		}

		int bitmapOffset = 0;
		textOffset = tabX + 8;

		wxString drawText = auiChopText( dc, caption, tabWidth - ( textOffset - tabX ) - closeButtonWidth );

		// draw tab text
		if ( pane.active )
		{
			dc.SetTextForeground( ACTIVE_TEXT_COLOUR );
		}
		else
		{
			dc.SetTextForeground( INACTIVE_TEXT_COLOUR );
		}

		dc.SetFont( m_normalFont );
		dc.DrawText( drawText, textOffset, drawnTabYOffset + ( drawnTabHeight ) / 2 - ( textY / 2 ) );

		// draw focus rectangle
		if ( pane.active && ( wnd->FindFocus() == wnd ) )
		{
			wxRect focusRectText( textOffset, ( drawnTabYOffset + ( drawnTabHeight ) / 2 - ( textY / 2 ) - 1 ), selectedTextX, selectedTextY );

			wxRect focusRect;
			wxRect focusRectBitmap;

			if ( pane.bitmap.IsOk() )
			{
				focusRectBitmap = wxRect( bitmapOffset, drawnTabYOffset + ( drawnTabHeight / 2 ) - ( pane.bitmap.GetHeight() / 2 ), pane.bitmap.GetWidth(), pane.bitmap.GetHeight() );
			}

			if ( pane.bitmap.IsOk() && drawText.IsEmpty() )
			{
				focusRect = focusRectBitmap;
			}
			else if ( !pane.bitmap.IsOk() && !drawText.IsEmpty() )
			{
				focusRect = focusRectText;
			}
			else if ( pane.bitmap.IsOk() && !drawText.IsEmpty() )
			{
				focusRect = focusRectText.Union( focusRectBitmap );
			}

			focusRect.Inflate( 2, 2 );

			wxRendererNative::Get().DrawFocusRect( wnd, dc, focusRect, 0 );
		}

		// draw close button if necessary
		if ( closeButtonState != wxAUI_BUTTON_STATE_HIDDEN )
		{
			wxBitmap bmp = m_disabledCloseBmp;

			if ( closeButtonState == wxAUI_BUTTON_STATE_HOVER || closeButtonState == wxAUI_BUTTON_STATE_PRESSED )
			{
				bmp = m_activeCloseBmp;
			}

			int offsetY = tabY - 1;

			if ( m_flags & wxAUI_NB_BOTTOM )
			{
				offsetY = 1;
			}

			wxRect rect( tabX + tabWidth - closeButtonWidth - 1,
				offsetY + ( tabHeight / 2 ) - ( bmp.GetHeight() / 2 ),
				closeButtonWidth,
				tabHeight );

			indentPressedBitmap( &rect, closeButtonState );
			dc.DrawBitmap( bmp, rect.x, rect.y, true );

			*outButtonRect = rect;
		}

		*outTabRect = wxRect( tabX, tabY, tabWidth, tabHeight );

		dc.DestroyClippingRegion();
	}
}
