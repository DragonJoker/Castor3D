#include "GradientButton.hpp"

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
		wxBufferedPaintDC l_dc( this );

		wxRect l_clientRect = GetClientRect();
		wxRect l_gradientRect = l_clientRect;
		l_gradientRect.SetHeight( l_gradientRect.GetHeight() / 2 );

		if ( GetCapture() != this )
		{
			l_dc.GradientFillLinear( l_gradientRect, m_gradientTopStartColour, m_gradientTopEndColour, wxSOUTH );
		}
		else
		{
			l_dc.SetPen( wxPen( m_pressedColourTop ) );
			l_dc.SetBrush( wxBrush( m_pressedColourTop ) );
			l_dc.DrawRectangle( l_gradientRect );
		}

		l_gradientRect.Offset( 0, l_gradientRect.GetHeight() );

		if ( GetCapture() != this )
		{
			l_dc.GradientFillLinear( l_gradientRect, m_gradientBottomStartColour, m_gradientBottomEndColour, wxSOUTH );
		}
		else
		{
			l_dc.SetPen( wxPen( m_pressedColourBottom ) );
			l_dc.SetBrush( wxBrush( m_pressedColourBottom ) );
			l_dc.DrawRectangle( l_gradientRect );
		}

		l_dc.SetPen( wxPen( GetBackgroundColour() ) );
		l_dc.SetBrush( *wxTRANSPARENT_BRUSH );
		l_dc.DrawRectangle( 0, 0, l_clientRect.GetWidth(), l_clientRect.GetHeight() );
		l_dc.SetFont( GetFont() );
		l_dc.SetTextForeground( GetForegroundColour() );

		if ( GetCapture() == this )
		{
			l_clientRect.Offset( 1, 1 );
		}

		l_dc.DrawLabel( GetLabel(), l_clientRect, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL );
	}
}
