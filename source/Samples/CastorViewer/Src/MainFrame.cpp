#include "RenderPanel.hpp"

#include "MainFrame.hpp"
#include "CastorViewer.hpp"
#include "PropertiesHolder.hpp"

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

using namespace Castor3D;
using namespace Castor;
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

		void IndentPressedBitmap( wxRect * rect, int button_state )
		{
			if ( button_state == wxAUI_BUTTON_STATE_PRESSED )
			{
				rect->x++;
				rect->y++;
			}
		}

		void DooUpdateLog( std::vector< std::pair< wxString, bool > > & p_queue, std::mutex & p_mutex, wxListBox & p_log )
		{
			std::vector< std::pair< wxString, bool > > l_flush;
			{
				std::lock_guard< std::mutex > l_lock( p_mutex );
				std::swap( l_flush, p_queue );
			}

			if ( !l_flush.empty() )
			{
				for ( auto & l_message : l_flush )
				{
					if ( l_message.second )
					{
						p_log.Insert( l_message.first, 0 );
					}
					else
					{
						p_log.SetString( 0, l_message.first );
					}
				}
			}
		}
	}

	MainFrame::MainFrame( SplashScreen * p_splashScreen, wxString const & p_strTitle )
		: wxFrame( nullptr, wxID_ANY, p_strTitle, wxDefaultPosition, wxSize( 800, 700 ) )
		, m_pRenderPanel( nullptr )
		, m_timer( nullptr )
		, m_timerMsg( nullptr )
		, m_timerErr( nullptr )
		, m_logTabsContainer( nullptr )
		, m_messageLog( nullptr )
		, m_errorLog( nullptr )
		, m_iLogsHeight( 100 )
		, m_iPropertiesWidth( 240 )
		, m_sceneObjectsList( nullptr )
		, m_materialsList( nullptr )
		, m_propertiesContainer( nullptr )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
		, m_toolBar( nullptr )
		, m_splashScreen( p_splashScreen )
		, m_recorder()
		, m_recordFps( CASTOR_RECORD_FPS )
	{
	}

	MainFrame::~MainFrame()
	{
		m_auiManager.UnInit();
	}

	bool MainFrame::Initialise()
	{
		Logger::RegisterCallback( std::bind( &MainFrame::DoLogCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ), this );
		bool l_return = DoInitialiseImages();

		if ( l_return )
		{
			DoPopulateStatusBar();
			DoPopulateToolBar();
			wxIcon l_icon = wxIcon( castor_xpm );
			SetIcon( l_icon );
			DoInitialiseGUI();
			DoInitialisePerspectives();
			l_return = DoInitialise3D();
		}

		Show( l_return );
		return l_return;
	}

	void MainFrame::DoCleanupScene()
	{
		auto l_scene = m_pMainScene.lock();
		m_pMainScene.reset();

		if ( l_scene )
		{
			m_materialsList->UnloadMaterials();
			m_sceneObjectsList->UnloadScene();
			m_pMainCamera.reset();
			m_sceneNode.reset();
			l_scene->Cleanup();
			wxGetApp().GetCastor()->GetRenderLoop().Cleanup();
			wxGetApp().GetCastor()->GetSceneCache().Remove( l_scene->GetName() );
			Logger::LogDebug( cuT( "MainFrame::DoCleanupScene - Scene related objects unloaded." ) );
			l_scene.reset();
		}

	}

	void MainFrame::LoadScene( wxString const & p_strFileName )
	{
		if ( m_pRenderPanel && wxGetApp().GetCastor() )
		{
			if ( !p_strFileName.empty() )
			{
				m_strFilePath = Path{ ( wxChar const * )p_strFileName.c_str() };
			}

			if ( !m_strFilePath.empty() )
			{
				if ( wxGetApp().GetCastor()->IsThreaded() )
				{
					wxGetApp().GetCastor()->GetRenderLoop().Pause();
				}

				Logger::LogDebug( cuT( "MainFrame::LoadScene - " ) + m_strFilePath );
				DoCleanupScene();

				m_pRenderPanel->SetRenderWindow( nullptr );
				RenderWindowSPtr l_window = GuiCommon::LoadScene( *wxGetApp().GetCastor(), m_strFilePath, wxGetApp().GetCastor()->GetRenderLoop().GetWantedFps(), wxGetApp().GetCastor()->IsThreaded() );

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

						if ( !IsMaximized() )
						{
							SetClientSize( l_window->GetSize().width(), l_window->GetSize().height() );
						}
						else
						{
							Maximize( false );
							SetClientSize( l_window->GetSize().width(), l_window->GetSize().height() );
							Maximize();
						}

						Logger::LogInfo( cuT( "Scene file read" ) );
					}
					else
					{
						wxMessageBox( _( "Can't initialise the render window.\nLook into CastorViewer.log for more details" ) );
					}

					if ( CASTOR3D_THREADED )
					{
						wxGetApp().GetCastor()->GetRenderLoop().StartRendering();
					}

					auto l_scene = m_pMainScene.lock();

					if ( l_scene )
					{
						m_sceneObjectsList->LoadScene( wxGetApp().GetCastor(), l_scene );
						m_materialsList->LoadMaterials( wxGetApp().GetCastor(), *l_scene );
					}

#if wxCHECK_VERSION( 2, 9, 0 )

					wxSize l_size = GetClientSize();
					SetMinClientSize( l_size );

#endif

					m_toolBar->EnableTool( eID_TOOL_PRINT_SCREEN, true );

#if defined( GUICOMMON_RECORDS )

					m_toolBar->EnableTool( eID_TOOL_RECORD, true );

#endif
				}

				if ( wxGetApp().GetCastor()->IsThreaded() )
				{
					wxGetApp().GetCastor()->GetRenderLoop().Resume();
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
			p_log = new wxListBox( m_logTabsContainer, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxBORDER_NONE );
			p_log->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			p_log->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			m_logTabsContainer->AddPage( p_log, p_name, true );
		};

		l_logCreator( _( "Messages" ), m_messageLog );
		l_logCreator( _( "Errors" ), m_errorLog );

		m_sceneObjectsList = new SceneObjectsList( m_propertiesContainer, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize );
		m_sceneObjectsList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_sceneObjectsList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_sceneTabsContainer->AddPage( m_sceneObjectsList, _( "Scene" ), true );

		m_materialsList = new MaterialsList( m_propertiesContainer, m_sceneTabsContainer, wxDefaultPosition, wxDefaultSize );
		m_materialsList->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_materialsList->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_sceneTabsContainer->AddPage( m_materialsList, _( "Materials" ), true );

		m_auiManager.Update();
	}

	bool MainFrame::DoInitialise3D()
	{
		bool l_return = true;
		Logger::LogInfo( cuT( "Initialising Castor3D" ) );

		try
		{
			wxGetApp().GetCastor()->Initialise( CASTOR_WANTED_FPS, CASTOR3D_THREADED );
			Logger::LogInfo( cuT( "Castor3D Initialised" ) );

			if ( !CASTOR3D_THREADED && !m_timer )
			{
				m_timer = new wxTimer( this, eID_RENDER_TIMER );
				m_timer->Start( 1000 / wxGetApp().GetCastor()->GetRenderLoop().GetWantedFps() );
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
			l_return = false;
		}
		catch ( ... )
		{
			wxMessageBox( _( "Problem occured while initialising Castor3D.\nLook at CastorViewer.log for more details" ), _( "Exception" ), wxOK | wxCENTRE | wxICON_ERROR );
			l_return = false;
		}

		return l_return;
	}

	bool MainFrame::DoInitialiseImages()
	{
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
		m_splashScreen->Step( _( "Loading toolbar" ), 1 );
		m_toolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_HORIZONTAL );
		m_toolBar->SetArtProvider( new AuiToolBarArt );
		m_toolBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_toolBar->SetToolBitmapSize( wxSize( 32, 32 ) );
		m_toolBar->AddTool( eID_TOOL_LOAD_SCENE, _( "Load Scene" ), wxImage( *ImagesLoader::GetBitmap( eBMP_SCENES ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Open a new scene" ) );
		m_splashScreen->Step( 1 );
		m_toolBar->AddTool( eID_TOOL_EXPORT_SCENE, _( "Export Scene" ), wxImage( *ImagesLoader::GetBitmap( eBMP_EXPORT ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Export the current scene" ) );
		m_splashScreen->Step( 1 );
		m_toolBar->AddSeparator();
		m_toolBar->AddTool( eID_TOOL_SHOW_LOGS, _( "Logs" ), wxImage( *ImagesLoader::GetBitmap( eBMP_LOGS ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display logs" ) );
		m_splashScreen->Step( 1 );
		m_toolBar->AddTool( eID_TOOL_SHOW_LISTS, _( "Lists" ), wxImage( *ImagesLoader::GetBitmap( eBMP_MATERIALS ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display lists" ) );
		m_splashScreen->Step( 1 );
		m_toolBar->AddTool( eID_TOOL_SHOW_PROPERTIES, _( "Properties" ), wxImage( *ImagesLoader::GetBitmap( eBMP_PROPERTIES ) ).Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Display properties" ) );
		m_splashScreen->Step( 1 );

		wxMemoryInputStream l_isPrint( print_screen_png, sizeof( print_screen_png ) );
		wxImage l_imgPrint( l_isPrint, wxBITMAP_TYPE_PNG );
		m_toolBar->AddTool( eID_TOOL_PRINT_SCREEN, _( "Snapshot" ), l_imgPrint.Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Take a snapshot" ) );
		m_toolBar->EnableTool( eID_TOOL_PRINT_SCREEN, false );

#if defined( GUICOMMON_RECORDS )

		wxImage l_imgRecord;
		l_imgRecord.Create( record_xpm );
		wxImage l_imgStop;
		l_imgStop.Create( stop_xpm );
		wxImage l_imgRecordDis = l_imgRecord.ConvertToGreyscale();
		wxImage l_imgStopDis = l_imgStop.ConvertToGreyscale();
		auto l_tool = m_toolBar->AddTool( eID_TOOL_RECORD, _( "Record" ), l_imgRecord.Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Record a video" ) );
		l_tool->SetDisabledBitmap( l_imgRecordDis.Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ) );
		l_tool = m_toolBar->AddTool( eID_TOOL_STOP, _( "Stop" ), l_imgStop.Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ), _( "Stop recording" ) );
		l_tool->SetDisabledBitmap( l_imgStopDis.Rescale( 32, 32, wxIMAGE_QUALITY_HIGH ) );
		m_toolBar->EnableTool( eID_TOOL_RECORD, false );
		m_toolBar->EnableTool( eID_TOOL_STOP, false );

#endif

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

	void MainFrame::DoLogCallback( String const & p_strLog, LogType p_eLogType, bool p_newLine )
	{
		switch ( p_eLogType )
		{
		case Castor::LogType::eDebug:
		case Castor::LogType::eInfo:
		{
			std::lock_guard< std::mutex > l_lock( m_msgLogListMtx );
			m_msgLogList.emplace_back( p_strLog, p_newLine );
		}
		break;

		case Castor::LogType::eWarning:
		case Castor::LogType::eError:
		{
			std::lock_guard< std::mutex > l_lock( m_errLogListMtx );
			m_errLogList.emplace_back( p_strLog, p_newLine );
		}
		break;

		default:
			break;
		}
	}

	void MainFrame::DoSaveFrame()
	{
		if ( m_pRenderPanel && m_pRenderPanel->GetRenderWindow() )
		{
			wxBitmap l_bitmap;
			auto & l_castor = *wxGetApp().GetCastor();

			if ( l_castor.IsThreaded() && !m_recorder.IsRecording() )
			{
				l_castor.GetRenderLoop().Pause();
				m_pRenderPanel->GetRenderWindow()->SaveFrame();
				l_castor.GetRenderLoop().RenderSyncFrame();
				auto l_buffer = m_pRenderPanel->GetRenderWindow()->GetSavedFrame();
				l_castor.GetRenderLoop().Resume();
				Size l_size = l_buffer->dimensions();
				CreateBitmapFromBuffer( l_buffer, true, l_bitmap );
			}
			else
			{
				m_pRenderPanel->GetRenderWindow()->SaveFrame();
				l_castor.GetRenderLoop().RenderSyncFrame();
				auto l_buffer = m_pRenderPanel->GetRenderWindow()->GetSavedFrame();
				Size l_size = l_buffer->dimensions();
				CreateBitmapFromBuffer( l_buffer, true, l_bitmap );
			}

			wxString l_strWildcard = _( "All supported files" );
			l_strWildcard += wxT( " (*.bmp;*.gif;*.png;*.jpg)|*.bmp;*.gif;*.png;*.jpg|" );
			l_strWildcard += _( "BITMAP files" );
			l_strWildcard += wxT( " (*.bmp)|*.bmp|" );
			l_strWildcard += _( "GIF files" );
			l_strWildcard += wxT( " (*.gif)|*.gif|" );
			l_strWildcard += _( "JPEG files" );
			l_strWildcard += wxT( " (*.jpg)|*.jpg|" );
			l_strWildcard += _( "PNG files" );
			l_strWildcard += wxT( " (*.png)|*.png" );
			wxFileDialog l_dialog( this, _( "Please choose an image file name" ), wxEmptyString, wxEmptyString, l_strWildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

			if ( l_dialog.ShowModal() == wxID_OK )
			{
				auto l_image = l_bitmap.ConvertToImage();
				l_image.SaveFile( l_dialog.GetPath() );
			}
		}
	}

	bool MainFrame::DoStartRecord()
	{
		bool l_return = false;

		if ( m_pRenderPanel )
		{
			try
			{
				l_return = m_recorder.StartRecord( m_pRenderPanel->GetRenderWindow()->GetRenderTarget()->GetSize(), m_recordFps );
			}
			catch ( std::exception & p_exc )
			{
				wxMessageBox( wxString( p_exc.what(), wxMBConvLibc() ) );
				l_return = false;
			}
		}

#if defined( GUICOMMON_RECORDS )

		if ( l_return )
		{
			if ( CASTOR3D_THREADED )
			{
				m_timer = new wxTimer( this, eID_RENDER_TIMER );
				wxGetApp().GetCastor()->GetRenderLoop().Pause();
			}

			m_timer->Stop();
			m_timer->Start( 1000 / m_recordFps );
		}

#endif
		return l_return;
	}

	void MainFrame::DoRecordFrame()
	{
#if defined( GUICOMMON_RECORDS )

		auto & l_castor = *wxGetApp().GetCastor();
		m_pRenderPanel->GetRenderWindow()->SaveFrame();
		l_castor.GetRenderLoop().RenderSyncFrame();
		auto l_buffer = m_pRenderPanel->GetRenderWindow()->GetSavedFrame();

		try
		{
			m_recorder.RecordFrame( l_buffer );
		}
		catch ( std::exception & p_exc )
		{
			DoStopRecord();
			wxMessageBox( wxString( p_exc.what(), wxMBConvLibc() ) );
		}

#endif
	}

	void MainFrame::DoStopRecord()
	{
		m_recorder.StopRecord();

#if defined( GUICOMMON_RECORDS )

		m_toolBar->EnableTool( eID_TOOL_STOP, false );
		m_toolBar->EnableTool( eID_TOOL_RECORD, true );

		if ( m_timer )
		{
			if ( CASTOR3D_THREADED )
			{
				wxGetApp().GetCastor()->GetRenderLoop().Resume();
				m_timer->Stop();
				delete m_timer;
				m_timer = nullptr;
			}
			else
			{
				m_timer->Stop();
				m_timer->Start( 1000 / wxGetApp().GetCastor()->GetRenderLoop().GetWantedFps() );
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

	void MainFrame::OnPaint( wxPaintEvent & p_event )
	{
		wxPaintDC l_paintDC( this );
		p_event.Skip();
	}

	void MainFrame::OnRenderTimer( wxTimerEvent & p_event )
	{
		auto l_castor = wxGetApp().GetCastor();

		if ( l_castor )
		{
			if ( !l_castor->IsCleaned() )
			{
				if ( m_pRenderPanel && m_recorder.IsRecording() && m_recorder.UpdateTime() )
				{
					DoRecordFrame();
				}
				else if ( !l_castor->IsThreaded() )
				{
					l_castor->GetRenderLoop().RenderSyncFrame();
				}
			}
		}
	}

	void MainFrame::OnTimer( wxTimerEvent & p_event )
	{
		if ( p_event.GetId() == eID_MSGLOG_TIMER && m_messageLog )
		{
			DooUpdateLog( m_msgLogList, m_msgLogListMtx, *m_messageLog );
		}
		else if ( p_event.GetId() == eID_ERRLOG_TIMER && m_errorLog )
		{
			DooUpdateLog( m_errLogList, m_errLogListMtx, *m_errorLog );
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
		m_messageLog = nullptr;
		m_errorLog = nullptr;
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
			if ( wxGetApp().GetCastor()->IsThreaded() )
			{
				wxGetApp().GetCastor()->GetRenderLoop().Pause();
			}

			m_pRenderPanel->SetRenderWindow( nullptr );

			if ( wxGetApp().GetCastor()->IsThreaded() )
			{
				wxGetApp().GetCastor()->GetRenderLoop().Resume();
			}
		}

		if ( wxGetApp().GetCastor() )
		{
			wxGetApp().GetCastor()->Cleanup();
		}

		if ( m_pRenderPanel )
		{
			m_pRenderPanel->UnFocus();
			m_pRenderPanel->Close( true );
			m_pRenderPanel = nullptr;
		}

		DestroyChildren();
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
		l_wildcard << wxT( " (*.cscn;*.zip)|*.cscn;*.zip|" );
		l_wildcard << _( "Castor3D scene file" );
		l_wildcard << CSCN_WILDCARD;
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
		wxString l_wildcard = _( "Castor3D scene" );
		l_wildcard += CSCN_WILDCARD;
		l_wildcard += _( "Wavefront OBJ" );
		l_wildcard += OBJ_WILDCARD;
		l_wildcard += wxT( "|" );
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
					l_exporter.ExportScene( *m_pMainScene.lock(), l_pathFile );
				}
				else if ( l_pathFile.GetExtension() == cuT( "cscn" ) )
				{
					CscnSceneExporter l_exporter;
					l_exporter.ExportScene( *m_pMainScene.lock(), l_pathFile );
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

	void MainFrame::OnPrintScreen( wxCommandEvent & p_event )
	{
		DoSaveFrame();
		p_event.Skip();
	}

	void MainFrame::OnRecord( wxCommandEvent & p_event )
	{
		if ( DoStartRecord() )
		{
#if defined( GUICOMMON_RECORDS )

			m_toolBar->EnableTool( eID_TOOL_STOP, true );
			m_toolBar->EnableTool( eID_TOOL_RECORD, false );

#endif
		}

		p_event.Skip();
	}

	void MainFrame::OnStop( wxCommandEvent & p_event )
	{
		DoStopRecord();
		p_event.Skip();
	}
}
