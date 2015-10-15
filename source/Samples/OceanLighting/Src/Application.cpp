#include "Application.hpp"
#include "MainFrame.hpp"

using namespace Castor;

IMPLEMENT_APP( OceanLighting::Application )

namespace OceanLighting
{
	bool Application::OnInit()
	{
		long 		l_lLanguage		= wxLANGUAGE_DEFAULT;
		Path		l_pathCurrent	= File::GetWorkingDirectory().GetPath();

		// load language if possible, fall back to english otherwise
		if ( wxLocale::IsAvailable( l_lLanguage ) )
		{
			m_pLocale = new wxLocale( l_lLanguage, wxLOCALE_LOAD_DEFAULT );
			// add locale search paths
			m_pLocale->AddCatalogLookupPathPrefix( l_pathCurrent / cuT( "share" ) / cuT( "OceanLighting" ) );
#if defined( _MSC_VER )
#	if defined( NDEBUG )
			m_pLocale->AddCatalogLookupPathPrefix( l_pathCurrent.GetPath() / cuT( "share" ) / cuT( "Release" ) / cuT( "OceanLighting" ) );
#	else
			m_pLocale->AddCatalogLookupPathPrefix( l_pathCurrent.GetPath() / cuT( "share" ) / cuT( "Debug" ) / cuT( "OceanLighting" ) );
#	endif
#endif
			m_pLocale->AddCatalog( wxT( "OceanLighting" ) );

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
			std::cout << "The selected language is not supported by your system."
					  << "Try installing support for this language." << std::endl;
			l_lLanguage = wxLANGUAGE_ENGLISH;
			m_pLocale = new wxLocale( l_lLanguage );
		}

		//	wxAppConsole::SetAppDisplayName(	wxT( "Ocean Lighting"	) );
		wxAppConsole::SetAppName(	wxT( "ocean_lighting"	) );
		wxAppConsole::SetVendorName(	wxT( "dragonjoker"	) );
		//	wxAppConsole::SetVendorDisplayName(	wxT( "DragonJoker"		) );
		wxCmdLineParser l_parser( argc, argv );
		l_parser.AddSwitch(	wxT( "h"	),	wxT( "help"	),	_( "Displays this help"	)	);
		l_parser.AddOption(	wxT( "l"	),	wxT( "log"	),	_( "Defines log level"	),	wxCMD_LINE_VAL_NUMBER	);
		bool l_bReturn = l_parser.Parse( false ) == 0;

		// S'il y avait des erreurs ou "-h" ou "--help", on affiche l'aide et on sort
		if ( !l_bReturn || l_parser.Found( wxT( 'h' ) ) )
		{
			l_parser.Usage();
			delete m_pLocale;
			l_bReturn = false;
		}

		if ( l_bReturn )
		{
			try
			{
				ELogType l_eLogLevel;

				if ( !l_parser.Found( wxT( 'l' ), reinterpret_cast< long * >( &l_eLogLevel ) ) )
				{
#if defined( NDEBUG )
					l_eLogLevel = ELogType_INFO;
#else
					l_eLogLevel = ELogType_DEBUG;
#endif
				}

				Logger::Initialise( l_eLogLevel );
				Logger::SetFileName( File::GetWorkingDirectory() / cuT( "OceanLighting.log" ) );
				Logger::LogInfo( cuT( "OceanLighting - Start" ) );
				wxInitAllImageHandlers();
				m_pMainFrame = new MainFrame();
				SetTopWindow( m_pMainFrame );
				m_pMainFrame->Show();

				if ( !m_pMainFrame->Initialise() )
				{
					delete m_pMainFrame;
					m_pMainFrame = NULL;
					l_bReturn = false;
				}
			}
			catch ( ... )
			{
				l_bReturn = false;
			}
		}

		return l_bReturn;
	}

	int Application::OnExit()
	{
		delete m_pLocale;
		Logger::LogInfo( cuT( "OceanLighting - Exit" ) );
		Logger::Cleanup();
		wxImage::CleanUpHandlers();
		return wxApp::OnExit();
	}
}
