#include "GuiCommon/CastorApplication.hpp"

#include "GuiCommon/ImagesLoader.hpp"
#include "GuiCommon/RendererSelector.hpp"
#include "GuiCommon/SplashScreen.hpp"

#include <wx/cmdline.h>
#include <wx/display.h>

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Plugin/RendererPlugin.hpp>

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Exception/Exception.hpp>

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
#include "GuiCommon/xpms/submesh.xpm"
#include "GuiCommon/xpms/submesh_sel.xpm"
#include "GuiCommon/xpms/text.xpm"
#include "GuiCommon/xpms/text_sel.xpm"
#include "GuiCommon/xpms/texture.xpm"
#include "GuiCommon/xpms/texture_sel.xpm"
#include "GuiCommon/xpms/tone_mapping.xpm"
#include "GuiCommon/xpms/tone_mapping_sel.xpm"
#include "GuiCommon/xpms/viewport.xpm"
#include "GuiCommon/xpms/viewport_sel.xpm"

#if defined( __WXGTK__ )
#	include <X11/Xlib.h>
#endif

using namespace castor;
using namespace castor3d;

namespace GuiCommon
{
#if defined( NDEBUG )

	static const LogType ELogType_DEFAULT = LogType::eInfo;

#else

	static const LogType ELogType_DEFAULT = LogType::eDebug;

#endif

	CastorApplication::CastorApplication( String const & p_internalName
		, String const & p_displayName
		, uint32_t p_steps
		, Version const & p_version
		, String const & p_rendererType )
		: m_internalName{ p_internalName }
		, m_displayName{ p_displayName }
		, m_castor{ nullptr }
		, m_rendererType{ p_rendererType }
		, m_steps{ p_steps + 4 }
		, m_splashScreen{ nullptr }
		, m_version{ p_version }
	{
#if defined( __WXGTK__ )
		XInitThreads();
#endif
	}

	bool CastorApplication::OnInit()
	{
#if defined( CU_PlatformWindows ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )

		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

#endif

		bool result = doParseCommandLine();
		wxDisplay display;
		wxRect rect = display.GetClientArea();
		SplashScreen splashScreen( m_displayName, wxPoint( 10, 230 ), wxPoint( 200, 300 ), wxPoint( 180, 260 ), wxPoint( ( rect.width - 512 ) / 2, ( rect.height - 384 ) / 2 ), m_steps, m_version );
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
					window = doInitialiseMainFrame( &splashScreen );
					result = window != nullptr;
				}
			}
			catch ( Exception & exc )
			{
				Logger::logError( std::stringstream() << string::stringCast< char >( m_internalName ) << " - Initialisation failed : " << exc.getFullDescription() );
				doCleanupCastor();
				result = false;
			}
			catch ( std::exception & exc )
			{
				Logger::logError( std::stringstream() << string::stringCast< char >( m_internalName ) << " - Initialisation failed : " << exc.what() );
				doCleanupCastor();
				result = false;
			}
		}

		wxApp::SetTopWindow( window );
		splashScreen.Close();
		m_splashScreen = nullptr;

		if ( !result )
		{
			doCleanup();
		}

		return result;
	}

	int CastorApplication::OnExit()
	{
		Logger::logInfo( m_internalName + cuT( " - Exit" ) );
		doCleanup();
		return wxApp::OnExit();
	}

	bool CastorApplication::doParseCommandLine()
	{
		wxCmdLineParser parser( wxApp::argc, wxApp::argv );
		parser.AddSwitch( wxT( "h" ), wxT( "help" ), _( "Displays this help." ) );
		parser.AddSwitch( wxT( "v" ), wxT( "validate" ), _( "Enables rendering API validation." ) );
		parser.AddSwitch( wxT( "u" ), wxT( "unlimited" ), _( "Disables FPS limit." ) );
		parser.AddOption( wxT( "l" ), wxT( "log" ), _( "Defines log level." ), wxCMD_LINE_VAL_NUMBER );
		parser.AddParam( _( "The initial scene file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
		parser.AddSwitch( wxT( "opengl3" ), wxEmptyString, _( "Defines the renderer to OpenGl 3.x." ) );
		parser.AddSwitch( wxT( "opengl4" ), wxEmptyString, _( "Defines the renderer to OpenGl 4.x." ) );
		parser.AddSwitch( wxT( "vulkan" ), wxEmptyString, _( "Defines the renderer to Vulkan." ) );
		parser.AddSwitch( wxT( "direct3d11" ), wxEmptyString, _( "Defines the renderer to Direct3D 11." ) );
		parser.AddSwitch( wxT( "test" ), wxEmptyString, _( "Defines the renderer to Test." ) );
		bool result = parser.Parse( false ) == 0;

		// S'il y avait des erreurs ou "-h" ou "--help", on affiche l'aide et on sort
		if ( !result || parser.Found( wxT( 'h' ) ) )
		{
			parser.Usage();
			result = false;
		}

		if ( result )
		{
			LogType eLogLevel = LogType::eCount;
			long log;

			if ( !parser.Found( wxT( "l" ), &log ) )
			{
				eLogLevel = ELogType_DEFAULT;
			}
			else
			{
				eLogLevel = LogType( log );
			}

			Logger::initialise( eLogLevel );
			m_validation = parser.Found( wxT( 'v' ) );
			m_unlimitedFps = parser.Found( wxT( 'u' ) );

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
			else if ( parser.Found( wxT( "direct3d11" ) ) )
			{
				m_rendererType = cuT( "direct3d11" );
			}
			else if ( parser.Found( wxT( "test" ) ) )
			{
				m_rendererType = cuT( "test" );
			}

			wxString strFileName;

			if ( parser.GetParamCount() > 0 )
			{
				m_fileName = make_String( parser.GetParam( 0 ) );
			}
		}

		return result;
	}

	bool CastorApplication::doInitialiseLocale( SplashScreen & p_splashScreen )
	{
		//p_splashScreen.Step( _( "Loading language" ), 1 );
		//long lLanguage = wxLANGUAGE_DEFAULT;
		//Path pathCurrent = File::getExecutableDirectory().getPath();

		//// load language if possible, fall back to english otherwise
		//if ( wxLocale::IsAvailable( lLanguage ) )
		//{
		//	m_locale = std::make_unique< wxLocale >( lLanguage, wxLOCALE_LOAD_DEFAULT );
		//	// add locale search paths
		//	m_locale->AddCatalogLookupPathPrefix( pathCurrent / cuT( "share" ) / m_internalName );
		//	m_locale->AddCatalog( m_internalName );

		//	if ( !m_locale->IsOk() )
		//	{
		//		std::cerr << "Selected language is wrong" << std::endl;
		//		lLanguage = wxLANGUAGE_ENGLISH;
		//		m_locale = std::make_unique< wxLocale >( lLanguage );
		//	}
		//}
		//else
		//{
		//	std::cerr << "The selected language is not supported by your system."
		//			  << "Try installing support for this language." << std::endl;
		//	lLanguage = wxLANGUAGE_ENGLISH;
		//	m_locale = std::make_unique< wxLocale >( lLanguage );
		//}

		return true;
	}

	bool CastorApplication::doInitialiseCastor( SplashScreen & p_splashScreen )
	{
		bool result = true;

		if ( !File::directoryExists( Engine::getEngineDirectory() ) )
		{
			File::directoryCreate( Engine::getEngineDirectory() );
		}

		Logger::setFileName( Engine::getEngineDirectory() / ( m_internalName + cuT( ".log" ) ) );
		Logger::logInfo( m_internalName + cuT( " - Start" ) );

		m_castor = new Engine{ m_internalName, m_version, m_validation };
		doloadPlugins( p_splashScreen );

		p_splashScreen.Step( _( "Initialising Castor3D" ), 1 );
		auto renderers = m_castor->getPluginCache().getPlugins( PluginType::eRenderer );

		if ( renderers.empty() )
		{
			CU_Exception( "No renderer plug-ins" );
		}
		else if ( renderers.size() == 1 )
		{
			m_rendererType = std::static_pointer_cast< RendererPlugin >( renderers.begin()->second )->getRendererType();
		}

		if ( m_rendererType == RENDERER_TYPE_UNDEFINED )
		{
			RendererSelector m_dialog( m_castor, nullptr, m_displayName );
			int iReturn = m_dialog.ShowModal();

			if ( iReturn == wxID_OK )
			{
				m_rendererType = m_dialog.getSelectedRenderer();
			}
			else
			{
				result = false;
			}
		}
		else
		{
			result = true;
		}

		if ( result )
		{
			result = m_castor->loadRenderer( m_rendererType );
		}

		return result;
	}

	void CastorApplication::doloadPlugins( SplashScreen & p_splashScreen )
	{
		p_splashScreen.Step( _( "Loading plug-ins" ), 1 );
		GuiCommon::loadPlugins( *m_castor );
	}

	void CastorApplication::doLoadImages( SplashScreen & p_splashScreen )
	{
		p_splashScreen.Step( _( "Loading images" ), 1 );
		wxInitAllImageHandlers();
		ImagesLoader::addBitmap( CV_IMG_CASTOR, castor_transparent_xpm );
		ImagesLoader::addBitmap( eBMP_ANIMATED_OBJECTGROUP, animated_object_group_xpm );
		ImagesLoader::addBitmap( eBMP_ANIMATED_OBJECTGROUP_SEL, animated_object_group_sel_xpm );
		ImagesLoader::addBitmap( eBMP_ANIMATED_OBJECT, animated_object_xpm );
		ImagesLoader::addBitmap( eBMP_ANIMATED_OBJECT_SEL, animated_object_sel_xpm );
		ImagesLoader::addBitmap( eBMP_ANIMATION, animation_xpm );
		ImagesLoader::addBitmap( eBMP_ANIMATION_SEL, animation_sel_xpm );
		ImagesLoader::addBitmap( eBMP_BONE, bone_xpm );
		ImagesLoader::addBitmap( eBMP_BONE_SEL, bone_sel_xpm );
		ImagesLoader::addBitmap( eBMP_SCENE, scene_xpm );
		ImagesLoader::addBitmap( eBMP_SCENE_SEL, scene_sel_xpm );
		ImagesLoader::addBitmap( eBMP_SKELETON, skeleton_xpm );
		ImagesLoader::addBitmap( eBMP_SKELETON_SEL, skeleton_sel_xpm );
		ImagesLoader::addBitmap( eBMP_NODE, node_xpm );
		ImagesLoader::addBitmap( eBMP_NODE_SEL, node_sel_xpm );
		ImagesLoader::addBitmap( eBMP_CAMERA, camera_xpm );
		ImagesLoader::addBitmap( eBMP_CAMERA_SEL, camera_sel_xpm );
		ImagesLoader::addBitmap( eBMP_GEOMETRY, geometry_xpm );
		ImagesLoader::addBitmap( eBMP_GEOMETRY_SEL, geometry_sel_xpm );
		ImagesLoader::addBitmap( eBMP_DIRECTIONAL_LIGHT, directional_xpm );
		ImagesLoader::addBitmap( eBMP_DIRECTIONAL_LIGHT_SEL, directional_sel_xpm );
		ImagesLoader::addBitmap( eBMP_POINT_LIGHT, point_xpm );
		ImagesLoader::addBitmap( eBMP_POINT_LIGHT_SEL, point_sel_xpm );
		ImagesLoader::addBitmap( eBMP_SPOT_LIGHT, spot_xpm );
		ImagesLoader::addBitmap( eBMP_SPOT_LIGHT_SEL, spot_sel_xpm );
		ImagesLoader::addBitmap( eBMP_SUBMESH, submesh_xpm );
		ImagesLoader::addBitmap( eBMP_SUBMESH_SEL, submesh_sel_xpm );
		ImagesLoader::addBitmap( eBMP_PANEL_OVERLAY, panel_xpm );
		ImagesLoader::addBitmap( eBMP_PANEL_OVERLAY_SEL, panel_sel_xpm );
		ImagesLoader::addBitmap( eBMP_BORDER_PANEL_OVERLAY, border_panel_xpm );
		ImagesLoader::addBitmap( eBMP_BORDER_PANEL_OVERLAY_SEL, border_panel_sel_xpm );
		ImagesLoader::addBitmap( eBMP_TEXT_OVERLAY, text_xpm );
		ImagesLoader::addBitmap( eBMP_TEXT_OVERLAY_SEL, text_sel_xpm );
		ImagesLoader::addBitmap( eBMP_MATERIAL, material_xpm );
		ImagesLoader::addBitmap( eBMP_MATERIAL_SEL, material_sel_xpm );
		ImagesLoader::addBitmap( eBMP_PASS, pass_xpm );
		ImagesLoader::addBitmap( eBMP_PASS_SEL, pass_sel_xpm );
		ImagesLoader::addBitmap( eBMP_TEXTURE, texture_xpm );
		ImagesLoader::addBitmap( eBMP_TEXTURE_SEL, texture_sel_xpm );
		ImagesLoader::addBitmap( eBMP_BILLBOARD, billboard_xpm );
		ImagesLoader::addBitmap( eBMP_BILLBOARD_SEL, billboard_sel_xpm );
		ImagesLoader::addBitmap( eBMP_VIEWPORT, viewport_xpm );
		ImagesLoader::addBitmap( eBMP_VIEWPORT_SEL, viewport_sel_xpm );
		ImagesLoader::addBitmap( eBMP_POST_EFFECT, post_effect_xpm );
		ImagesLoader::addBitmap( eBMP_POST_EFFECT_SEL, post_effect_sel_xpm );
		ImagesLoader::addBitmap( eBMP_TONE_MAPPING, tone_mapping_xpm );
		ImagesLoader::addBitmap( eBMP_TONE_MAPPING_SEL, tone_mapping_sel_xpm );
		ImagesLoader::addBitmap( eBMP_RENDER_TARGET, render_target_xpm );
		ImagesLoader::addBitmap( eBMP_RENDER_TARGET_SEL, render_target_sel_xpm );
		ImagesLoader::addBitmap( eBMP_RENDER_WINDOW, render_window_xpm );
		ImagesLoader::addBitmap( eBMP_RENDER_WINDOW_SEL, render_window_sel_xpm );
		ImagesLoader::addBitmap( eBMP_FRAME_VARIABLE, frame_variable_xpm );
		ImagesLoader::addBitmap( eBMP_FRAME_VARIABLE_SEL, frame_variable_sel_xpm );
		ImagesLoader::addBitmap( eBMP_FRAME_VARIABLE_BUFFER, frame_variable_buffer_xpm );
		ImagesLoader::addBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL, frame_variable_buffer_sel_xpm );
		ImagesLoader::addBitmap( eBMP_BACKGROUND, background_xpm );
		ImagesLoader::addBitmap( eBMP_BACKGROUND_SEL, background_sel_xpm );
		ImagesLoader::addBitmap( eBMP_COLLAPSE_ALL, collapse_all_xpm );
		ImagesLoader::addBitmap( eBMP_EXPAND_ALL, expand_all_xpm );
		doLoadAppImages();
		ImagesLoader::waitAsyncLoads();
	}

	void CastorApplication::doCleanup()
	{
		doCleanupCastor();
		m_locale.reset();
		ImagesLoader::cleanup();
		Logger::cleanup();
		wxImage::CleanUpHandlers();
	}

	void CastorApplication::doCleanupCastor()
	{
		delete m_castor;
		m_castor = nullptr;
	}
}
