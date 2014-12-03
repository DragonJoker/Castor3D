#include "SplashScreen.hpp"

#include <RequiredVersion.hpp>
#include <Logger.hpp>

#include "xpms/splash.xpm"

#include <wx/dcbuffer.h>

using namespace GuiCommon;
using namespace Castor3D;
using namespace Castor;

wxSplashScreen::wxSplashScreen( wxWindow * p_pParent, wxString const & p_strTitle, wxString const & p_strCopyright, wxPoint const & p_ptTitlePos, wxPoint const & p_ptCopyrightPos, wxPoint const & p_ptVersionPos, wxPoint p_ptPos, int p_iRange )
	:	wxFrame( p_pParent, wxID_ANY, p_strTitle, p_ptPos, wxSize( 512, 384 ), wxCLIP_CHILDREN | wxFRAME_FLOAT_ON_PARENT | wxBORDER_NONE	)
	,	m_bmpSplash( splash_xpm	)
	,	m_ptTitlePosition( p_ptTitlePos	)
	,	m_ptCopyrightPosition( p_ptCopyrightPos	)
	,	m_ptVersionPosition( p_ptVersionPos	)
	,	m_strCopyright( p_strCopyright	)
	,	m_strEngineVersion( _( "Castor3D Version " )	)
{
	SetBackgroundStyle( wxBG_STYLE_CUSTOM );
	m_strEngineVersion.clear();
	m_strEngineVersion << CASTOR_VERSION_MAJOR << wxT( "." ) << CASTOR_VERSION_MINOR << wxT( "." ) << CASTOR_VERSION_BUILD;
	wxSize l_size = GetClientSize();
	m_pPanelBmp = new wxPanel( this, wxID_ANY, wxPoint( 0, 0 ), wxSize( l_size.x, l_size.y - 20 ) );
	m_pGauge = new wxGauge( this, wxID_ANY, p_iRange, wxPoint( 0, l_size.y - 20 ), wxSize( l_size.x, 20 ), wxGA_SMOOTH | wxGA_HORIZONTAL | wxBORDER_NONE );
	Show();
	Update();
}

wxSplashScreen::~wxSplashScreen()
{
}

void wxSplashScreen::Step( wxString const & p_strText, int p_iIncrement )
{
	Logger::LogMessage( String( cuT( "wxSplashScreen::Step - " ) ) + ( wxChar const * )p_strText.c_str() );
	m_strStatus = p_strText;
	Step( p_iIncrement );
}

void wxSplashScreen::Step( int p_iIncrement )
{
	m_pGauge->SetValue( m_pGauge->GetValue() + p_iIncrement );
	m_strSubStatus.clear();
	//	Refresh();
	wxClientDC l_clientDC( m_pPanelBmp );
	DoDraw( & l_clientDC );
}

void wxSplashScreen::SubStatus( wxString const & p_strText )
{
	m_strSubStatus = p_strText;
	//	Refresh();
	wxClientDC l_clientDC( m_pPanelBmp );
	DoDraw( & l_clientDC );
}

void wxSplashScreen::DoDraw( wxDC * p_pDC )
{
	if ( IsVisible() )
	{
		wxSize l_size;
		wxFont l_font( 40, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT( "Arial" ) );
		p_pDC->SetBackgroundMode( wxTRANSPARENT );
//		p_pDC->SetTextBackground( *wxBLACK );
		p_pDC->DrawBitmap( m_bmpSplash, wxPoint( 0, 0 ) );
		p_pDC->SetTextForeground( wxColour( 92, 92, 92 ) );
		l_font.SetPointSize( 70 );
		p_pDC->SetFont( l_font );
		p_pDC->DrawText( m_strEngineVersion, m_ptVersionPosition );
		p_pDC->SetTextForeground( *wxWHITE );
		l_font.SetPointSize( 40 );
		p_pDC->SetFont( l_font );
		p_pDC->DrawText( GetTitle(), m_ptTitlePosition );
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

BEGIN_EVENT_TABLE( wxSplashScreen, wxFrame )
	EVT_PAINT(	wxSplashScreen::OnPaint )
	EVT_ERASE_BACKGROUND(	wxSplashScreen::OnEraseBackground )
END_EVENT_TABLE()

void wxSplashScreen::OnPaint( wxPaintEvent & p_event )
{
	wxAutoBufferedPaintDC l_paintDC( this );
	DoDraw( & l_paintDC );
	p_event.Skip();
}

void wxSplashScreen::OnEraseBackground( wxEraseEvent & p_event )
{
	p_event.Skip();
}
