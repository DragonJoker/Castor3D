#include "CastorTestParser/CastorTestParser.hpp"
#include "CastorTestParser/MainFrame.hpp"

#include <wx/cmdline.h>
#include <wx/fileconf.h>
#include <wx/image.h>

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Exception/Exception.hpp>
#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Miscellaneous/StringUtils.hpp>

wxIMPLEMENT_APP( test_parser::CastorTestParser );

namespace test_parser
{
	CastorTestParser::CastorTestParser()
	{
#if defined( __WXGTK__ )
		XInitThreads();
#endif
	}

	bool CastorTestParser::doParseCommandLine( Config & config )
	{
		wxAppConsole::SetAppName( wxT( "castor_tests_parser" ) );
		wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );

#if wxCHECK_VERSION( 2, 9, 0 )
		wxAppConsole::SetAppDisplayName( wxT( "Castor3D Tests Parser" ) );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );
#endif

		wxCmdLineParser parser( wxApp::argc, wxApp::argv );
		parser.AddSwitch( wxT( "h" ), wxT( "help" ), _( "Displays this help." ) );
		parser.AddSwitch( wxT( "s" ), wxT( "skip" ), _( "Skip database initialisation." ) );
		parser.AddOption( wxT( "f" ), wxT( "folder" ), _( "Specifies the tests directory." ), wxCMD_LINE_VAL_STRING, 0 );
		parser.AddOption( wxT( "c" ), wxT( "config" ), _( "Specifies the tests config file." ), wxCMD_LINE_VAL_STRING, 0 );
		parser.AddOption( wxT( "w" ), wxT( "work" ), _( "Specifies the working directory." ), wxCMD_LINE_VAL_STRING, 0 );

		if ( ( parser.Parse( false ) != 0 )
			|| parser.Found( wxT( 'h' ) ) )
		{
			parser.Usage();
			return false;
		}

		castor::Logger::initialise( castor::LogType::eInfo );
		wxString value;
		wxString cfgFile{};
		parser.Found( wxT( 'c' ), &value );
		static const wxString Entry_Folder{ wxT( "Folder" ) };
		static const wxString Entry_Work{ wxT( "Work" ) };
		wxFileConfig configFile{ wxEmptyString, wxEmptyString, value, wxEmptyString, wxCONFIG_USE_LOCAL_FILE };

		if ( parser.Found( wxT( 'f' ), &value ) )
		{
			config.test = castor::Path( value.mb_str( wxConvUTF8 ).data() );
		}
		else if ( configFile.HasEntry( Entry_Folder ) )
		{
			configFile.Read( Entry_Folder, &value );
			config.test = castor::Path( value.mb_str( wxConvUTF8 ).data() );
		}
		else
		{
			parser.SetLogo( wxT( "Missing mandatory tests directory" ) );
			parser.Usage();
			return false;
		}

		if ( parser.Found( wxT( 'w' ), &value ) )
		{
			config.work = castor::Path( value.mb_str( wxConvUTF8 ).data() );
		}
		else if ( configFile.HasEntry( Entry_Work ) )
		{
			configFile.Read( Entry_Work, &value );
			config.work = castor::Path( value.mb_str( wxConvUTF8 ).data() );
		}
		else
		{
			config.work = config.test;
		}

		if ( parser.Found( wxT( 's' ) ) )
		{
			config.skip = true;
		}

#if defined( _WIN32 )
		static castor::String const BinExt = cuT( ".exe" );
#else
		static castor::String const BinExt;
#endif

		config.launcher = castor::File::getExecutableDirectory() / ( cuT( "CastorTestLauncher" ) + BinExt );
		config.viewer = castor::File::getExecutableDirectory() / ( cuT( "CastorViewer" ) + BinExt );
		config.differ = castor::File::getExecutableDirectory() / ( cuT( "DiffImage" ) + BinExt );

		configFile.Write( Entry_Folder, makeWxString( config.test ) );
		configFile.Write( Entry_Work, makeWxString( config.work ) );

		return true;
	}

	bool CastorTestParser::OnInit()
	{
		Config config;
		auto result = doParseCommandLine( config );

		if ( result )
		{
			result = false;
			wxInitAllImageHandlers();

			if ( !castor::File::directoryExists( config.test ) )
			{
				castor::Logger::logError( std::stringstream() << "Tests folder [" << config.test << "] doesn't exist." );
			}

			if ( !castor::File::directoryExists( config.work ) )
			{
				castor::File::directoryCreate( config.work );
			}

			castor::Logger::setFileName( config.work / cuT( "Results.log" ) );
			castor::Logger::logInfo( "Test folder: " + config.test );
			castor::Logger::logInfo( "Work folder: " + config.work );

			try
			{
				MainFrame * mainFrame{ new MainFrame{ std::move( config ) } };
				SetTopWindow( mainFrame );
				mainFrame->initialise();
				result = true;
			}
			catch ( castor::Exception & exc )
			{
				castor::Logger::logError( std::stringstream() << "Initialisation failed : " << exc.getFullDescription() );
			}
			catch ( std::exception & exc )
			{
				castor::Logger::logError( std::stringstream() << "Initialisation failed : " << exc.what() );
			}
		}

		if ( !result )
		{
			castor::Logger::logInfo( cuT( "Stop" ) );
			castor::Logger::cleanup();
			wxImage::CleanUpHandlers();
		}

		return result;
	}

	int CastorTestParser::OnExit()
	{
		castor::Logger::logInfo( cuT( "Stop" ) );
		castor::Logger::cleanup();
		wxImage::CleanUpHandlers();
		return 0;
	}
}
