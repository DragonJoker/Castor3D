#include "GuiCommon/System/SplashScreen.hpp"

#include <Castor3D/RequiredVersion.hpp>
#include <Castor3D/Miscellaneous/Version.hpp>

#include <CastorUtils/Log/Logger.hpp>

namespace splash
{
#include "GuiCommon/xpms/splash.xpm"
}

#include <wx/dcbuffer.h>

namespace GuiCommon
{
	SplashScreen::SplashScreen( wxString const & title
		, wxPoint const & titlePos
		, wxPoint const & copyrightPos
		, wxPoint const & versionPos
		, wxPoint const & pos
		, int range
		, castor3d::Version const & appVersion
		, castor3d::Version const & engineVersion )
		: wxFrame( nullptr, wxID_ANY, title, pos, wxSize( 512, 384 ), wxCLIP_CHILDREN | wxBORDER_NONE )
		, m_bmpSplash( splash::splash_xpm )
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

	void SplashScreen::Step( wxString const & strText, int iIncrement )
	{
		castor::Logger::logDebug( castor::String( cuT( "SplashScreen::Step - " ) ) + make_String( strText ) );
		m_strStatus = strText;
		Step( iIncrement );
	}

	void SplashScreen::Step( int iIncrement )
	{
		m_pGauge->SetValue( m_pGauge->GetValue() + iIncrement );
		m_strSubStatus.clear();
		wxClientDC clientDC( m_pPanelBmp );
		doDraw( &clientDC );
	}

	void SplashScreen::SubStatus( wxString const & strText )
	{
		m_strSubStatus = strText;
		wxClientDC clientDC( m_pPanelBmp );
		doDraw( &clientDC );
	}

	void SplashScreen::doDraw( wxDC * pDC )
	{
		if ( IsVisible() )
		{
			wxString name = GetTitle();
			name.Replace( wxT( " " ), wxT( "\n" ) );
			wxSize size;
			wxFont font( 40, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT( "Arial" ) );
			pDC->SetBackgroundMode( wxTRANSPARENT );
			pDC->DrawBitmap( m_bmpSplash, wxPoint( 0, 0 ) );
			pDC->SetTextForeground( wxColour( 92, 92, 92 ) );
			font.SetPointSize( 70 );
			pDC->SetFont( font );
			pDC->DrawText( m_strAppVersion, m_ptVersionPosition );
			pDC->SetTextForeground( *wxWHITE );
			font.SetPointSize( 40 );
			pDC->SetFont( font );
			pDC->DrawText( name, m_ptTitlePosition );
			pDC->SetTextForeground( *wxWHITE );
			font.SetPointSize( 10 );
			pDC->SetFont( font );
			pDC->DrawText( m_strCopyright, m_ptCopyrightPosition );
			pDC->SetTextForeground( *wxWHITE );
			font.SetPointSize( 8 );
			pDC->SetFont( font );
			pDC->DrawText( m_strEngineVersion, wxPoint( 350, 350 ) );
			pDC->SetFont( font );
			pDC->DrawText( m_strStatus, wxPoint( 10, 350 ) );
			pDC->SetTextForeground( *wxWHITE );
			font.SetPointSize( 8 );
			pDC->SetFont( font );
			size = pDC->GetTextExtent( m_strSubStatus );
			pDC->DrawText( m_strSubStatus, wxPoint( ( 502 - size.x ), 350 ) );
		}
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( SplashScreen, wxFrame )
	EVT_PAINT( SplashScreen::OnPaint )
	EVT_ERASE_BACKGROUND( SplashScreen::OnEraseBackground )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

		void SplashScreen::OnPaint( wxPaintEvent & event )
	{
		wxPaintDC paintDC( this );
		doDraw( &paintDC );
		event.Skip();
	}

	void SplashScreen::OnEraseBackground( wxEraseEvent & event )
	{
		event.Skip();
	}
}
