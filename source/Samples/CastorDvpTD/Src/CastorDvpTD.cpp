#include "CastorDvpTD.hpp"

#include "MainFrame.hpp"

wxIMPLEMENT_APP( castortd::CastorDvpTD );

namespace castortd
{
	CastorDvpTD::CastorDvpTD()
		: CastorApplication( cuT( "CastorDvpTD" ), cuT( "Castor Tower Defense" ), 7 )
	{
	}

	void CastorDvpTD::DoLoadAppImages()
	{
	}

	wxWindow * CastorDvpTD::DoInitialiseMainFrame( GuiCommon::SplashScreen * p_splashScreen )
	{
		wxAppConsole::SetAppName( wxT( "CastorDvpTD" ) );
		wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );

#if wxCHECK_VERSION( 2, 9, 0 )

		wxAppConsole::SetAppDisplayName( m_displayName );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );

#endif

		m_mainFrame = new MainFrame;
		m_mainFrame->Initialise();
		m_mainFrame->Maximize();

		SetTopWindow( m_mainFrame );
		return m_mainFrame;
	}
}
