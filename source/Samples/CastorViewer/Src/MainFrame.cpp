#include "RenderPanel.hpp"

#include "MainFrame.hpp"
#include "CastorViewer.hpp"
#include "PropertiesContainer.hpp"
#include "PropertiesHolder.hpp"
#include "TreeHolder.hpp"

#include <wx/display.h>
#include <wx/mstream.h>
#include <wx/renderer.h>

#include <Graphics/PixelBufferBase.hpp>

#include <Material/Material.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderLoop.hpp>
#include <Render/RenderWindow.hpp>

#include <ImagesLoader.hpp>
#include <MaterialsList.hpp>
#include <RendererSelector.hpp>
#include <SplashScreen.hpp>
#include <AuiDockArt.hpp>
#include <AuiTabArt.hpp>
#include <AuiToolBarArt.hpp>
#include <SceneExporter.hpp>

#include <xpms/castor.xpm>
#include <xpms/print_screen.xpm>

#if defined( GUICOMMON_RECORDS )
#	include <xpms/record.xpm>
#	include <xpms/stop.xpm>
#endif

using namespace castor3d;
using namespace castor;
using namespace GuiCommon;


namespace CastorViewer
{
	namespace
	{
		static const bool CASTOR3D_THREADED = true;
#if defined( NDEBUG )
		static const int CASTOR_WANTED_FPS = 1000;
#else
		static const int CASTOR_WANTED_FPS = 60;
#endif

		static const int CASTOR_RECORD_FPS = 30;
		static const wxString OBJ_WILDCARD = wxT( " (*.obj)|*.obj|" );

		typedef enum eID
		{
			eID_TOOL_EXIT,
			eID_TOOL_LOAD_SCENE,
			eID_TOOL_EXPORT_SCENE,
			eID_TOOL_MATERIALS,
			eID_TOOL_SHOW_LOGS,
			eID_TOOL_SHOW_PROPERTIES,
			eID_TOOL_SHOW_LISTS,
			eID_TOOL_PRINT_SCREEN,
			eID_TOOL_RECORD,
			eID_TOOL_STOP,
			eID_RENDER_TIMER,
			eID_MSGLOG_TIMER,
			eID_ERRLOG_TIMER,
		}	eID;

		void IndentPressedBitmap( wxRect * rect, int buttonState )
		{
			if ( buttonState == wxAUI_BUTTON_STATE_PRESSED )
			{
				rect->x++;
				rect->y++;
			}
		}

		void dooUpdateLog( std::vector< std::pair< wxString, bool > > & queue, std::mutex & mutex, wxListBox & log )
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

	MainFrame::MainFrame( SplashScreen * splashScreen, wxString const & title )
		: wxFrame( nullptr, wxID_ANY, title, wxDefaultPosition, wxSize( 800, 700 ) )
		, m_renderPanel( nullptr )
		, m_timer( nullptr )
		, m_timerMsg( nullptr )
		, m_timerErr( nullptr )
		, m_logTabsContainer( nullptr )
		, m_messageLog( nullptr )
		, m_errorLog( nullptr )
		, m_logsHeight( 100 )
		, m_propertiesWidth( 240 )
		, m_sceneObjectsList( nullptr )
		, m_materialsList( nullptr )
		, m_propertiesContainer( nullptr )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
		, m_toolBar( nullptr )
		, m_splashScreen( splashScreen )
		, m_recorder()
		, m_recordFps( CASTOR_RECORD_FPS )
	{
	}

	MainFrame::~MainFrame()
	{
		m_auiManager.UnInit();
	}

	bool MainFrame::initialise()
	{
		Logger::registerCallback( std::bind( &MainFrame::doLogCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ), this );
		bool result = doInitialiseImages();

		if ( result )
		{
			doPopulateStatusBar();
			doPopulateToolBar();
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
		m_mainScene.reset();
		auto scene = m_mainScene.lock();

		if ( scene )
		{
			m_materialsList->UnloadMaterials();
			m_sceneObjectsList->unloadScene();
			m_mainCamera.reset();
			m_sceneNode.reset();
			wxGetApp().getCastor()->getRenderWindowCache().cleanup();
			scene->cleanup();
			wxGetApp().getCastor()->getRenderLoop().cleanup();
			wxGetApp().getCastor()->getSceneCache().remove( scene->getName() );
			Logger::logDebug( cuT( "MainFrame::doCleanupScene - Scene related objects unloaded." ) );
			scene.reset();
		}

	}

	void MainFrame::loadScene( wxString const & fileName )
	{
		if ( m_renderPanel && wxGetApp().getCastor() )
		{
			if ( !fileName.empty() )
			{
				m_filePath = Path{ ( wxChar const * )fileName.c_str() };
			}

			if ( !m_filePath.empty() )
			{
				if ( wxGetApp().getCastor()->isThreaded() )
				{
					wxGetApp().getCastor()->getRenderLoop().pause();
				}

				Logger::logDebug( cuT( "MainFrame::loadScene - " ) + m_filePath );
				doCleanupScene();

				m_renderPanel->setRenderWindow( nullptr );
				RenderWindowSPtr window = GuiCommon::loadScene( *wxGetApp().getCastor(), m_filePath, wxGetApp().getCastor()->getRenderLoop().getWantedFps(), wxGetApp().getCastor()->isThreaded() );

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
						}
						else
						{
							m_renderPanel->disableWindowResize();
							Maximize( false );
							SetClientSize( size );
							m_renderPanel->enableWindowResize();
							Maximize();
						}

#if wxCHECK_VERSION( 2, 9, 0 )

						SetMinClientSize( size );

#endif

						Logger::logInfo( cuT( "Scene file read" ) );
					}
					else
					{
						wxMessageBox( _( "Can't initialise the render window.\nLook into CastorViewer.log for more details" ) );
					}

					if ( CASTOR3D_THREADED )
					{
						wxGetApp().getCastor()->getRenderLoop().beginRendering();
					}

					auto scene = m_mainScene.lock();

					if ( scene )
					{
						m_sceneObjectsList->loadScene( wxGetApp().getCastor(), scene );
						m_materialsList->LoadMaterials( wxGetApp().getCastor(), *scene );
					}

					m_toolBar->EnableTool( eID_TOOL_PRINT_SCREEN, true );

#if defined( GUICOMMON_RECORDS )

					m_toolBar->EnableTool( eID_TOOL_RECORD, true );

#endif
					SetTitle( wxT( "CastorViewer - " ) + m_filePath.getFileName( true ) );
				}

				if ( wxGetApp().getCastor()->isThreaded() )
				{
					wxGetApp().getCastor()->getRenderLoop().resume();
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
		}
		else
		{
			m_auiManager.LoadPerspective( m_currentPerspective );
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
		m_sceneTabsContainer = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH );
		m_sceneTabsContainer->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_sceneTabsContainer->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_sceneTabsContainer->SetArtProvider( new AuiTabArt );
		m_propertiesHolder = new PropertiesHolder{ this };
		m_propertiesHolder->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_propertiesHolder->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_propertiesContainer = new PropertiesContainer( true, m_propertiesHolder, wxDefaultPosition, wxDefaultSize );
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
		m_propertiesHolder->setGrid( m_propertiesContainer );

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
		m_sceneObjectsList = new SceneObjectsList( m_propertiesContainer, holder, wxDefaultPosition, wxDefaultSize );
		m_sceneObjectsList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_sceneObjectsList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		holder->setTree( m_sceneObjectsList );
		m_sceneTabsContainer->AddPage( holder, _( "Scene" ), true );

		holder = new TreeHolder{ m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize };
		m_materialsList = new MaterialsList( m_propertiesContainer, holder, wxDefaultPosition, wxDefaultSize );
		m_materialsList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_materialsList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		holder->setTree( m_materialsList );
		m_sceneTabsContainer->AddPage( holder, _( "Materials" ), true );

		m_auiManager.Update();
	}

	bool MainFrame::doInitialise3D()
	{
		bool result = true;
		Logger::logInfo( cuT( "Initialising Castor3D" ) );

		try
		{
			wxGetApp().getCastor()->initialise( CASTOR_WANTED_FPS, CASTOR3D_THREADED );
			Logger::logInfo( cuT( "Castor3D Initialised" ) );

			if ( !CASTOR3D_THREADED && !m_timer )
			{
				m_timer = new wxTimer( this, eID_RENDER_TIMER );
				m_timer->Start( 1000 / wxGetApp().getCastor()->getRenderLoop().getWantedFps() );
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

	void MainFrame::doPopulateToolBar()
	{
		m_splashScreen->Step( _( "Loading toolbar" ), 1 );
		m_toolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_HORIZONTAL );
		m_toolBar->SetArtProvider( new AuiToolBarArt );
		m_toolBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_toolBar->SetToolBitmapSize( wxSize( 32, 32 ) );
		m_toolBar->AddTool( eID_TOOL_LOAD_SCENE, _( "Load Scene" ), wxImage( *ImagesLoader::getBitmap( eBMP_SCENES ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "open a new scene" ) );
		m_splashScreen->Step( 1 );
		m_toolBar->AddTool( eID_TOOL_EXPORT_SCENE, _( "Export Scene" ), wxImage( *ImagesLoader::getBitmap( eBMP_EXPORT ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Export the current scene" ) );
		m_splashScreen->Step( 1 );
		m_toolBar->AddSeparator();
		m_toolBar->AddTool( eID_TOOL_SHOW_LOGS, _( "Logs" ), wxImage( *ImagesLoader::getBitmap( eBMP_LOGS ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display logs" ) );
		m_splashScreen->Step( 1 );
		m_toolBar->AddTool( eID_TOOL_SHOW_LISTS, _( "Lists" ), wxImage( *ImagesLoader::getBitmap( eBMP_MATERIALS ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display lists" ) );
		m_splashScreen->Step( 1 );
		m_toolBar->AddTool( eID_TOOL_SHOW_PROPERTIES, _( "Properties" ), wxImage( *ImagesLoader::getBitmap( eBMP_PROPERTIES ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display properties" ) );
		m_splashScreen->Step( 1 );

		wxMemoryInputStream isPrint( print_screen_png, sizeof( print_screen_png ) );
		wxImage imgPrint( isPrint, wxBITMAP_TYPE_PNG );
		m_toolBar->AddTool( eID_TOOL_PRINT_SCREEN, _( "Snapshot" ), imgPrint.Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Take a snapshot" ) );
		m_toolBar->EnableTool( eID_TOOL_PRINT_SCREEN, false );

#if defined( GUICOMMON_RECORDS )

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

#endif

		m_toolBar->Realize();
		m_auiManager.AddPane( m_toolBar, wxAuiPaneInfo().Name( wxT( "MainToolBar" ) ).ToolbarPane().Top().Row( 1 ).Dockable( false ).Gripper( false ) );
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

		m_auiManager.GetPane( m_toolBar ).Hide();
		m_fullScreenPerspective = m_auiManager.SavePerspective();
		m_auiManager.LoadPerspective( m_currentPerspective );
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
				m_renderPanel->getRenderWindow()->enableSaveFrame();
				castor.getRenderLoop().renderSyncFrame();
				auto buffer = m_renderPanel->getRenderWindow()->getSavedFrame();
				castor.getRenderLoop().resume();
				Size size = buffer->dimensions();
				CreateBitmapFromBuffer( buffer
					, false
					, bitmap );
			}
			else
			{
				m_renderPanel->getRenderWindow()->enableSaveFrame();
				castor.getRenderLoop().renderSyncFrame();
				auto buffer = m_renderPanel->getRenderWindow()->getSavedFrame();
				Size size = buffer->dimensions();
				CreateBitmapFromBuffer( buffer
					, false
					, bitmap );
			}

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
		}
	}

	bool MainFrame::doStartRecord()
	{
		bool result = false;

		if ( m_renderPanel )
		{
			try
			{
				result = m_recorder.StartRecord( m_renderPanel->getRenderWindow()->getRenderTarget()->getSize(), m_recordFps );
			}
			catch ( std::exception & p_exc )
			{
				wxMessageBox( wxString( p_exc.what(), wxMBConvLibc() ) );
				result = false;
			}
		}

#if defined( GUICOMMON_RECORDS )

		if ( result )
		{
			if ( CASTOR3D_THREADED )
			{
				m_timer = new wxTimer( this, eID_RENDER_TIMER );
				wxGetApp().getCastor()->getRenderLoop().pause();
			}

			m_timer->Stop();
			m_timer->Start( 1000 / m_recordFps );
		}

#endif
		return result;
	}

	void MainFrame::doRecordFrame()
	{
#if defined( GUICOMMON_RECORDS )

		auto & castor = *wxGetApp().getCastor();
		m_renderPanel->getRenderWindow()->saveFrame();
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
		m_recorder.StopRecord();

#if defined( GUICOMMON_RECORDS )

		m_toolBar->EnableTool( eID_TOOL_STOP, false );
		m_toolBar->EnableTool( eID_TOOL_RECORD, true );

		if ( m_timer )
		{
			if ( CASTOR3D_THREADED )
			{
				wxGetApp().getCastor()->getRenderLoop().resume();
				m_timer->Stop();
				delete m_timer;
				m_timer = nullptr;
			}
			else
			{
				m_timer->Stop();
				m_timer->Start( 1000 / wxGetApp().getCastor()->getRenderLoop().getWantedFps() );
			}
		}

#endif
	}

	BEGIN_EVENT_TABLE( MainFrame, wxFrame )
		EVT_TIMER( eID_RENDER_TIMER, MainFrame::OnRenderTimer )
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
		EVT_TOOL( eID_TOOL_PRINT_SCREEN, MainFrame::OnPrintScreen )
		EVT_TOOL( eID_TOOL_RECORD, MainFrame::OnRecord )
		EVT_TOOL( eID_TOOL_STOP, MainFrame::OnStop )
	END_EVENT_TABLE()

	void MainFrame::OnPaint( wxPaintEvent & event )
	{
		wxPaintDC paintDC( this );
		event.Skip();
	}

	void MainFrame::OnRenderTimer( wxTimerEvent & event )
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

	void MainFrame::OnTimer( wxTimerEvent & event )
	{
		if ( event.GetId() == eID_MSGLOG_TIMER && m_messageLog )
		{
			dooUpdateLog( m_msgLogList, m_msgLogListMtx, *m_messageLog );
		}
		else if ( event.GetId() == eID_ERRLOG_TIMER && m_errorLog )
		{
			dooUpdateLog( m_errLogList, m_errLogListMtx, *m_errorLog );
		}

		event.Skip();
	}

	void MainFrame::OnInit( wxInitDialogEvent & event )
	{
	}

	void MainFrame::OnClose( wxCloseEvent & event )
	{
		Logger::unregisterCallback( this );
		m_auiManager.DetachPane( m_sceneTabsContainer );
		m_auiManager.DetachPane( m_propertiesHolder );
		m_auiManager.DetachPane( m_logTabsContainer );
		m_auiManager.DetachPane( m_renderPanel );
		m_auiManager.DetachPane( m_toolBar );
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

		if ( m_renderPanel )
		{
			if ( wxGetApp().getCastor()->isThreaded() )
			{
				wxGetApp().getCastor()->getRenderLoop().pause();
			}

			m_renderPanel->setRenderWindow( nullptr );

			if ( wxGetApp().getCastor()->isThreaded() )
			{
				wxGetApp().getCastor()->getRenderLoop().resume();
			}
		}

		if ( wxGetApp().getCastor() )
		{
			wxGetApp().getCastor()->cleanup();
		}

		if ( m_renderPanel )
		{
			m_renderPanel->UnFocus();
			m_renderPanel->Close( true );
			m_renderPanel = nullptr;
		}

		DestroyChildren();
		event.Skip();
	}

	void MainFrame::OnEnterWindow( wxMouseEvent & event )
	{
		SetFocus();
		event.Skip();
	}

	void MainFrame::OnLeaveWindow( wxMouseEvent & event )
	{
		event.Skip();
	}

	void MainFrame::OnEraseBackground( wxEraseEvent & event )
	{
		event.Skip();
	}

	void MainFrame::OnLoadScene( wxCommandEvent & event )
	{
		wxString wildcard = _( "Castor3D scene files" );
		wildcard << wxT( " (*.cscn;*.zip)|*.cscn;*.zip|" );
		wildcard << _( "Castor3D scene file" );
		wildcard << CSCN_WILDCARD;
		wildcard << _( "Zip archive" );
		wildcard << ZIP_WILDCARD;
		wildcard << wxT( "|" );
		wxFileDialog fileDialog( this, _( "open a scene" ), wxEmptyString, wxEmptyString, wildcard );

		if ( fileDialog.ShowModal() == wxID_OK )
		{
			loadScene( ( wxChar const * )fileDialog.GetPath().c_str() );
		}

		event.Skip();
	}

	void MainFrame::OnExportScene( wxCommandEvent & event )
	{
		wxString wildcard = _( "Castor3D scene" );
		wildcard += CSCN_WILDCARD;
		wildcard += _( "Wavefront OBJ" );
		wildcard += OBJ_WILDCARD;
		wildcard += wxT( "|" );
		wxFileDialog fileDialog( this, _( "Export the scene" ), wxEmptyString, wxEmptyString, wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
		SceneSPtr scene = m_mainScene.lock();

		if ( scene )
		{
			if ( fileDialog.ShowModal() == wxID_OK )
			{
				try
				{
					Path pathFile( ( wxChar const * )fileDialog.GetPath().c_str() );

					if ( pathFile.getExtension() == cuT( "obj" ) )
					{
						ObjSceneExporter exporter;
						exporter.ExportScene( *m_mainScene.lock(), pathFile );
					}
					else if ( pathFile.getExtension() == cuT( "cscn" ) )
					{
						CscnSceneExporter exporter;
						exporter.ExportScene( *m_mainScene.lock(), pathFile );
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

	void MainFrame::OnShowLogs( wxCommandEvent & event )
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

	void MainFrame::OnShowProperties( wxCommandEvent & event )
	{
		if ( !m_propertiesHolder->IsShown() )
		{
			m_auiManager.GetPane( m_propertiesHolder ).Show();
		}
		else
		{
			m_auiManager.GetPane( m_propertiesHolder ).Hide();
		}

		m_auiManager.Update();
		event.Skip();
	}

	void MainFrame::OnShowLists( wxCommandEvent & event )
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

	void MainFrame::OnPrintScreen( wxCommandEvent & event )
	{
		doSaveFrame();
		event.Skip();
	}

	void MainFrame::OnRecord( wxCommandEvent & event )
	{
		if ( doStartRecord() )
		{
#if defined( GUICOMMON_RECORDS )

			m_toolBar->EnableTool( eID_TOOL_STOP, true );
			m_toolBar->EnableTool( eID_TOOL_RECORD, false );

#endif
		}

		event.Skip();
	}

	void MainFrame::OnStop( wxCommandEvent & event )
	{
		doStopRecord();
		event.Skip();
	}
}
