#include "CastorViewer/PrecompiledHeader.hpp"

#include "CastorViewer/CastorViewer.hpp"
#include "CastorViewer/MainFrame.hpp"

using namespace CastorViewer;
using namespace Castor;
using namespace Castor3D;

#if CHECK_MEMORYLEAKS && defined( VLD_AVAILABLE ) && USE_VLD
#	include <vld.h>
#endif

IMPLEMENT_APP( CastorViewerApp)

bool CastorViewerApp :: OnInit()
{
#if defined( _WIN32 ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	long 		l_lLanguage		= wxLANGUAGE_DEFAULT;
	Path		l_pathCurrent	= File::DirectoryGetCurrent().GetPath();

    // load language if possible, fall back to english otherwise
    if( wxLocale::IsAvailable( l_lLanguage ) )
    {
        m_pLocale = new wxLocale( l_lLanguage, wxLOCALE_LOAD_DEFAULT );

		// add locale search paths
		m_pLocale->AddCatalogLookupPathPrefix( l_pathCurrent / cuT( "share" ) / cuT( "CastorViewer" ) );
#if defined( _MSC_VER )
#	if defined( NDEBUG )
		m_pLocale->AddCatalogLookupPathPrefix( l_pathCurrent.GetPath() / cuT( "share" ) / cuT( "Release" ) / cuT( "CastorViewer" ) );
#	else
		m_pLocale->AddCatalogLookupPathPrefix( l_pathCurrent.GetPath() / cuT( "share" ) / cuT( "Debug" ) / cuT( "CastorViewer" ) );
#	endif
#endif

        m_pLocale->AddCatalog( wxT("CastorViewer" ) );

        if( !m_pLocale->IsOk() )
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

//	wxAppConsole::SetAppDisplayName(	wxT( "Castor Viewer"	) );
	wxAppConsole::SetAppName(			wxT( "castor_viewer"	) );
	wxAppConsole::SetVendorName(		wxT( "dragonjoker"		) );
//	wxAppConsole::SetVendorDisplayName(	wxT( "DragonJoker"		) );

	wxCmdLineParser l_parser( argc, argv );
	l_parser.AddSwitch(	wxT( "h"			),	wxT( "help"		),	_( "Displays this help"					)							);
	l_parser.AddOption(	wxT( "l"			),	wxT( "log"		),	_( "Defines log level"					),	wxCMD_LINE_VAL_NUMBER	);
	l_parser.AddOption(	wxT( "f"			),	wxT( "file"		),	_( "Defines initial scene file"			),	wxCMD_LINE_VAL_STRING	);
	l_parser.AddSwitch(	wxT( "opengl"		),	wxEmptyString,		_( "Defines the renderer to OpenGl"		)							);
	l_parser.AddSwitch(	wxT( "directx9"		),	wxEmptyString,		_( "Defines the renderer to Direct3D9"	)							);
	l_parser.AddSwitch(	wxT( "directx10"	),	wxEmptyString,		_( "Defines the renderer to Direct3D10"	)							);
	l_parser.AddSwitch(	wxT( "directx11"	),	wxEmptyString,		_( "Defines the renderer to Direct3D11"	)							);

	bool l_bReturn = l_parser.Parse( false ) == 0;

	// S'il y avait des erreurs ou "-h" ou "--help", on affiche l'aide et on sort
	if( !l_bReturn || l_parser.Found( wxT( 'h' ) ) )
	{
		l_parser.Usage();
		delete m_pLocale;
		l_bReturn = false;
	}

	if( l_bReturn )
	{
		try
		{
			eLOG_TYPE		l_eLogLevel;
			eRENDERER_TYPE	l_eRenderer;
			wxString		l_strRenderer;

			if( !l_parser.Found( wxT( 'l' ), reinterpret_cast< long * >( &l_eLogLevel ) ) )
			{
#if defined( NDEBUG )
				l_eLogLevel = eLOG_TYPE_MESSAGE;
#else
				l_eLogLevel = eLOG_TYPE_DEBUG;
#endif
			}

			if( l_parser.Found( wxT( "opengl" ) ) )
			{
				l_eRenderer = eRENDERER_TYPE_OPENGL;
			}
			else if( l_parser.Found( wxT( "directx11" ) ) )
			{
				l_eRenderer = eRENDERER_TYPE_DIRECT3D11;
			}
			else if( l_parser.Found( wxT( "directx10" ) ) )
			{
				l_eRenderer = eRENDERER_TYPE_DIRECT3D10;
			}
			else if( l_parser.Found( wxT( "directx9" ) ) )
			{
				l_eRenderer = eRENDERER_TYPE_DIRECT3D9;
			}
			else
			{
				l_eRenderer	 = eRENDERER_TYPE_UNDEFINED;
			}

			Logger::Initialise( l_eLogLevel );
			Logger::SetFileName( File::DirectoryGetCurrent() / cuT( "CastorViewer.log" ) );
			Logger::LogMessage( cuT( "CastorViewer - Start" ) );

			wxInitAllImageHandlers();
			m_pMainFrame = new MainFrame( NULL, wxT( "CastorViewer" ), l_eRenderer );
			l_bReturn = m_pMainFrame->Initialise();

			if( l_bReturn )
			{
				SetTopWindow( m_pMainFrame );
				wxString l_strFileName;

				if( l_parser.Found( wxT( 'f' ), &l_strFileName ) )
				{
					m_pMainFrame->LoadScene( l_strFileName );
				}
			}
			else
			{
				delete m_pMainFrame;
				m_pMainFrame = NULL;
			}
		}
		catch( ... )
		{
			l_bReturn = false;
		}
	}

	return l_bReturn;
}

int CastorViewerApp :: OnExit()
{
	delete m_pLocale;
	Logger::LogMessage( cuT( "CastorViewer - Exit" ) );
	Logger::Cleanup();
	wxImage::CleanUpHandlers();
	return wxApp::OnExit();
}
