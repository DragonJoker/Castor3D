#include "CastorDvpTD.hpp"

#include "MainFrame.hpp"

wxIMPLEMENT_APP( castortd::CastorDvpTD );

namespace castortd
{
	static const bool CASTOR3D_THREADED = true;

	CastorDvpTD::CastorDvpTD()
		: CastorApplication{ cuT( "CastorDvpTD" )
			, cuT( "Castor TD" )
			, 7
			, castor3d::Version{ CastorDvpTD_VERSION_MAJOR, CastorDvpTD_VERSION_MINOR, CastorDvpTD_VERSION_BUILD }
			, 120u
			, CASTOR3D_THREADED }
	{
	}

	void CastorDvpTD::doLoadAppImages()
	{
	}

	wxWindow * CastorDvpTD::doInitialiseMainFrame( GuiCommon::SplashScreen & splashScreen )
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
