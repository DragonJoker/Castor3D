#include "SplashScreen.hpp"

#include <RequiredVersion.hpp>
#include <Logger.hpp>

#include "xpms/splash.xpm"

#include <wx/dcbuffer.h>

using namespace GuiCommon;
using namespace Castor3D;
using namespace Castor;

SplashScreen::SplashScreen( wxString const & p_strTitle, wxPoint const & p_ptTitlePos, wxPoint const & p_ptCopyrightPos, wxPoint const & p_ptVersionPos, wxPoint p_ptPos, int p_iRange )
	: wxFrame( NULL, wxID_ANY, p_strTitle, p_ptPos, wxSize( 512, 384 ), wxCLIP_CHILDREN | wxBORDER_NONE )
	, m_bmpSplash( splash_xpm )
	, m_ptTitlePosition( p_ptTitlePos )
	, m_ptCopyrightPosition( p_ptCopyrightPos )
	, m_ptVersionPosition( p_ptVersionPos )
	, m_strEngineVersion( _( "Castor3D Version " ) )
{
	m_strCopyright << wxDateTime().Now().GetCurrentYear() << wxT( " " ) << _( "DragonJoker, All rights shared" );
	SetBackgroundStyle( wxBG_STYLE_CUSTOM );
	m_strEngineVersion.clear();
	m_strEngineVersion << CASTOR_VERSION_MAJOR << wxT( "." ) << CASTOR_VERSION_MINOR << wxT( "." ) << CASTOR_VERSION_BUILD;
	wxSize l_size = GetClientSize();
	m_pPanelBmp = new wxPanel( this, wxID_ANY, wxPoint( 0, 0 ), wxSize( l_size.x, l_size.y - 20 ) );
	m_pGauge = new wxGauge( this, wxID_ANY, p_iRange, wxPoint( 0, l_size.y - 20 ), wxSize( l_size.x, 20 ), wxGA_SMOOTH | wxGA_HORIZONTAL | wxBORDER_NONE );
	Show();
	Update();
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::Step( wxString const & p_strText, int p_iIncrement )
{
	Logger::LogInfo( String( cuT( "SplashScreen::Step - " ) ) + make_String( p_strText ) );
	m_strStatus = p_strText;
	Step( p_iIncrement );
}

void SplashScreen::Step( int p_iIncrement )
{
	m_pGauge->SetValue( m_pGauge->GetValue() + p_iIncrement );
	m_strSubStatus.clear();
	wxClientDC l_clientDC( m_pPanelBmp );
	DoDraw( & l_clientDC );
}

void SplashScreen::SubStatus( wxString const & p_strText )
{
	m_strSubStatus = p_strText;
	wxClientDC l_clientDC( m_pPanelBmp );
	DoDraw( & l_clientDC );
}

void SplashScreen::DoDraw( wxDC * p_pDC )
{
	if ( IsVisible() )
	{
		wxString l_name = GetTitle();
		l_name.Replace( wxT( " " ), wxT( "\n" ) );
		wxSize l_size;
		wxFont l_font( 40, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT( "Arial" ) );
		p_pDC->SetBackgroundMode( wxTRANSPARENT );
		p_pDC->DrawBitmap( m_bmpSplash, wxPoint( 0, 0 ) );
		p_pDC->SetTextForeground( wxColour( 92, 92, 92 ) );
		l_font.SetPointSize( 70 );
		p_pDC->SetFont( l_font );
		p_pDC->DrawText( m_strEngineVersion, m_ptVersionPosition );
		p_pDC->SetTextForeground( *wxWHITE );
		l_font.SetPointSize( 40 );
		p_pDC->SetFont( l_font );
		p_pDC->DrawText( l_name, m_ptTitlePosition );
		p_pDC->SetTextForeground( *wxWHITE );
		l_font.SetPointSize( 10 );
		p_pDC->SetFont( l_font );
		p_pDC->DrawText( m_strCopyright, m_ptCopyrightPosition );
		p_pDC->SetTextForeground( *wxWHITE );
		l_font.SetPointSize( 8 );
		p_pDC->SetFont( l_font );
		p_pDC->DrawText( m_strStatus, wxPoint( 10, 350 ) );
		p_pDC->SetTextForeground( *wxWHITE );
		l_font.SetPointSize( 8 );
		p_pDC->SetFont( l_font );
		l_size = p_pDC->GetTextExtent( m_strSubStatus );
		p_pDC->DrawText( m_strSubStatus, wxPoint( ( 502 - l_size.x ), 350 ) );
	}
}

BEGIN_EVENT_TABLE( SplashScreen, wxFrame )
	EVT_PAINT( SplashScreen::OnPaint )
	EVT_ERASE_BACKGROUND( SplashScreen::OnEraseBackground )
END_EVENT_TABLE()

void SplashScreen::OnPaint( wxPaintEvent & p_event )
{
	wxPaintDC l_paintDC( this );
	DoDraw( &l_paintDC );
	p_event.Skip();
}

void SplashScreen::OnEraseBackground( wxEraseEvent & p_event )
{
	p_event.Skip();
}
