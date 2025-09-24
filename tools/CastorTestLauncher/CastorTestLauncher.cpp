#include "CastorTestLauncher/CastorTestLauncher.hpp"
#include "CastorTestLauncher/MainFrame.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/PluginCache.hpp>

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Exception/Exception.hpp>
#include <CastorUtils/Miscellaneous/StringUtils.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/cmdline.h>
#include <wx/image.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

wxIMPLEMENT_APP( test_launcher::CastorTestLauncher );

namespace test_launcher
{
	namespace
	{
		namespace option
		{
			namespace lg
			{
				const wxString Help{ wxT( "help" ) };
				const wxString ConfigFile{ wxT( "config" ) };
				const wxString LogLevel{ wxT( "log" ) };
				const wxString Validate{ wxT( "validate" ) };
				const wxString Generate{ wxT( "generate" ) };
				const wxString FrameCount{ wxT( "frames" ) };
				const wxString DisUpdOptim{ wxT( "disable_update_optim" ) };
				const wxString DisRandom{ wxT( "disable_random" ) };
			}

			namespace st
			{
				const wxString Help{ wxT( "h" ) };
				const wxString ConfigFile{ wxT( "c" ) };
				const wxString LogLevel{ wxT( "l" ) };
				const wxString Validate{ wxT( "a" ) };
				const wxString Generate{ wxT( "e" ) };
				const wxString FrameCount{ wxT( "f" ) };
				const wxString DisUpdOptim{ wxT( "d" ) };
				const wxString DisRandom{ wxT( "r" ) };
			}

			namespace df
			{
#if defined( NDEBUG )
				constexpr c3d::LogType LogLevel = c3d::LogType::eInfo;
#else
				constexpr c3d::LogType LogLevel = c3d::LogType::eTrace;
#endif
				constexpr uint32_t FrameCount{ 10u };
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
		static const wxString FrameCount{ _( "The number of frames before capture." ) };
		static const wxString DisUpdOptim{ _( "Disable update optimisations." ) };
		static const wxString DisRandom{ _( "Disable full randomisation in random buffer." ) };
		static const wxString SceneFile{ _( "The tested scene file." ) };

		wxCmdLineParser parser( wxApp::argc, wxApp::argv );
		parser.AddSwitch( option::st::Help, option::lg::Help, Help, wxCMD_LINE_OPTION_HELP );
		parser.AddOption( option::st::ConfigFile, option::lg::ConfigFile, ConfigFile, wxCMD_LINE_VAL_STRING, 0 );
		parser.AddSwitch( option::st::Validate, option::lg::Validate, Validate );
		parser.AddOption( option::st::LogLevel, option::lg::LogLevel, LogLevel, wxCMD_LINE_VAL_NUMBER );
		parser.AddSwitch( option::st::Generate, option::lg::Generate, Generate );
		parser.AddOption( option::st::FrameCount, option::lg::FrameCount, FrameCount, wxCMD_LINE_VAL_NUMBER );
		parser.AddSwitch( option::st::DisUpdOptim, option::lg::DisUpdOptim, DisUpdOptim );
		parser.AddSwitch( option::st::DisRandom, option::lg::DisRandom, DisUpdOptim );
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

		m_config.log = getLong( option::st::LogLevel, option::df::LogLevel );
		m_config.validate = has( option::st::Validate );
		m_config.generate = has( option::st::Generate );
		m_config.maxFrameCount = getLong( option::st::FrameCount, option::df::FrameCount );
		m_config.disableUpdateOptimisations = has( option::st::DisUpdOptim );
		m_config.disableRandom = has( option::st::DisRandom );

		if ( result )
		{
			c3d::Logger::initialise( m_config.log );

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
						m_config.renderer = c3d::makeString( plugin.name );
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
				m_config.fileName = c3d::Path( parser.GetParam( 0 ).mb_str( wxConvUTF8 ).data() );
			}
		}

		return result;
	}

	c3d::EngineUPtr CastorTestLauncher::doInitialiseCastor()const
	{
		if ( !c3d::File::directoryExists( c3d::Engine::getEngineDirectory() ) )
		{
			c3d::File::directoryCreate( c3d::Engine::getEngineDirectory() );
		}

		c3d::EngineConfig config{ cuT( "CastorTestLauncher" )
			, c3d::Version{ CastorTestLauncher_VERSION_MAJOR, CastorTestLauncher_VERSION_MINOR, CastorTestLauncher_VERSION_BUILD }
			, m_config.validate
			, !m_config.disableRandom
			, !m_config.disableUpdateOptimisations };
		auto castor = c3d::makeUnique< c3d::Engine >( c3d::move( config )
			, * c3d::Logger::getSingleton().getInstance() );
		c3d::PathArray arrayFiles;
		c3d::File::listDirectoryFiles( c3d::Engine::getPluginsDirectory(), arrayFiles );

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		if ( !arrayFiles.empty() )
		{
			c3d::PathArray arrayFailed;
			c3d::PathArray otherPlugins;

			for ( auto const & file : arrayFiles )
			{
				if ( file.getExtension() == CU_SharedLibExt
					&& !castor->getPluginCache().loadPlugin( file ) )
				{
					arrayFailed.push_back( file );
				}
			}
		}

		castor->loadRenderer( m_config.renderer );
		return castor;
	}

	void CastorTestLauncher::doRunTest( c3d::Engine & engine )
	{
		FrameTimes frameTimes{ Clock::now() };
		auto mainFrame = c3d::makeRawUnique< MainFrame >( engine, m_config.maxFrameCount );

		try
		{
			if ( mainFrame->initialise() )
			{
				c3d::Logger::logInfo( cuT( "Load scene" ) );
				mainFrame->loadScene( m_config.fileName );
				c3d::Logger::logInfo( cuT( "Save frame" ) );
				mainFrame->saveFrame( m_outputFileSuffix, frameTimes );
				c3d::Logger::logInfo( cuT( "Cleanup frame" ) );
				mainFrame->cleanup( m_outputFileSuffix, frameTimes );
			}

			c3d::Logger::logInfo( cuT( "Close window" ) );
			mainFrame->Close();
		}
		catch ( ... )
		{
			mainFrame->Close();
		}
	}

	bool CastorTestLauncher::OnInit()
	{
		wxInitAllImageHandlers();

		if ( doParseCommandLine() )
		{
			if ( !c3d::File::directoryExists( m_config.fileName.getPath() / cuT( "Compare" ) ) )
			{
				c3d::File::directoryCreate( m_config.fileName.getPath() / cuT( "Compare" ) );
			}

			c3d::Logger::setFileName( m_config.fileName.getPath() / cuT( "Compare" ) / ( m_config.fileName.getFileName() + cuT( "_" ) + m_config.renderer + cuT( ".log" ) ) );
			c3d::Logger::logInfo( cuT( "Start" ) );

			try
			{
				if ( auto engine = doInitialiseCastor() )
					doRunTest( *engine );
			}
			catch ( c3d::Exception & exc )
			{
				c3d::Logger::logError( c3d::makeStringStream() << "Initialisation failed : " << c3d::makeString( exc.getFullDescription() ) );
			}
			catch ( std::exception & exc )
			{
				c3d::Logger::logError( c3d::makeStringStream() << "Initialisation failed : " << c3d::makeString( exc.what() ) );
			}

			c3d::Logger::logInfo( cuT( "Stop" ) );
			c3d::Logger::cleanup();
		}

		wxImage::CleanUpHandlers();
		return true;
	}

	int CastorTestLauncher::OnRun()
	{
		return 0;
	}
}
