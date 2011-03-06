#include "GuiCommon/PrecompiledHeader.h"

#include "GuiCommon/SplashScreen.h"

#include "xpms/splash.xpm"

using namespace GuiCommon;
using namespace Castor3D;

SplashScreen :: SplashScreen( const String & p_strTitle, const String & p_strCopyright, wxPoint p_ptPos, int p_iRange)
	:	wxFrame( NULL, wxID_ANY, p_strTitle, p_ptPos, wxSize( 512, 384), wxFRAME_NO_TASKBAR | wxBORDER_NONE)
	,	m_bmpSplash( splash_xpm)
	,	m_strCopyright( p_strCopyright)
{
	Show();
	Refresh();
	CreateStatusBar();
	wxSize l_size = GetClientSize();
	m_pBmpPanel = new wxPanel( this, wxID_ANY, wxPoint( 0, 0), wxSize( l_size.x, l_size.y - 20));
	m_pGauge = new wxGauge( this, wxID_ANY, p_iRange, wxPoint( 0, l_size.y - 20), wxSize( l_size.x, 20), wxGA_SMOOTH | wxGA_HORIZONTAL | wxBORDER_NONE);
	wxClientDC l_clientDC( m_pBmpPanel);
	_draw( & l_clientDC);
}

SplashScreen :: ~SplashScreen()
{
}

void SplashScreen :: Step( const String & p_strText, int p_iIncrement)
{
	Logger::LogMessage( CU_T( "SplashScreen :: Step - ") + p_strText);
	GetStatusBar()->SetStatusText( p_strText);
	Step( p_iIncrement);
}

void SplashScreen :: Step( int p_iIncrement)
{
	m_pGauge->SetValue( m_pGauge->GetValue() + p_iIncrement);
	Refresh();
	wxClientDC l_clientDC( m_pBmpPanel);
	_draw( & l_clientDC);
}

void SplashScreen :: _draw( wxDC * p_pDC)
{
	wxFont l_font( 40, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");
	p_pDC->SetTextBackground( * wxBLACK);
	p_pDC->SetTextForeground( * wxWHITE);
	p_pDC->SetFont( l_font);
	p_pDC->DrawBitmap( m_bmpSplash, wxPoint( 0, 0));
	wxString l_strTitle = GetTitle();
	wxSize l_size = p_pDC->GetTextExtent( l_strTitle);
	p_pDC->DrawText( l_strTitle, wxPoint( (512 - l_size.x) / 2, 10));

	l_font.SetPointSize( 10);
	p_pDC->SetFont( l_font);
	l_size = p_pDC->GetTextExtent( m_strCopyright);
	p_pDC->DrawText( m_strCopyright, wxPoint( (502 - l_size.x), 310));
}

BEGIN_EVENT_TABLE( SplashScreen, wxFrame)
	EVT_PAINT(				SplashScreen::_onPaint)
	EVT_ERASE_BACKGROUND(	SplashScreen::_onEraseBackground)
END_EVENT_TABLE()

void SplashScreen :: _onPaint( wxPaintEvent & p_event)
{
	wxPaintDC l_paintDC( this);
	wxClientDC l_clientDC( m_pBmpPanel);
	_draw( & l_clientDC);
	p_event.Skip();
}

void SplashScreen :: _onEraseBackground( wxEraseEvent & p_event)
{
}
