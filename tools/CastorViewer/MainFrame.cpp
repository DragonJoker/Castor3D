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
#include <GuiCommon/System/MaterialsList.hpp>
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
#include <Castor3D/Scene/Scene.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Miscellaneous/BlockTimer.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#include <wx/display.h>
#include <wx/mstream.h>
#include <wx/renderer.h>
#pragma warning( pop )

#include <GuiCommon/xpms/castor.xpm>

#if defined( GUICOMMON_RECORDS )
#	include <GuiCommon/xpms/record.xpm>
#	include <GuiCommon/xpms/stop.xpm>
#endif

using namespace castor3d;
using namespace castor;
using namespace GuiCommon;

#define CV_DefaultDebugPerspective 0

namespace CastorViewer
{
	namespace
	{
		static const int recordFPS = 30;
		static const wxString objWildcard = wxT( " (*.obj)|*.obj|" );

		enum eID
		{
			eID_TOOL_EXIT,
			eID_TOOL_LOAD_SCENE,
			eID_TOOL_EXPORT_SCENE,
			eID_TOOL_MATERIALS,
			eID_TOOL_SHOW_LOGS,
			eID_TOOL_SHOW_LISTS,
			eID_TOOL_PRINT_SCREEN,
			eID_TOOL_RECORD,
			eID_TOOL_STOP,
			eID_PANE_RENDER,
			eID_PANE_LISTS,
			eID_PANE_LOGS,
			eID_RENDER_TIMER,
			eID_MSGLOG_TIMER,
			eID_ERRLOG_TIMER,
#ifndef NDEBUG
			eID_DBGLOG_TIMER,
#endif
			eID_FPS_TIMER,
			eID_LOAD_END,
		};

		void updateLog( LogContainer & log )
		{
			std::vector< std::pair< wxString, bool > > flush;
			{
				std::lock_guard< std::mutex > lock( log.mutex );
				std::swap( flush, log.queue );
			}

			if ( !flush.empty() )
			{
				for ( auto & message : flush )
				{
					if ( message.second )
					{
						log.listBox->Insert( message.first, 0 );
					}
					else
					{
						log.listBox->SetString( 0, message.first );
					}
				}
			}
		}
	}

	MainFrame::MainFrame( wxString const & title )
		: wxFrame( nullptr, wxID_ANY, title, wxPoint{}, wxSize{ 800, 700 } )
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
		, m_recordFps{ recordFPS }
	{
	}

	MainFrame::~MainFrame()
	{
		m_auiManager.UnInit();
	}

	bool MainFrame::initialise( SplashScreen & splashScreen )
	{
		Logger::registerCallback( [this]( String const & logText, LogType logType, bool newLine )
			{
				doLogCallback( logText, logType, newLine );
			}, this );
		bool result = doInitialiseImages();

		if ( result )
		{
			doPopulateStatusBar();
			doPopulateToolBar( splashScreen );
			wxIcon icon = wxIcon( castor_xpm );
			SetIcon( icon );
			doInitialiseGUI();
			doInitialiseTimers();
			doInitialisePerspectives();
		}

		Show( result );
		return result;
	}

	void MainFrame::loadScene( wxString const & fileName )
	{
		auto engine = wxGetApp().getCastor();

		if ( m_renderPanel && engine )
		{
			if ( !fileName.empty() )
			{
				m_filePath = Path{ make_String( fileName ) };
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
						, m_filePath
						, &window.getProgressBar()
						, this
						, eID_LOAD_END );
				}	
				else
				{
					auto target = GuiCommon::loadScene( *engine
						, m_filePath
						, &window.getProgressBar() );
					doSceneLoadEnd( target );
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
			m_auiManager.LoadPerspective( m_fullScreenPerspective );
#if !CV_MainFrameToolbar
			m_menuBar->Hide();
#endif
		}
		else
		{
			m_auiManager.LoadPerspective( m_currentPerspective );
#if !CV_MainFrameToolbar
			m_menuBar->Show();
#endif
		}
	}

	void MainFrame::select( castor3d::GeometrySPtr geometry, castor3d::SubmeshSPtr submesh )
	{
		if ( m_sceneObjects && m_sceneObjects->getList() )
		{
			m_sceneObjects->getList()->select( geometry, submesh );
		}
	}

	void MainFrame::doInitialiseTimers()
	{
		auto engine = wxGetApp().getCastor();

		if ( !engine->isThreaded() && !m_timer )
		{
			m_timer = new wxTimer( this, eID_RENDER_TIMER );
			m_timer->Start( 1000 / int( engine->getRenderLoop().getWantedFps() ) );
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

		if ( !m_fpsTimer )
		{
			m_fpsTimer = new wxTimer( this, eID_FPS_TIMER );
		}
	}

	void MainFrame::doInitialiseGUI()
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		SetClientSize( 800 + m_propertiesWidth, 600 + m_logsHeight );
		wxSize size = GetClientSize();

#if wxCHECK_VERSION( 2, 9, 0 )

		SetMinClientSize( size );

#endif

		m_auiManager.SetArtProvider( new AuiDockArt );
		m_renderPanel = new RenderPanel( this, eID_PANE_RENDER, wxDefaultPosition, wxSize( size.x - m_propertiesWidth, size.y - m_logsHeight ) );
		m_logTabsContainer = new wxAuiNotebook( this, eID_PANE_LOGS, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_logTabsContainer->SetArtProvider( new AuiTabArt );
		m_sceneTabsContainer = new wxAuiNotebook( this, eID_PANE_LISTS, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_sceneTabsContainer->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_sceneTabsContainer->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_sceneTabsContainer->SetArtProvider( new AuiTabArt );

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
				.CaptionVisible( false )
				.Hide()
				.CloseButton()
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
				.PaneBorder( false ) );
		m_auiManager.AddPane( m_sceneTabsContainer
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.Hide()
				.CloseButton()
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
				.PaneBorder( false ) );

		auto createLog = [this]( wxString const & name
			, LogContainer & log )
		{
			log.listBox = new wxListBox( m_logTabsContainer, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxBORDER_NONE );
			log.listBox->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			log.listBox->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			m_logTabsContainer->AddPage( log.listBox, name, true );
		};

		createLog( _( "Messages" ), m_messageLog );
		createLog( _( "Errors" ), m_errorLog );
#ifndef NDEBUG
		createLog( _( "Debug" ), m_debugLog );
#endif
		m_logTabsContainer->ChangeSelection( 0u );

		m_sceneObjects = new GuiCommon::TreeListContainerT< GuiCommon::SceneObjectsList >{ m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize };
		m_sceneTabsContainer->AddPage( m_sceneObjects, _( "Scene" ), true );

		m_materials = new GuiCommon::TreeListContainerT< GuiCommon::MaterialsList >{ m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize };
		m_sceneTabsContainer->AddPage( m_materials, _( "Materials" ), true );
		m_sceneTabsContainer->ChangeSelection( 0u );

		m_auiManager.Update();
	}

	bool MainFrame::doInitialiseImages()
	{
		return true;
	}

	void MainFrame::doPopulateStatusBar()
	{
		wxStatusBar * statusBar = CreateStatusBar();
		statusBar->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		statusBar->SetForegroundColour( INACTIVE_TEXT_COLOUR );
	}

	void MainFrame::doPopulateToolBar( SplashScreen & splashScreen )
	{
		splashScreen.Step( _( "Loading toolbar" ), 1 );

#if CV_MainFrameToolbar

		m_toolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_HORIZONTAL );
		m_toolBar->SetArtProvider( new AuiToolBarArt );
		m_toolBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_toolBar->SetToolBitmapSize( wxSize( 32, 32 ) );
		m_toolBar->AddTool( eID_TOOL_LOAD_SCENE, _( "Load Scene" ), ImagesLoader::getBitmap( eBMP_SCENES )->Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Open a new scene" ) );
		splashScreen.Step( 1 );
		m_toolBar->AddTool( eID_TOOL_EXPORT_SCENE, _( "Export Scene" ), ImagesLoader::getBitmap( eBMP_EXPORT )->Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Export the current scene" ) );
		m_toolBar->EnableTool( eID_TOOL_EXPORT_SCENE, false );
		splashScreen.Step( 1 );
		m_toolBar->AddSeparator();
		m_toolBar->AddTool( eID_TOOL_SHOW_LOGS, _( "Logs" ), ImagesLoader::getBitmap( eBMP_LOGS )->Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display logs" ) );
		splashScreen.Step( 1 );
		m_toolBar->AddTool( eID_TOOL_SHOW_LISTS, _( "Lists" ), ImagesLoader::getBitmap( eBMP_MATERIALS )->Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display lists" ) );
		splashScreen.Step( 1 );
		m_toolBar->AddTool( eID_TOOL_PRINT_SCREEN, _( "Snapshot" ), ImagesLoader::getBitmap( eBMP_PRINTSCREEN )->Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Take a snapshot" ) );
		m_toolBar->EnableTool( eID_TOOL_PRINT_SCREEN, false );
		splashScreen.Step( 1 );

#	if defined( GUICOMMON_RECORDS )

		wxImage imgRecord;
		imgRecord.Create( record_xpm );
		wxImage imgStop;
		imgStop.Create( stop_xpm );
		wxImage imgRecordDis = imgRecord.ConvertToGreyscale();
		wxImage imgStopDis = imgStop.ConvertToGreyscale();
		auto tool = m_toolBar->AddTool( eID_TOOL_RECORD, _( "Record" ), imgRecord.Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Record a video" ) );
		tool->SetDisabledBitmap( imgRecordDis.Scale( 32, 32, wxIMAGE_QUALITY_HIGH ) );
		tool = m_toolBar->AddTool( eID_TOOL_STOP, _( "Stop" ), imgStop.Scale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Stop recording" ) );
		tool->SetDisabledBitmap( imgStopDis.Scale( 32, 32, wxIMAGE_QUALITY_HIGH ) );
		m_toolBar->EnableTool( eID_TOOL_RECORD, false );
		m_toolBar->EnableTool( eID_TOOL_STOP, false );

#	endif

		m_toolBar->Realize();
		m_auiManager.AddPane( m_toolBar, wxAuiPaneInfo().Name( wxT( "MainToolBar" ) ).ToolbarPane().Top().Row( 1 ).Dockable( false ).Gripper( false ) );

#else

		m_fileMenu = new wxMenu;
		m_fileMenu->Append( eID_TOOL_LOAD_SCENE, _( "Open a new scene" ) );
		splashScreen.Step( 1 );
		m_fileMenu->Append( eID_TOOL_EXPORT_SCENE, _( "Export the current scene" ) );
		splashScreen.Step( 1 );
		m_fileMenu->Enable( eID_TOOL_EXPORT_SCENE, false );
		m_fileMenu->AppendSeparator();
		m_fileMenu->Append( wxID_EXIT, _( "Exit" ) );

		m_tabsMenu = new wxMenu;
		m_tabsMenu->Append( eID_TOOL_SHOW_LOGS, _( "Display logs" ) );
		splashScreen.Step( 1 );
		m_tabsMenu->Append( eID_TOOL_SHOW_LISTS, _( "Display lists" ) );
		splashScreen.Step( 1 );

		m_captureMenu = new wxMenu;
		m_captureMenu->Append( eID_TOOL_PRINT_SCREEN, _( "Take a snapshot" ) );
		splashScreen.Step( 1 );
		m_captureMenu->Enable( eID_TOOL_PRINT_SCREEN, false );

#	if defined( GUICOMMON_RECORDS )

		m_captureMenu->Append( eID_TOOL_RECORD, _( "Record a video" ) );
		m_captureMenu->Enable( eID_TOOL_RECORD, false );
		m_captureMenu->Append( eID_TOOL_STOP, _( "Stop recording" ) );
		m_captureMenu->Enable( eID_TOOL_STOP, false );

#	endif

		m_menuBar = new wxMenuBar;
		m_menuBar->Append( m_fileMenu, _( "File" ) );
		m_menuBar->Append( m_tabsMenu, _( "Tabs" ) );
		m_menuBar->Append( m_captureMenu, _( "Capture" ) );

		SetMenuBar( m_menuBar );

#endif
	}

	void MainFrame::doInitialisePerspectives()
	{
		wxAuiPaneInfoArray panes = m_auiManager.GetAllPanes();
		std::vector< bool > visibilities;
		m_currentPerspective = m_auiManager.SavePerspective();

		for ( size_t i = 0; i < panes.size(); ++i )
		{
			panes[i].Hide();
		}

#if CV_MainFrameToolbar
		m_auiManager.GetPane( m_toolBar ).Hide();
#endif
		m_fullScreenPerspective = m_auiManager.SavePerspective();

		m_auiManager.GetPane( m_sceneTabsContainer ).Show();
		m_auiManager.GetPane( m_renderPanel ).Show();
		m_debugPerspective = m_auiManager.SavePerspective();

#if CV_DefaultDebugPerspective
		m_auiManager.LoadPerspective( m_debugPerspective );
#else
		m_auiManager.LoadPerspective( m_currentPerspective );
#endif
	}

	void MainFrame::doLogCallback( String const & log, LogType logType, bool newLine )
	{
		switch ( logType )
		{
#ifndef NDEBUG
		case castor::LogType::eTrace:
		case castor::LogType::eDebug:
			{
				std::lock_guard< std::mutex > lock( m_debugLog.mutex );
				m_debugLog.queue.emplace_back( make_wxString( log ), newLine );
			}
			break;
#endif
		case castor::LogType::eInfo:
			{
				std::lock_guard< std::mutex > lock( m_messageLog.mutex );
				m_messageLog.queue.emplace_back( make_wxString( log ), newLine );
			}
			break;

		case castor::LogType::eWarning:
		case castor::LogType::eError:
			{
				std::lock_guard< std::mutex > lock( m_errorLog.mutex );
				m_errorLog.queue.emplace_back( make_wxString( log ), newLine );
			}
			break;

		default:
			break;
		}
	}

	void MainFrame::doCleanupScene()
	{
		m_fpsTimer->Stop();
		auto scene = m_mainScene.lock();

		if ( scene )
		{
			auto engine = wxGetApp().getCastor();

			m_materials->getList()->unloadMaterials();
			m_sceneObjects->getList()->unloadScene();
			m_mainCamera.reset();
			m_sceneNode.reset();

			if ( engine->isThreaded() )
			{
				engine->getRenderLoop().pause();
			}

			m_renderPanel->reset();
			scene->cleanup();
			engine->getRenderLoop().renderSyncFrame();

			auto target = m_renderPanel->getRenderWindow().getRenderTarget();

			if ( target )
			{
				engine->getRenderTargetCache().remove( target );
			}

			engine->getRenderLoop().cleanup();
			engine->getSceneCache().remove( scene->getName() );
			Logger::logDebug( cuT( "MainFrame::doCleanupScene - Scene related objects unloaded." ) );
			scene.reset();

			if ( engine->isThreaded() )
			{
				engine->getRenderLoop().resume();
			}
		}

		m_mainScene.reset();
	}

	void MainFrame::doSaveFrame()
	{
		if ( m_renderPanel )
		{
			wxBitmap bitmap;
			auto & castor = *wxGetApp().getCastor();

			if ( castor.isThreaded() && !m_recorder.IsRecording() )
			{
				castor.getRenderLoop().pause();
			}

			auto & window = m_renderPanel->getRenderWindow();
			window.enableSaveFrame();
			castor.getRenderLoop().renderSyncFrame();
			auto buffer = window.getSavedFrame();
			createBitmapFromBuffer( buffer
				, false
				, bitmap );

			wxString strWildcard = _( "All supported files" );
			strWildcard += wxT( " (*.bmp;*.gif;*.png;*.jpg)|*.bmp;*.gif;*.png;*.jpg|" );
			strWildcard += _( "BITMAP files" );
			strWildcard += wxT( " (*.bmp)|*.bmp|" );
			strWildcard += _( "GIF files" );
			strWildcard += wxT( " (*.gif)|*.gif|" );
			strWildcard += _( "JPEG files" );
			strWildcard += wxT( " (*.jpg)|*.jpg|" );
			strWildcard += _( "PNG files" );
			strWildcard += wxT( " (*.png)|*.png" );
			wxFileDialog dialog( this, _( "Please choose an image file name" ), wxEmptyString, wxEmptyString, strWildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

			if ( dialog.ShowModal() == wxID_OK )
			{
				auto image = bitmap.ConvertToImage();
				image.SaveFile( dialog.GetPath() );
			}

			if ( castor.isThreaded() && !m_recorder.IsRecording() )
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
						, int( 1000.0f / std::chrono::duration_cast< std::chrono::milliseconds >( time ).count() ) ) );
				result = m_recorder.StartRecord( m_renderPanel->getRenderWindow().getRenderTarget()->getSize()
					, recordFps );
			}
			catch ( std::exception & p_exc )
			{
				wxMessageBox( wxString( p_exc.what(), wxMBConvLibc() ) );
				result = false;
			}
		}

		if ( result )
		{
			if ( isCastor3DThreaded )
			{
				m_timer = new wxTimer( this, eID_RENDER_TIMER );
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

		auto & castor = *wxGetApp().getCastor();
		m_renderPanel->getRenderWindow().enableSaveFrame();
		castor.getRenderLoop().renderSyncFrame();
		auto buffer = m_renderPanel->getRenderWindow().getSavedFrame();

		try
		{
			m_recorder.RecordFrame( buffer );
		}
		catch ( std::exception & p_exc )
		{
			doStopRecord();
			wxMessageBox( wxString( p_exc.what(), wxMBConvLibc() ) );
		}

#endif
	}

	void MainFrame::doStopRecord()
	{
#if defined( GUICOMMON_RECORDS )

		auto engine = wxGetApp().getCastor();
		m_recorder.StopRecord();

#	if CV_MainFrameToolbar
		m_toolBar->EnableTool( eID_TOOL_STOP, false );
		m_toolBar->EnableTool( eID_TOOL_RECORD, true );
#	else
		m_captureMenu->Enable( eID_TOOL_STOP, false );
		m_captureMenu->Enable( eID_TOOL_RECORD, true );
#	endif

		if ( m_timer )
		{
			if ( engine->isThreaded() )
			{
				engine->getRenderLoop().resume();
				m_timer->Stop();
				delete m_timer;
				m_timer = nullptr;
			}
			else
			{
				m_timer->Stop();
				m_timer->Start( 1000 / engine->getRenderLoop().getWantedFps() );
			}
		}

#endif
	}

	void MainFrame::doSceneLoadEnd( RenderTargetSPtr target )
	{
		auto size = make_wxSize( target->getSize() );

		if ( !IsMaximized() )
		{
			SetClientSize( size );
			SetPosition( wxPoint{} );
		}
		else
		{
			Maximize( false );
			SetPosition( wxPoint{} );
			SetClientSize( size );
		}

#if wxCHECK_VERSION( 2, 9, 0 )
		SetMinClientSize( size );
#endif

		m_renderPanel->setTarget( target );
		m_mainScene = target->getScene();
		auto scene = m_mainScene.lock();
		auto engine = wxGetApp().getCastor();

		if ( scene )
		{
			m_sceneObjects->getList()->loadScene( engine, m_renderPanel->getRenderWindow(), scene );
			m_materials->getList()->loadMaterials( engine, *scene );
		}

#if CV_MainFrameToolbar
		m_toolBar->EnableTool( eID_TOOL_PRINT_SCREEN, true );
		m_toolBar->EnableTool( eID_TOOL_EXPORT_SCENE, true );
#else
		m_fileMenu->Enable( eID_TOOL_EXPORT_SCENE, true );
		m_captureMenu->Enable( eID_TOOL_PRINT_SCREEN, true );
#endif

#if defined( GUICOMMON_RECORDS )
#	if CV_MainFrameToolbar
		m_toolBar->EnableTool( eID_TOOL_RECORD, true );
#	else
		m_captureMenu->Enable( eID_TOOL_RECORD, true );
#	endif
#endif
		m_title = wxT( "Castor Viewer - " )
			+ make_wxString( scene->getEngine()->getRenderSystem()->getRendererType() )
			+ wxT( " - " )
			+ m_filePath.getFileName( true );
		SetTitle( m_title );
		m_fpsTimer->Start( 1000 );
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
	BEGIN_EVENT_TABLE( MainFrame, wxFrame )
		EVT_TIMER( eID_RENDER_TIMER, MainFrame::onRenderTimer )
		EVT_TIMER( eID_MSGLOG_TIMER, MainFrame::onTimer )
		EVT_TIMER( eID_ERRLOG_TIMER, MainFrame::onTimer )
		EVT_TIMER( eID_FPS_TIMER, MainFrame::onFpsTimer )
		EVT_THREAD( eID_LOAD_END, MainFrame::onSceneLoadEnd )
		EVT_PAINT( MainFrame::onPaint )
		EVT_INIT_DIALOG( MainFrame::onInit )
		EVT_CLOSE( MainFrame::onClose )
		EVT_ENTER_WINDOW( MainFrame::onEnterWindow )
		EVT_LEAVE_WINDOW( MainFrame::onLeaveWindow )
		EVT_ERASE_BACKGROUND( MainFrame::onEraseBackground )
		EVT_KEY_UP( MainFrame::onKeyUp )
#if CV_MainFrameToolbar
		EVT_TOOL( eID_TOOL_LOAD_SCENE, MainFrame::onLoadScene )
		EVT_TOOL( eID_TOOL_EXPORT_SCENE, MainFrame::onExportScene )
		EVT_TOOL( eID_TOOL_SHOW_LOGS, MainFrame::onShowLogs )
		EVT_TOOL( eID_TOOL_SHOW_LISTS, MainFrame::onShowLists )
		EVT_TOOL( eID_TOOL_PRINT_SCREEN, MainFrame::onPrintScreen )
		EVT_TOOL( eID_TOOL_RECORD, MainFrame::onRecord )
		EVT_TOOL( eID_TOOL_STOP, MainFrame::onStop )
#else
		EVT_MENU( eID_TOOL_LOAD_SCENE, MainFrame::onLoadScene )
		EVT_MENU( eID_TOOL_EXPORT_SCENE, MainFrame::onExportScene )
		EVT_MENU( eID_TOOL_SHOW_LOGS, MainFrame::onShowLogs )
		EVT_MENU( eID_TOOL_SHOW_LISTS, MainFrame::onShowLists )
		EVT_MENU( eID_TOOL_PRINT_SCREEN, MainFrame::onPrintScreen )
		EVT_MENU( eID_TOOL_RECORD, MainFrame::onRecord )
		EVT_MENU( eID_TOOL_STOP, MainFrame::onStop )
#endif
	END_EVENT_TABLE()
#pragma GCC diagnostic pop

	void MainFrame::onPaint( wxPaintEvent & event )
	{
		wxPaintDC paintDC( this );
		event.Skip();
	}

	void MainFrame::onRenderTimer( wxTimerEvent & event )
	{
		auto castor = wxGetApp().getCastor();

		if ( castor )
		{
			if ( !castor->isCleaned() )
			{
				if ( m_renderPanel && m_recorder.IsRecording() && m_recorder.UpdateTime() )
				{
					doRecordFrame();
				}
				else if ( !castor->isThreaded() )
				{
					castor->getRenderLoop().renderSyncFrame();
				}
			}
		}
	}

	void MainFrame::onTimer( wxTimerEvent & event )
	{
		if ( event.GetId() == eID_MSGLOG_TIMER && m_messageLog.listBox )
		{
			updateLog( m_messageLog );
		}
		else if ( event.GetId() == eID_ERRLOG_TIMER && m_errorLog.listBox )
		{
			updateLog( m_errorLog );
		}
#ifndef NDEBUG
		else if ( event.GetId() == eID_DBGLOG_TIMER && m_debugLog.listBox )
		{
			updateLog( m_debugLog );
		}
#endif

		event.Skip();
	}

	void MainFrame::onFpsTimer( wxTimerEvent & event )
	{
		if ( wxGetApp().getCastor()
			&& wxGetApp().getCastor()->hasRenderLoop() )
		{
			auto time = wxGetApp().getCastor()->getRenderLoop().getLastFrameTime();

			if ( time.count() )
			{
				SetTitle( wxString::Format( "%s - ~%.2f FPS (%.2f ms)"
					, m_title
					, 1000000.0f / float( time.count() )
					, float( time.count() ) / 1000.0f ) );
			}
		}
	}

	void MainFrame::onInit( wxInitDialogEvent & event )
	{
	}

	void MainFrame::onClose( wxCloseEvent & event )
	{
		castor::Logger::logInfo( cuT( "Cleaning up MainFrame." ) );
		Logger::unregisterCallback( this );
		m_auiManager.DetachPane( m_sceneTabsContainer );
		m_auiManager.DetachPane( m_logTabsContainer );
		m_auiManager.DetachPane( m_renderPanel );
#if CV_MainFrameToolbar
		m_auiManager.DetachPane( m_toolBar );
#endif
		m_messageLog.listBox = nullptr;
		m_errorLog.listBox = nullptr;
#ifndef NDEBUG
		m_debugLog.listBox = nullptr;
#endif

		if ( m_renderPanel )
		{
			m_renderPanel->disableWindowResize();
		}

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

		m_mainScene.reset();
		auto castor = wxGetApp().getCastor();
		CU_Require( castor );

		if ( m_renderPanel )
		{
			if ( castor->isThreaded() )
			{
				castor->getRenderLoop().pause();
			}

			m_renderPanel->reset();

			if ( castor->isThreaded() )
			{
				castor->getRenderLoop().resume();
			}
		}

		castor->cleanup();

		if ( m_renderPanel )
		{
			m_renderPanel->Close( true );
			m_renderPanel = nullptr;
		}

		DestroyChildren();
		event.Skip();
		castor::Logger::logInfo( cuT( "MainFrame cleaned up." ) );
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
		{
			m_renderPanel->onKeyUp( event );
		}
		else
		{
			event.Skip();
		}
	}

	void MainFrame::onLoadScene( wxCommandEvent & event )
	{
		wxString wildcard = _( "Castor3D scene files" );
		wildcard << wxT( " (*.cscn;*.zip)|*.cscn;*.zip|" );
		wildcard << _( "Castor3D scene file" );
		wildcard << CSCN_WILDCARD;
		wildcard << _( "Zip archive" );
		wildcard << ZIP_WILDCARD;
		wildcard << wxT( "|" );
		wxFileDialog fileDialog( this, _( "Open a scene" ), wxEmptyString, wxEmptyString, wildcard );

		if ( fileDialog.ShowModal() == wxID_OK )
		{
			loadScene( fileDialog.GetPath() );
		}

		event.Skip();
	}

	void MainFrame::onExportScene( wxCommandEvent & event )
	{
		exporter::ExportOptions options;
		PropertiesDialog dialog{ this
			, _( "Export" )
			, std::make_unique< GuiCommon::ExportOptionsTreeItemProperty >( true, options ) };

		if ( dialog.ShowModal() == wxID_CANCEL )
		{
			return;
		}
		
		SceneSPtr scene = m_mainScene.lock();

		if ( scene )
		{
			wxString wildcard = _( "Castor3D scene" );
			wildcard += CSCN_WILDCARD;
			wildcard += wxT( "|" );
			wxFileDialog fileDialog( this, _( "Export the scene" ), wxEmptyString, wxEmptyString, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

			if ( fileDialog.ShowModal() == wxID_OK )
			{
				try
				{
					Path pathFile( make_String( fileDialog.GetPath() ) );
					exporter::CscnSceneExporter exporter{ options };
					auto result = exporter.exportScene( *scene, pathFile );

					if ( result )
					{
						wxMessageBox( _( "Scene export completed" )
							, wxMessageBoxCaptionStr
							, wxOK | wxCENTRE | wxICON_INFORMATION );
					}
					else
					{
						throw std::runtime_error{ "See CastorViewer.log for more details." };
					}
				}
				catch ( std::exception & exc )
				{
					wxMessageBox( _( "Scene export failed:\n" ) + make_wxString( exc.what() )
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
		else
		{
			m_auiManager.GetPane( m_logTabsContainer ).Hide();
		}

		m_auiManager.Update();
		event.Skip();
	}

	void MainFrame::onShowLists( wxCommandEvent & event )
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
#	if CV_MainFrameToolbar
			m_toolBar->EnableTool( eID_TOOL_STOP, true );
			m_toolBar->EnableTool( eID_TOOL_RECORD, false );
#	else
			m_captureMenu->Enable( eID_TOOL_STOP, true );
			m_captureMenu->Enable( eID_TOOL_RECORD, false );
#	endif
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
		{
			return;
		}

		auto var = static_cast< wxVariant * >( event.GetEventObject() );
		auto rawTarget = static_cast< RenderTarget * >( var->GetVoidPtr() );
		delete var;

		if ( rawTarget )
		{
			doSceneLoadEnd( rawTarget->shared_from_this() );
		}
	}
}
