#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/main/CastorShape.h"
#include "CastorShape/main/MainFrame.h"
#include "xpms/castor.xpm"

using namespace CastorShape;

IMPLEMENT_APP( CastorShapeApp)

bool CastorShapeApp :: OnInit()
{
	wxDisplay l_display;
	wxRect l_rect = l_display.GetClientArea();
	m_pSplash = new GuiCommon::SplashScreen( "Castor Shape", "2010 ® DragonJoker, All rights reserved", wxPoint( (l_rect.width - 512) / 2, (l_rect.height - 384) / 2), 9);
	Castor3D::Logger::SetFileName( "CastorShape.log");
	m_mainFrame = new MainFrame( NULL, CU_T( "Castor Shape"), wxDefaultPosition, wxSize( 800, 600));
	m_mainFrame->Show( true);
	SetTopWindow( m_mainFrame);
	wxInitAllImageHandlers();
	wxIcon l_icon = wxIcon( castor_xpm);
	m_mainFrame->SetIcon( l_icon);
	return true;
}

void CastorShapeApp :: DestroySplashScreen()
{
	m_pSplash->Destroy();
	m_pSplash = NULL;
}

//******************************************************************************
