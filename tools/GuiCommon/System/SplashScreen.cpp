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
		, c3d::Version const & appVersion
		, c3d::Version const & engineVersion )
		: wxFrame{ nullptr, wxID_ANY, title, pos, wxSize( 512, 384 ), wxCLIP_CHILDREN | wxBORDER_NONE }
		, m_bmpSplash{ splash::splash_xpm }
		, m_ptTitlePosition{ titlePos }
		, m_ptCopyrightPosition{ copyrightPos }
		, m_ptVersionPosition{ versionPos }
		, m_strCopyright{ wxString{} << wxDateTime::GetCurrentYear() << wxT( " " ) << _( "DragonJoker, All rights shared" ) }
		, m_strAppVersion{ wxString{} << appVersion.getMajor() << wxT( "." ) << appVersion.getMinor() << wxT( "." ) << appVersion.getBuild() }
		, m_strEngineVersion{ wxString{} << _( "Based on Castor3D" ) << wxT( " v" ) << engineVersion.getMajor() << wxT( "." ) << engineVersion.getMinor() << wxT( "." ) << engineVersion.getBuild() }
		, m_range{ range }
	{
		SetBackgroundStyle( wxBG_STYLE_CUSTOM );
		wxSize size = GetClientSize();
		m_pGauge = new wxGauge( this, wxID_ANY, m_range, wxPoint( 0, size.y - 20 ), wxSize( size.x, 20 ), wxGA_SMOOTH | wxGA_HORIZONTAL | wxBORDER_NONE );
		Show();
		Update();
	}

	void SplashScreen::Step( wxString const & strText, int iIncrement )
	{
		c3d::Logger::logDebug( c3d::String( cuT( "SplashScreen::Step - " ) ) + make_String( strText ) );
		m_strStatus = strText;
		Step( iIncrement );
	}

	void SplashScreen::Step( int iIncrement )
	{
		m_pGauge->SetValue( m_pGauge->GetValue() + iIncrement );
		m_strSubStatus.clear();
		Refresh();
		Update();
	}

	void SplashScreen::SubStatus( wxString const & strText )
	{
		m_strSubStatus = strText;
		Refresh();
		Update();
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( SplashScreen, wxFrame )
	EVT_PAINT( SplashScreen::onPaint )
	EVT_ERASE_BACKGROUND( SplashScreen::onEraseBackground )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void SplashScreen::onPaint( wxPaintEvent & event )
	{
		if ( IsVisible() )
		{
			wxPaintDC paintDC{ this };
			wxString name = GetTitle();
			name.Replace( wxT( " " ), wxT( "\n" ) );
			wxSize size;
			wxFont font( 40, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT( "Arial" ) );
			paintDC.SetBackgroundMode( wxTRANSPARENT );
			paintDC.DrawBitmap( m_bmpSplash, wxPoint( 0, 0 ) );
			paintDC.SetTextForeground( wxColour( 92, 92, 92 ) );
			font.SetPointSize( 70 );
			paintDC.SetFont( font );
			paintDC.DrawText( m_strAppVersion, m_ptVersionPosition );
			paintDC.SetTextForeground( *wxWHITE );
			font.SetPointSize( 40 );
			paintDC.SetFont( font );
			paintDC.DrawText( name, m_ptTitlePosition );
			paintDC.SetTextForeground( *wxWHITE );
			font.SetPointSize( 10 );
			paintDC.SetFont( font );
			paintDC.DrawText( m_strCopyright, m_ptCopyrightPosition );
			paintDC.SetTextForeground( *wxWHITE );
			font.SetPointSize( 8 );
			paintDC.SetFont( font );
			paintDC.DrawText( m_strEngineVersion, wxPoint( 350, 350 ) );
			paintDC.SetFont( font );
			paintDC.DrawText( m_strStatus, wxPoint( 10, 350 ) );
			paintDC.SetTextForeground( *wxWHITE );
			font.SetPointSize( 8 );
			paintDC.SetFont( font );
			size = paintDC.GetTextExtent( m_strSubStatus );
			paintDC.DrawText( m_strSubStatus, wxPoint( ( 502 - size.x ), 350 ) );
		}
		event.Skip();
	}

	void SplashScreen::onEraseBackground( wxEraseEvent & event )
	{
		event.Skip();
	}
}
