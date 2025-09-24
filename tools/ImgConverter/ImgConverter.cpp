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
		wxStandardPathsBase const & stdPaths = wxStandardPaths::Get();
		wxFileName current = wxFileName( stdPaths.GetExecutablePath() ).GetPath();
		static wxString const separator = wxFileName::GetPathSeparator();

		// load language if possible, fall back to english otherwise
		if ( wxLocale::IsAvailable( language ) )
		{
			m_locale = std::make_unique< wxLocale >( language, wxLOCALE_LOAD_DEFAULT );
			// add locale search paths
			auto share = current.GetPath() + separator + wxT( "share" ) + separator + wxT( "ImgConverter" );
			wxLocale::AddCatalogLookupPathPrefix( share );
#if defined( _MSC_VER )
#	if defined( NDEBUG )
			auto plugins = wxFileName( current.GetPath() ).GetPath() + separator + wxT( "plug-ins" ) + separator + wxT( "Release" ) + separator + wxT( "ImgConverter" );
#	else
			auto plugins = wxFileName( current.GetPath() ).GetPath() + separator + wxT( "plug-ins" ) + separator + wxT( "Debug" ) + separator + wxT( "ImgConverter" );
#	endif
			wxLocale::AddCatalogLookupPathPrefix( plugins );
#endif
			m_locale->AddCatalog( wxT( "ImgConverter" ) );

			if ( !m_locale->IsOk() )
			{
				std::cerr << "Selected language is wrong" << std::endl;
				m_locale.reset();;
				language = wxLANGUAGE_ENGLISH;
				m_locale = std::make_unique< wxLocale >( language );
			}
		}
		else
		{
			std::cout << "The selected language is not supported by your system. Try installing support for this language." << std::endl;
			language = wxLANGUAGE_ENGLISH;
			m_locale = std::make_unique< wxLocale >( language );
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
