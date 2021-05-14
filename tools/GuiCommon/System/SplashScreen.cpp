#include "GuiCommon/System/SplashScreen.hpp"

#include <Castor3D/RequiredVersion.hpp>
#include <Castor3D/Miscellaneous/Version.hpp>

#include <CastorUtils/Log/Logger.hpp>

#include "GuiCommon/xpms/splash.xpm"

#include <wx/dcbuffer.h>

using namespace GuiCommon;
using namespace castor3d;
using namespace castor;

SplashScreen::SplashScreen( wxString const & title
	, wxPoint const & titlePos
	, wxPoint const & copyrightPos
	, wxPoint const & versionPos
	, wxPoint const & pos
	, int range
	, castor3d::Version const & appVersion
	, castor3d::Version const & engineVersion )
	: wxFrame( nullptr, wxID_ANY, title, pos, wxSize( 512, 384 ), wxCLIP_CHILDREN | wxBORDER_NONE )
	, m_bmpSplash( splash_xpm )
	, m_ptTitlePosition( titlePos )
	, m_ptCopyrightPosition( copyrightPos )
	, m_ptVersionPosition( versionPos )
	, m_strAppVersion( wxString{} << appVersion.getMajor() << wxT( "." ) << appVersion.getMinor() << wxT( "." ) << appVersion.getBuild() )
	, m_strEngineVersion( wxString{} << _( "Based on Castor3D" ) << wxT( " v" ) << engineVersion.getMajor() << wxT( "." ) << engineVersion.getMinor() << wxT( "." ) << engineVersion.getBuild() )
{
	m_strCopyright << wxDateTime().Now().GetCurrentYear() << wxT( " " ) << _( "DragonJoker, All rights shared" );
	SetBackgroundStyle( wxBG_STYLE_CUSTOM );
	wxSize size = GetClientSize();
	m_pPanelBmp = new wxPanel( this, wxID_ANY, wxPoint( 0, 0 ), wxSize( size.x, size.y - 20 ) );
	m_pGauge = new wxGauge( this, wxID_ANY, range, wxPoint( 0, size.y - 20 ), wxSize( size.x, 20 ), wxGA_SMOOTH | wxGA_HORIZONTAL | wxBORDER_NONE );
	Show();
	Update();
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::Step( wxString const & p_strText, int p_iIncrement )
{
	Logger::logDebug( String( cuT( "SplashScreen::Step - " ) ) + make_String( p_strText ) );
	m_strStatus = p_strText;
	Step( p_iIncrement );
}

void SplashScreen::Step( int p_iIncrement )
{
	m_pGauge->SetValue( m_pGauge->GetValue() + p_iIncrement );
	m_strSubStatus.clear();
	wxClientDC clientDC( m_pPanelBmp );
	doDraw( & clientDC );
}

void SplashScreen::SubStatus( wxString const & p_strText )
{
	m_strSubStatus = p_strText;
	wxClientDC clientDC( m_pPanelBmp );
	doDraw( & clientDC );
}

void SplashScreen::doDraw( wxDC * p_pDC )
{
	if ( IsVisible() )
	{
		wxString name = GetTitle();
		name.Replace( wxT( " " ), wxT( "\n" ) );
		wxSize size;
		wxFont font( 40, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT( "Arial" ) );
		p_pDC->SetBackgroundMode( wxTRANSPARENT );
		p_pDC->DrawBitmap( m_bmpSplash, wxPoint( 0, 0 ) );
		p_pDC->SetTextForeground( wxColour( 92, 92, 92 ) );
		font.SetPointSize( 70 );
		p_pDC->SetFont( font );
		p_pDC->DrawText( m_strAppVersion, m_ptVersionPosition );
		p_pDC->SetTextForeground( *wxWHITE );
		font.SetPointSize( 40 );
		p_pDC->SetFont( font );
		p_pDC->DrawText( name, m_ptTitlePosition );
		p_pDC->SetTextForeground( *wxWHITE );
		font.SetPointSize( 10 );
		p_pDC->SetFont( font );
		p_pDC->DrawText( m_strCopyright, m_ptCopyrightPosition );
		p_pDC->SetTextForeground( *wxWHITE );
		font.SetPointSize( 8 );
		p_pDC->SetFont( font );
		p_pDC->DrawText( m_strEngineVersion, wxPoint( 350, 350 ) );
		p_pDC->SetFont( font );
		p_pDC->DrawText( m_strStatus, wxPoint( 10, 350 ) );
		p_pDC->SetTextForeground( *wxWHITE );
		font.SetPointSize( 8 );
		p_pDC->SetFont( font );
		size = p_pDC->GetTextExtent( m_strSubStatus );
		p_pDC->DrawText( m_strSubStatus, wxPoint( ( 502 - size.x ), 350 ) );
	}
}

BEGIN_EVENT_TABLE( SplashScreen, wxFrame )
	EVT_PAINT( SplashScreen::OnPaint )
	EVT_ERASE_BACKGROUND( SplashScreen::OnEraseBackground )
END_EVENT_TABLE()

void SplashScreen::OnPaint( wxPaintEvent & p_event )
{
	wxPaintDC paintDC( this );
	doDraw( &paintDC );
	p_event.Skip();
}

void SplashScreen::OnEraseBackground( wxEraseEvent & p_event )
{
	p_event.Skip();
}
