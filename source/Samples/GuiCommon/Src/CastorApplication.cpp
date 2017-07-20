#include "CastorApplication.hpp"

#include "ImagesLoader.hpp"
#include "RendererSelector.hpp"
#include "SplashScreen.hpp"

#include <wx/cmdline.h>
#include <wx/display.h>

#include <Engine.hpp>
#include <Cache/PluginCache.hpp>
#include <Plugin/RendererPlugin.hpp>

#include <Data/File.hpp>
#include <Exception/Exception.hpp>

#include "xpms/animated_object_group.xpm"
#include "xpms/animated_object_group_sel.xpm"
#include "xpms/animated_object.xpm"
#include "xpms/animated_object_sel.xpm"
#include "xpms/animation.xpm"
#include "xpms/animation_sel.xpm"
#include "xpms/billboard.xpm"
#include "xpms/billboard_sel.xpm"
#include "xpms/border_panel.xpm"
#include "xpms/border_panel_sel.xpm"
#include "xpms/camera.xpm"
#include "xpms/camera_sel.xpm"
#include "xpms/castor_transparent.xpm"
#include "xpms/directional.xpm"
#include "xpms/directional_sel.xpm"
#include "xpms/frame_variable.xpm"
#include "xpms/frame_variable_sel.xpm"
#include "xpms/frame_variable_buffer.xpm"
#include "xpms/frame_variable_buffer_sel.xpm"
#include "xpms/geometry.xpm"
#include "xpms/geometry_sel.xpm"
#include "xpms/node.xpm"
#include "xpms/node_sel.xpm"
#include "xpms/material.xpm"
#include "xpms/material_sel.xpm"
#include "xpms/pass.xpm"
#include "xpms/pass_sel.xpm"
#include "xpms/panel.xpm"
#include "xpms/panel_sel.xpm"
#include "xpms/point.xpm"
#include "xpms/point_sel.xpm"
#include "xpms/render_target.xpm"
#include "xpms/render_target_sel.xpm"
#include "xpms/render_window.xpm"
#include "xpms/render_window_sel.xpm"
#include "xpms/scene.xpm"
#include "xpms/scene_sel.xpm"
#include "xpms/spot.xpm"
#include "xpms/spot_sel.xpm"
#include "xpms/submesh.xpm"
#include "xpms/submesh_sel.xpm"
#include "xpms/text.xpm"
#include "xpms/text_sel.xpm"
#include "xpms/texture.xpm"
#include "xpms/texture_sel.xpm"
#include "xpms/viewport.xpm"
#include "xpms/viewport_sel.xpm"

#if defined( __WXGTK__ )
#	include <X11/Xlib.h>
#endif

using namespace Castor;
using namespace Castor3D;

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
#if defined( CASTOR_PLATFORM_WINDOWS ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )

		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

#endif

		bool result = DoParseCommandLine();
		wxDisplay display;
		wxRect rect = display.GetClientArea();
		SplashScreen splashScreen( m_displayName, wxPoint( 10, 230 ), wxPoint( 200, 300 ), wxPoint( 180, 260 ), wxPoint( ( rect.width - 512 ) / 2, ( rect.height - 384 ) / 2 ), m_steps, m_version );
		m_splashScreen = &splashScreen;
		wxApp::SetTopWindow( m_splashScreen );
		wxWindow * window = nullptr;

		if ( result )
		{
			result = DoInitialiseLocale( splashScreen );
		}

		if ( result )
		{
			try
			{
				DoLoadImages( splashScreen );
				result = DoInitialiseCastor( splashScreen );

				if ( result )
				{
					window = DoInitialiseMainFrame( &splashScreen );
					result = window != nullptr;
				}
			}
			catch ( Exception & exc )
			{
				Logger::LogError( std::stringstream() << string::string_cast< char >( m_internalName ) << " - Initialisation failed : " << exc.GetFullDescription() );
				result = false;
			}
			catch ( std::exception & exc )
			{
				Logger::LogError( std::stringstream() << string::string_cast< char >( m_internalName ) << " - Initialisation failed : " << exc.what() );
				result = false;
			}
		}

		wxApp::SetTopWindow( window );
		splashScreen.Close();
		m_splashScreen = nullptr;

		if ( !result )
		{
			DoCleanup();
		}

		return result;
	}

	int CastorApplication::OnExit()
	{
		Logger::LogInfo( m_internalName + cuT( " - Exit" ) );
		DoCleanup();
		return wxApp::OnExit();
	}

	bool CastorApplication::DoParseCommandLine()
	{
		wxCmdLineParser parser( wxApp::argc, wxApp::argv );
		parser.AddSwitch( wxT( "h" ), wxT( "help" ), _( "Displays this help" ) );
		parser.AddOption( wxT( "l" ), wxT( "log" ), _( "Defines log level" ), wxCMD_LINE_VAL_NUMBER );
		parser.AddParam( _( "The initial scene file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
		parser.AddSwitch( wxT( "opengl" ), wxEmptyString, _( "Defines the renderer to OpenGl" ) );
		parser.AddSwitch( wxT( "test" ), wxEmptyString, _( "Defines the renderer to Test" ) );
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

			Logger::Initialise( eLogLevel );

			if ( parser.Found( wxT( "opengl" ) ) )
			{
				m_rendererType = cuT( "opengl" );
			}

			if ( parser.Found( wxT( "test" ) ) )
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

	bool CastorApplication::DoInitialiseLocale( SplashScreen & p_splashScreen )
	{
		p_splashScreen.Step( _( "Loading language" ), 1 );
		long lLanguage = wxLANGUAGE_DEFAULT;
		Path pathCurrent = File::GetExecutableDirectory().GetPath();

		// load language if possible, fall back to english otherwise
		if ( wxLocale::IsAvailable( lLanguage ) )
		{
			m_locale = std::make_unique< wxLocale >( lLanguage, wxLOCALE_LOAD_DEFAULT );
			// add locale search paths
			m_locale->AddCatalogLookupPathPrefix( pathCurrent / cuT( "share" ) / m_internalName );
			m_locale->AddCatalog( m_internalName );

			if ( !m_locale->IsOk() )
			{
				std::cerr << "Selected language is wrong" << std::endl;
				lLanguage = wxLANGUAGE_ENGLISH;
				m_locale = std::make_unique< wxLocale >( lLanguage );
			}
		}
		else
		{
			std::cerr << "The selected language is not supported by your system."
					  << "Try installing support for this language." << std::endl;
			lLanguage = wxLANGUAGE_ENGLISH;
			m_locale = std::make_unique< wxLocale >( lLanguage );
		}

		return true;
	}

	bool CastorApplication::DoInitialiseCastor( SplashScreen & p_splashScreen )
	{
		bool result = true;

		if ( !File::DirectoryExists( Engine::GetEngineDirectory() ) )
		{
			File::DirectoryCreate( Engine::GetEngineDirectory() );
		}

		Logger::SetFileName( Engine::GetEngineDirectory() / ( m_internalName + cuT( ".log" ) ) );
		Logger::LogInfo( m_internalName + cuT( " - Start" ) );

		m_castor = new Engine();
		DoLoadPlugins( p_splashScreen );

		p_splashScreen.Step( _( "Initialising Castor3D" ), 1 );
		auto renderers = m_castor->GetPluginCache().GetPlugins( PluginType::eRenderer );

		if ( renderers.empty() )
		{
			CASTOR_EXCEPTION( "No renderer plug-ins" );
		}
		else if ( renderers.size() == 1 )
		{
			m_rendererType = std::static_pointer_cast< RendererPlugin >( renderers.begin()->second )->GetRendererType();
		}

		if ( m_rendererType == RENDERER_TYPE_UNDEFINED )
		{
			RendererSelector m_dialog( m_castor, nullptr, m_displayName );
			int iReturn = m_dialog.ShowModal();

			if ( iReturn == wxID_OK )
			{
				m_rendererType = m_dialog.GetSelectedRenderer();
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
			result = m_castor->LoadRenderer( m_rendererType );
		}

		return result;
	}

	void CastorApplication::DoLoadPlugins( SplashScreen & p_splashScreen )
	{
		p_splashScreen.Step( _( "Loading plug-ins" ), 1 );
		GuiCommon::LoadPlugins( *m_castor );
	}

	void CastorApplication::DoLoadImages( SplashScreen & p_splashScreen )
	{
		p_splashScreen.Step( _( "Loading images" ), 1 );
		wxInitAllImageHandlers();
		ImagesLoader::AddBitmap( CV_IMG_CASTOR, castor_transparent_xpm );
		ImagesLoader::AddBitmap( eBMP_ANIMATED_OBJECTGROUP, animated_object_group_xpm );
		ImagesLoader::AddBitmap( eBMP_ANIMATED_OBJECTGROUP_SEL, animated_object_group_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_ANIMATED_OBJECT, animated_object_xpm );
		ImagesLoader::AddBitmap( eBMP_ANIMATED_OBJECT_SEL, animated_object_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_ANIMATION, animation_xpm );
		ImagesLoader::AddBitmap( eBMP_ANIMATION_SEL, animation_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_SCENE, scene_xpm );
		ImagesLoader::AddBitmap( eBMP_SCENE_SEL, scene_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_NODE, node_xpm );
		ImagesLoader::AddBitmap( eBMP_NODE_SEL, node_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_CAMERA, camera_xpm );
		ImagesLoader::AddBitmap( eBMP_CAMERA_SEL, camera_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_GEOMETRY, geometry_xpm );
		ImagesLoader::AddBitmap( eBMP_GEOMETRY_SEL, geometry_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_DIRECTIONAL_LIGHT, directional_xpm );
		ImagesLoader::AddBitmap( eBMP_DIRECTIONAL_LIGHT_SEL, directional_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_POINT_LIGHT, point_xpm );
		ImagesLoader::AddBitmap( eBMP_POINT_LIGHT_SEL, point_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_SPOT_LIGHT, spot_xpm );
		ImagesLoader::AddBitmap( eBMP_SPOT_LIGHT_SEL, spot_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_SUBMESH, submesh_xpm );
		ImagesLoader::AddBitmap( eBMP_SUBMESH_SEL, submesh_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_PANEL_OVERLAY, panel_xpm );
		ImagesLoader::AddBitmap( eBMP_PANEL_OVERLAY_SEL, panel_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_BORDER_PANEL_OVERLAY, border_panel_xpm );
		ImagesLoader::AddBitmap( eBMP_BORDER_PANEL_OVERLAY_SEL, border_panel_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_TEXT_OVERLAY, text_xpm );
		ImagesLoader::AddBitmap( eBMP_TEXT_OVERLAY_SEL, text_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_MATERIAL, material_xpm );
		ImagesLoader::AddBitmap( eBMP_MATERIAL_SEL, material_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_PASS, pass_xpm );
		ImagesLoader::AddBitmap( eBMP_PASS_SEL, pass_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_TEXTURE, texture_xpm );
		ImagesLoader::AddBitmap( eBMP_TEXTURE_SEL, texture_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_BILLBOARD, billboard_xpm );
		ImagesLoader::AddBitmap( eBMP_BILLBOARD_SEL, billboard_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_VIEWPORT, viewport_xpm );
		ImagesLoader::AddBitmap( eBMP_VIEWPORT_SEL, viewport_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_RENDER_TARGET, render_target_xpm );
		ImagesLoader::AddBitmap( eBMP_RENDER_TARGET_SEL, render_target_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_RENDER_WINDOW, render_window_xpm );
		ImagesLoader::AddBitmap( eBMP_RENDER_WINDOW_SEL, render_window_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_FRAME_VARIABLE, frame_variable_xpm );
		ImagesLoader::AddBitmap( eBMP_FRAME_VARIABLE_SEL, frame_variable_sel_xpm );
		ImagesLoader::AddBitmap( eBMP_FRAME_VARIABLE_BUFFER, frame_variable_buffer_xpm );
		ImagesLoader::AddBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL, frame_variable_buffer_sel_xpm );
		DoLoadAppImages();
		ImagesLoader::WaitAsyncLoads();
	}

	void CastorApplication::DoCleanup()
	{
		DoCleanupCastor();
		m_locale.reset();
		ImagesLoader::Cleanup();
		Logger::Cleanup();
		wxImage::CleanUpHandlers();
	}

	void CastorApplication::DoCleanupCastor()
	{
		delete m_castor;
		m_castor = nullptr;
	}
}
