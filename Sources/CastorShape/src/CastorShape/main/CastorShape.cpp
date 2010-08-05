//******************************************************************************
#include "PrecompiledHeaders.h"

#include "main/CastorShape.h"
#include "main/MainFrame.h"
//******************************************************************************
IMPLEMENT_APP( CSCastorShape)
//******************************************************************************

bool CSCastorShape :: OnInit()
{
	m_mainFrame = new CSMainFrame( NULL, C3D_T( "Castor Shape"), wxPoint(), wxSize(800, 600));
	m_mainFrame->Show( true);
	SetTopWindow( m_mainFrame);
	wxInitAllImageHandlers();
	wxIcon l_icon( C3D_T( "castor.bmp"));
	m_mainFrame->SetIcon( l_icon);
	return true;
}

//******************************************************************************
