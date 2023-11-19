#include "GuiCommon/System/CastorApplication.hpp"

#include "GuiCommon/System/RendererSelector.hpp"
#include "GuiCommon/System/SplashScreen.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/PluginCache.hpp>

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Exception/Exception.hpp>

#include <ashespp/Core/RendererList.hpp>

namespace app
{
#include "GuiCommon/xpms/animated_object_group.xpm"
#include "GuiCommon/xpms/animated_object_group_sel.xpm"
#include "GuiCommon/xpms/animated_object.xpm"
#include "GuiCommon/xpms/animated_object_sel.xpm"
#include "GuiCommon/xpms/animation.xpm"
#include "GuiCommon/xpms/animation_sel.xpm"
#include "GuiCommon/xpms/background.xpm"
#include "GuiCommon/xpms/background_sel.xpm"
#include "GuiCommon/xpms/billboard.xpm"
#include "GuiCommon/xpms/billboard_sel.xpm"
#include "GuiCommon/xpms/bone.xpm"
#include "GuiCommon/xpms/bone_sel.xpm"
#include "GuiCommon/xpms/border_panel.xpm"
#include "GuiCommon/xpms/border_panel_sel.xpm"
#include "GuiCommon/xpms/camera.xpm"
#include "GuiCommon/xpms/camera_sel.xpm"
#include "GuiCommon/xpms/castor_transparent.xpm"
#include "GuiCommon/xpms/collapse_all.xpm"
#include "GuiCommon/xpms/colour_grading.xpm"
#include "GuiCommon/xpms/colour_grading_sel.xpm"
#include "GuiCommon/xpms/control.xpm"
#include "GuiCommon/xpms/control_sel.xpm"
#include "GuiCommon/xpms/controls.xpm"
#include "GuiCommon/xpms/controls_sel.xpm"
#include "GuiCommon/xpms/directional.xpm"
#include "GuiCommon/xpms/directional_sel.xpm"
#include "GuiCommon/xpms/expand_all.xpm"
#include "GuiCommon/xpms/frame_variable.xpm"
#include "GuiCommon/xpms/frame_variable_sel.xpm"
#include "GuiCommon/xpms/frame_variable_buffer.xpm"
#include "GuiCommon/xpms/frame_variable_buffer_sel.xpm"
#include "GuiCommon/xpms/geometry.xpm"
#include "GuiCommon/xpms/geometry_sel.xpm"
#include "GuiCommon/xpms/node.xpm"
#include "GuiCommon/xpms/node_sel.xpm"
#include "GuiCommon/xpms/material.xpm"
#include "GuiCommon/xpms/material_sel.xpm"
#include "GuiCommon/xpms/pass.xpm"
#include "GuiCommon/xpms/pass_sel.xpm"
#include "GuiCommon/xpms/panel.xpm"
#include "GuiCommon/xpms/panel_sel.xpm"
#include "GuiCommon/xpms/point.xpm"
#include "GuiCommon/xpms/point_sel.xpm"
#include "GuiCommon/xpms/post_effect.xpm"
#include "GuiCommon/xpms/post_effect_sel.xpm"
#include "GuiCommon/xpms/render_target.xpm"
#include "GuiCommon/xpms/render_target_sel.xpm"
#include "GuiCommon/xpms/render_window.xpm"
#include "GuiCommon/xpms/render_window_sel.xpm"
#include "GuiCommon/xpms/scene.xpm"
#include "GuiCommon/xpms/scene_sel.xpm"
#include "GuiCommon/xpms/skeleton.xpm"
#include "GuiCommon/xpms/skeleton_sel.xpm"
#include "GuiCommon/xpms/spot.xpm"
#include "GuiCommon/xpms/spot_sel.xpm"
#include "GuiCommon/xpms/styles.xpm"
#include "GuiCommon/xpms/styles_sel.xpm"
#include "GuiCommon/xpms/style.xpm"
#include "GuiCommon/xpms/style_sel.xpm"
#include "GuiCommon/xpms/submesh.xpm"
#include "GuiCommon/xpms/submesh_sel.xpm"
#include "GuiCommon/xpms/ssao_config.xpm"
#include "GuiCommon/xpms/ssao_config_sel.xpm"
#include "GuiCommon/xpms/text.xpm"
#include "GuiCommon/xpms/text_sel.xpm"
#include "GuiCommon/xpms/texture.xpm"
#include "GuiCommon/xpms/texture_sel.xpm"
#include "GuiCommon/xpms/theme.xpm"
#include "GuiCommon/xpms/theme_sel.xpm"
#include "GuiCommon/xpms/tone_mapping.xpm"
#include "GuiCommon/xpms/tone_mapping_sel.xpm"
#include "GuiCommon/xpms/viewport.xpm"
#include "GuiCommon/xpms/viewport_sel.xpm"
}

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#if defined( __WXGTK__ )
#	include <X11/Xlib.h>
#endif
#include <wx/cmdline.h>
#include <wx/display.h>
#include <wx/fileconf.h>
#include <wx/propgrid/propgrid.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace GuiCommon
{
	namespace
	{
		static constexpr uint32_t DefaultGpuIndex = 0u;
#if defined( NDEBUG )
		static constexpr castor::LogType DefaultLogType = castor::LogType::eInfo;
		static constexpr bool DefaultValidation = false;
#else
		static constexpr castor::LogType DefaultLogType = castor::LogType::eTrace;
		static constexpr bool DefaultValidation = true;
#endif

		namespace option
		{
			namespace lg
			{
				static const wxString Help{ wxT( "help" ) };
				static const wxString Config{ wxT( "config" ) };
				static const wxString LogLevel{ wxT( "log" ) };
				static const wxString Validate{ wxT( "validate" ) };
				static const wxString ShaderDebugLevel{ wxT( "shader_debug_level" ) };
				static const wxString SyncRender{ wxT( "sync" ) };
				static const wxString UnlimFPS{ wxT( "unlimited" ) };
				static const wxString FixedFPS{ wxT( "fps" ) };
				static const wxString GpuIndex{ wxT( "gpu" ) };
				static const wxString DisUpdOptim{ wxT( "disable_update_optim" ) };
				static const wxString DisRandom{ wxT( "disable_random" ) };
				static const wxString EnShaderVal{ wxT( "shader_validation" ) };
				static const wxString EnApiTrace{ wxT( "api_trace" ) };
				static const wxString KeepTextShd{ wxT( "text_shaders" ) };
				static const wxString MaxImgSize{ wxT( "max_image_size" ) };
				static const wxString WaitDebugger{ wxT( "wait_debugger" ) };
				static const wxString DebugTargets{ wxT( "debug_targets" ) };
			}

			namespace st
			{
				static const wxString Validate{ wxT( "a" ) };
				static const wxString Config{ wxT( "c" ) };
				static const wxString DisUpdOptim{ wxT( "d" ) };
				static const wxString ShaderDebugLevel{ wxT( "e" ) };
				static const wxString FixedFPS{ wxT( "f" ) };
				static const wxString GpuIndex{ wxT( "g" ) };
				static const wxString Help{ wxT( "h" ) };
				static const wxString MaxImgSize{ wxT( "i" ) };
				static const wxString KeepTextShd{ wxT( "k" ) };
				static const wxString LogLevel{ wxT( "l" ) };
				static const wxString DisRandom{ wxT( "r" ) };
				static const wxString SyncRender{ wxT( "s" ) };
				static const wxString EnApiTrace{ wxT( "t" ) };
				static const wxString UnlimFPS{ wxT( "u" ) };
				static const wxString EnShaderVal{ wxT( "v" ) };
				static const wxString WaitDebugger{ wxT( "w" ) };
				static const wxString DebugTargets{ wxT( "dt" ) };
			}
		}

		struct Options
		{
			Options( Options const & ) = delete;
			Options & operator=( Options const & ) = delete;
			Options( Options && ) = delete;
			Options & operator=( Options && ) = delete;

			Options( int argc, wxCmdLineArgsArray const & argv )
				: parser{ argc, argv }
			{
				static const wxString Help{ _( "Displays this help." ) };
				static const wxString Config{ _( "Specifies the configuration file." ) };
				static const wxString LogLevel{ _( "Defines log level (from 0=trace to 4=error)." ) };
				static const wxString Validate{ _( "Enables rendering API validation." ) };
				static const wxString ShaderDebugLevel{ _( "Defines the shader debug level on compilation (0=none, 1=names, 2=full." ) };
				static const wxString SyncRender{ _( "Sets the rendering to synchronous (render loop is user triggered)." ) };
				static const wxString UnlimFPS{ _( "Disables FPS limit (has no effect if '" ) + option::lg::SyncRender + _( "' option is specified)." ) };
				static const wxString FixedFPS{ _( "Defines wanted FPS (has no effect if '" ) + option::lg::UnlimFPS + _( "' option is specified)." ) };
				static const wxString GpuIndex{ _( "The index of the wanted Vulkan physical device." ) };
				static const wxString DisUpdOptim{ _( "Disable update optimisations." ) };
				static const wxString DisRandom{ _( "Disable full random in random buffer." ) };
				static const wxString EnShaderVal{ _( "Enable Shaders Validation." ) };
				static const wxString EnApiTrace{ _( "Enable API Trace." ) };
				static const wxString KeepTextShd{ _( "Keep text shaders." ) };
				static const wxString MaxImgSize{ _( "The maximum material images size." ) };
				static const wxString SceneFile{ _( "The initial scene file." ) };
				static const wxString WaitDebugger{ _( "Wait for debugger at startup." ) };
				static const wxString DebugTargets{ _( "Enable debug targets." ) };

				parser.AddSwitch( option::st::Help, option::lg::Help, Help, wxCMD_LINE_OPTION_HELP );
				parser.AddOption( option::st::Config, option::lg::Config, Config, wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( option::st::LogLevel, option::lg::LogLevel, LogLevel, wxCMD_LINE_VAL_NUMBER );
				parser.AddSwitch( option::st::Validate, option::lg::Validate, Validate );
				parser.AddOption( option::st::ShaderDebugLevel, option::lg::ShaderDebugLevel, ShaderDebugLevel, wxCMD_LINE_VAL_NUMBER );
				parser.AddSwitch( option::st::SyncRender, option::lg::SyncRender, SyncRender );
				parser.AddSwitch( option::st::UnlimFPS, option::lg::UnlimFPS, UnlimFPS );
				parser.AddOption( option::st::FixedFPS, option::lg::FixedFPS, FixedFPS, wxCMD_LINE_VAL_NUMBER );
				parser.AddOption( option::st::GpuIndex, option::lg::GpuIndex, GpuIndex, wxCMD_LINE_VAL_NUMBER );
				parser.AddSwitch( option::st::DisUpdOptim, option::lg::DisUpdOptim, DisUpdOptim );
				parser.AddSwitch( option::st::DisRandom, option::lg::DisRandom, DisRandom );
				parser.AddSwitch( option::st::EnShaderVal, option::lg::EnShaderVal, EnShaderVal );
				parser.AddSwitch( option::st::EnApiTrace, option::lg::EnApiTrace, EnApiTrace );
				parser.AddSwitch( option::st::KeepTextShd, option::lg::KeepTextShd, KeepTextShd );
				parser.AddOption( option::st::MaxImgSize, option::lg::MaxImgSize, MaxImgSize, wxCMD_LINE_VAL_NUMBER );
				parser.AddSwitch( option::st::WaitDebugger, option::lg::WaitDebugger, WaitDebugger );
				parser.AddSwitch( option::st::DebugTargets, option::lg::DebugTargets, DebugTargets );
				parser.AddParam( SceneFile, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );

				for ( auto & plugin : list )
				{
					auto desc = make_wxString( plugin.description );
					desc.Replace( wxT( " renderer for Ashes" ), wxEmptyString );
					parser.AddSwitch( make_wxString( plugin.name )
						, wxEmptyString
						, _( "Defines the renderer to " ) + desc + wxT( "." ) );
				}

				if ( ( parser.Parse( false ) != 0 )
					|| parser.Found( option::st::Help ) )
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

			template< typename ValueT >
			ValueT getLong( wxString const & option
				, ValueT const & defaultValue = ValueT{} )
			{
				long value;
				ValueT result;

				if ( parser.Found( option, &value ) )
				{
					result = ValueT( value );
				}
				else if ( configFile->HasEntry( option ) )
				{
					configFile->Read( option, &value );
					result = ValueT( value );
				}
				else
				{
					result = defaultValue;
				}

				return result;
			}

			void read( CastorApplication::Config & config )
			{
#if defined( _MSC_VER )
				if ( has( option::st::WaitDebugger ) )
				{
					while ( !::IsDebuggerPresent() )
					{
						std::this_thread::sleep_for( 1_ms );
					}
				}
#endif

				config.log = getLong( option::st::LogLevel, DefaultLogType );
				config.validate = has( option::st::Validate );
				config.shaderDebugLevel = getLong( option::st::ShaderDebugLevel, 0u );
				config.syncRender = has( option::st::SyncRender );
				config.disableUpdateOptimisations = has( option::st::DisUpdOptim );
				config.disableRandom = has( option::st::DisRandom );
				config.enableShaderValidation = has( option::st::EnShaderVal );
				config.enableApiTrace = has( option::st::EnApiTrace );
				config.keepTextShaders = has( option::st::KeepTextShd );
				config.enableDebugTargets = has( option::st::DebugTargets );

				if ( !config.syncRender )
				{
					config.unlimFPS = has( option::st::UnlimFPS );
				}

				if ( !config.unlimFPS )
				{
					config.fixedFPS = getLong( option::st::FixedFPS, config.fixedFPS );
				}

				for ( auto & plugin : list )
				{
					if ( has( make_wxString( plugin.name ) ) )
					{
						config.rendererName = plugin.name;
					}
				}

				config.gpuIndex = getLong( option::st::GpuIndex, DefaultGpuIndex );
				config.maxImageSize = getLong( option::st::MaxImgSize, 0u );

				wxString strFileName;

				if ( parser.GetParamCount() > 0 )
				{
					config.fileName = make_String( parser.GetParam( 0 ) );
				}
			}

			static wxString findConfigFile( wxCmdLineParser const & parser )
			{
				wxString cfg;
				parser.Found( wxT( 'c' ), &cfg );
				return cfg;
			}

		private:
			wxCmdLineParser parser;
			ashes::RendererList list;
			wxFileConfig * configFile{ nullptr };
		};
	}

	CastorApplication::CastorApplication( castor::String internalName
		, castor::String displayName
		, uint32_t steps
		, castor3d::Version version
		, uint32_t wantedFPS
		, bool isCastorThreaded
		, castor::String rendererType )
		: m_internalName{ std::move( internalName ) }
		, m_displayName{ std::move( displayName ) }
		, m_castor{ nullptr }
		, m_steps{ steps + 4 }
		, m_splashScreen{ nullptr }
		, m_version{ std::move( version ) }
		, m_config{ DefaultValidation
			, 0u
			, false
			, DefaultLogType
			, wantedFPS
			, !isCastorThreaded
			, rendererType }
	{
		wxSetAssertHandler( assertHandler );
#if defined( __WXGTK__ )
		XInitThreads();
#endif
	}

	void CastorApplication::assertHandler( wxString const & file
		, int line
		, wxString const & func
		, wxString const & cond
		, wxString const & msg )
	{
		std::clog << file << ":" << line << "(" << func << ") Assertion failed (" << cond << ") " << msg << std::endl;
	}

	bool CastorApplication::OnInit()
	{
		wxPropertyGrid::RegisterAdditionalEditors();
		bool result = doParseCommandLine();
		wxDisplay display;
		wxRect rect = display.GetClientArea();
		SplashScreen splashScreen
		{
			m_displayName,
			wxPoint{ 10, 230 },
			wxPoint{ 200, 300 },
			wxPoint{ 180, 260 },
			wxPoint{ ( rect.width - 512 ) / 2, ( rect.height - 384 ) / 2 },
			int( m_steps ),
			m_version,
			castor3d::Version{}
		};
		m_splashScreen = &splashScreen;
		wxApp::SetTopWindow( m_splashScreen );
		wxWindow * window = nullptr;

		if ( result )
		{
			result = doInitialiseLocale( splashScreen );
		}

		if ( result )
		{
			try
			{
				doLoadImages( splashScreen );
				result = doInitialiseCastor( splashScreen );

				if ( result )
				{
					window = doInitialiseMainFrame( splashScreen );
					result = window != nullptr;
				}
			}
			catch ( castor::Exception & exc )
			{
				wxMessageBox( _( "Problem occured while initialising Castor3D." ) + wxT( "\n" )
						+ make_wxString( exc.getFullDescription() )
					, _( "Exception" )
					, wxOK | wxCENTRE | wxICON_ERROR );
				doCleanupCastor();
				result = false;
			}
			catch ( std::exception & exc )
			{
				wxMessageBox( _( "Problem occured while initialising Castor3D." ) + wxT( "\n" )
						+ make_wxString( exc.what() )
					, _( "Exception" )
					, wxOK | wxCENTRE | wxICON_ERROR );
				doCleanupCastor();
				result = false;
			}
			catch ( ... )
			{
				wxMessageBox( _( "Problem occured while initialising Castor3D." ) + wxT( "\n" )
						+ _( "Look at CastorViewer.log for more details" )
					, _( "Exception" )
					, wxOK | wxCENTRE | wxICON_ERROR );
				doCleanupCastor();
				result = false;
			}
		}

		wxApp::SetTopWindow( window );
		m_splashScreen = nullptr;
		splashScreen.Close();

		if ( !result )
		{
			doCleanup();
		}

		return result;
	}

	int CastorApplication::OnExit()
	{
		castor::Logger::logInfo( m_internalName + cuT( " - Exit" ) );
		doCleanup();
		return wxApp::OnExit();
	}

	void CastorApplication::OnAssertFailure( const wxChar * file
		, int line
		, const wxChar * func
		, const wxChar * cond
		, const wxChar * msg )
	{
		assertHandler( file, line, func, cond, msg );
	}

	bool CastorApplication::doParseCommandLine()
	{
		bool result = true;

		try
		{
			Options options{ wxApp::argc, wxApp::argv };
			options.read( m_config );
			castor::Logger::initialise( m_config.log );
		}
		catch ( bool )
		{
			result = false;
		}

		return result;
	}

	bool CastorApplication::doInitialiseLocale( SplashScreen & splashScreen )
	{
		splashScreen.Step( _( "Loading language" ), 1 );
		int language = wxLANGUAGE_DEFAULT;
		castor::Path pathCurrent = castor::File::getExecutableDirectory().getPath();

		// load language if possible, fall back to english otherwise
		if ( wxLocale::IsAvailable( language ) )
		{
			m_locale = std::make_unique< wxLocale >( language, wxLOCALE_LOAD_DEFAULT );
			// add locale search paths
			m_locale->AddCatalogLookupPathPrefix( pathCurrent / cuT( "share" ) / m_internalName );
			m_locale->AddCatalog( m_internalName );

			if ( !m_locale->IsOk() )
			{
				std::cerr << "Selected language is wrong" << std::endl;
				m_locale = std::make_unique< wxLocale >( wxLANGUAGE_ENGLISH );
			}
		}
		else
		{
			std::cerr << "The selected language is not supported by your system."
					  << "Try installing support for this language." << std::endl;
			m_locale = std::make_unique< wxLocale >( wxLANGUAGE_ENGLISH );
		}

		return true;
	}

	bool CastorApplication::doInitialiseCastor( SplashScreen & splashScreen )
	{
		if ( !castor::File::directoryExists( castor3d::Engine::getEngineDirectory() ) )
		{
			castor::File::directoryCreate( castor3d::Engine::getEngineDirectory() );
		}

		castor::Logger::setFileName( castor3d::Engine::getEngineDirectory() / ( m_internalName + cuT( ".log" ) ) );
		castor::Logger::logInfo( m_internalName + cuT( " - Start" ) );

		castor3d::EngineConfig config{ m_internalName
			, m_version
			, m_config.validate
			, !m_config.disableRandom
			, !m_config.disableUpdateOptimisations
			, m_config.shaderDebugLevel
			, m_config.enableShaderValidation
			, m_config.enableApiTrace
			, m_config.keepTextShaders
			, m_config.enableDebugTargets };
		m_castor = castor::makeUnique< castor3d::Engine >( std::move( config ) );
		doloadPlugins( splashScreen );

		splashScreen.Step( _( "Initialising Castor3D" ), 1 );
		auto & renderers = m_castor->getRenderersList();

		if ( renderers.empty() )
		{
			CU_Exception( "No renderer plug-ins" );
		}
		else if ( std::next( renderers.begin() ) == renderers.end() )
		{
			m_config.rendererName = renderers.begin()->name;
		}

		if ( m_config.rendererName == castor3d::RenderTypeUndefined )
		{
			RendererSelector m_dialog( *m_castor, nullptr, m_displayName );

			if ( m_dialog.ShowModal() != wxID_OK )
			{
				return false;
			}

			m_castor->loadRenderer( m_dialog.getSelected() );
		}
		else if ( auto it = m_castor->getRenderersList().find( m_config.rendererName );
			it != m_castor->getRenderersList().end() )
		{
			m_castor->loadRenderer( castor3d::Renderer{ *m_castor
				, *it
				, {}
				, m_config.gpuIndex } );
		}
		else
		{
			CU_Exception( "Renderer plugin " + m_config.rendererName + " not found" );
		}

		if ( !isUnlimitedFps() )
		{
			m_castor->initialise( m_config.fixedFPS, !m_config.syncRender );
		}
		else
		{
			m_castor->initialise( 0xFFFFFFFFu, !m_config.syncRender );
		}

		if ( m_config.maxImageSize > 0u )
		{
			m_castor->setMaxImageSize( m_config.maxImageSize );
		}

		castor::Logger::logInfo( cuT( "Castor3D Initialised." ) );
		return true;
	}

	void CastorApplication::doloadPlugins( SplashScreen & splashScreen )
	{
		splashScreen.Step( _( "Loading plug-ins" ), 1 );
		GuiCommon::loadPlugins( *m_castor );
	}

	void CastorApplication::doLoadImages( SplashScreen & splashScreen )
	{
		splashScreen.Step( _( "Loading images" ), 1 );
		wxInitAllImageHandlers();
		m_imagesLoader.addBitmap( CV_IMG_CASTOR, app::castor_transparent_xpm );
		m_imagesLoader.addBitmap( eBMP_ANIMATED_OBJECTGROUP, app::animated_object_group_xpm );
		m_imagesLoader.addBitmap( eBMP_ANIMATED_OBJECTGROUP_SEL, app::animated_object_group_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_ANIMATED_OBJECT, app::animated_object_xpm );
		m_imagesLoader.addBitmap( eBMP_ANIMATED_OBJECT_SEL, app::animated_object_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_ANIMATION, app::animation_xpm );
		m_imagesLoader.addBitmap( eBMP_ANIMATION_SEL, app::animation_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_BONE, app::bone_xpm );
		m_imagesLoader.addBitmap( eBMP_BONE_SEL, app::bone_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_SCENE, app::scene_xpm );
		m_imagesLoader.addBitmap( eBMP_SCENE_SEL, app::scene_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_SKELETON, app::skeleton_xpm );
		m_imagesLoader.addBitmap( eBMP_SKELETON_SEL, app::skeleton_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_NODE, app::node_xpm );
		m_imagesLoader.addBitmap( eBMP_NODE_SEL, app::node_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_CAMERA, app::camera_xpm );
		m_imagesLoader.addBitmap( eBMP_CAMERA_SEL, app::camera_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_GEOMETRY, app::geometry_xpm );
		m_imagesLoader.addBitmap( eBMP_GEOMETRY_SEL, app::geometry_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_DIRECTIONAL_LIGHT, app::directional_xpm );
		m_imagesLoader.addBitmap( eBMP_DIRECTIONAL_LIGHT_SEL, app::directional_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_POINT_LIGHT, app::point_xpm );
		m_imagesLoader.addBitmap( eBMP_POINT_LIGHT_SEL, app::point_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_SPOT_LIGHT, app::spot_xpm );
		m_imagesLoader.addBitmap( eBMP_SPOT_LIGHT_SEL, app::spot_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_SUBMESH, app::submesh_xpm );
		m_imagesLoader.addBitmap( eBMP_SUBMESH_SEL, app::submesh_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_PANEL_OVERLAY, app::panel_xpm );
		m_imagesLoader.addBitmap( eBMP_PANEL_OVERLAY_SEL, app::panel_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_BORDER_PANEL_OVERLAY, app::border_panel_xpm );
		m_imagesLoader.addBitmap( eBMP_BORDER_PANEL_OVERLAY_SEL, app::border_panel_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_TEXT_OVERLAY, app::text_xpm );
		m_imagesLoader.addBitmap( eBMP_TEXT_OVERLAY_SEL, app::text_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_MATERIAL, app::material_xpm );
		m_imagesLoader.addBitmap( eBMP_MATERIAL_SEL, app::material_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_PASS, app::pass_xpm );
		m_imagesLoader.addBitmap( eBMP_PASS_SEL, app::pass_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_TEXTURE, app::texture_xpm );
		m_imagesLoader.addBitmap( eBMP_TEXTURE_SEL, app::texture_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_BILLBOARD, app::billboard_xpm );
		m_imagesLoader.addBitmap( eBMP_BILLBOARD_SEL, app::billboard_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_VIEWPORT, app::viewport_xpm );
		m_imagesLoader.addBitmap( eBMP_VIEWPORT_SEL, app::viewport_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_POST_EFFECT, app::post_effect_xpm );
		m_imagesLoader.addBitmap( eBMP_POST_EFFECT_SEL, app::post_effect_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_TONE_MAPPING, app::tone_mapping_xpm );
		m_imagesLoader.addBitmap( eBMP_TONE_MAPPING_SEL, app::tone_mapping_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_RENDER_TARGET, app::render_target_xpm );
		m_imagesLoader.addBitmap( eBMP_RENDER_TARGET_SEL, app::render_target_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_RENDER_WINDOW, app::render_window_xpm );
		m_imagesLoader.addBitmap( eBMP_RENDER_WINDOW_SEL, app::render_window_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_FRAME_VARIABLE, app::frame_variable_xpm );
		m_imagesLoader.addBitmap( eBMP_FRAME_VARIABLE_SEL, app::frame_variable_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_FRAME_VARIABLE_BUFFER, app::frame_variable_buffer_xpm );
		m_imagesLoader.addBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL, app::frame_variable_buffer_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_SSAO_CONFIG, app::ssao_config_xpm );
		m_imagesLoader.addBitmap( eBMP_SSAO_CONFIG_SEL, app::ssao_config_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_COLOURGRADING_CONFIG, app::colour_grading_xpm );
		m_imagesLoader.addBitmap( eBMP_COLOURGRADING_CONFIG_SEL, app::colour_grading_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_CLUSTERS_CONFIG, app::ssao_config_xpm );
		m_imagesLoader.addBitmap( eBMP_CLUSTERS_CONFIG_SEL, app::ssao_config_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_VCT_CONFIG, app::ssao_config_xpm );
		m_imagesLoader.addBitmap( eBMP_VCT_CONFIG_SEL, app::ssao_config_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_BACKGROUND, app::background_xpm );
		m_imagesLoader.addBitmap( eBMP_BACKGROUND_SEL, app::background_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_COLLAPSE_ALL, app::collapse_all_xpm );
		m_imagesLoader.addBitmap( eBMP_EXPAND_ALL, app::expand_all_xpm );
		m_imagesLoader.addBitmap( eBMP_CONTROLS, app::controls_xpm );
		m_imagesLoader.addBitmap( eBMP_CONTROLS_SEL, app::controls_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_CONTROL, app::control_xpm );
		m_imagesLoader.addBitmap( eBMP_CONTROL_SEL, app::control_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_STYLES, app::styles_xpm );
		m_imagesLoader.addBitmap( eBMP_STYLES_SEL, app::styles_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_STYLE, app::style_xpm );
		m_imagesLoader.addBitmap( eBMP_STYLE_SEL, app::style_sel_xpm );
		m_imagesLoader.addBitmap( eBMP_THEME, app::theme_xpm );
		m_imagesLoader.addBitmap( eBMP_THEME_SEL, app::theme_sel_xpm );
		doLoadAppImages();
		ImagesLoader::waitAsyncLoads();
	}

	void CastorApplication::doCleanup()
	{
		doCleanupCastor();
		m_locale.reset();
		ImagesLoader::cleanup();
		castor::Logger::cleanup();
		wxImage::CleanUpHandlers();
	}

	void CastorApplication::doCleanupCastor()
	{
		if ( m_castor )
		{
			m_castor->cleanup();
			m_castor.reset();
		}
	}
}
