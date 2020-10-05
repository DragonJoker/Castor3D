#include "CastorViewer/RenderPanel.hpp"

#include "CastorViewer/MainFrame.hpp"
#include "CastorViewer/CastorViewer.hpp"

#include <wx/display.h>
#include <wx/mstream.h>
#include <wx/renderer.h>

#include <GuiCommon/Aui/AuiDockArt.hpp>
#include <GuiCommon/Aui/AuiTabArt.hpp>
#include <GuiCommon/Aui/AuiToolBarArt.hpp>
#include <GuiCommon/Properties/Math/PropertiesContainer.hpp>
#include <GuiCommon/Properties/Math/PropertiesHolder.hpp>
#include <GuiCommon/Properties/TreeItems/TreeHolder.hpp>
#include <GuiCommon/System/ImagesLoader.hpp>
#include <GuiCommon/System/MaterialsList.hpp>
#include <GuiCommon/System/RendererSelector.hpp>
#include <GuiCommon/System/SplashScreen.hpp>
#include <GuiCommon/System/SceneExporter.hpp>

#include <Castor3D/Cache/SceneCache.hpp>
#include <Castor3D/Cache/WindowCache.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <CastorUtils/Graphics/PixelBufferBase.hpp>
#include <CastorUtils/Miscellaneous/BlockTimer.hpp>

#include <GuiCommon/xpms/castor.xpm>
#include <GuiCommon/xpms/print_screen.xpm>

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
		static const bool isCastor3DThreaded = true;
#if defined( NDEBUG )
		static const int wantedFPS = 1000;
#else
		static const int wantedFPS = 60;
#endif

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
			eID_TAB_SCENE,
			eID_RENDER_TIMER,
			eID_MSGLOG_TIMER,
			eID_ERRLOG_TIMER,
			eID_FPS_TIMER,
		};

		void updateLog( std::vector< std::pair< wxString, bool > > & queue, std::mutex & mutex, wxListBox & log )
		{
			std::vector< std::pair< wxString, bool > > flush;
			{
				std::lock_guard< std::mutex > lock( mutex );
				std::swap( flush, queue );
			}

			if ( !flush.empty() )
			{
				for ( auto & message : flush )
				{
					if ( message.second )
					{
						log.Insert( message.first, 0 );
					}
					else
					{
						log.SetString( 0, message.first );
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
			doInitialisePerspectives();
			result = doInitialise3D();
		}

		Show( result );
		return result;
	}

	void MainFrame::doCleanupScene()
	{
		m_fpsTimer->Stop();
		auto scene = m_mainScene.lock();

		if ( scene )
		{
			m_materialsList->unloadMaterials();
			m_sceneObjectsList->unloadScene();
			m_mainCamera.reset();
			m_sceneNode.reset();
			auto engine = wxGetApp().getCastor();
			engine->getRenderWindowCache().cleanup();
			scene->cleanup();
			engine->getRenderLoop().renderSyncFrame();
			engine->getRenderWindowCache().clear();
			engine->getRenderLoop().cleanup();
			engine->getSceneCache().remove( scene->getName() );
			Logger::logDebug( cuT( "MainFrame::doCleanupScene - Scene related objects unloaded." ) );
			scene.reset();
		}

		m_mainScene.reset();
	}

	void MainFrame::loadScene( wxString const & fileName )
	{
		auto engine = wxGetApp().getCastor();

		if ( m_renderPanel && engine )
		{
			if ( !fileName.empty() )
			{
				m_filePath = Path{ ( wxChar const * )fileName.c_str() };
			}

			if ( !m_filePath.empty() )
			{
				if ( engine->isThreaded() )
				{
					engine->getRenderLoop().pause();
				}

				m_renderPanel->resetRenderWindow();
				doCleanupScene();
				RenderWindowSPtr window = GuiCommon::loadScene( *engine, m_filePath );

				if ( window )
				{
					m_renderPanel->setRenderWindow( window );

					if ( window->isInitialised() )
					{
						m_mainScene = window->getScene();

						if ( window->isFullscreen() )
						{
							ShowFullScreen( true, wxFULLSCREEN_ALL );
						}

						auto size = make_wxSize( window->getRenderTarget()->getSize() );

						if ( !IsMaximized() )
						{
							SetClientSize( size );
							SetPosition( wxPoint{} );
						}
						else
						{
							m_renderPanel->disableWindowResize();
							Maximize( false );
							SetPosition( wxPoint{} );
							SetClientSize( size );
							m_renderPanel->enableWindowResize();
						}

#if wxCHECK_VERSION( 2, 9, 0 )

						SetMinClientSize( size );

#endif
					}
					else
					{
						wxMessageBox( _( "Can't initialise the render window.\nLook into CastorViewer.log for more details" ) );
					}

					if ( isCastor3DThreaded )
					{
						engine->getRenderLoop().beginRendering();
					}

					auto scene = m_mainScene.lock();

					if ( scene )
					{
						m_sceneObjectsList->loadScene( engine, scene );
						m_materialsList->loadMaterials( engine, *scene );
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

				if ( engine->isThreaded() )
				{
					engine->getRenderLoop().resume();
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
		if ( m_sceneObjectsList )
		{
			m_sceneObjectsList->select( geometry, submesh );
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
		m_renderPanel = new RenderPanel( this, wxID_ANY, wxDefaultPosition, wxSize( size.x - m_propertiesWidth, size.y - m_logsHeight ) );
		m_logTabsContainer = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_logTabsContainer->SetArtProvider( new AuiTabArt );
		m_sceneTabsContainer = new wxAuiNotebook( this, eID_TAB_SCENE, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_sceneTabsContainer->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_sceneTabsContainer->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_sceneTabsContainer->SetArtProvider( new AuiTabArt );
		m_propertiesHolder = new PropertiesHolder{ this };
		m_propertiesHolder->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_propertiesHolder->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_scenePropertiesContainer = new PropertiesContainer( true, m_propertiesHolder, wxDefaultPosition, wxDefaultSize );
		m_materialPropertiesContainer = new PropertiesContainer( true, m_propertiesHolder, wxDefaultPosition, wxDefaultSize );
		m_propertiesHolder->setGrid( m_scenePropertiesContainer );

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
		m_auiManager.AddPane( m_propertiesHolder
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.Hide()
				.CloseButton()
				.Name( wxT( "Properties" ) )
				.Caption( _( "Properties" ) )
				.Left()
				.Dock()
				.LeftDockable()
				.RightDockable()
				.Movable()
				.PinButton()
				.MinSize( m_propertiesWidth, size.y / 3 )
				.Layer( 2 )
				.PaneBorder( false ) );

		auto createLog = [this, &size]( wxString const & p_name, wxListBox *& p_log )
		{
			p_log = new wxListBox( m_logTabsContainer, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxBORDER_NONE );
			p_log->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			p_log->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			m_logTabsContainer->AddPage( p_log, p_name, true );
		};

		createLog( _( "Messages" ), m_messageLog );
		createLog( _( "Errors" ), m_errorLog );

		auto holder = new TreeHolder{ m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize };
		m_sceneObjectsList = new SceneObjectsList( m_scenePropertiesContainer, holder, wxDefaultPosition, wxDefaultSize );
		m_sceneObjectsList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_sceneObjectsList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		holder->setTree( m_sceneObjectsList );
		m_sceneTabsContainer->AddPage( holder, _( "Scene" ), true );

		holder = new TreeHolder{ m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize };
		m_materialsList = new MaterialsList( m_materialPropertiesContainer, holder, wxDefaultPosition, wxDefaultSize );
		m_materialsList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_materialsList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		holder->setTree( m_materialsList );
		m_sceneTabsContainer->AddPage( holder, _( "Materials" ), true );
		m_sceneTabsContainer->ChangeSelection( 0u );

		m_auiManager.Update();
	}

	bool MainFrame::doInitialise3D()
	{
		auto engine = wxGetApp().getCastor();
		bool result = true;
		Logger::logInfo( cuT( "Initialising Castor3D" ) );

		try
		{
			if ( !wxGetApp().isUnlimitedFps() )
			{
				engine->initialise( wantedFPS, isCastor3DThreaded );
			}
			else
			{
				engine->initialise( 1000u, isCastor3DThreaded );
			}

			Logger::logInfo( cuT( "Castor3D Initialised" ) );

			if ( !isCastor3DThreaded && !m_timer )
			{
				m_timer = new wxTimer( this, eID_RENDER_TIMER );
				m_timer->Start( 1000 / engine->getRenderLoop().getWantedFps() );
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
		catch ( std::exception & exc )
		{
			wxMessageBox( _( "Problem occured while initialising Castor3D." ) + wxString( wxT( "\n" ) ) + wxString( exc.what(), wxMBConvLibc() ), _( "Exception" ), wxOK | wxCENTRE | wxICON_ERROR );
			result = false;
		}
		catch ( ... )
		{
			wxMessageBox( _( "Problem occured while initialising Castor3D.\nLook at CastorViewer.log for more details" ), _( "Exception" ), wxOK | wxCENTRE | wxICON_ERROR );
			result = false;
		}

		return result;
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
		m_toolBar->AddTool( eID_TOOL_LOAD_SCENE, _( "Load Scene" ), wxImage( *ImagesLoader::getBitmap( eBMP_SCENES ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Open a new scene" ) );
		splashScreen.Step( 1 );
		m_toolBar->AddTool( eID_TOOL_EXPORT_SCENE, _( "Export Scene" ), wxImage( *ImagesLoader::getBitmap( eBMP_EXPORT ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Export the current scene" ) );
		m_toolBar->EnableTool( eID_TOOL_EXPORT_SCENE, false );
		splashScreen.Step( 1 );
		m_toolBar->AddSeparator();
		m_toolBar->AddTool( eID_TOOL_SHOW_LOGS, _( "Logs" ), wxImage( *ImagesLoader::getBitmap( eBMP_LOGS ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display logs" ) );
		splashScreen.Step( 1 );
		m_toolBar->AddTool( eID_TOOL_SHOW_LISTS, _( "Lists" ), wxImage( *ImagesLoader::getBitmap( eBMP_MATERIALS ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display lists" ) );
		splashScreen.Step( 1 );
		m_toolBar->AddTool( eID_TOOL_PRINT_SCREEN, _( "Snapshot" ), wxImage( *ImagesLoader::getBitmap( eBMP_PRINTSCREEN ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Take a snapshot" ) );
		m_toolBar->EnableTool( eID_TOOL_PRINT_SCREEN, false );
		splashScreen.Step( 1 );

#	if defined( GUICOMMON_RECORDS )

		wxImage imgRecord;
		imgRecord.Create( record_xpm );
		wxImage imgStop;
		imgStop.Create( stop_xpm );
		wxImage imgRecordDis = imgRecord.ConvertToGreyscale();
		wxImage imgStopDis = imgStop.ConvertToGreyscale();
		auto tool = m_toolBar->AddTool( eID_TOOL_RECORD, _( "Record" ), imgRecord.Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Record a video" ) );
		tool->SetDisabledBitmap( imgRecordDis.Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ) );
		tool = m_toolBar->AddTool( eID_TOOL_STOP, _( "Stop" ), imgStop.Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Stop recording" ) );
		tool->SetDisabledBitmap( imgStopDis.Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ) );
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
		m_auiManager.GetPane( m_propertiesHolder ).Show();
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
		case castor::LogType::eDebug:
		case castor::LogType::eInfo:
		{
			std::lock_guard< std::mutex > lock( m_msgLogListMtx );
			m_msgLogList.emplace_back( log, newLine );
		}
		break;

		case castor::LogType::eWarning:
		case castor::LogType::eError:
		{
			std::lock_guard< std::mutex > lock( m_errLogListMtx );
			m_errLogList.emplace_back( log, newLine );
		}
		break;

		default:
			break;
		}
	}

	void MainFrame::doSaveFrame()
	{
		if ( m_renderPanel && m_renderPanel->getRenderWindow() )
		{
			wxBitmap bitmap;
			auto & castor = *wxGetApp().getCastor();

			if ( castor.isThreaded() && !m_recorder.IsRecording() )
			{
				castor.getRenderLoop().pause();
			}

			m_renderPanel->getRenderWindow()->enableSaveFrame();
			castor.getRenderLoop().renderSyncFrame();
			auto buffer = m_renderPanel->getRenderWindow()->getSavedFrame();
			CreateBitmapFromBuffer( buffer
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
				result = m_recorder.StartRecord( m_renderPanel->getRenderWindow()->getRenderTarget()->getSize()
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
		m_renderPanel->getRenderWindow()->enableSaveFrame();
		castor.getRenderLoop().renderSyncFrame();
		auto buffer = m_renderPanel->getRenderWindow()->getSavedFrame();

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
			if ( isCastor3DThreaded )
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

	BEGIN_EVENT_TABLE( MainFrame, wxFrame )
		EVT_TIMER( eID_RENDER_TIMER, MainFrame::onRenderTimer )
		EVT_TIMER( eID_MSGLOG_TIMER, MainFrame::onTimer )
		EVT_TIMER( eID_ERRLOG_TIMER, MainFrame::onTimer )
		EVT_TIMER( eID_FPS_TIMER, MainFrame::onFpsTimer )
		EVT_PAINT( MainFrame::onPaint )
		EVT_INIT_DIALOG( MainFrame::onInit )
		EVT_CLOSE( MainFrame::onClose )
		EVT_ENTER_WINDOW( MainFrame::onEnterWindow )
		EVT_LEAVE_WINDOW( MainFrame::onLeaveWindow )
		EVT_ERASE_BACKGROUND( MainFrame::onEraseBackground )
		EVT_KEY_UP( MainFrame::onKeyUp )
		EVT_AUINOTEBOOK_PAGE_CHANGING( eID_TAB_SCENE, MainFrame::onSceneTabChanging )
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
		if ( event.GetId() == eID_MSGLOG_TIMER && m_messageLog )
		{
			updateLog( m_msgLogList, m_msgLogListMtx, *m_messageLog );
		}
		else if ( event.GetId() == eID_ERRLOG_TIMER && m_errorLog )
		{
			updateLog( m_errLogList, m_errLogListMtx, *m_errorLog );
		}

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
					, 1000000.0f / time.count()
					, time.count() / 1000.0f ) );
			}
		}
	}

	void MainFrame::onInit( wxInitDialogEvent & event )
	{
	}

	void MainFrame::onClose( wxCloseEvent & event )
	{
		Logger::unregisterCallback( this );
		m_auiManager.DetachPane( m_sceneTabsContainer );
		m_auiManager.DetachPane( m_propertiesHolder );
		m_auiManager.DetachPane( m_logTabsContainer );
		m_auiManager.DetachPane( m_renderPanel );
#if CV_MainFrameToolbar
		m_auiManager.DetachPane( m_toolBar );
#endif
		m_messageLog = nullptr;
		m_errorLog = nullptr;

		if ( m_renderPanel )
		{
			m_renderPanel->disableWindowResize();
		}

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

		m_mainScene.reset();
		auto castor = wxGetApp().getCastor();

		if ( castor )
		{
			if ( m_renderPanel )
			{
				if ( castor->isThreaded() )
				{
					castor->getRenderLoop().pause();
				}

				m_renderPanel->resetRenderWindow();

				if ( castor->isThreaded() )
				{
					castor->getRenderLoop().resume();
				}
			}

			castor->cleanup();
		}
		else if ( m_renderPanel )
		{
			m_renderPanel->resetRenderWindow();
		}

		if ( m_renderPanel )
		{
			m_renderPanel->Close( true );
			m_renderPanel = nullptr;
		}

		DestroyChildren();
		event.Skip();
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

	void MainFrame::onSceneTabChanging( wxAuiNotebookEvent & event )
	{
		auto index = event.GetSelection();

		if ( index == 0 )
		{
			m_propertiesHolder->setGrid( m_scenePropertiesContainer );
		}
		else
		{
			m_propertiesHolder->setGrid( m_materialPropertiesContainer );
		}

		event.Skip();
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
			loadScene( ( wxChar const * )fileDialog.GetPath().c_str() );
		}

		event.Skip();
	}

	void MainFrame::onExportScene( wxCommandEvent & event )
	{
		SceneSPtr scene = m_mainScene.lock();

		if ( scene )
		{
			wxString wildcard = _( "Castor3D scene" );
			wildcard += CSCN_WILDCARD;
			wildcard += _( "Wavefront OBJ" );
			wildcard += objWildcard;
			wildcard += wxT( "|" );
			wxFileDialog fileDialog( this, _( "Export the scene" ), wxEmptyString, wxEmptyString, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

			if ( fileDialog.ShowModal() == wxID_OK )
			{
				try
				{
					Path pathFile( ( wxChar const * )fileDialog.GetPath().c_str() );

					if ( pathFile.getExtension() == cuT( "obj" ) )
					{
						ObjSceneExporter exporter;
						exporter.exportScene( *scene, pathFile );
					}
					else if ( pathFile.getExtension() == cuT( "cscn" ) )
					{
						CscnSceneExporter exporter;
						exporter.exportScene( *scene, pathFile );
					}
				}
				catch ( std::exception & exc )
				{
					wxMessageBox( _( "Scene export failed:" ) + make_wxString( exc.what() )
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
			m_auiManager.GetPane( m_propertiesHolder ).Show();
		}
		else
		{
			m_auiManager.GetPane( m_sceneTabsContainer ).Hide();
			m_auiManager.GetPane( m_propertiesHolder ).Hide();
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
}
