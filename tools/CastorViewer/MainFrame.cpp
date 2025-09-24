#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/wx.h>
#include <wx/display.h>
#include <wx/mstream.h>
#include <wx/renderer.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#include "CastorViewer/RenderPanel.hpp"

#include "CastorViewer/MainFrame.hpp"
#include "CastorViewer/CastorViewer.hpp"

#include <GuiCommon/Aui/AuiDockArt.hpp>
#include <GuiCommon/Aui/AuiTabArt.hpp>
#include <GuiCommon/Aui/AuiToolBarArt.hpp>
#include <GuiCommon/Properties/PropertiesContainer.hpp>
#include <GuiCommon/Properties/PropertiesDialog.hpp>
#include <GuiCommon/Properties/PropertiesHolder.hpp>
#include <GuiCommon/Properties/TreeItems/TreeHolder.hpp>
#include <GuiCommon/Properties/TreeItems/ExportOptionsTreeItemProperty.hpp>
#include <GuiCommon/System/ImagesLoader.hpp>
#include <GuiCommon/System/RendererSelector.hpp>
#include <GuiCommon/System/SplashScreen.hpp>
#include <GuiCommon/System/TreeListContainer.hpp>

#include <SceneExporter/CscnExporter.hpp>

#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Miscellaneous/BlockTimer.hpp>

#include <GuiCommon/xpms/castor.xpm>

#if defined( GUICOMMON_RECORDS )
#	include <GuiCommon/xpms/record.xpm>
#	include <GuiCommon/xpms/stop.xpm>
#endif

namespace CastorViewer
{
	namespace main
	{
#if defined( __WXOSX_COCOA__ )
		static constexpr bool frameToolbar = false;
#else
		static constexpr bool frameToolbar = true;
#endif

		static constexpr bool defaultDebugPerspective = false;
		static constexpr int recordFPS = 30;
		static const wxString objWildcard = wxT( " (*.obj)|*.obj|" );

		enum class eID
		{
			eTOOL_EXIT,
			eTOOL_LOAD_SCENE,
			eTOOL_EXPORT_SCENE,
			eTOOL_MATERIALS,
			eTOOL_SHOW_LOGS,
			eTOOL_SHOW_LISTS,
			eTOOL_PRINT_SCREEN,
			eTOOL_RECORD,
			eTOOL_STOP,
			ePANE_RENDER,
			ePANE_LISTS,
			ePANE_LOGS,
			eRENDER_TIMER,
			eMSGLOG_TIMER,
			eERRLOG_TIMER,
#ifndef NDEBUG
			eDBGLOG_TIMER,
#endif
			eFPS_TIMER,
			eLOAD_END,
		};

		static void updateLog( LogContainer & log )
		{
			c3d::Vector< c3d::Pair< wxString, bool > > flush;
			{
				auto lock = c3d::makeUniqueLock( log.mutex );
				c3d::swap( flush, log.queue );
			}

			if ( !flush.empty() )
			{
				for ( auto const & [message, insert] : flush )
				{
					if ( insert )
						log.listBox->Insert( message, 0 );
					else
						log.listBox->SetString( 0, message );
				}
			}
		}

		static void addWildcard( wxString & result
			, wxString const & name
			, wxString const & extensions )
		{
			if ( !result.empty() )
			{
				result += wxT( "|" );
			}

			result += name
				+ wxT( " (" ) + extensions
				+ wxT( ")|" ) + extensions;
		}
	}

	MainFrame::MainFrame( wxString const & title )
		: wxFrame( nullptr, wxID_ANY, title, wxPoint{}, wxSize{ 800, 700 } )
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
		, m_recordFps{ main::recordFPS }
	{
		if ( main::frameToolbar )
		{
			Connect( int( main::eID::eTOOL_LOAD_SCENE ), wxEVT_TOOL, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onLoadScene ) );
			Connect( int( main::eID::eTOOL_EXPORT_SCENE ), wxEVT_TOOL, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onExportScene ) );
			Connect( int( main::eID::eTOOL_SHOW_LOGS ), wxEVT_TOOL, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onShowLogs ) );
			Connect( int( main::eID::eTOOL_SHOW_LISTS ), wxEVT_TOOL, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onShowLists ) );
			Connect( int( main::eID::eTOOL_PRINT_SCREEN ), wxEVT_TOOL, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onPrintScreen ) );
			Connect( int( main::eID::eTOOL_RECORD ), wxEVT_TOOL, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onRecord ) );
			Connect( int( main::eID::eTOOL_STOP ), wxEVT_TOOL, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onStop ) );
		}
		else
		{
			Connect( int( main::eID::eTOOL_LOAD_SCENE ), wxEVT_MENU, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onLoadScene ) );
			Connect( int( main::eID::eTOOL_EXPORT_SCENE ), wxEVT_MENU, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onExportScene ) );
			Connect( int( main::eID::eTOOL_SHOW_LOGS ), wxEVT_MENU, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onShowLogs ) );
			Connect( int( main::eID::eTOOL_SHOW_LISTS ), wxEVT_MENU, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onShowLists ) );
			Connect( int( main::eID::eTOOL_PRINT_SCREEN ), wxEVT_MENU, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onPrintScreen ) );
			Connect( int( main::eID::eTOOL_RECORD ), wxEVT_MENU, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onRecord ) );
			Connect( int( main::eID::eTOOL_STOP ), wxEVT_MENU, wxEVENT_HANDLER_CAST( wxCommandEventFunction, MainFrame::onStop ) );
		}
	}

	MainFrame::~MainFrame()
	{
		m_auiManager.UnInit();
	}

	bool MainFrame::initialise( GuiCommon::SplashScreen & splashScreen )
	{
		c3d::Logger::registerCallback( [this]( c3d::MbString const & logText
			, c3d::LogType logType, bool newLine )
			{
				doLogCallback( logText, logType, newLine );
			}, this );
		doPopulateStatusBar();
		doPopulateToolBar( splashScreen );
		SetIcon( wxIcon{ castor_xpm } );
		doInitialiseGUI();
		doInitialiseTimers();
		doInitialisePerspectives();
		Show( true );
		return true;
	}

	void MainFrame::loadScene( wxString const & fileName )
	{
		auto engine = wxGetApp().getCastor();

		if ( m_renderPanel && engine )
		{
			if ( !fileName.empty() )
			{
				m_filePath = c3d::Path{ GuiCommon::make_String( fileName ) };
			}

			if ( !m_filePath.empty() )
			{
				auto & window = m_renderPanel->getRenderWindow();
				window.enableLoading();
				doCleanupScene();

				if ( engine->isThreaded() )
				{
					engine->getRenderLoop().beginRendering();
					GuiCommon::loadScene( *engine
						, cuT( "Castor3D" )
						, m_filePath
						, &window.getProgressBar()
						, this
						, int( main::eID::eLOAD_END ) );
				}	
				else
				{
					doSceneLoadEnd( GuiCommon::loadScene( *engine
						, cuT( "Castor3D" )
						, m_filePath
						, nullptr ) );
				}
			}
			else
			{
				wxMessageBox( _( "Can't open a scene file : empty file name." ) );
			}
		}
		else
		{
			wxMessageBox( _( "Can't open a scene file : no engine loaded." ) );
		}
	}

	void MainFrame::toggleFullScreen( bool fullscreen )
	{
		ShowFullScreen( fullscreen, wxFULLSCREEN_ALL );

		if ( fullscreen )
		{
			m_currentPerspective = m_auiManager.SavePerspective();

			if ( main::frameToolbar )
				m_auiManager.GetPane( m_toolBar ).Hide();
			m_fullScreenPerspective = m_auiManager.SavePerspective();
			m_auiManager.LoadPerspective( m_fullScreenPerspective );
			if ( !main::frameToolbar )
				m_menuBar->Hide();
		}
		else
		{
			m_auiManager.LoadPerspective( m_currentPerspective );
			if ( !main::frameToolbar )
				m_menuBar->Show();
		}
	}

	void MainFrame::select( c3d::Geometry const * geometry, c3d::Submesh const * submesh )const
	{
		if ( m_objectsTree && m_objectsTree->getList() )
		{
			m_objectsTree->getList()->select( geometry, submesh );
			m_materialsTree->getList()->select( geometry->getMaterial( *submesh ) );
		}
	}

	void MainFrame::doInitialiseTimers()
	{
		if ( auto engine = wxGetApp().getCastor();
			!engine->isThreaded() && !m_timer )
		{
			m_timer = c3d::makeRawUnique< wxTimer >( this, int( main::eID::eRENDER_TIMER ) );
			m_timer->Start( 1000 / int( engine->getRenderLoop().getWantedFps() ) );
		}

		if ( !m_timerMsg )
		{
			m_timerMsg = c3d::makeRawUnique< wxTimer >( this, int( main::eID::eMSGLOG_TIMER ) );
			m_timerMsg->Start( 100 );
		}

		if ( !m_timerErr )
		{
			m_timerErr = c3d::makeRawUnique< wxTimer >( this, int( main::eID::eERRLOG_TIMER ) );
			m_timerErr->Start( 100 );
		}

		if ( !m_fpsTimer )
		{
			m_fpsTimer = c3d::makeRawUnique< wxTimer >( this, int( main::eID::eFPS_TIMER ) );
		}
	}

	void MainFrame::doInitialiseGUI()
	{
		SetBackgroundColour( GuiCommon::PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( GuiCommon::PANEL_FOREGROUND_COLOUR );

#if wxCHECK_VERSION( 3, 1, 0 )
		auto size = this->FromDIP( wxSize{ 800 + m_propertiesWidth, 600 + m_logsHeight } );
#else
		auto size = wxSize{ 800 + m_propertiesWidth, 600 + m_logsHeight };
#endif
		SetClientSize( size );
#if wxCHECK_VERSION( 2, 9, 0 )
		SetMinClientSize( size );
#endif

		m_auiManager.SetArtProvider( new GuiCommon::AuiDockArt );
		m_renderPanel = new RenderPanel( this, int( main::eID::ePANE_RENDER ), wxDefaultPosition, wxSize( size.x - m_propertiesWidth, size.y - m_logsHeight ) );
		m_logTabsContainer = new wxAuiNotebook( this, int( main::eID::ePANE_LOGS ), wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_logTabsContainer->SetArtProvider( new GuiCommon::AuiTabArt );
		m_sceneTabsContainer = new wxAuiNotebook( this, int( main::eID::ePANE_LISTS ), wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE );
		m_sceneTabsContainer->SetBackgroundColour( GuiCommon::PANEL_BACKGROUND_COLOUR );
		m_sceneTabsContainer->SetForegroundColour( GuiCommon::PANEL_FOREGROUND_COLOUR );
		m_sceneTabsContainer->SetArtProvider( new GuiCommon::AuiTabArt );

		m_auiManager.AddPane( m_renderPanel
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.Center()
				.CloseButton( false )
				.Name( wxT( "Render" ) )
				.MinSize( size.x - m_propertiesWidth, size.y - m_logsHeight )
				.Layer( 0 )
				.Movable( false )
				.PaneBorder( false )
				.Dockable( false ) );
		m_auiManager.AddPane( m_logTabsContainer
			, wxAuiPaneInfo()
				.CaptionVisible()
				.Hide()
				.CloseButton( false )
				.MaximizeButton( true )
				.Resizable( true )
				.PinButton( true )
				.Name( wxT( "Logs" ) )
				.Caption( _( "Logs" ) )
				.Bottom()
				.Dock()
				.BottomDockable()
				.TopDockable()
				.Movable()
				.PinButton()
				.MinSize( size.x, m_logsHeight )
				.Layer( 1 )
				.PaneBorder()
				.Floatable() );
		m_auiManager.AddPane( m_sceneTabsContainer
			, wxAuiPaneInfo()
				.CaptionVisible()
				.Hide()
				.CloseButton( false )
				.MaximizeButton( true )
				.Resizable( true )
				.PinButton( true )
				.Name( wxT( "Objects" ) )
				.Caption( _( "Objects" ) )
				.Left()
				.Dock()
				.LeftDockable()
				.RightDockable()
				.Movable()
				.PinButton()
				.MinSize( m_propertiesWidth, size.y / 3 )
				.Layer( 2 )
				.PaneBorder()
				.Floatable() );

		auto createLog = [this]( wxString const & name
			, LogContainer & log )
		{
			log.listBox = new wxListBox( m_logTabsContainer, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxBORDER_NONE );
			log.listBox->SetBackgroundColour( GuiCommon::PANEL_BACKGROUND_COLOUR );
			log.listBox->SetForegroundColour( GuiCommon::PANEL_FOREGROUND_COLOUR );
			m_logTabsContainer->AddPage( log.listBox, name, true );
		};

		createLog( _( "Messages" ), m_messageLog );
		createLog( _( "Errors" ), m_errorLog );
#ifndef NDEBUG
		createLog( _( "Debug" ), m_debugLog );
#endif
		m_logTabsContainer->ChangeSelection( 0u );

		auto & imagesLoader = wxGetApp().getImagesLoader();
		m_sceneTree = new GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree >{ imagesLoader, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize };
		m_objectsTree = new GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree >{ imagesLoader, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize };
		m_nodesTree = new GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree >{ imagesLoader, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize };
		m_lightsTree = new GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree >{ imagesLoader, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize };
		m_materialsTree = new GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree >{ imagesLoader, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize };
		m_overlaysTree = new GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree >{ imagesLoader, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize };
		m_guiTree = new GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsTree >{ imagesLoader, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize };
		m_sceneTabsContainer->AddPage( m_sceneTree, _( "Scene" ), true );

		if ( m_objectsTree )
		{
			m_sceneTabsContainer->AddPage( m_objectsTree, _( "Objects" ), false );
			m_selectSubmesh = m_objectsTree->getList()->onSelectSubmesh.connect( [this]( c3d::Geometry const * geometry
				, c3d::Submesh const * submesh )
				{
					if ( m_renderPanel )
						m_renderPanel->select( geometry, submesh );
				} );
		}

		if ( m_nodesTree )
		{
			m_sceneTabsContainer->AddPage( m_nodesTree, _( "Nodes" ), false );
			m_selectNode = m_nodesTree->getList()->onSelectNode.connect( [this]( c3d::SceneNode * node )
				{
					if ( m_renderPanel )
						m_renderPanel->select( node );
				} );
		}

		if ( m_lightsTree )
		{
			m_sceneTabsContainer->AddPage( m_lightsTree, _( "Lights" ), false );
			m_selectLight = m_lightsTree->getList()->onSelectLight.connect( [this]( c3d::LightInstance const * light )
				{
					if ( m_renderPanel )
						m_renderPanel->select( light );
				} );
		}

		if ( m_materialsTree )
			m_sceneTabsContainer->AddPage( m_materialsTree, _( "Materials" ), false );
		if ( m_overlaysTree )
			m_sceneTabsContainer->AddPage( m_overlaysTree, _( "Overlays" ), false );
		if ( m_guiTree )
			m_sceneTabsContainer->AddPage( m_guiTree, _( "GUI" ), false );

		m_auiManager.Update();
	}

	void MainFrame::doPopulateStatusBar()
	{
		wxStatusBar * statusBar = CreateStatusBar();
		statusBar->SetBackgroundColour( GuiCommon::INACTIVE_TAB_COLOUR );
		statusBar->SetForegroundColour( GuiCommon::INACTIVE_TEXT_COLOUR );
	}

	void MainFrame::doPopulateToolBar( GuiCommon::SplashScreen & splashScreen )
	{
		splashScreen.Step( _( "Loading toolbar" ), 1 );

		if ( main::frameToolbar )
		{
			auto & imagesLoader = wxGetApp().getImagesLoader();
			m_toolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_HORIZONTAL );
			m_toolBar->SetArtProvider( new GuiCommon::AuiToolBarArt );
			m_toolBar->SetBackgroundColour( GuiCommon::PANEL_BACKGROUND_COLOUR );
			m_toolBar->SetToolBitmapSize( wxSize( 32, 32 ) );
			m_toolBar->AddTool( int( main::eID::eTOOL_LOAD_SCENE ), _( "Load Scene" ), imagesLoader.getBitmapT( eBMP::eScenes )->Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Open a new scene" ) );
			splashScreen.Step( 1 );
			m_toolBar->AddTool( int( main::eID::eTOOL_EXPORT_SCENE ), _( "Export Scene" ), imagesLoader.getBitmapT( eBMP::eExport )->Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Export the current scene" ) );
			m_toolBar->EnableTool( int( main::eID::eTOOL_EXPORT_SCENE ), false );
			splashScreen.Step( 1 );
			m_toolBar->AddSeparator();
			m_toolBar->AddTool( int( main::eID::eTOOL_SHOW_LOGS ), _( "Logs" ), imagesLoader.getBitmapT( eBMP::eLogs )->Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display logs" ) );
			splashScreen.Step( 1 );
			m_toolBar->AddTool( int( main::eID::eTOOL_SHOW_LISTS ), _( "Lists" ), imagesLoader.getBitmapT( eBMP::eMaterials )->Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display lists" ) );
			splashScreen.Step( 1 );
			m_toolBar->AddTool( int( main::eID::eTOOL_PRINT_SCREEN ), _( "Snapshot" ), imagesLoader.getBitmapT( eBMP::ePrintScreen )->Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Take a snapshot" ) );
			m_toolBar->EnableTool( int( main::eID::eTOOL_PRINT_SCREEN ), false );
			splashScreen.Step( 1 );

#if defined( GUICOMMON_RECORDS )

			wxImage imgRecord;
			imgRecord.Create( record_xpm );
			wxImage imgStop;
			imgStop.Create( stop_xpm );
			wxImage imgRecordDis = imgRecord.ConvertToGreyscale();
			wxImage imgStopDis = imgStop.ConvertToGreyscale();
			auto tool = m_toolBar->AddTool( int( main::eID::eTOOL_RECORD ), _( "Record" ), imgRecord.Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Record a video" ) );
			tool->SetDisabledBitmap( imgRecordDis.Scale( 32, 32, wxIMAGE_QUALITY_HIGH ) );
			tool = m_toolBar->AddTool( int( main::eID::eTOOL_STOP ), _( "Stop" ), imgStop.Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Stop recording" ) );
			tool->SetDisabledBitmap( imgStopDis.Scale( 32, 32, wxIMAGE_QUALITY_HIGH ) );
			m_toolBar->EnableTool( int( main::eID::eTOOL_RECORD ), false );
			m_toolBar->EnableTool( int( main::eID::eTOOL_STOP ), false );

#endif

			m_toolBar->Realize();
			m_auiManager.AddPane( m_toolBar, wxAuiPaneInfo().Name( wxT( "MainToolBar" ) ).ToolbarPane().Top().Row( 1 ).Dockable( false ).Gripper( false ) );
		}
		else
		{
			m_fileMenu = new wxMenu;
			m_fileMenu->Append( int( main::eID::eTOOL_LOAD_SCENE ), _( "Open a new scene" ) );
			splashScreen.Step( 1 );
			m_fileMenu->Append( int( main::eID::eTOOL_EXPORT_SCENE ), _( "Export the current scene" ) );
			splashScreen.Step( 1 );
			m_fileMenu->Enable( int( main::eID::eTOOL_EXPORT_SCENE ), false );
			m_fileMenu->AppendSeparator();
			m_fileMenu->Append( wxID_EXIT, _( "Exit" ) );

			m_tabsMenu = new wxMenu;
			m_tabsMenu->Append( int( main::eID::eTOOL_SHOW_LOGS ), _( "Display logs" ) );
			splashScreen.Step( 1 );
			m_tabsMenu->Append( int( main::eID::eTOOL_SHOW_LISTS ), _( "Display lists" ) );
			splashScreen.Step( 1 );

			m_captureMenu = new wxMenu;
			m_captureMenu->Append( int( main::eID::eTOOL_PRINT_SCREEN ), _( "Take a snapshot" ) );
			splashScreen.Step( 1 );
			m_captureMenu->Enable( int( main::eID::eTOOL_PRINT_SCREEN ), false );

#if defined( GUICOMMON_RECORDS )

			m_captureMenu->Append( int( main::eID::eTOOL_RECORD ), _( "Record a video" ) );
			m_captureMenu->Enable( int( main::eID::eTOOL_RECORD ), false );
			m_captureMenu->Append( int( main::eID::eTOOL_STOP ), _( "Stop recording" ) );
			m_captureMenu->Enable( int( main::eID::eTOOL_STOP ), false );

#endif

			m_menuBar = new wxMenuBar;
			m_menuBar->Append( m_fileMenu, _( "File" ) );
			m_menuBar->Append( m_tabsMenu, _( "Tabs" ) );
			m_menuBar->Append( m_captureMenu, _( "Capture" ) );

			SetMenuBar( m_menuBar );
		}
	}

	void MainFrame::doInitialisePerspectives()
	{
		m_currentPerspective = m_auiManager.SavePerspective();

		m_auiManager.GetPane( m_sceneTabsContainer ).Show();
		m_auiManager.GetPane( m_renderPanel ).Show();
		m_debugPerspective = m_auiManager.SavePerspective();

		if constexpr ( main::defaultDebugPerspective )
			m_auiManager.LoadPerspective( m_debugPerspective );
		else
			m_auiManager.LoadPerspective( m_currentPerspective );
	}

	void MainFrame::doLogCallback( c3d::MbString const & log, c3d::LogType logType, bool newLine )
	{
		switch ( logType )
		{
#ifndef NDEBUG
		case c3d::LogType::eTrace:
		case c3d::LogType::eDebug:
			{
				auto lock = c3d::makeUniqueLock( m_debugLog.mutex );
				m_debugLog.queue.emplace_back( GuiCommon::make_wxString( log ), newLine );
			}
			break;
#endif
		case c3d::LogType::eInfo:
			{
				auto lock = c3d::makeUniqueLock( m_messageLog.mutex );
				m_messageLog.queue.emplace_back( GuiCommon::make_wxString( log ), newLine );
			}
			break;

		case c3d::LogType::eWarning:
		case c3d::LogType::eError:
			{
				auto lock = c3d::makeUniqueLock( m_errorLog.mutex );
				m_errorLog.queue.emplace_back( GuiCommon::make_wxString( log ), newLine );
			}
			break;

		default:
			break;
		}
	}

	void MainFrame::doCleanupScene()
	{
		m_fpsTimer->Stop();

		if ( m_mainScene )
		{
			auto engine = wxGetApp().getCastor();

			if ( m_sceneTree )
				m_sceneTree->getList()->unloadScene();
			if ( m_objectsTree )
				m_objectsTree->getList()->unloadScene();
			if ( m_nodesTree )
				m_nodesTree->getList()->unloadScene();
			if ( m_lightsTree )
				m_lightsTree->getList()->unloadScene();
			if ( m_materialsTree )
				m_materialsTree->getList()->unloadScene();
			if ( m_overlaysTree )
				m_overlaysTree->getList()->unloadScene();
			if ( m_guiTree )
				m_guiTree->getList()->unloadScene();

			m_mainCamera = {};
			m_sceneNode = {};

			if ( engine->isThreaded() )
				engine->getRenderLoop().pause();

			m_renderPanel->reset();
			engine->getRenderLoop().renderSyncFrame();
			m_mainScene->cleanup();
			engine->getRenderLoop().renderSyncFrame();

			if ( auto target = m_renderPanel->getRenderWindow().getRenderTarget() )
			{
				engine->getRenderTargetCache().remove( target );
			}

			engine->getRenderLoop().cleanup();
			engine->removeScene( m_mainScene->getName() );
			c3d::Logger::logDebug( cuT( "MainFrame::doCleanupScene - Scene related objects unloaded." ) );

			if ( engine->isThreaded() )
			{
				engine->getRenderLoop().resume();
			}
		}

		m_mainScene = {};
	}

	void MainFrame::doSaveFrame()
	{
		if ( m_renderPanel )
		{
			wxBitmap bitmap;
			auto const & castor = *wxGetApp().getCastor();

			if ( castor.isThreaded() && !m_recorder.isRecording() )
			{
				castor.getRenderLoop().pause();
			}

			auto & window = m_renderPanel->getRenderWindow();
			window.enableSaveFrame();
			castor.getRenderLoop().renderSyncFrame();
			auto buffer = window.getSavedFrame();
			GuiCommon::createBitmapFromBuffer( *buffer
				, false
				, bitmap );

			wxString strWildcard;
			main::addWildcard( strWildcard, _( "All supported image types" ), wxT( "*.bmp;*.gif;*.png;*.jpg" ) );
			main::addWildcard( strWildcard, _( "BITMAP image" ), wxT( "*.bmp" ) );
			main::addWildcard( strWildcard, _( "GIF image" ), wxT( "*.gif" ) );
			main::addWildcard( strWildcard, _( "JPEG image" ), wxT( "*.jpg" ) );
			main::addWildcard( strWildcard, _( "PNG image" ), wxT( "*.png" ) );

			if ( wxFileDialog dialog( this, _( "Please choose an image file name" ), wxEmptyString, wxEmptyString, strWildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
				dialog.ShowModal() == wxID_OK )
			{
				auto image = bitmap.ConvertToImage();
				image.SaveFile( dialog.GetPath() );
			}

			if ( castor.isThreaded() && !m_recorder.isRecording() )
			{
				castor.getRenderLoop().resume();
			}
		}
	}

	bool MainFrame::doStartRecord()
	{
		bool result = true;

#if defined( GUICOMMON_RECORDS )
		auto recordFps = m_recordFps;

		if ( m_renderPanel )
		{
			try
			{

				auto time = wxGetApp().getCastor()->getRenderLoop().getLastFrameTime();
				recordFps = std::min( m_recordFps
					, std::max( 1
						, int( 1000.0f / float( std::chrono::duration_cast< std::chrono::milliseconds >( time ).count() ) ) ) );
				result = m_recorder.startRecord( m_renderPanel->getRenderWindow().getRenderTarget()->getRenderSize()
					, recordFps );
			}
			catch ( std::exception & exc )
			{
				wxMessageBox( wxString( exc.what(), wxMBConvLibc() ) );
				result = false;
			}
		}

		if ( result )
		{
			if ( wxGetApp().getCastor()->isThreaded() )
			{
				m_timer = c3d::makeRawUnique< wxTimer >( this, int( main::eID::eRENDER_TIMER ) );
				wxGetApp().getCastor()->getRenderLoop().pause();
			}

			m_timer->Stop();
			m_timer->Start( 1000 / recordFps );
		}

#endif

		return result;
	}

	void MainFrame::doRecordFrame()
	{
#if defined( GUICOMMON_RECORDS )

		auto const & castor = *wxGetApp().getCastor();
		m_renderPanel->getRenderWindow().enableSaveFrame();
		castor.getRenderLoop().renderSyncFrame();
		auto buffer = m_renderPanel->getRenderWindow().getSavedFrame();

		try
		{
			m_recorder.recordFrame( buffer );
		}
		catch ( std::exception & exc )
		{
			doStopRecord();
			wxMessageBox( wxString( exc.what(), wxMBConvLibc() ) );
		}

#endif
	}

	void MainFrame::doStopRecord()
	{
#if defined( GUICOMMON_RECORDS )

		auto engine = wxGetApp().getCastor();
		m_recorder.stopRecord();

		if ( main::frameToolbar )
		{
			m_toolBar->EnableTool( int( main::eID::eTOOL_STOP ), false );
			m_toolBar->EnableTool( int( main::eID::eTOOL_RECORD ), true );
		}
		else
		{
			m_captureMenu->Enable( int( main::eID::eTOOL_STOP ), false );
			m_captureMenu->Enable( int( main::eID::eTOOL_RECORD ), true );
		}

		if ( m_timer )
		{
			if ( engine->isThreaded() )
			{
				engine->getRenderLoop().resume();
				m_timer->Stop();
				m_timer = {};
			}
			else
			{
				m_timer->Stop();
				m_timer->Start( int( 1000.0f / float( engine->getRenderLoop().getWantedFps() ) ) );
			}
		}

#endif
	}

	void MainFrame::doSceneLoadEnd( c3d::RenderWindowDesc const & window )
	{
		auto target = window.renderTarget;

		if ( !target )
		{
			return;
		}

#if wxCHECK_VERSION( 3, 1, 0 )
		auto size = this->FromDIP( GuiCommon::make_wxSize( target->getDisplaySize() ) );
#else
		auto size = GuiCommon::make_wxSize( target->getDisplaySize() );
#endif

		if ( IsMaximized() )
		{
			Maximize( false );
		}

		SetPosition( wxPoint{} );
		SetClientSize( size );
#if wxCHECK_VERSION( 2, 9, 0 )
		SetMinClientSize( size );
#endif

		m_renderPanel->updateWindow( window );
		m_mainScene = target->getScene();
		auto engine = wxGetApp().getCastor();

		if ( m_mainScene )
		{
			if ( m_sceneTree )
				m_sceneTree->getList()->loadScene( engine, m_renderPanel->getRenderWindow(), m_mainScene );
			if ( m_objectsTree )
				m_objectsTree->getList()->loadSceneObjects( engine, m_mainScene );
			if ( m_nodesTree )
				m_nodesTree->getList()->loadSceneNodes( engine, m_mainScene );
			if ( m_lightsTree )
				m_lightsTree->getList()->loadSceneLights( engine, m_mainScene );
			if ( m_materialsTree )
				m_materialsTree->getList()->loadSceneMaterials( engine, m_mainScene );
			if ( m_overlaysTree )
				m_overlaysTree->getList()->loadSceneOverlays( engine, m_mainScene );
			if ( m_guiTree )
				m_guiTree->getList()->loadSceneGui( engine, m_mainScene );
		}

		if ( main::frameToolbar )
		{
			m_toolBar->EnableTool( int( main::eID::eTOOL_PRINT_SCREEN ), true );
			m_toolBar->EnableTool( int( main::eID::eTOOL_EXPORT_SCENE ), true );
#if defined( GUICOMMON_RECORDS )
			m_toolBar->EnableTool( int( main::eID::eTOOL_RECORD ), true );
#endif
		}
		else
		{
			m_fileMenu->Enable( int( main::eID::eTOOL_EXPORT_SCENE ), true );
			m_captureMenu->Enable( int( main::eID::eTOOL_PRINT_SCREEN ), true );
#if defined( GUICOMMON_RECORDS )
			m_captureMenu->Enable( int( main::eID::eTOOL_RECORD ), true );
#endif
		}

		m_title = wxT( "Castor Viewer - " )
			+ GuiCommon::make_wxString( target->getEngine()->getRenderSystem()->getRendererType() )
			+ wxT( " - " )
			+ m_filePath.getFileName( true );
		SetTitle( m_title );
		m_fpsTimer->Start( 1000 );
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( MainFrame, wxFrame )
		EVT_TIMER( int( main::eID::eRENDER_TIMER ), MainFrame::onRenderTimer )
		EVT_TIMER( int( main::eID::eMSGLOG_TIMER ), MainFrame::onTimer )
		EVT_TIMER( int( main::eID::eERRLOG_TIMER ), MainFrame::onTimer )
		EVT_TIMER( int( main::eID::eFPS_TIMER ), MainFrame::onFpsTimer )
		EVT_THREAD( int( main::eID::eLOAD_END ), MainFrame::onSceneLoadEnd )
		EVT_PAINT( MainFrame::onPaint )
		EVT_CLOSE( MainFrame::onClose )
		EVT_ENTER_WINDOW( MainFrame::onEnterWindow )
		EVT_LEAVE_WINDOW( MainFrame::onLeaveWindow )
		EVT_ERASE_BACKGROUND( MainFrame::onEraseBackground )
		EVT_KEY_UP( MainFrame::onKeyUp )
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void MainFrame::onPaint( wxPaintEvent & event )
	{
		wxPaintDC paintDC( this );
		event.Skip();
	}

	void MainFrame::onRenderTimer( wxTimerEvent & event )
	{
		if ( auto castor = wxGetApp().getCastor();
			!castor->isCleaned() )
		{
			if ( m_renderPanel && m_recorder.isRecording() && m_recorder.updateTime() )
			{
				doRecordFrame();
			}
			else if ( !castor->isThreaded() )
			{
				auto wanted = c3d::Milliseconds{ 1000 / castor->getRenderLoop().getWantedFps() };
				castor->getRenderLoop().renderSyncFrame( wanted );
				auto frame = std::chrono::duration_cast< c3d::Milliseconds >( wxGetApp().getCastor()->getRenderLoop().getAvgFrameTime() );

				if ( frame.count() >= m_timer->GetInterval() )
				{
					m_minCount = 0u;

					if ( m_maxCount++ >= 100 )
					{
						m_maxCount = 0;
						m_timer->Stop();
						m_timer->Start( m_timer->GetInterval() * 2 );
					}
				}
				else if ( frame.count() < m_timer->GetInterval() / 2 )
				{
					m_maxCount = 0u;

					if ( m_minCount++ >= 100 )
					{
						m_minCount = 0;
						m_timer->Stop();
						m_timer->Start( m_timer->GetInterval() / 2 );
					}
				}
			}
		}

		event.Skip();
	}

	void MainFrame::onTimer( wxTimerEvent & event )
	{
		if ( event.GetId() == int( main::eID::eMSGLOG_TIMER ) && m_messageLog.listBox )
			main::updateLog( m_messageLog );
		else if ( event.GetId() == int( main::eID::eERRLOG_TIMER ) && m_errorLog.listBox )
			main::updateLog( m_errorLog );
#ifndef NDEBUG
		else if ( event.GetId() == main::eID::eDBGLOG_TIMER && m_debugLog.listBox )
			main::updateLog( m_debugLog );
#endif

		event.Skip();
	}

	void MainFrame::onFpsTimer( wxTimerEvent & event )
	{
		if ( wxGetApp().getCastor()
			&& wxGetApp().getCastor()->hasRenderLoop() )
		{
			auto time = std::chrono::duration_cast< c3d::Microseconds >( wxGetApp().getCastor()->getRenderLoop().getAvgFrameTime() );

			if ( time.count() )
			{
				SetTitle( wxString::Format( "%s - ~%.2f FPS (%.2f ms)"
					, m_title
					, 1000000.0f / float( time.count() )
					, float( time.count() ) / 1000.0f ) );
			}
		}
		event.Skip();
	}

	void MainFrame::onClose( wxCloseEvent & event )
	{
		c3d::Logger::logInfo( cuT( "Cleaning up MainFrame." ) );
		c3d::Logger::unregisterCallback( this );
		m_auiManager.DetachPane( m_sceneTabsContainer );
		m_auiManager.DetachPane( m_logTabsContainer );
		m_auiManager.DetachPane( m_renderPanel );
#if CV_MainFrameToolbar
		m_auiManager.DetachPane( m_toolBar );
#endif
		m_messageLog.listBox = {};
		m_errorLog.listBox = {};
#ifndef NDEBUG
		m_debugLog.listBox = {};
#endif

		if ( m_renderPanel )
			m_renderPanel->disableWindowResize();

		if ( m_fpsTimer )
		{
			m_fpsTimer->Stop();
			m_fpsTimer = {};
		}

		if ( m_timer )
		{
			m_timer->Stop();
			m_timer = {};
		}

		if ( m_timerMsg )
		{
			m_timerMsg->Stop();
			m_timerMsg = {};
		}

		if ( m_timerErr )
		{
			m_timerErr->Stop();
			m_timerErr = {};
		}

		m_mainScene = {};
		auto castor = wxGetApp().getCastor();
		CU_Require( castor );

		if ( m_renderPanel )
		{
			if ( castor && castor->isThreaded() )
				castor->getRenderLoop().pause();
			m_renderPanel->reset();
			if ( castor && castor->isThreaded() )
				castor->getRenderLoop().resume();
		}

		if ( castor )
			castor->cleanup();

		if ( m_renderPanel )
		{
			m_renderPanel->Close( true );
			m_renderPanel = {};
		}

		DestroyChildren();
		event.Skip();
		c3d::Logger::logInfo( cuT( "MainFrame cleaned up." ) );
	}

	void MainFrame::onEnterWindow( wxMouseEvent & event )
	{
		SetFocus();
		event.Skip();
	}

	void MainFrame::onLeaveWindow( wxMouseEvent & event )
	{
		event.Skip();
	}

	void MainFrame::onEraseBackground( wxEraseEvent & event )
	{
		event.Skip();
	}

	void MainFrame::onKeyUp( wxKeyEvent & event )
	{
		if ( m_renderPanel )
			m_renderPanel->onKeyUp( event );
		else
			event.Skip();
	}

	void MainFrame::onLoadScene( wxCommandEvent & event )
	{
		wxString wildcard = _( "Castor3D scene files" );
		wildcard << wxT( " (*.cscn;*.zip)|*.cscn;*.zip|" );
		wildcard << _( "Castor3D scene file" );
		wildcard << GuiCommon::CSCN_WILDCARD;
		wildcard << _( "Zip archive" );
		wildcard << GuiCommon::ZIP_WILDCARD;
		wildcard << wxT( "|" );

		if ( wxFileDialog fileDialog( this, _( "Open a scene" ), wxEmptyString, wxEmptyString, wildcard );
			fileDialog.ShowModal() == wxID_OK )
		{
			loadScene( fileDialog.GetPath() );
		}

		event.Skip();
	}

	void MainFrame::onExportScene( wxCommandEvent & event )
	{
		c3d::exporter::ExportOptions options;

		if ( GuiCommon::PropertiesDialog dialog{ this
			, _( "Export" )
			, c3d::makeUniqueDerived< GuiCommon::TreeItemProperty, GuiCommon::ExportOptionsTreeItemProperty >( wxGetApp().getImagesLoader(), true, options ) };
			dialog.ShowModal() == wxID_CANCEL )
		{
			return;
		}

		if ( m_mainScene )
		{
			wxString wildcard = _( "Castor3D scene" );
			wildcard += GuiCommon::CSCN_WILDCARD;
			wildcard += wxT( "|" );

			if ( wxFileDialog fileDialog( this, _( "Export the scene" ), wxEmptyString, wxEmptyString, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
				fileDialog.ShowModal() == wxID_OK )
			{
				try
				{
					c3d::Path pathFile( GuiCommon::make_String( fileDialog.GetPath() ) );
					c3d::exporter::CscnSceneExporter exporter{ options };
					auto result = exporter.exportScene( *m_mainScene, pathFile );

					if ( result )
					{
						wxMessageBox( _( "Scene export completed" )
							, wxMessageBoxCaptionStr
							, wxOK | wxCENTRE | wxICON_INFORMATION );
					}
					else
					{
						wxMessageBox( _( "Scene export failed:\nSee CastorViewer.log for more details." )
							, _( "Error" )
							, wxOK | wxCENTRE | wxICON_ERROR );
					}
				}
				catch ( std::exception & exc )
				{
					wxMessageBox( _( "Scene export failed:\n" ) + GuiCommon::make_wxString( exc.what() )
						, _( "Error" )
						, wxOK | wxCENTRE | wxICON_ERROR );
				}
			}
		}
		else
		{
			wxMessageBox( _( "No scene Loaded." )
				, _( "Error" )
				, wxOK | wxCENTRE | wxICON_ERROR );
		}

		event.Skip();
	}

	void MainFrame::onShowLogs( wxCommandEvent & event )
	{
		if ( !m_logTabsContainer->IsShown() )
		{
			m_auiManager.GetPane( m_logTabsContainer ).Show();
		}

		m_auiManager.Update();
		event.Skip();
	}

	void MainFrame::onShowLists( wxCommandEvent & event )
	{
		if ( !m_sceneTabsContainer->IsShown() )
			m_auiManager.GetPane( m_sceneTabsContainer ).Show();
		m_auiManager.Update();
		event.Skip();
	}

	void MainFrame::onPrintScreen( wxCommandEvent & event )
	{
		doSaveFrame();
		event.Skip();
	}

	void MainFrame::onRecord( wxCommandEvent & event )
	{
#if defined( GUICOMMON_RECORDS )

		if ( doStartRecord() )
		{
			if ( main::frameToolbar )
			{
				m_toolBar->EnableTool( int( main::eID::eTOOL_STOP ), true );
				m_toolBar->EnableTool( int( main::eID::eTOOL_RECORD ), false );
			}
			else
			{
				m_captureMenu->Enable( int( main::eID::eTOOL_STOP ), true );
				m_captureMenu->Enable( int( main::eID::eTOOL_RECORD ), false );
			}
		}

#endif
		event.Skip();
	}

	void MainFrame::onStop( wxCommandEvent & event )
	{
		doStopRecord();
		event.Skip();
	}

	void MainFrame::onSceneLoadEnd( wxThreadEvent & event )
	{
		if ( !event.GetEventObject() )
			return;

		auto var = static_cast< wxVariant * >( event.GetEventObject() );
		auto rawTarget = static_cast< c3d::RenderWindowDesc * >( var->GetVoidPtr() );
		delete var;

		if ( rawTarget )
		{
			doSceneLoadEnd( *rawTarget );
			delete rawTarget;
		}

		event.Skip();
	}
}
