#include "CastorShape/PrecompiledHeader.hpp"

#include "CastorShape/main/CastorShape.hpp"
#include "CastorShape/main/MainFrame.hpp"
#include "xpms/castor.xpm"

using namespace CastorShape;

IMPLEMENT_APP( CastorShapeApp)

bool CastorShapeApp :: OnInit()
{
	wxDisplay l_display;
	wxRect l_rect = l_display.GetClientArea();
	m_pSplash = new GuiCommon::wxSplashScreen( NULL, "Castor Shape", "2010 ® DragonJoker, All rights reserved", wxPoint( (l_rect.width - 512) / 2, (l_rect.height - 384) / 2), 12);
	Castor3D::Logger::SetFileName( "CastorShape.log");
	m_mainFrame = new MainFrame( nullptr, cuT( "Castor Shape"), wxDefaultPosition, wxSize( 800, 600));
	m_mainFrame->Show( true);
	SetTopWindow( m_mainFrame);
	wxInitAllImageHandlers();
	wxIcon l_icon = wxIcon( castor_xpm);
	m_mainFrame->SetIcon( l_icon);
	return true;
}

int CastorShapeApp :: OnExit()
{
	wxImage::CleanUpHandlers();
	return wxApp::OnExit();
}

void CastorShapeApp :: DestroySplashScreen()
{
	m_pSplash->Destroy();
	m_pSplash = nullptr;
}

//******************************************************************************
