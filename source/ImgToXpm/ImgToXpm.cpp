#include "ImgToXpm/PrecompiledHeader.hpp"

#include "ImgToXpm/ImgToXpm.hpp"
#include "ImgToXpm/MainFrame.hpp"

using namespace ImgToXpm;

IMPLEMENT_APP( ImgToXpmApp)

bool ImgToXpmApp :: OnInit()
{
	m_mainFrame = new MainFrame();
	m_mainFrame->Show( true);
	SetTopWindow( m_mainFrame);
	wxInitAllImageHandlers();
	return true;
}

int ImgToXpmApp :: OnExit()
{
	wxImage::CleanUpHandlers();
	return wxApp::OnExit();
}

//******************************************************************************
