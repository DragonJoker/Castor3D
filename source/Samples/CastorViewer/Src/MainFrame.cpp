#include "RenderPanel.hpp"

#include "MainFrame.hpp"
#include "CastorViewer.hpp"
#include "SceneExporter.hpp"
#include "PropertiesHolder.hpp"

#include <xpms/scene_blanc.xpm>
#include <xpms/mat_blanc.xpm>
#include <xpms/castor_transparent.xpm>
#include <xpms/castor.xpm>
#include <xpms/export.xpm>
#include <xpms/log.xpm>
#include <xpms/properties.xpm>
#include <xpms/node.xpm>
#include <xpms/node_sel.xpm>
#include <xpms/camera.xpm>
#include <xpms/camera_sel.xpm>
#include <xpms/directional.xpm>
#include <xpms/directional_sel.xpm>
#include <xpms/point.xpm>
#include <xpms/point_sel.xpm>
#include <xpms/spot.xpm>
#include <xpms/spot_sel.xpm>
#include <xpms/geometry.xpm>
#include <xpms/geometry_sel.xpm>
#include <xpms/submesh.xpm>
#include <xpms/submesh_sel.xpm>
#include <xpms/scene.xpm>
#include <xpms/scene_sel.xpm>
#include <xpms/panel.xpm>
#include <xpms/panel_sel.xpm>
#include <xpms/border_panel.xpm>
#include <xpms/border_panel_sel.xpm>
#include <xpms/text.xpm>
#include <xpms/text_sel.xpm>
#include <xpms/material.xpm>
#include <xpms/material_sel.xpm>
#include <xpms/pass.xpm>
#include <xpms/pass_sel.xpm>
#include <xpms/texture.xpm>
#include <xpms/texture_sel.xpm>
#include <xpms/viewport.xpm>
#include <xpms/viewport_sel.xpm>
#include <xpms/render_window.xpm>
#include <xpms/render_window_sel.xpm>
#include <xpms/render_target.xpm>
#include <xpms/render_target_sel.xpm>
#include <xpms/frame_variable.xpm>
#include <xpms/frame_variable_sel.xpm>
#include <xpms/frame_variable_buffer.xpm>
#include <xpms/frame_variable_buffer_sel.xpm>

#include <wx/display.h>
#include <wx/aui/auibar.h>
#include <wx/renderer.h>

#include <RenderTarget.hpp>
#include <ImagesLoader.hpp>
#include <FunctorEvent.hpp>
#include <InitialiseEvent.hpp>
#include <MaterialsList.hpp>
#include <RendererSelector.hpp>
#include <SplashScreen.hpp>
#include <AuiDockArt.hpp>
#include <AuiTabArt.hpp>
#include <AuiToolBarArt.hpp>

#include <GeometryBuffers.hpp>
#include <IndexBuffer.hpp>
#include <Material.hpp>
#include <MaterialManager.hpp>
#include <Mesh.hpp>
#include <Pass.hpp>
#include <RenderWindow.hpp>
#include <Sampler.hpp>
#include <Scene.hpp>
#include <SceneFileParser.hpp>
#include <Submesh.hpp>
#include <TextureUnit.hpp>
#include <Vertex.hpp>
#include <VertexBuffer.hpp>
#include <VersionException.hpp>
#include <PluginException.hpp>

using namespace Castor3D;
using namespace Castor;
using namespace GuiCommon;


namespace CastorViewer
{
	namespace
	{
		static const bool CASTOR3D_THREADED = false;
#if defined( NDEBUG )
		static const int CASTOR_WANTED_FPS	= 120;
#else
		static const int CASTOR_WANTED_FPS	= 30;
#endif
		static const wxString CSCN_WILDCARD = wxT( " (*.cscn)|*.cscn|" );
		static const wxString CBSN_WILDCARD = wxT( " (*.cbsn)|*.cbsn|" );
		static const wxString OBJ_WILDCARD = wxT( " (*.obj)|*.obj|" );
		static const wxString ZIP_WILDCARD = wxT( " (*.zip)|*.zip|" );

		typedef enum eID
		{
			eID_TOOL_EXIT,
			eID_TOOL_LOAD_SCENE,
			eID_TOOL_EXPORT_SCENE,
			eID_TOOL_MATERIALS,
			eID_TOOL_SHOW_LOGS,
			eID_TOOL_SHOW_PROPERTIES,
			eID_TOOL_SHOW_LISTS,
			eID_RENDER_TIMER,
			eID_MSGLOG_TIMER,
			eID_ERRLOG_TIMER,
		}	eID;

		typedef enum eBMP
		{
			eBMP_SCENES = GuiCommon::eBMP_COUNT,
			eBMP_MATERIALS,
			eBMP_EXPORT,
			eBMP_LOGS,
			eBMP_PROPERTIES,
		}	eBMP;

		void IndentPressedBitmap( wxRect * rect, int button_state )
		{
			if ( button_state == wxAUI_BUTTON_STATE_PRESSED )
			{
				rect->x++;
				rect->y++;
			}
		}
	}

	DECLARE_APP( CastorViewerApp )

	MainFrame::MainFrame( wxWindow * p_pParent, wxString const & p_strTitle, eRENDERER_TYPE p_eRenderer )
		: wxFrame( p_pParent, wxID_ANY, p_strTitle, wxDefaultPosition, wxSize( 800, 700 ) )
		, m_pCastor3D( NULL )
		, m_pRenderPanel( NULL )
		, m_pImagesLoader( new ImagesLoader )
		, m_timer( NULL )
		, m_timerMsg( NULL )
		, m_timerErr( NULL )
		, m_logTabsContainer( NULL )
		, m_messageLog( NULL )
		, m_errorLog( NULL )
		, m_iLogsHeight( 100 )
		, m_iPropertiesWidth( 240 )
		, m_eRenderer( p_eRenderer )
		, m_sceneObjectsList( NULL )
		, m_materialsList( NULL )
		, m_propertiesContainer( NULL )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
		, m_toolBar( NULL )
	{
	}

	MainFrame::~MainFrame()
	{
		m_auiManager.UnInit();
		delete m_pImagesLoader;
	}

	bool MainFrame::Initialise()
	{
		Logger::RegisterCallback( std::bind( &MainFrame::DoLogCallback, this, std::placeholders::_1, std::placeholders::_2 ), this );
		wxDisplay l_display;
		wxRect l_rect = l_display.GetClientArea();
		wxString l_strCopyright;
		l_strCopyright << wxDateTime().Now().GetCurrentYear();
		SplashScreen l_splashScreen( this, wxT( "Castor\nViewer" ), l_strCopyright + cuT( " " ) + _( " DragonJoker, All rights shared" ), wxPoint( 10, 230 ), wxPoint( 200, 300 ), wxPoint( 180, 260 ), wxPoint( ( l_rect.width - 512 ) / 2, ( l_rect.height - 384 ) / 2 ), 9 );
		m_pSplashScreen = &l_splashScreen;
		bool l_bReturn = DoInitialiseImages();

		if ( l_bReturn )
		{
			DoPopulateStatusBar();
			DoPopulateToolBar();
			wxIcon l_icon = wxIcon( castor_xpm );
			SetIcon( l_icon );
			DoInitialiseGUI();
			DoInitialisePerspectives();
			l_bReturn = DoInitialise3D();
		}

		l_splashScreen.Close();
		Show( l_bReturn );
		return l_bReturn;
	}

	void MainFrame::LoadScene( wxString const & p_strFileName )
	{
		if ( m_pRenderPanel && m_pCastor3D )
		{
			Logger::LogDebug( ( wxChar const * )( cuT( "MainFrame::LoadScene - " ) + p_strFileName ).c_str() );

			if ( !p_strFileName.empty() )
			{
				m_strFilePath = ( wxChar const * )p_strFileName.c_str();
			}

			if ( !m_strFilePath.empty() )
			{
				String l_strLowered = str_utils::lower_case( m_strFilePath );

				if ( m_pMainScene.lock() )
				{
					m_materialsList->UnloadMaterials();
					m_sceneObjectsList->UnloadScene();
					m_pMainScene.reset();
					Logger::LogDebug( cuT( "MainFrame::LoadScene - Scene unloaded" ) );
				}

				m_pRenderPanel->SetRenderWindow( nullptr );
				RenderWindowSPtr l_window = GuiCommon::LoadScene( *m_pCastor3D, m_strFilePath, CASTOR_WANTED_FPS, CASTOR3D_THREADED );

				if ( l_window )
				{
					m_pRenderPanel->SetRenderWindow( l_window );

					if ( l_window->IsInitialised() )
					{
						m_pMainScene = l_window->GetScene();

						if ( l_window->IsFullscreen() )
						{
							ShowFullScreen( true, wxFULLSCREEN_ALL );
						}

						SetClientSize( l_window->GetSize().width() + m_iPropertiesWidth, l_window->GetSize().height() + m_iLogsHeight );
						Logger::LogInfo( cuT( "Scene file read" ) );
					}
					else
					{
						wxMessageBox( _( "Can't initialise the render window" ) );
					}

					if ( CASTOR3D_THREADED )
					{
						m_pCastor3D->StartRendering();
					}

					m_sceneObjectsList->LoadScene( m_pCastor3D, m_pMainScene.lock() );
					m_materialsList->LoadMaterials( m_pCastor3D );
					wxSize l_size = GetClientSize();
#if wxCHECK_VERSION( 2, 9, 0 )
					SetMinClientSize( l_size );
#endif
				}
			}
		}
		else
		{
			wxMessageBox( _( "Can't open a scene file : no engine loaded" ) );
		}
	}

	void MainFrame::ToggleFullScreen( bool p_fullscreen )
	{
		ShowFullScreen( p_fullscreen, wxFULLSCREEN_ALL );

		if ( p_fullscreen )
		{
			m_currentPerspective = m_auiManager.SavePerspective();
			m_auiManager.LoadPerspective( m_fullScreenPerspective );
		}
		else
		{
			m_auiManager.LoadPerspective( m_currentPerspective );
		}
	}

	void MainFrame::DoLoadPlugins()
	{
		m_pSplashScreen->Step( _( "Loading plugins" ), 1 );
		GuiCommon::LoadPlugins( *m_pCastor3D );
	}

	void MainFrame::DoInitialiseGUI()
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		SetClientSize( 800 + m_iPropertiesWidth, 600 + m_iLogsHeight );
		wxSize l_size = GetClientSize();
#if wxCHECK_VERSION( 2, 9, 0 )
		SetMinClientSize( l_size );
#endif
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_pRenderPanel = new RenderPanel( this, wxID_ANY, wxDefaultPosition, wxSize( l_size.x - m_iPropertiesWidth, l_size.y - m_iLogsHeight ) );
		m_logTabsContainer = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_logTabsContainer->SetArtProvider( new AuiTabArt );
		m_sceneTabsContainer = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_sceneTabsContainer->SetArtProvider( new AuiTabArt );
		m_propertiesContainer = new PropertiesHolder( true, this, wxDefaultPosition, wxDefaultSize );
		m_propertiesContainer->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_propertiesContainer->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_propertiesContainer->SetCaptionBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_propertiesContainer->SetCaptionTextColour( PANEL_FOREGROUND_COLOUR );
		m_propertiesContainer->SetSelectionBackgroundColour( ACTIVE_TAB_COLOUR );
		m_propertiesContainer->SetSelectionTextColour( ACTIVE_TEXT_COLOUR );
		m_propertiesContainer->SetCellBackgroundColour( INACTIVE_TAB_COLOUR );
		m_propertiesContainer->SetCellTextColour( INACTIVE_TEXT_COLOUR );
		m_propertiesContainer->SetLineColour( BORDER_COLOUR );
		m_propertiesContainer->SetMarginColour( BORDER_COLOUR );

		m_auiManager.AddPane( m_pRenderPanel, wxAuiPaneInfo().CaptionVisible( false ).Center().CloseButton( false ).Name( wxT( "Render" ) ).MinSize( l_size.x - m_iPropertiesWidth, l_size.y - m_iLogsHeight ).Layer( 0 ).Movable( false ).PaneBorder( false ).Dockable( false ) );
		m_auiManager.AddPane( m_logTabsContainer, wxAuiPaneInfo().CaptionVisible( false ).Hide().CloseButton().Name( wxT( "Logs" ) ).Caption( _( "Logs" ) ).Bottom().Dock().BottomDockable().TopDockable().Movable().PinButton().MinSize( l_size.x, m_iLogsHeight ).Layer( 1 ).PaneBorder( false ) );
		m_auiManager.AddPane( m_sceneTabsContainer, wxAuiPaneInfo().CaptionVisible( false ).Hide().CloseButton().Name( wxT( "Objects" ) ).Caption( _( "Objects" ) ).Left().Dock().LeftDockable().RightDockable().Movable().PinButton().MinSize( m_iPropertiesWidth, l_size.y / 3 ).Layer( 2 ).PaneBorder( false ) );
		m_auiManager.AddPane( m_propertiesContainer, wxAuiPaneInfo().CaptionVisible( false ).Hide().CloseButton().Name( wxT( "Properties" ) ).Caption( _( "Properties" ) ).Left().Dock().LeftDockable().RightDockable().Movable().PinButton().MinSize( m_iPropertiesWidth, l_size.y / 3 ).Layer( 2 ).PaneBorder( false ) );

		auto l_logCreator = [this, &l_size]( wxString const & p_name, wxListBox *& p_log )
		{
			p_log = new wxListBox( m_logTabsContainer, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxBORDER_NONE );
			p_log->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			p_log->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			m_logTabsContainer->AddPage( p_log, p_name, true );
		};

		l_logCreator( _( "Messages" ), m_messageLog );
		l_logCreator( _( "Errors" ), m_errorLog );

		m_sceneObjectsList = new SceneObjectsList( m_propertiesContainer, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize );
		m_sceneObjectsList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_sceneObjectsList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_sceneTabsContainer->AddPage( m_sceneObjectsList, _( "Scenes" ), true );

		m_materialsList = new MaterialsList( m_propertiesContainer, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize );
		m_materialsList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_materialsList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_sceneTabsContainer->AddPage( m_materialsList, _( "Materials" ), true );

		m_auiManager.Update();
	}

	bool MainFrame::DoInitialise3D()
	{
		bool l_bReturn = true;
		Logger::LogInfo( cuT( "Initialising Castor3D" ) );
		m_pCastor3D = new Engine();
		DoLoadPlugins();

		try
		{
			m_pSplashScreen->Step( _( "Initialising main window" ), 1 );

			if ( m_eRenderer == eRENDERER_TYPE_UNDEFINED )
			{
				RendererSelector m_dialog( m_pCastor3D, this, wxT( "Castor Viewer" ) );
				int l_iReturn = m_dialog.ShowModal();

				if ( l_iReturn == wxID_OK )
				{
					m_eRenderer = m_dialog.GetSelectedRenderer();
				}
				else
				{
					l_bReturn = false;
				}
			}
			else
			{
				l_bReturn = true;
			}

			if ( l_bReturn )
			{
				l_bReturn = m_pCastor3D->LoadRenderer( m_eRenderer );
			}

			if ( l_bReturn )
			{
				Logger::LogInfo( cuT( "Castor3D Initialised" ) );

				if ( !CASTOR3D_THREADED && !m_timer )
				{
					m_timer = new wxTimer( this, eID_RENDER_TIMER );
					m_timer->Start( 1000 / CASTOR_WANTED_FPS );
				}

				if ( !m_timerMsg )
				{
					m_timerMsg = new wxTimer( this, eID_MSGLOG_TIMER );
					m_timerMsg->Start( 100 );
				}

				if ( !m_timerErr )
				{
					m_timerErr = new wxTimer( this, eID_ERRLOG_TIMER );
					m_timerErr->Start( 100 );
				}
			}
		}
		catch ( ... )
		{
			wxMessageBox( _( "Problem occured while initialising Castor3D.\nLook at CastorViewer.log for more details" ), _( "Exception" ), wxOK | wxCENTRE | wxICON_ERROR );
			l_bReturn = false;
		}

		return l_bReturn;
	}

	bool MainFrame::DoInitialiseImages()
	{
		m_pSplashScreen->Step( _( "Loading images" ), 1 );
		m_pImagesLoader->AddBitmap( eBMP_SCENE, scene_xpm );
		m_pImagesLoader->AddBitmap( eBMP_SCENE_SEL, scene_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_NODE, node_xpm );
		m_pImagesLoader->AddBitmap( eBMP_NODE_SEL, node_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_CAMERA, camera_xpm );
		m_pImagesLoader->AddBitmap( eBMP_CAMERA_SEL, camera_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_GEOMETRY, geometry_xpm );
		m_pImagesLoader->AddBitmap( eBMP_GEOMETRY_SEL, geometry_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_DIRECTIONAL_LIGHT, directional_xpm );
		m_pImagesLoader->AddBitmap( eBMP_DIRECTIONAL_LIGHT_SEL, directional_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_POINT_LIGHT, point_xpm );
		m_pImagesLoader->AddBitmap( eBMP_POINT_LIGHT_SEL, point_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_SPOT_LIGHT, spot_xpm );
		m_pImagesLoader->AddBitmap( eBMP_SPOT_LIGHT_SEL, spot_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_SUBMESH, submesh_xpm );
		m_pImagesLoader->AddBitmap( eBMP_SUBMESH_SEL, submesh_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_PANEL_OVERLAY, panel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_PANEL_OVERLAY_SEL, panel_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_BORDER_PANEL_OVERLAY, border_panel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_BORDER_PANEL_OVERLAY_SEL, border_panel_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_TEXT_OVERLAY, text_xpm );
		m_pImagesLoader->AddBitmap( eBMP_TEXT_OVERLAY_SEL, text_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_MATERIAL, material_xpm );
		m_pImagesLoader->AddBitmap( eBMP_MATERIAL_SEL, material_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_PASS, pass_xpm );
		m_pImagesLoader->AddBitmap( eBMP_PASS_SEL, pass_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_TEXTURE, texture_xpm );
		m_pImagesLoader->AddBitmap( eBMP_TEXTURE_SEL, texture_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_VIEWPORT, viewport_xpm );
		m_pImagesLoader->AddBitmap( eBMP_VIEWPORT_SEL, viewport_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_RENDER_TARGET, render_target_xpm );
		m_pImagesLoader->AddBitmap( eBMP_RENDER_TARGET_SEL, render_target_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_RENDER_WINDOW, render_window_xpm );
		m_pImagesLoader->AddBitmap( eBMP_RENDER_WINDOW_SEL, render_window_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_FRAME_VARIABLE, frame_variable_xpm );
		m_pImagesLoader->AddBitmap( eBMP_FRAME_VARIABLE_SEL, frame_variable_sel_xpm );
		m_pImagesLoader->AddBitmap( eBMP_FRAME_VARIABLE_BUFFER, frame_variable_buffer_xpm );
		m_pImagesLoader->AddBitmap( eBMP_FRAME_VARIABLE_BUFFER_SEL, frame_variable_buffer_sel_xpm );

		m_pImagesLoader->AddBitmap( CV_IMG_CASTOR, castor_transparent_xpm );
		m_pImagesLoader->AddBitmap( eBMP_SCENES, scene_blanc_xpm );
		m_pImagesLoader->AddBitmap( eBMP_MATERIALS, mat_blanc_xpm );
		m_pImagesLoader->AddBitmap( eBMP_EXPORT, export_xpm );
		m_pImagesLoader->AddBitmap( eBMP_LOGS, log_xpm );
		m_pImagesLoader->AddBitmap( eBMP_PROPERTIES, properties_xpm );
		m_pImagesLoader->WaitAsyncLoads();
		return true;
	}

	void MainFrame::DoPopulateStatusBar()
	{
		wxStatusBar * l_statusBar = CreateStatusBar();
		l_statusBar->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		l_statusBar->SetForegroundColour( INACTIVE_TEXT_COLOUR );
	}

	void MainFrame::DoPopulateToolBar()
	{
		m_pSplashScreen->Step( _( "Loading toolbar" ), 1 );
		m_toolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_HORIZONTAL );
		m_toolBar->SetArtProvider( new AuiToolBarArt );
		m_toolBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_toolBar->SetToolBitmapSize( wxSize( 32, 32 ) );
		m_toolBar->AddTool( eID_TOOL_LOAD_SCENE, _( "Load Scene" ), wxImage( *m_pImagesLoader->GetBitmap( eBMP_SCENES ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Open a new scene" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->AddTool( eID_TOOL_EXPORT_SCENE, _( "Export Scene" ), wxImage( *m_pImagesLoader->GetBitmap( eBMP_EXPORT ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Export the current scene" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->AddSeparator();
		m_toolBar->AddTool( eID_TOOL_SHOW_LOGS, _( "Logs" ), wxImage( *m_pImagesLoader->GetBitmap( eBMP_LOGS ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display logs" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->AddTool( eID_TOOL_SHOW_LISTS, _( "Lists" ), wxImage( *m_pImagesLoader->GetBitmap( eBMP_MATERIALS ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display lists" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->AddTool( eID_TOOL_SHOW_PROPERTIES, _( "Properties" ), wxImage( *m_pImagesLoader->GetBitmap( eBMP_PROPERTIES ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display properties" ) );
		m_pSplashScreen->Step( 1 );
		m_toolBar->Realize();
		m_auiManager.AddPane( m_toolBar, wxAuiPaneInfo().Name( wxT( "MainToolBar" ) ).ToolbarPane().Top().Row( 1 ).Dockable( false ).Gripper( false ) );
	}

	void MainFrame::DoInitialisePerspectives()
	{
		wxAuiPaneInfoArray l_panes = m_auiManager.GetAllPanes();
		std::vector< bool > l_visibilities;
		m_currentPerspective = m_auiManager.SavePerspective();

		for ( size_t i = 0; i < l_panes.size(); ++i )
		{
			l_panes[i].Hide();
		}

		m_auiManager.GetPane( m_toolBar ).Hide();
		m_fullScreenPerspective = m_auiManager.SavePerspective();
		m_auiManager.LoadPerspective( m_currentPerspective );
	}

	void MainFrame::DoLogCallback( String const & p_strLog, ELogType p_eLogType )
	{
		switch ( p_eLogType )
		{
		case Castor::ELogType_DEBUG:
		case Castor::ELogType_INFO:
			{
				std::lock_guard< std::mutex > l_lock( m_msgLogListMtx );
				m_msgLogList.push_back( p_strLog );
			}
			break;

		case Castor::ELogType_WARNING:
		case Castor::ELogType_ERROR:
			{
				std::lock_guard< std::mutex > l_lock( m_errLogListMtx );
				m_errLogList.push_back( p_strLog );
			}
			break;

		default:
			break;
		}
	}

	BEGIN_EVENT_TABLE( MainFrame, wxFrame )
		EVT_TIMER( eID_RENDER_TIMER, MainFrame::OnTimer )
		EVT_TIMER( eID_MSGLOG_TIMER, MainFrame::OnTimer )
		EVT_TIMER( eID_ERRLOG_TIMER, MainFrame::OnTimer )
		EVT_PAINT( MainFrame::OnPaint )
		EVT_INIT_DIALOG( MainFrame::OnInit )
		EVT_CLOSE( MainFrame::OnClose )
		EVT_ENTER_WINDOW( MainFrame::OnEnterWindow )
		EVT_LEAVE_WINDOW( MainFrame::OnLeaveWindow )
		EVT_ERASE_BACKGROUND( MainFrame::OnEraseBackground )
		EVT_TOOL( eID_TOOL_LOAD_SCENE, MainFrame::OnLoadScene )
		EVT_TOOL( eID_TOOL_EXPORT_SCENE, MainFrame::OnExportScene )
		EVT_TOOL( eID_TOOL_SHOW_LOGS, MainFrame::OnShowLogs )
		EVT_TOOL( eID_TOOL_SHOW_LISTS, MainFrame::OnShowLists )
		EVT_TOOL( eID_TOOL_SHOW_PROPERTIES, MainFrame::OnShowProperties )
	END_EVENT_TABLE()

	void MainFrame::OnPaint( wxPaintEvent & p_event )
	{
		wxPaintDC l_paintDC( this );
		p_event.Skip();
	}

	void MainFrame::OnTimer( wxTimerEvent & p_event )
	{
		if ( p_event.GetId() == eID_RENDER_TIMER )
		{
			if ( m_pCastor3D )
			{
				m_pCastor3D->RenderOneFrame();
			}
		}
		else if ( p_event.GetId() == eID_MSGLOG_TIMER && m_messageLog )
		{
			wxArrayString l_flush;
			{
				std::lock_guard< std::mutex > l_lock( m_msgLogListMtx );
				std::swap( l_flush, m_msgLogList );
			}
			if ( !l_flush.empty() )
			{
				m_messageLog->Insert( l_flush, 0 );
			}
		}
		else if ( p_event.GetId() == eID_ERRLOG_TIMER && m_errorLog )
		{
			wxArrayString l_flush;
			{
				std::lock_guard< std::mutex > l_lock( m_errLogListMtx );
				std::swap( l_flush, m_errLogList );
			}
			if ( !l_flush.empty() )
			{
				m_errorLog->Insert( l_flush, 0 );
			}
		}

		p_event.Skip();
	}

	void MainFrame::OnInit( wxInitDialogEvent & p_event )
	{
	}

	void MainFrame::OnClose( wxCloseEvent & p_event )
	{
		Logger::UnregisterCallback( this );
		m_auiManager.DetachPane( m_sceneTabsContainer );
		m_auiManager.DetachPane( m_propertiesContainer );
		m_auiManager.DetachPane( m_logTabsContainer );
		m_auiManager.DetachPane( m_pRenderPanel );
		m_auiManager.DetachPane( m_toolBar );
		m_messageLog = NULL;
		m_errorLog = NULL;
		Hide();

		if ( m_timer )
		{
			m_timer->Stop();
			delete m_timer;
			m_timer = nullptr;
		}

		if ( m_timerMsg )
		{
			m_timerMsg->Stop();
			delete m_timerMsg;
			m_timerMsg = nullptr;
		}

		if ( m_timerErr )
		{
			m_timerErr->Stop();
			delete m_timerErr;
			m_timerErr = nullptr;
		}

		m_pMainScene.reset();

		if ( m_pRenderPanel )
		{
			m_pRenderPanel->SetRenderWindow( nullptr );
		}

		if ( m_pCastor3D )
		{
#if CASTOR3D_THREADED
			m_pCastor3D->EndRendering();
#endif
			m_pCastor3D->Cleanup();
		}

		if ( m_pRenderPanel )
		{
			m_pRenderPanel->UnFocus();
			m_pRenderPanel->Close( true );
			m_pRenderPanel = NULL;
		}

		DestroyChildren();

		if ( m_pImagesLoader )
		{
			m_pImagesLoader->Cleanup();
		}

		delete m_pCastor3D;
		m_pCastor3D = NULL;
		p_event.Skip();
	}

	void MainFrame::OnEnterWindow( wxMouseEvent & p_event )
	{
		SetFocus();
		p_event.Skip();
	}

	void MainFrame::OnLeaveWindow( wxMouseEvent & p_event )
	{
		p_event.Skip();
	}

	void MainFrame::OnEraseBackground( wxEraseEvent & p_event )
	{
		p_event.Skip();
	}

	void MainFrame::OnLoadScene( wxCommandEvent & p_event )
	{
		wxString l_wildcard = _( "Castor3D scene files" );
		l_wildcard << wxT( " (*.cscn;*.cbsn;*.zip)|*.cscn;*.cbsn;*.zip|" );
		l_wildcard << _( "Castor3D text scene file" );
		l_wildcard << CSCN_WILDCARD;
		l_wildcard << _( "Castor3D binary scene file" );
		l_wildcard << CBSN_WILDCARD;
		l_wildcard << _( "Zip archive" );
		l_wildcard << ZIP_WILDCARD;
		l_wildcard << wxT( "|" );
		wxFileDialog l_fileDialog( this, _( "Open a scene" ), wxEmptyString, wxEmptyString, l_wildcard );

		if ( l_fileDialog.ShowModal() == wxID_OK )
		{
			LoadScene( ( wxChar const * )l_fileDialog.GetPath().c_str() );
		}

		p_event.Skip();
	}

	void MainFrame::OnExportScene( wxCommandEvent & p_event )
	{
		wxString l_wildcard = _( "Castor3D text scene" );
		l_wildcard += CSCN_WILDCARD;
		l_wildcard += _( "Castor3D binary scene" );
		l_wildcard += CBSN_WILDCARD;
		l_wildcard += _( "Wavefront OBJ" );
		l_wildcard += OBJ_WILDCARD;
		wxFileDialog l_fileDialog( this, _( "Export the scene" ), wxEmptyString, wxEmptyString, l_wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
		SceneSPtr l_scene = m_pMainScene.lock();

		if ( l_scene )
		{
			if ( l_fileDialog.ShowModal() == wxID_OK )
			{
				Path l_pathFile( ( wxChar const * )l_fileDialog.GetPath().c_str() );

				if ( l_pathFile.GetExtension() == cuT( "obj" ) )
				{
					ObjSceneExporter l_exporter;
					l_exporter.ExportScene( m_pCastor3D, *m_pMainScene.lock(), l_pathFile );
				}
				else if ( l_pathFile.GetExtension() == cuT( "cscn" ) )
				{
					CscnSceneExporter l_exporter;
					l_exporter.ExportScene( m_pCastor3D, *m_pMainScene.lock(), l_pathFile );
				}
				else if ( l_pathFile.GetExtension() == cuT( "cbsn" ) )
				{
					CbsnSceneExporter l_exporter;
					l_exporter.ExportScene( m_pCastor3D, *m_pMainScene.lock(), l_pathFile );
				}
			}
		}
		else
		{
			wxMessageBox( _( "No scene Loaded." ), _( "Error" ), wxOK | wxCENTRE | wxICON_ERROR );
		}

		p_event.Skip();
	}

	void MainFrame::OnShowLogs( wxCommandEvent & p_event )
	{
		if ( !m_logTabsContainer->IsShown() )
		{
			m_auiManager.GetPane( m_logTabsContainer ).Show();
		}
		else
		{
			m_auiManager.GetPane( m_logTabsContainer ).Hide();
		}

		m_auiManager.Update();
		p_event.Skip();
	}

	void MainFrame::OnShowProperties( wxCommandEvent & p_event )
	{
		if ( !m_propertiesContainer->IsShown() )
		{
			m_auiManager.GetPane( m_propertiesContainer ).Show();
		}
		else
		{
			m_auiManager.GetPane( m_propertiesContainer ).Hide();
		}

		m_auiManager.Update();
		p_event.Skip();
	}

	void MainFrame::OnShowLists( wxCommandEvent & p_event )
	{
		if ( !m_sceneTabsContainer->IsShown() )
		{
			m_auiManager.GetPane( m_sceneTabsContainer ).Show();
		}
		else
		{
			m_auiManager.GetPane( m_sceneTabsContainer ).Hide();
		}

		m_auiManager.Update();
		p_event.Skip();
	}
}
