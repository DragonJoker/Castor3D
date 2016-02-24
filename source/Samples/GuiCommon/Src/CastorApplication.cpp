#include "CastorApplication.hpp"

#include "ImagesLoader.hpp"
#include "RendererSelector.hpp"
#include "SplashScreen.hpp"

#include <wx/cmdline.h>
#include <wx/display.h>

#include <Engine.hpp>
#include <PluginManager.hpp>
#include <RendererPlugin.hpp>

#include <File.hpp>
#include <Exception.hpp>

#include "xpms/animated_object_group.xpm"
#include "xpms/animated_object_group_sel.xpm"
#include "xpms/animated_object.xpm"
#include "xpms/animated_object_sel.xpm"
#include "xpms/animation.xpm"
#include "xpms/animation_sel.xpm"
#include "xpms/border_panel.xpm"
#include "xpms/border_panel_sel.xpm"
#include "xpms/camera.xpm"
#include "xpms/camera_sel.xpm"
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

	static const ELogType ELogType_DEFAULT = ELogType_INFO;

#else

	static const ELogType ELogType_DEFAULT = ELogType_DEBUG;

#endif

	CastorApplication::CastorApplication( String const & p_internalName, String const & p_displayName, uint32_t p_steps )
		: m_internalName( p_internalName )
		, m_displayName( p_displayName )
		, m_castor( nullptr )
		, m_rendererType( eRENDERER_TYPE_UNDEFINED )
		, m_steps( p_steps + 4 )
		, m_splashScreen( nullptr )
	{
#if defined( __WXGTK__ )
		XInitThreads();
#endif
	}

	bool CastorApplication::OnInit()
	{
#if defined( _MSC_VER ) && !defined( NDEBUG ) && !defined( VLD_AVAILABLE )

		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

#endif

		bool l_return = DoParseCommandLine();
		wxDisplay l_display;
		wxRect l_rect = l_display.GetClientArea();
		SplashScreen l_splashScreen( m_displayName, wxPoint( 10, 230 ), wxPoint( 200, 300 ), wxPoint( 180, 260 ), wxPoint( ( l_rect.width - 512 ) / 2, ( l_rect.height - 384 ) / 2 ), m_steps );
		m_splashScreen = &l_splashScreen;
		wxApp::SetTopWindow( m_splashScreen );
		wxWindow * l_window = nullptr;

		if ( l_return )
		{
			l_return = DoInitialiseLocale( l_splashScreen );
		}

		if ( l_return )
		{
			try
			{
				DoLoadImages( l_splashScreen );
				l_return = DoInitialiseCastor( l_splashScreen );

				if ( l_return )
				{
					l_window = DoInitialiseMainFrame( &l_splashScreen );
					l_return = l_window != nullptr;
				}
			}
			catch ( Exception & exc )
			{
				Logger::LogError( std::stringstream() << string::string_cast< char >( m_internalName ) << " - Initialisation failed : " << exc.GetFullDescription() );
				l_return = false;
			}
			catch ( std::exception & exc )
			{
				Logger::LogError( std::stringstream() << string::string_cast< char >( m_internalName ) << " - Initialisation failed : " << exc.what() );
				l_return = false;
			}
		}

		wxApp::SetTopWindow( l_window );
		l_splashScreen.Close();
		m_splashScreen = nullptr;

		return l_return;
	}

	int CastorApplication::OnExit()
	{
		DoCleanupCastor();
		m_locale.reset();
		ImagesLoader::Cleanup();
		Logger::LogInfo( m_internalName + cuT( " - Exit" ) );
		Logger::Cleanup();
		wxImage::CleanUpHandlers();
		return wxApp::OnExit();
	}

	bool CastorApplication::DoParseCommandLine()
	{
		wxCmdLineParser l_parser( wxApp::argc, wxApp::argv );
		l_parser.AddSwitch( wxT( "h" ), wxT( "help" ), _( "Displays this help" ) );
		l_parser.AddOption( wxT( "l" ), wxT( "log" ), _( "Defines log level" ), wxCMD_LINE_VAL_NUMBER );
		l_parser.AddParam( _( "Defines initial scene file" ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
		l_parser.AddSwitch( wxT( "opengl" ), wxEmptyString, _( "Defines the renderer to OpenGl" ) );
		l_parser.AddSwitch( wxT( "directx" ), wxEmptyString, _( "Defines the renderer to Direct3D (11)" ) );
		bool l_return = l_parser.Parse( false ) == 0;

		// S'il y avait des erreurs ou "-h" ou "--help", on affiche l'aide et on sort
		if ( !l_return || l_parser.Found( wxT( 'h' ) ) )
		{
			l_parser.Usage();
			l_return = false;
		}

		if ( l_return )
		{
			ELogType l_eLogLevel = ELogType_COUNT;

			if ( !l_parser.Found( wxT( 'l' ), reinterpret_cast< long * >( &l_eLogLevel ) ) )
			{
				l_eLogLevel = ELogType_DEFAULT;
			}

			Logger::Initialise( l_eLogLevel );

			if ( l_parser.Found( wxT( "opengl" ) ) )
			{
				m_rendererType = eRENDERER_TYPE_OPENGL;
			}

			wxString l_strFileName;

			if ( l_parser.GetParamCount() > 0 )
			{
				m_fileName = make_String( l_parser.GetParam( 0 ) );
			}
		}

		return l_return;
	}

	bool CastorApplication::DoInitialiseLocale( SplashScreen & p_splashScreen )
	{
		p_splashScreen.Step( _( "Loading language" ), 1 );
		long l_lLanguage = wxLANGUAGE_DEFAULT;
		Path l_pathCurrent = File::GetExecutableDirectory().GetPath();

		// load language if possible, fall back to english otherwise
		if ( wxLocale::IsAvailable( l_lLanguage ) )
		{
			m_locale = std::make_unique< wxLocale >( l_lLanguage, wxLOCALE_LOAD_DEFAULT );
			// add locale search paths
			m_locale->AddCatalogLookupPathPrefix( l_pathCurrent / cuT( "share" ) / m_internalName );

#if defined( _MSC_VER )
#	if defined( NDEBUG )

			m_locale->AddCatalogLookupPathPrefix( l_pathCurrent.GetPath() / cuT( "share" ) / cuT( "Release" ) / m_internalName );

#	else

			m_locale->AddCatalogLookupPathPrefix( l_pathCurrent.GetPath() / cuT( "share" ) / cuT( "Debug" ) / m_internalName );

#	endif
#endif
			m_locale->AddCatalog( m_internalName );

			if ( !m_locale->IsOk() )
			{
				std::cerr << "Selected language is wrong" << std::endl;
				l_lLanguage = wxLANGUAGE_ENGLISH;
				m_locale = std::make_unique< wxLocale >( l_lLanguage );
			}
		}
		else
		{
			std::cerr << "The selected language is not supported by your system."
					  << "Try installing support for this language." << std::endl;
			l_lLanguage = wxLANGUAGE_ENGLISH;
			m_locale = std::make_unique< wxLocale >( l_lLanguage );
		}

		return true;
	}

	bool CastorApplication::DoInitialiseCastor( SplashScreen & p_splashScreen )
	{
		bool l_return = true;

		if ( !File::DirectoryExists( Engine::GetEngineDirectory() ) )
		{
			File::DirectoryCreate( Engine::GetEngineDirectory() );
		}

		Logger::SetFileName( Engine::GetEngineDirectory() / ( m_internalName + cuT( ".log" ) ) );
		Logger::LogInfo( m_internalName + cuT( " - Start" ) );

		m_castor = new Engine();
		DoLoadPlugins( p_splashScreen );

		p_splashScreen.Step( _( "Initialising Castor3D" ), 1 );
		auto l_renderers = m_castor->GetPluginManager().GetPlugins( ePLUGIN_TYPE_RENDERER );

		if ( l_renderers.empty() )
		{
			CASTOR_EXCEPTION( "No renderer plugins" );
		}
		else if ( l_renderers.size() == 1 )
		{
			m_rendererType = std::static_pointer_cast< RendererPlugin >( l_renderers.begin()->second )->GetRendererType();
		}

		if ( m_rendererType == eRENDERER_TYPE_UNDEFINED )
		{
			RendererSelector m_dialog( m_castor, nullptr, m_displayName );
			int l_iReturn = m_dialog.ShowModal();

			if ( l_iReturn == wxID_OK )
			{
				m_rendererType = m_dialog.GetSelectedRenderer();
			}
			else
			{
				l_return = false;
			}
		}
		else
		{
			l_return = true;
		}

		if ( l_return )
		{
			l_return = m_castor->LoadRenderer( m_rendererType );
		}

		return l_return;
	}

	void CastorApplication::DoCleanupCastor()
	{
		delete m_castor;
		m_castor = nullptr;
	}

	void CastorApplication::DoLoadPlugins( SplashScreen & p_splashScreen )
	{
		p_splashScreen.Step( _( "Loading plugins" ), 1 );
		GuiCommon::LoadPlugins( *m_castor );
	}

	void CastorApplication::DoLoadImages( SplashScreen & p_splashScreen )
	{
		p_splashScreen.Step( _( "Loading images" ), 1 );
		wxInitAllImageHandlers();
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
}
