#include "CastorDvpTD/CastorDvpTD.hpp"

#include "CastorDvpTD/MainFrame.hpp"

wxIMPLEMENT_APP( castortd::CastorDvpTD );

namespace castortd
{
	CastorDvpTD::CastorDvpTD()
		: CastorApplication{ cuT( "CastorDvpTD" )
			, cuT( "Castor Tower Defense" )
			, 7
			, castor3d::Version{ CastorDvpTD_VERSION_MAJOR, CastorDvpTD_VERSION_MINOR, CastorDvpTD_VERSION_BUILD } }
	{
	}

	void CastorDvpTD::doLoadAppImages()
	{
	}

	wxWindow * CastorDvpTD::doInitialiseMainFrame( GuiCommon::SplashScreen * p_splashScreen )
	{
		wxAppConsole::SetAppName( wxT( "CastorDvpTD" ) );
		wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );

#if wxCHECK_VERSION( 2, 9, 0 )

		wxAppConsole::SetAppDisplayName( m_displayName );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );

#endif

		m_mainFrame = new MainFrame;

		SetTopWindow( m_mainFrame );
		return m_mainFrame;
	}
}
