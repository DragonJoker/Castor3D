#include "PrecompiledHeader.h"

#include "CastorViewer.h"

#include "MainFrame.h"

using namespace CastorViewer;

IMPLEMENT_APP( CastorViewerApp)

bool CastorViewerApp :: OnInit()
{
	wxInitAllImageHandlers();
	wxIcon l_icon( C3D_T( "castor.bmp"));

	m_mainFrame = new MainFrame( NULL, C3D_T( "Castor Viewer"));
	m_mainFrame->SetIcon( l_icon);
	m_mainFrame->Show();

	SetTopWindow( m_mainFrame);

	if (argc > 1)
	{
		String l_strFileName = makeString( argv[1] );
		m_mainFrame->LoadScene( l_strFileName);
	}

	return true;
}
