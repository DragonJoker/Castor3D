#include "CastorShape.hpp"
#include "MainFrame.hpp"

#include <wx/cmdline.h>

#include <File.hpp>
#include <Exception.hpp>

using namespace Castor;
using namespace Castor3D;

#if CHECK_MEMORYLEAKS && defined( VLD_AVAILABLE ) && USE_VLD
#	include <vld.h>
#endif

namespace CastorShape
{
	IMPLEMENT_APP( CastorShapeApp )

	bool CastorShapeApp::OnInit()
	{
#if defined( _MSC_VER ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
		long l_lLanguage = wxLANGUAGE_DEFAULT;
		Path l_pathCurrent = File::DirectoryGetCurrent().GetPath();

		// load language if possible, fall back to english otherwise
		if ( wxLocale::IsAvailable( l_lLanguage ) )
		{
			m_locale = std::make_unique< wxLocale >( l_lLanguage, wxLOCALE_LOAD_DEFAULT );
			// add locale search paths
			m_locale->AddCatalogLookupPathPrefix( l_pathCurrent / cuT( "share" ) / cuT( "CastorShape" ) );
#if defined( _MSC_VER )
#	if defined( NDEBUG )
			m_locale->AddCatalogLookupPathPrefix( l_pathCurrent.GetPath() / cuT( "share" ) / cuT( "Release" ) / cuT( "CastorShape" ) );
#	else
			m_locale->AddCatalogLookupPathPrefix( l_pathCurrent.GetPath() / cuT( "share" ) / cuT( "Debug" ) / cuT( "CastorShape" ) );
#	endif
#endif
			m_locale->AddCatalog( wxT( "CastorShape" ) );

			if ( !m_locale->IsOk() )
			{
				std::cerr << "Selected language is wrong" << std::endl;
				l_lLanguage = wxLANGUAGE_ENGLISH;
				m_locale = std::make_unique< wxLocale >( l_lLanguage );
			}
		}
		else
		{
			std::cout << "The selected language is not supported by your system."
					  << "Try installing support for this language." << std::endl;
			l_lLanguage = wxLANGUAGE_ENGLISH;
			m_locale = std::make_unique< wxLocale >( l_lLanguage );
		}

		//wxAppConsole::SetAppDisplayName( wxT( "Castor Shape" ) );
		wxAppConsole::SetAppName( wxT( "castor_shape" ) );
		wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );
		//wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );
		wxCmdLineParser l_parser( argc, argv );
		l_parser.AddSwitch(	wxT( "h" ), wxT( "help" ), _( "Displays this help" ) );
		l_parser.AddOption(	wxT( "l" ), wxT( "log" ), _( "Defines log level" ), wxCMD_LINE_VAL_NUMBER );
		l_parser.AddOption(	wxT( "f" ), wxT( "file" ), _( "Defines initial scene file" ), wxCMD_LINE_VAL_STRING );
		l_parser.AddSwitch(	wxT( "opengl" ), wxEmptyString, _( "Defines the renderer to OpenGl" ) );
		l_parser.AddSwitch(	wxT( "directx" ), wxEmptyString, _( "Defines the renderer to Direct3D (11)" ) );
		bool l_bReturn = l_parser.Parse( false ) == 0;

		// S'il y avait des erreurs ou "-h" ou "--help", on affiche l'aide et on sort
		if ( !l_bReturn || l_parser.Found( wxT( 'h' ) ) )
		{
			l_parser.Usage();
			l_bReturn = false;
		}

		if ( l_bReturn )
		{
			try
			{
				ELogType l_eLogLevel;
				eRENDERER_TYPE l_eRenderer;
				wxString l_strRenderer;

				if ( !l_parser.Found( wxT( 'l' ), reinterpret_cast< long * >( &l_eLogLevel ) ) )
				{
#if defined( NDEBUG )
					l_eLogLevel = ELogType_INFO;
#else
					l_eLogLevel = ELogType_DEBUG;
#endif
				}

				if ( l_parser.Found( wxT( "opengl" ) ) )
				{
					l_eRenderer = eRENDERER_TYPE_OPENGL;
				}
				else if ( l_parser.Found( wxT( "directx" ) ) )
				{
					l_eRenderer = eRENDERER_TYPE_DIRECT3D;
				}
				else
				{
					l_eRenderer	 = eRENDERER_TYPE_UNDEFINED;
				}

				if ( !File::DirectoryExists( Castor3D::Engine::GetEngineDirectory() ) )
				{
					File::DirectoryCreate( Castor3D::Engine::GetEngineDirectory() );
				}

				Logger::Initialise( l_eLogLevel );
				Logger::SetFileName( Castor3D::Engine::GetEngineDirectory() / cuT( "CastorShape.log" ) );
				Logger::LogInfo( cuT( "CastorShape - Start" ) );
				wxInitAllImageHandlers();
				m_mainFrame = new MainFrame( NULL, wxT( "CastorShape"	), l_eRenderer );
				l_bReturn = m_mainFrame->Initialise();

				if ( l_bReturn )
				{
					SetTopWindow( m_mainFrame );
					wxString l_strFileName;

					if ( l_parser.Found( wxT( 'f' ), &l_strFileName ) )
					{
						m_mainFrame->LoadScene( l_strFileName );
					}
				}
				else
				{
					delete m_mainFrame;
					m_mainFrame = NULL;
				}
			}
			catch ( Castor::Exception & exc )
			{
				Logger::LogError( "CastorViewer - Initialisation failed : " + exc.GetFullDescription() );
				l_bReturn = false;
			}
			catch ( std::exception & exc )
			{
				Logger::LogError( std::stringstream() << "CastorViewer - Initialisation failed : " << exc.what() );
				l_bReturn = false;
			}
		}

		return l_bReturn;
	}

	int CastorShapeApp::OnExit()
	{
		m_locale.reset();
		Logger::LogInfo( cuT( "CastorShape - Exit" ) );
		Logger::Cleanup();
		wxImage::CleanUpHandlers();
		return wxApp::OnExit();
	}
}
