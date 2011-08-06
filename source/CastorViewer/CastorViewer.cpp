#include "CastorViewer/PrecompiledHeader.hpp"

#include "CastorViewer/CastorViewer.hpp"
#include "CastorViewer/MainFrame.hpp"

using namespace CastorViewer;

IMPLEMENT_APP( CastorViewerApp)

bool CastorViewerApp :: OnInit()
{
	bool l_bReturn = true;
	Castor3D::Logger::SetFileName( wxT( "CastorViewer.log"));

	if (l_bReturn)
	{
		wxInitAllImageHandlers();
		m_pMainFrame = new MainFrame( NULL, wxT( "Castor Viewer"));

		SetTopWindow( m_pMainFrame);

 		if (argc > 1)
 		{
 			wxString l_strFileName = argv[1];
 			m_pMainFrame->LoadScene( l_strFileName);
 		}
	}

	return l_bReturn;
}

int CastorViewerApp :: OnExit()
{
	wxImage::CleanUpHandlers();
	return wxApp::OnExit();
}
