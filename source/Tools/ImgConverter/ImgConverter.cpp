#include "ImgConverter/ImgConverter.hpp"
#include "ImgConverter/MainFrame.hpp"

#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/stdpaths.h>

#include <iostream>

using namespace ImgToIco;

IMPLEMENT_APP( ImgToIcoApp )

bool ImgToIcoApp::OnInit()
{
	long 					l_lLanguage		= wxLANGUAGE_DEFAULT;
	wxStandardPathsBase &	l_stdPaths		= wxStandardPaths::Get();
	wxFileName				l_strCurrent	= wxFileName( l_stdPaths.GetExecutablePath() ).GetPath();
	wxString				l_strSeparator	= wxFileName::GetPathSeparator();
	wxString				l_strShare;
	wxString				l_strPlugins;

	// load language if possible, fall back to english otherwise
	if ( wxLocale::IsAvailable( l_lLanguage ) )
	{
		m_pLocale = new wxLocale( l_lLanguage, wxLOCALE_LOAD_DEFAULT );
		// add locale search paths
		l_strShare = l_strCurrent.GetPath() + l_strSeparator + wxT( "share" ) + l_strSeparator + wxT( "ImgConverter" );
		m_pLocale->AddCatalogLookupPathPrefix( l_strShare );
#if defined( _MSC_VER )
#	if defined( NDEBUG )
		l_strPlugins = wxFileName( l_strCurrent.GetPath() ).GetPath() + l_strSeparator + wxT( "plug-ins" ) + l_strSeparator + wxT( "Release" ) + l_strSeparator + wxT( "ImgConverter" );
#	else
		l_strPlugins = wxFileName( l_strCurrent.GetPath() ).GetPath() + l_strSeparator + wxT( "plug-ins" ) + l_strSeparator + wxT( "Debug" ) + l_strSeparator + wxT( "ImgConverter" );
#	endif
		m_pLocale->AddCatalogLookupPathPrefix( l_strPlugins );
#endif
		m_pLocale->AddCatalog( wxT( "ImgConverter" ) );

		if ( !m_pLocale->IsOk() )
		{
			std::cerr << "Selected language is wrong" << std::endl;
			delete m_pLocale;
			l_lLanguage = wxLANGUAGE_ENGLISH;
			m_pLocale = new wxLocale( l_lLanguage );
		}
	}
	else
	{
		std::cout << "The selected language is not supported by your system. Try installing support for this language." << std::endl;
		l_lLanguage = wxLANGUAGE_ENGLISH;
		m_pLocale = new wxLocale( l_lLanguage );
	}

	m_mainFrame = new MainFrame();
	m_mainFrame->Show( true );
	SetTopWindow( m_mainFrame );
	wxInitAllImageHandlers();
	return true;
}

int ImgToIcoApp::OnExit()
{
	wxImage::CleanUpHandlers();
	return wxApp::OnExit();
}

//******************************************************************************
