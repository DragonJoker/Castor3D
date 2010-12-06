#include "PrecompiledHeader.h"

#include "CastorViewer.h"

#include "MainFrame.h"

using namespace CastorViewer;

IMPLEMENT_APP( CastorViewerApp)

bool CastorViewerApp :: OnInit()
{
	Castor3D::Logger::SetFileName( "CastorViewer.log");
	bool l_bReturn = true;

	if (l_bReturn)
	{
		wxInitAllImageHandlers();
		wxIcon l_icon( CU_T( "castor.bmp"));

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