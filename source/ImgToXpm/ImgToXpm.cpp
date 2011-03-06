#include "ImgToXpm/PrecompiledHeader.h"

#include "ImgToXpm/ImgToXpm.h"
#include "ImgToXpm/MainFrame.h"

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

//******************************************************************************
