#include "CastorTestLauncher.hpp"
#include "MainFrame.hpp"

#include <wx/cmdline.h>
#include <wx/image.h>

#include <Engine.hpp>
#include <Plugin/RendererPlugin.hpp>

#include <Data/File.hpp>
#include <Exception/Exception.hpp>
#include <Miscellaneous/StringUtils.hpp>

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
		wxCmdLineParser parser( wxApp::argc, wxApp::argv );
		parser.AddSwitch( wxT( "h" ), wxT( "help" ), _( "Displays this help." ) );
		parser.AddSwitch( wxT( "opengl3" ), wxEmptyString, _( "Defines the renderer to OpenGl 3.x." ) );
		parser.AddSwitch( wxT( "opengl4" ), wxEmptyString, _( "Defines the renderer to OpenGl 4.x." ) );
		parser.AddSwitch( wxT( "vulkan" ), wxEmptyString, _( "Defines the renderer to Vulkan." ) );
		parser.AddParam( _( "The initial scene file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY );
		bool result = parser.Parse( false ) == 0;

		// S'il y avait des erreurs ou "-h" ou "--help", on affiche l'aide et on sort
		if ( !result || parser.Found( wxT( 'h' ) ) )
		{
			parser.Usage();
			result = false;
		}

		if ( result )
		{
			Logger::initialise( LogType::eInfo );

			if ( parser.Found( wxT( "opengl3" ) ) )
			{
				m_rendererType = cuT( "opengl3" );
			}
			else if ( parser.Found( wxT( "opengl4" ) ) )
			{
				m_rendererType = cuT( "opengl4" );
			}
			else if ( parser.Found( wxT( "vulkan" ) ) )
			{
				m_rendererType = cuT( "vulkan" );
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

		castor3d::EngineSPtr castor = std::make_shared< Engine >( cuT( "CastorTestLauncher" ), false );
		PathArray arrayFiles;
		File::listDirectoryFiles( Engine::getPluginsDirectory(), arrayFiles );

		// Exclude debug plug-in in release builds, and release plug-ins in debug builds
		if ( !arrayFiles.empty() )
		{
			PathArray arrayFailed;
			PathArray otherPlugins;

			for ( auto file : arrayFiles )
			{
				if ( file.getExtension() == CASTOR_DLL_EXT )
				{
					if ( !castor->getPluginCache().loadPlugin( file ) )
					{
						arrayFailed.push_back( file );
					}
				}
			}
		}

		auto renderers = castor->getPluginCache().getPlugins( PluginType::eRenderer );

		if ( renderers.empty() )
		{
			CASTOR_EXCEPTION( "No renderer plug-ins" );
		}

		if ( m_rendererType == RENDERER_TYPE_UNDEFINED )
		{
			CASTOR_EXCEPTION( "Renderer plug-in was not selected" );
		}

		castor->loadRenderer( m_rendererType );
		return castor;
	}

	bool CastorTestLauncher::OnInit()
	{
		wxInitAllImageHandlers();

		if ( doParseCommandLine() )
		{
			Logger::setFileName( m_fileName.getPath() / cuT( "Compare" ) / m_fileName.getFileName() + cuT( "_" ) + m_rendererType + cuT( ".log" ) );
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
							mainFrame->saveFrame( m_rendererType );
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
		return false;
	}
}
