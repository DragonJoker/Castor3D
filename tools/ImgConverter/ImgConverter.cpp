#include "ImgConverter/ImgConverter.hpp"
#include "ImgConverter/MainFrame.hpp"

#pragma warning( push )
#pragma warning( disable: 4251 )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#pragma warning( disable: 5262 )
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/stdpaths.h>

#include <iostream>
#pragma warning( pop )

IMPLEMENT_APP( ImgToIco::ImgToIcoApp )

namespace ImgToIco
{
	bool ImgToIcoApp::OnInit()
	{
		int language = wxLANGUAGE_DEFAULT;
		wxStandardPathsBase & stdPaths = wxStandardPaths::Get();
		wxFileName current = wxFileName( stdPaths.GetExecutablePath() ).GetPath();
		static wxString const separator = wxFileName::GetPathSeparator();

		// load language if possible, fall back to english otherwise
		if ( wxLocale::IsAvailable( language ) )
		{
			m_pLocale = new wxLocale( language, wxLOCALE_LOAD_DEFAULT );
			// add locale search paths
			auto share = current.GetPath() + separator + wxT( "share" ) + separator + wxT( "ImgConverter" );
			m_pLocale->AddCatalogLookupPathPrefix( share );
#if defined( _MSC_VER )
#	if defined( NDEBUG )
			auto plugins = wxFileName( current.GetPath() ).GetPath() + separator + wxT( "plug-ins" ) + separator + wxT( "Release" ) + separator + wxT( "ImgConverter" );
#	else
			auto plugins = wxFileName( current.GetPath() ).GetPath() + separator + wxT( "plug-ins" ) + separator + wxT( "Debug" ) + separator + wxT( "ImgConverter" );
#	endif
			m_pLocale->AddCatalogLookupPathPrefix( plugins );
#endif
			m_pLocale->AddCatalog( wxT( "ImgConverter" ) );

			if ( !m_pLocale->IsOk() )
			{
				std::cerr << "Selected language is wrong" << std::endl;
				delete m_pLocale;
				language = wxLANGUAGE_ENGLISH;
				m_pLocale = new wxLocale( language );
			}
		}
		else
		{
			std::cout << "The selected language is not supported by your system. Try installing support for this language." << std::endl;
			language = wxLANGUAGE_ENGLISH;
			m_pLocale = new wxLocale( language );
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
}
