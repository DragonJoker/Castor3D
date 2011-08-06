#include "GuiCommon/PrecompiledHeader.hpp"

#include "GuiCommon/SplashScreen.hpp"

#include "xpms/splash.xpm"

using namespace GuiCommon;
using namespace Castor3D;

wxSplashScreen :: wxSplashScreen( wxWindow * p_pParent, const wxString & p_strTitle, const wxString & p_strCopyright, wxPoint p_ptPos, int p_iRange)
	:	wxFrame( p_pParent, wxID_ANY, p_strTitle, p_ptPos, wxSize( 512, 384), wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT | wxBORDER_NONE)
	,	m_bmpSplash( splash_xpm)
	,	m_strCopyright( p_strCopyright)
	,	m_strEngineVersion( wxT( "Castor3D Version "))
{
	m_strEngineVersion << CASTOR_VERSION_MAJOR << wxT( ".") << CASTOR_VERSION_ANNEX << wxT( ".") << CASTOR_VERSION_CORRECT << wxT( ".") << CASTOR_VERSION_POINT;
	wxSize l_size = GetClientSize();
	m_pBmpPanel = new wxPanel( this, wxID_ANY, wxPoint( 0, 0), wxSize( l_size.x, l_size.y - 20));
	m_pGauge = new wxGauge( this, wxID_ANY, p_iRange, wxPoint( 0, l_size.y - 20), wxSize( l_size.x, 20), wxGA_SMOOTH | wxGA_HORIZONTAL | wxBORDER_NONE);
	Show();
	Update();
}

wxSplashScreen :: ~wxSplashScreen()
{
}

void wxSplashScreen :: Step( const wxString & p_strText, int p_iIncrement)
{
	Logger::LogMessage( String( cuT( "wxSplashScreen :: Step - ")) + (const wxChar *)p_strText.c_str());
	m_strStatus = p_strText;
	Step( p_iIncrement);
}

void wxSplashScreen :: Step( int p_iIncrement)
{
	m_pGauge->SetValue( m_pGauge->GetValue() + p_iIncrement);
	m_strSubStatus.clear();
	wxClientDC l_clientDC( m_pBmpPanel);
	_draw( & l_clientDC);
}

void wxSplashScreen :: SubStatus( const wxString & p_strText)
{
	m_strSubStatus = p_strText;
	wxClientDC l_clientDC( m_pBmpPanel);
	_draw( & l_clientDC);
}

void wxSplashScreen :: _draw( wxDC * p_pDC)
{
	wxBufferedDC l_dc( p_pDC, m_pBmpPanel->GetClientSize());
	wxFont l_font( 40, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT( "Arial"));
	l_dc.SetTextBackground( * wxBLACK);
	l_dc.SetTextForeground( * wxWHITE);
	l_dc.SetFont( l_font);
	l_dc.DrawBitmap( m_bmpSplash, wxPoint( 0, 0));
	wxString l_strTitle = GetTitle();
	wxSize l_size = l_dc.GetTextExtent( l_strTitle);
	l_dc.DrawText( l_strTitle, wxPoint( (512 - l_size.x) / 2, 10));

	l_font.SetPointSize( 10);
	l_dc.SetFont( l_font);
	l_size = l_dc.GetTextExtent( m_strCopyright);
	l_dc.DrawText( m_strCopyright, wxPoint( (502 - l_size.x), 300));

	l_font.SetPointSize( 8);
	l_dc.SetFont( l_font);
	l_size = l_dc.GetTextExtent( m_strEngineVersion);
	l_dc.DrawText( m_strEngineVersion, wxPoint( (502 - l_size.x), 315));

	l_dc.DrawText( m_strStatus, wxPoint( 10, 350));

	l_size = l_dc.GetTextExtent( m_strSubStatus);
	l_dc.DrawText( m_strSubStatus, wxPoint( (502 - l_size.x), 350));
}

BEGIN_EVENT_TABLE( wxSplashScreen, wxFrame)
	EVT_PAINT(				wxSplashScreen::_onPaint)
	EVT_ERASE_BACKGROUND(	wxSplashScreen::_onEraseBackground)
END_EVENT_TABLE()

void wxSplashScreen :: _onPaint( wxPaintEvent & p_event)
{
	wxPaintDC l_paintDC( this);
	wxClientDC l_clientDC( m_pBmpPanel);
	_draw( & l_clientDC);
	p_event.Skip();
}

void wxSplashScreen :: _onEraseBackground( wxEraseEvent & p_event)
{
}
