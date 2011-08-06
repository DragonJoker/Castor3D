#include "ImgToIco/PrecompiledHeader.hpp"

#include "ImgToIco/ImgToIco.hpp"
#include "ImgToIco/MainFrame.hpp"

using namespace ImgToIco;

IMPLEMENT_APP( ImgToIcoApp)

bool ImgToIcoApp :: OnInit()
{
	m_mainFrame = new MainFrame();
	m_mainFrame->Show( true);
	SetTopWindow( m_mainFrame);
	wxInitAllImageHandlers();
	return true;
}

int ImgToIcoApp :: OnExit()
{
	wxImage::CleanUpHandlers();
	return wxApp::OnExit();
}

//******************************************************************************
