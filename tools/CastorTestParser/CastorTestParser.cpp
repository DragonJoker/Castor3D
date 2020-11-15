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
	namespace
	{
#if defined( _WIN32 )
		static castor::String const BinExt = cuT( ".exe" );
		static castor::String const DynlibExt = cuT( ".dll" );
		static castor::String const DynlibPre;
#else
		static castor::String const BinExt;
		static castor::String const DynlibExt = cuT( ".so" );
		static castor::String const DynlibPre = cuT( "lib" );
#endif

		namespace option
		{
			static const wxString Database{ wxT( "database" ) };
			static const wxString Diff{ wxT( "diff" ) };
			static const wxString Test{ wxT( "test" ) };
			static const wxString Launcher{ wxT( "launcher" ) };
			static const wxString Viewer{ wxT( "viewer" ) };
			static const wxString Work{ wxT( "work" ) };
			static const wxString Castor3D{ wxT( "castor3d" ) };
		}

		struct Options
		{
			Options( int argc, wxCmdLineArgsArray const & argv )
				: parser{ argc, argv }
			{
				parser.AddSwitch( wxT( "h" ), wxT( "help" ), _( "Displays this help." ) );
				parser.AddSwitch( wxT( "s" ), wxT( "skip" ), _( "Skip database initialisation." ) );
				parser.AddOption( wxT( "c" ), wxT( "config" ), _( "Specifies the tests config file." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "d" ), option::Database, _( "Specifies the database file." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "i" ), option::Diff, _( "Path to DiffImage." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "t" ), option::Test, _( "Specifies the tests directory." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "l" ), option::Launcher, _( "Path to CastorTestLauncher." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "v" ), option::Viewer, _( "Path to CastorViewer." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "w" ), option::Work, _( "Specifies the working directory." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "a" ), option::Castor3D, _( "Specifies the path to Castor3D's shared library." ), wxCMD_LINE_VAL_STRING, 0 );

				if ( ( parser.Parse( false ) != 0 )
					|| parser.Found( wxT( 'h' ) ) )
				{
					parser.Usage();
					throw false;
				}

				configFile = new wxFileConfig{ wxEmptyString
					, wxEmptyString
					, findConfigFile( parser )
					, wxEmptyString
					, wxCONFIG_USE_LOCAL_FILE };
			}

			~Options()
			{
				delete configFile;
			}

			bool has( wxString const & option )
			{
				return parser.Found( option );
			}

			castor::Path get( wxString const & option
				, bool mandatory
				, castor::Path const & defaultValue = castor::Path{} )
			{
				wxString value;
				castor::Path result;

				if ( parser.Found( option, &value ) )
				{
					result = castor::Path( value.mb_str( wxConvUTF8 ).data() );
				}
				else if ( configFile->HasEntry( option ) )
				{
					configFile->Read( option, &value );
					result = castor::Path( value.mb_str( wxConvUTF8 ).data() );
				}
				else if ( mandatory )
				{
					parser.SetLogo( wxT( "Missing mandatory directory" ) );
					parser.Usage();
					throw false;
				}
				else
				{
					result = defaultValue;
				}

				if ( mandatory
					&& !castor::File::directoryExists( result ) )
				{
					parser.SetLogo( wxT( "Mandatory directory doesn't exist: " ) + result );
					parser.Usage();
					throw false;
				}

				return result;
			}

			void write( Config const & config )
			{
				configFile->Write( option::Test, makeWxString( config.test ) );
				configFile->Write( option::Work, makeWxString( config.work ) );
				configFile->Write( option::Database, makeWxString( config.database ) );
				configFile->Write( option::Launcher, makeWxString( config.launcher ) );
				configFile->Write( option::Viewer, makeWxString( config.viewer ) );
				configFile->Write( option::Diff, makeWxString( config.differ ) );
				configFile->Write( option::Castor3D, makeWxString( config.castor ) );
			}

			static wxString findConfigFile( wxCmdLineParser const & parser )
			{
				wxString cfg;
				parser.Found( wxT( 'c' ), &cfg );
				return cfg;
			}

		private:
			wxCmdLineParser parser;
			wxFileConfig * configFile{ nullptr };
		};
	}

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
		wxAppConsole::SetAppDisplayName( wxT( "Castor3D Tests Monitor" ) );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );
#endif

		try
		{
			Options options{ wxApp::argc, wxApp::argv };
			config.test = options.get( option::Test, true );
			config.work = options.get( option::Work, false, config.test );
			config.database = options.get( option::Database, false, config.work / cuT( "db.sqlite" ) );
			config.launcher = options.get( option::Launcher, false, castor::File::getExecutableDirectory() / ( cuT( "CastorTestLauncher" ) + BinExt ) );
			config.viewer = options.get( option::Viewer, false, castor::File::getExecutableDirectory() / ( cuT( "CastorViewer" ) + BinExt ) );
			config.differ = options.get( option::Diff, false, castor::File::getExecutableDirectory() / ( cuT( "DiffImage" ) + BinExt ) );
			config.castor = options.get( option::Castor3D, false, castor::File::getExecutableDirectory() / ( DynlibPre + cuT( "Castor3D" ) + DynlibExt ) );
			config.skip = options.has( wxT( 's' ) );
			options.write( config );
		}
		catch ( bool )
		{
			return false;
		}

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

			if ( !castor::File::directoryExists( config.work ) )
			{
				castor::File::directoryCreate( config.work );
			}

			castor::Logger::initialise( castor::LogType::eInfo );
			castor::Logger::setFileName( config.work / cuT( "Results.log" ) );
			castor::Logger::logInfo( "Test folder: " + config.test );
			castor::Logger::logInfo( "Work folder: " + config.work );
			castor::Logger::logInfo( "Database: " + config.database );

			try
			{
				MainFrame * mainFrame{ new MainFrame{ std::move( config ) } };
				SetTopWindow( mainFrame );
				mainFrame->Show();
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
