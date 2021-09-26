#include "CastorTestLauncher/CastorTestLauncher.hpp"
#include "CastorTestLauncher/MainFrame.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/PluginCache.hpp>

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Exception/Exception.hpp>
#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#include <wx/cmdline.h>
#include <wx/image.h>
#pragma warning( pop )

wxIMPLEMENT_APP( test_launcher::CastorTestLauncher );

namespace test_launcher
{
	namespace
	{
#if defined( NDEBUG )
		static constexpr castor::LogType DefaultLogType = castor::LogType::eInfo;
#else
		static constexpr castor::LogType DefaultLogType = castor::LogType::eTrace;
#endif

		namespace option
		{
			namespace lg
			{
				static const wxString Help{ wxT( "help" ) };
				static const wxString ConfigFile{ wxT( "config" ) };
				static const wxString LogLevel{ wxT( "log" ) };
				static const wxString Validate{ wxT( "validate" ) };
				static const wxString Generate{ wxT( "generate" ) };
			}

			namespace st
			{
				static const wxString Help{ wxT( "h" ) };
				static const wxString ConfigFile{ wxT( "c" ) };
				static const wxString LogLevel{ wxT( "l" ) };
				static const wxString Validate{ wxT( "a" ) };
				static const wxString Generate{ wxT( "e" ) };
			}
		}
	}

	CastorTestLauncher::CastorTestLauncher()
	{
#if defined( __WXGTK__ )
		XInitThreads();
#endif
	}

	bool CastorTestLauncher::doParseCommandLine()
	{
		ashes::RendererList list;

		auto result = !list.empty();

		if ( !result )
		{
			CU_Exception( "No renderer plug-ins" );
		}

		static const wxString Help{ _( "Displays this help." ) };
		static const wxString ConfigFile{ _( "Specifies the configuration file." ) };
		static const wxString LogLevel{ _( "Defines log level (from 0=trace to 4=error)." ) };
		static const wxString Validate{ _( "Enables rendering API validation." ) };
		static const wxString Generate{ _( "Generates the reference image, using Vulkan renderer." ) };
		static const wxString SceneFile{ _( "The tested scene file." ) };

		wxCmdLineParser parser( wxApp::argc, wxApp::argv );
		parser.AddSwitch( option::st::Help, option::lg::Help, Help, wxCMD_LINE_OPTION_HELP );
		parser.AddOption( option::st::ConfigFile, option::lg::ConfigFile, ConfigFile, wxCMD_LINE_VAL_STRING, 0 );
		parser.AddSwitch( option::st::Validate, option::lg::Validate, Validate );
		parser.AddOption( option::st::LogLevel, option::lg::LogLevel, LogLevel, wxCMD_LINE_VAL_NUMBER );
		parser.AddSwitch( option::st::Generate, option::lg::Generate, Generate );
		parser.AddParam( SceneFile, wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY );

		for ( auto & plugin : list )
		{
			auto desc = wxString{ plugin.description };
			desc.Replace( wxT( " renderer for Ashes" ), wxEmptyString );
			parser.AddSwitch( wxString{ plugin.name }
				, wxEmptyString
				, _( "Defines the renderer to " ) + desc + wxT( "." ) );
		}

		if ( ( parser.Parse( false ) != 0 )
			|| parser.Found( option::st::Help ) )
		{
			parser.Usage();
			return false;
		}

		Config config{};

		auto has = [&parser]( wxString const & option )
		{
			return parser.Found( option );
		};

		auto getLong = [&parser]( wxString const & option
			, auto defaultValue )
		{
			using ValueT = decltype( defaultValue );
			long value;
			ValueT result;

			if ( parser.Found( option, &value ) )
			{
				result = ValueT( value );
			}
			else
			{
				result = defaultValue;
			}

			return result;
		};

		m_config.log = getLong( option::st::LogLevel, DefaultLogType );
		m_config.validate = has( option::st::Validate );
		m_config.generate = has( option::st::Generate );

		if ( result )
		{
			castor::Logger::initialise( m_config.log );

			if ( parser.Found( wxT( "generate" ) ) )
			{
				m_config.renderer = cuT( "vk" );
				m_outputFileSuffix = cuT( "ref" );
			}
			else
			{
				for ( auto & plugin : list )
				{
					if ( parser.Found( wxString{ plugin.name } ) )
					{
						m_config.renderer = plugin.name;
						m_outputFileSuffix = m_config.renderer;
					}
				}
			}

			if ( m_config.renderer.empty() )
			{
				parser.AddUsageText( _( "Please select a renderer type." ) );
				parser.Usage();
				result = false;
			}

			if ( parser.GetParamCount() > 0 )
			{
				m_config.fileName = castor::Path( parser.GetParam( 0 ).mb_str( wxConvUTF8 ).data() );
			}
		}

		return result;
	}

	castor3d::EngineSPtr CastorTestLauncher::doInitialiseCastor()
	{
		if ( !castor::File::directoryExists( castor3d::Engine::getEngineDirectory() ) )
		{
			castor::File::directoryCreate( castor3d::Engine::getEngineDirectory() );
		}

		castor3d::EngineSPtr castor = std::make_shared< castor3d::Engine >( cuT( "CastorTestLauncher" )
			, castor3d::Version{ CastorTestLauncher_VERSION_MAJOR, CastorTestLauncher_VERSION_MINOR, CastorTestLauncher_VERSION_BUILD }
			, m_config.validate
			, * castor::Logger::getSingleton().getInstance() );
		castor::PathArray arrayFiles;
		castor::File::listDirectoryFiles( castor3d::Engine::getPluginsDirectory(), arrayFiles );

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		if ( !arrayFiles.empty() )
		{
			castor::PathArray arrayFailed;
			castor::PathArray otherPlugins;

			for ( auto file : arrayFiles )
			{
				if ( file.getExtension() == CU_SharedLibExt )
				{
					if ( !castor->getPluginCache().loadPlugin( file ) )
					{
						arrayFailed.push_back( file );
					}
				}
			}
		}

		castor->loadRenderer( m_config.renderer );
		return castor;
	}

	bool CastorTestLauncher::OnInit()
	{
		wxInitAllImageHandlers();

		if ( doParseCommandLine() )
		{
			if ( !castor::File::directoryExists( m_config.fileName.getPath() / cuT( "Compare" ) ) )
			{
				castor::File::directoryCreate( m_config.fileName.getPath() / cuT( "Compare" ) );
			}

			castor::Logger::setFileName( m_config.fileName.getPath() / cuT( "Compare" ) / ( m_config.fileName.getFileName() + cuT( "_" ) + m_config.renderer + cuT( ".log" ) ) );
			castor::Logger::logInfo( cuT( "Start" ) );

			try
			{
				if ( auto engine = doInitialiseCastor() )
				{
					MainFrame * mainFrame{ new MainFrame{ *engine } };

					try
					{
						if ( mainFrame->initialise() )
						{
							castor::Logger::logInfo( cuT( "Load scene" ) );
							mainFrame->loadScene( m_config.fileName );
							castor::Logger::logInfo( cuT( "Save frame" ) );
							mainFrame->saveFrame( m_outputFileSuffix );
							castor::Logger::logInfo( cuT( "Cleanup frame" ) );
							mainFrame->cleanup();
						}

						castor::Logger::logInfo( cuT( "Close window" ) );
						mainFrame->Close();
						delete mainFrame;
					}
					catch ( ... )
					{
						mainFrame->Close();
						delete mainFrame;
						throw;
					}

				}
			}
			catch ( castor::Exception & exc )
			{
				castor::Logger::logError( std::stringstream() << "Initialisation failed : " << exc.getFullDescription() );
			}
			catch ( std::exception & exc )
			{
				castor::Logger::logError( std::stringstream() << "Initialisation failed : " << exc.what() );
			}

			castor::Logger::logInfo( cuT( "Stop" ) );
			castor::Logger::cleanup();
		}

		wxImage::CleanUpHandlers();
		return true;
	}

	int CastorTestLauncher::OnRun()
	{
		return 0;
	}
}
