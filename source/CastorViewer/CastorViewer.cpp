#include "CastorViewer/PrecompiledHeader.h"

#include "CastorViewer/CastorViewer.h"

#include "CastorViewer/MainFrame.h"
#include "xpms/castor.xpm"

using namespace CastorViewer;

IMPLEMENT_APP( CastorViewerApp)

bool CastorViewerApp :: OnInit()
{
	wxDisplay l_display;
	wxRect l_rect = l_display.GetClientArea();
	m_pSplash = new SplashScreen( "Castor Viewer", "2010 ® DragonJoker, All rights reserved", wxPoint( (l_rect.width - 512) / 2, (l_rect.height - 384) / 2), 4);
	Castor3D::Logger::SetFileName( "CastorViewer.log");
	bool l_bReturn = true;

	if (l_bReturn)
	{
		wxInitAllImageHandlers();
		wxIcon l_icon = wxIcon( castor_xpm);

		m_mainFrame = new MainFrame( NULL, CU_T( "Castor Viewer"));
		m_mainFrame->SetIcon( l_icon);
		m_mainFrame->Show();

		SetTopWindow( m_mainFrame);

 		if (argc > 1)
 		{
 			String l_strFileName = argv[1];
 			m_mainFrame->LoadScene( l_strFileName);
 		}
	}

	return l_bReturn;
}

int CastorViewerApp :: OnExit()
{
	return wxApp::OnExit();
}

void CastorViewerApp :: DestroySplashScreen()
{
	m_pSplash->Destroy();
	m_pSplash = NULL;
}