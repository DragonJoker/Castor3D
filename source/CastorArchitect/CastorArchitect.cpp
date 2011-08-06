#include "CastorArchitect/PrecompiledHeader.hpp"

#include "CastorArchitect/CastorArchitect.hpp"

#include "CastorArchitect/MainFrame.hpp"
#include "xpms/castor.xpm"

using namespace CastorArchitect;

IMPLEMENT_APP( CastorArchitectApp)

bool CastorArchitectApp :: OnInit()
{
	wxDisplay l_display;
	wxRect l_rect = l_display.GetClientArea();
	m_pSplash = new wxSplashScreen( NULL, "Castor Architect", "2010 ® DragonJoker, All rights reserved", wxPoint( (l_rect.width - 512) / 2, (l_rect.height - 384) / 2), 6);
	Castor3D::Logger::SetFileName( "CastorArchitect.log");
	wxInitAllImageHandlers();
	wxIcon l_icon = wxIcon( castor_xpm);

	m_mainFrame = new MainFrame( nullptr, cuT( "Castor Architect"));
	m_mainFrame->SetIcon( l_icon);
	m_mainFrame->Show();

	SetTopWindow( m_mainFrame);

	if (argc > 1)
	{
		String l_strFileName = argv[1];
		m_mainFrame->LoadScene( l_strFileName);
	}

	return true;
}

int CastorArchitectApp :: OnExit()
{
	wxImage::CleanUpHandlers();
	return wxApp::OnExit();
}

void CastorArchitectApp :: DestroySplashScreen()
{
	m_pSplash->Destroy();
	m_pSplash = nullptr;
}
