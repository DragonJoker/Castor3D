#include "GradientButton.hpp"

#include <wx/dcbuffer.h>

namespace GuiCommon
{
	GradientButton::GradientButton()
		: wxButton()
	{
		DoInitialise();
	}

	GradientButton::GradientButton( wxWindow * p_parent, wxWindowID p_id, wxString const & p_label, wxPoint const & p_pos, wxSize const & p_size, long p_style, wxValidator const & p_validator, wxString const & p_name )
		: wxButton( p_parent, p_id, p_label, p_pos, p_size, p_style, p_validator, p_name )
	{
		DoInitialise();
	}

	void GradientButton::DoInitialise()
	{
		m_gradientTopStartColour = INACTIVE_TEXT_COLOUR;
		m_gradientTopEndColour = BORDER_COLOUR;
		m_gradientBottomStartColour = PANEL_BACKGROUND_COLOUR;
		m_gradientBottomEndColour = INACTIVE_TAB_COLOUR;
		m_pressedColourTop = INACTIVE_TAB_COLOUR;
		m_pressedColourBottom = PANEL_BACKGROUND_COLOUR;

		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
	}

	BEGIN_EVENT_TABLE( GradientButton, wxButton )
		EVT_PAINT( GradientButton::OnPaint )
	END_EVENT_TABLE()

	void GradientButton::OnPaint( wxPaintEvent & p_event )
	{
		wxBufferedPaintDC dc( this );

		wxRect clientRect = GetClientRect();
		wxRect gradientRect = clientRect;
		gradientRect.SetHeight( gradientRect.GetHeight() / 2 );

		if ( GetCapture() != this )
		{
			dc.GradientFillLinear( gradientRect, m_gradientTopStartColour, m_gradientTopEndColour, wxSOUTH );
		}
		else
		{
			dc.SetPen( wxPen( m_pressedColourTop ) );
			dc.SetBrush( wxBrush( m_pressedColourTop ) );
			dc.DrawRectangle( gradientRect );
		}

		gradientRect.Offset( 0, gradientRect.GetHeight() );

		if ( GetCapture() != this )
		{
			dc.GradientFillLinear( gradientRect, m_gradientBottomStartColour, m_gradientBottomEndColour, wxSOUTH );
		}
		else
		{
			dc.SetPen( wxPen( m_pressedColourBottom ) );
			dc.SetBrush( wxBrush( m_pressedColourBottom ) );
			dc.DrawRectangle( gradientRect );
		}

		dc.SetPen( wxPen( GetBackgroundColour() ) );
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		dc.DrawRectangle( 0, 0, clientRect.GetWidth(), clientRect.GetHeight() );
		dc.SetFont( GetFont() );
		dc.SetTextForeground( GetForegroundColour() );

		if ( GetCapture() == this )
		{
			clientRect.Offset( 1, 1 );
		}

		dc.DrawLabel( GetLabel(), clientRect, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL );
	}
}
