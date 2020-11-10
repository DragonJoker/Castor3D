#include "CastorTestLauncher/CastorTestLauncher.hpp"
#include "CastorTestLauncher/MainFrame.hpp"

#include <wx/cmdline.h>
#include <wx/image.h>

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/PluginCache.hpp>

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Exception/Exception.hpp>
#include <CastorUtils/Miscellaneous/StringUtils.hpp>

using namespace castor;
using namespace castor3d;

#if CHECK_MEMORYLEAKS && defined( VLD_AVAILABLE ) && USE_VLD
#	include <vld.h>
#endif

wxIMPLEMENT_APP( test_launcher::CastorTestLauncher );

namespace test_launcher
{
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

		wxCmdLineParser parser( wxApp::argc, wxApp::argv );
		parser.AddSwitch( wxT( "h" ), wxT( "help" ), _( "Displays this help." ) );
		parser.AddSwitch( wxT( "g" ), wxT( "generate" ), _( "Generates the reference image, using Vulkan renderer." ) );
		parser.AddParam( _( "The initial scene file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY );

		for ( auto & plugin : list )
		{
			auto desc = wxString{ plugin.description };
			desc.Replace( wxT( " renderer for Ashes" ), wxEmptyString );
			parser.AddSwitch( wxString{ plugin.name }
				, wxEmptyString
				, _( "Defines the renderer to " ) + desc + wxT( "." ) );
		}

		result = parser.Parse( false ) == 0;

		// S'il y avait des erreurs ou "-h" ou "--help", on affiche l'aide et on sort
		if ( !result || parser.Found( wxT( 'h' ) ) )
		{
			parser.Usage();
			result = false;
		}

		if ( result )
		{
			Logger::initialise( LogType::eInfo );

			if ( parser.Found( wxT( "generate" ) ) )
			{
				m_rendererType = cuT( "vk" );
				m_outputFileSuffix = cuT( "ref" );
			}
			else
			{
				for ( auto & plugin : list )
				{
					if ( parser.Found( wxString{ plugin.name } ) )
					{
						m_rendererType = plugin.name;
						m_outputFileSuffix = m_rendererType;
					}
				}
			}

			if ( m_rendererType.empty() )
			{
				parser.AddUsageText( _( "Please select a renderer type." ) );
				parser.Usage();
				result = false;
			}

			if ( parser.GetParamCount() > 0 )
			{
				m_fileName = castor::Path( parser.GetParam( 0 ).mb_str( wxConvUTF8 ).data() );
			}
		}

		return result;
	}

	castor3d::EngineSPtr CastorTestLauncher::doInitialiseCastor()
	{
		if ( !File::directoryExists( Engine::getEngineDirectory() ) )
		{
			File::directoryCreate( Engine::getEngineDirectory() );
		}

		castor3d::EngineSPtr castor = std::make_shared< Engine >( cuT( "CastorTestLauncher" )
			, castor3d::Version{ CastorTestLauncher_VERSION_MAJOR, CastorTestLauncher_VERSION_MINOR, CastorTestLauncher_VERSION_BUILD }
			, false
			, *Logger::getSingleton().getInstance() );
		PathArray arrayFiles;
		File::listDirectoryFiles( Engine::getPluginsDirectory(), arrayFiles );

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		if ( !arrayFiles.empty() )
		{
			PathArray arrayFailed;
			PathArray otherPlugins;

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

		castor->loadRenderer( m_rendererType );
		return castor;
	}

	bool CastorTestLauncher::OnInit()
	{
		wxInitAllImageHandlers();

		if ( doParseCommandLine() )
		{
			if ( !File::directoryExists( m_fileName.getPath() / cuT( "Compare" ) ) )
			{
				File::directoryCreate( m_fileName.getPath() / cuT( "Compare" ) );
			}

			Logger::setFileName( m_fileName.getPath() / cuT( "Compare" ) / ( m_fileName.getFileName() + cuT( "_" ) + m_rendererType + cuT( ".log" ) ) );
			Logger::logInfo( cuT( "Start" ) );

			try
			{
				if ( auto engine = doInitialiseCastor() )
				{
					MainFrame * mainFrame{ new MainFrame{ *engine } };

					try
					{
						if ( mainFrame->initialise() )
						{
							Logger::logInfo( cuT( "Load scene" ) );
							mainFrame->loadScene( m_fileName );
							Logger::logInfo( cuT( "Save frame" ) );
							mainFrame->saveFrame( m_outputFileSuffix );
							Logger::logInfo( cuT( "Cleanup frame" ) );
							mainFrame->cleanup();
						}

						Logger::logInfo( cuT( "Close window" ) );
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
			catch ( Exception & exc )
			{
				Logger::logError( std::stringstream() << "Initialisation failed : " << exc.getFullDescription() );
			}
			catch ( std::exception & exc )
			{
				Logger::logError( std::stringstream() << "Initialisation failed : " << exc.what() );
			}

			Logger::logInfo( cuT( "Stop" ) );
			Logger::cleanup();
		}

		wxImage::CleanUpHandlers();
		return true;
	}

	int CastorTestLauncher::OnRun()
	{
		return 0;
	}
}
