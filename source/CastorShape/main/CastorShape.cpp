#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/main/CastorShape.h"
#include "CastorShape/main/MainFrame.h"

using namespace CastorShape;

IMPLEMENT_APP( CastorShapeApp)

bool CastorShapeApp :: OnInit()
{
	m_mainFrame = new MainFrame( NULL, CU_T( "Castor Shape"), wxDefaultPosition, wxSize(800, 600));
	m_mainFrame->Show( true);
	SetTopWindow( m_mainFrame);
	wxInitAllImageHandlers();
	wxIcon l_icon = wxICON( Castor);
	m_mainFrame->SetIcon( l_icon);
	return true;
}

//******************************************************************************
