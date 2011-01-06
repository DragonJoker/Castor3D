#include "CastorArchitect/PrecompiledHeader.h"

#include "CastorArchitect/CastorArchitect.h"

#include "CastorArchitect/MainFrame.h"

using namespace CastorArchitect;

IMPLEMENT_APP( CastorArchitectApp)

bool CastorArchitectApp :: OnInit()
{
	wxInitAllImageHandlers();
	wxIcon l_icon = wxICON( Castor);

	m_mainFrame = new MainFrame( NULL, CU_T( "Castor Architect"));
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