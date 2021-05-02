#include "Aria/MainFrame.hpp"

#include "Aria/CategoryPanel.hpp"
#include "Aria/DatabaseTest.hpp"
#include "Aria/LayeredPanel.hpp"
#include "Aria/RendererPage.hpp"
#include "Aria/TestPanel.hpp"
#include "Aria/TestsCounts.hpp"
#include "Aria/Aui/AuiDockArt.hpp"
#include "Aria/Aui/AuiTabArt.hpp"
#include "Aria/Database/DbDateTimeHelpers.hpp"
#include "Aria/Database/DbResult.hpp"
#include "Aria/Database/DbStatement.hpp"
#include "Aria/Editor/SceneFileDialog.hpp"
#include "Aria/Model/TreeModel.hpp"
#include "Aria/Model/TreeModelNode.hpp"

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Miscellaneous/Utils.hpp>

#include <castor.xpm>

#include <wx/choicdlg.h>
#include <wx/dc.h>
#include <wx/gauge.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textdlg.h>

#define CTP_UseAsync 1

namespace aria
{
	//*********************************************************************************************

	namespace
	{
#if CTP_UseAsync
		static auto constexpr ExecMode = wxEXEC_ASYNC;
#else
		static auto constexpr ExecMode = wxEXEC_SYNC;
#endif

		wxString const & getVersion()
		{
			static wxString const result{ wxString{ wxT( "v" ) } << Aria_VERSION_MAJOR << wxT( "." ) << Aria_VERSION_MINOR << wxT( "." ) << Aria_VERSION_BUILD };
			return result;
		}
	}

	//*********************************************************************************************

	MainFrame::TestProcess::TestProcess( MainFrame * mainframe
		, int flags )
		: wxProcess{ flags }
		, m_mainframe{ mainframe }
	{
	}

	void MainFrame::TestProcess::OnTerminate( int pid, int status )
	{
		castor::Logger::logInfo( castor::makeStringStream() << "Terminating process " << pid << "(" << status << ")" );
		auto event = new wxProcessEvent{ wxID_ANY, pid, status };
		m_mainframe->QueueEvent( event );
	}

	//*********************************************************************************************

	TestNode MainFrame::RunningTest::current()
	{
		return running;
	}

	void MainFrame::RunningTest::push( TestNode node )
	{
		pending.emplace_back( std::move( node ) );
	}

	TestNode MainFrame::RunningTest::next()
	{
		if ( !pending.empty() )
		{
			running = *pending.begin();
			running.status = TestStatus::eRunning_0;
			pending.erase( pending.begin() );
		}
		else
		{
			running = {};
		}

		return running;
	}

	void MainFrame::RunningTest::end()
	{
		running = {};
	}

	void MainFrame::RunningTest::clear()
	{
		for ( auto & running : pending )
		{
			running.test->updateStatusNW( running.status );
		}

		pending.clear();

		if ( running.test )
		{
			running.test->updateStatusNW( running.status );
		}

		running = {};
	}

	bool MainFrame::RunningTest::empty()const
	{
		return size() == 0u;
	}

	size_t MainFrame::RunningTest::size()const
	{
		return pending.size()
			+ ( running.test ? 1u : 0u );
	}

	bool MainFrame::RunningTest::isRunning()const
	{
		return running.test;
	}

	//*********************************************************************************************

	MainFrame::MainFrame( Config config )
		: wxFrame{ nullptr, wxID_ANY, wxT( "Aria " ) + getVersion(), wxDefaultPosition, wxSize( 800, 700 ) }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
		, m_config{ std::move( config ) }
		, m_database{ m_config }
	{
		SetClientSize( 900, 600 );
		doInitMenus();
		doInitMenuBar();
		{
			wxProgressDialog progress{ wxT( "Initialising" )
				, wxT( "Initialising..." )
				, int( 1 )
				, this };
			int index = 0;
			m_database.initialise( progress, index );
			m_tests.tests = m_database.listTests( progress, index );
			doInitGui();
		}
		Bind( wxEVT_CLOSE_WINDOW
			, [this]( wxCloseEvent & evt )
			{
				onClose( evt );
			} );
		Bind( wxEVT_SIZE
			, [this]( wxSizeEvent & evt )
			{
				onSize( evt );
			} );
		auto onTimer = [this]( wxTimerEvent & evt )
		{
			if ( evt.GetId() == eID_TEST_UPDATER )
			{
				onTestUpdateTimer( evt );
			}
			else if ( evt.GetId() == eID_CATEGORY_UPDATER )
			{
				onCategoryUpdateTimer( evt );
			}
		};
		m_testUpdater = new wxTimer{ this, eID_TEST_UPDATER };
		Bind( wxEVT_TIMER
			, onTimer
			, eID_TEST_UPDATER );
		m_testUpdater->Start( 100 );
		m_categoriesUpdater = new wxTimer{ this, eID_CATEGORY_UPDATER };
		Bind( wxEVT_TIMER
			, onTimer
			, eID_CATEGORY_UPDATER );
		m_testUpdater->Start( 100 );
	}

	MainFrame::~MainFrame()
	{
		m_testsPages.clear();
		m_auiManager.UnInit();
	}

	void MainFrame::initialise()
	{
		{
			wxProgressDialog progress{ wxT( "Initialising" )
				, wxT( "Initialising..." )
				, int( 1 )
				, this };
			int index = 0;
			doFillLists( progress, index );
		}

		m_runningTest.genProcess = std::make_unique< TestProcess >( this, wxPROCESS_DEFAULT );
#if !ARIA_UseDiffImageLib
		m_runningTest.difProcess = std::make_unique< TestProcess >( this, wxPROCESS_DEFAULT );
#endif
		m_runningTest.disProcess = std::make_unique< TestProcess >( this, wxPROCESS_DEFAULT );

		Connect( wxEVT_END_PROCESS
			, wxProcessEventHandler( MainFrame::onProcessEnd )
			, nullptr
			, this );
		m_statusText->SetLabel( _( "Idle" ) );
	}

	void MainFrame::updateTestStatus( DatabaseTest & test
		, TestStatus newStatus
		, bool reference )
	{
		test.updateStatus( newStatus, reference );
		doUpdateTestView( test );
	}

	void MainFrame::doInitTestsList( Renderer renderer )
	{
		auto runsIt = m_tests.runs.emplace( renderer, TestRunCategoryMap{} ).first;
		auto & rendererCounts = m_tests.counts->addRenderer( renderer );
		auto it = m_testsPages.find( renderer );

		if ( it == m_testsPages.end() )
		{
			auto page = new RendererPage{ m_config
				, renderer
				, runsIt->second
				, rendererCounts
				, m_testsBook
				, this
				, m_testMenu.get()
				, m_categoryMenu.get()
				, m_rendererMenu.get()
				, m_allMenu.get()
				, m_busyMenu.get() };
			m_testsPages.emplace( renderer, page );
			it = m_testsPages.find( renderer );
		}

		auto testsPage = it->second;
		m_testsBook->AddPage( testsPage, renderer->name );
	}

	wxWindow * MainFrame::doInitTestsLists()
	{
		m_testsBook = new wxAuiNotebook{ this
			, eID_TESTS_BOOK
			, wxDefaultPosition
			, wxDefaultSize
			, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH };
		m_testsBook->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_testsBook->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_testsBook->SetArtProvider( new AuiTabArt );
		m_testsBook->Connect( wxEVT_AUINOTEBOOK_PAGE_CHANGED
			, wxAuiNotebookEventHandler( MainFrame::onTestsPageChange )
			, nullptr
			, this );
		m_tests.counts = std::make_shared< AllTestsCounts >( m_config );

		for ( auto & renderer : m_database.getRenderers() )
		{
			doInitTestsList( renderer.second.get() );
		}

		m_testsBook->SetSelection( 0u );
		return m_testsBook;
	}

	void MainFrame::doInitGui()
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		doInitTestsLists();

		auto statusBar = CreateStatusBar();
		statusBar->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		statusBar->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_testProgress = new wxGauge{ statusBar, wxID_ANY, 100, wxPoint( 10, 3 ), wxSize( 100, statusBar->GetSize().GetHeight() - 6 ), wxGA_SMOOTH, wxDefaultValidator };
		m_testProgress->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		m_testProgress->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_testProgress->SetValue( 0 );
		m_testProgress->Hide();
		m_statusText = new wxStaticText{ statusBar, wxID_ANY, _( "Idle" ), wxPoint( 120, 5 ), wxDefaultSize, 0 };
		m_statusText->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		m_statusText->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_testsBook
			, wxAuiPaneInfo()
			.Layer( 0 )
			.MinSize( 0, 200 )
			.CaptionVisible( false )
			.CloseButton( false )
			.PaneBorder( false )
			.Center()
			.Movable( false )
			.Dockable( false ) );
		m_auiManager.Update();
	}

	void MainFrame::doInitMenus( )
	{
		auto addTestBaseMenus = []( wxMenu & menu )
		{
			menu.Append( eID_TEST_COPY_FILE_NAME, _( "Copy test file path" ) + wxT( "\tF2" ) );
			menu.Append( eID_TEST_VIEW_FILE, _( "View test scene file" ) + wxT( "\tF3" ) );
			menu.Append( eID_TEST_SET_REF, _( "Set Reference" ) + wxT( "\tF4" ) );
			menu.Append( eID_TEST_VIEW, _( "View Test" ) + wxT( "\tF5" ) );
			menu.Append( eID_TEST_IGNORE_RESULT, _( "Ignore result" ) + wxT( "\tF6" ), wxEmptyString, true );
			menu.Append( eID_TEST_UPDATE_CASTOR, _( "Update Castor3D's date" ) + wxT( "\tF7" ) );
			menu.Append( eID_TEST_UPDATE_SCENE, _( "Update Scene's date" ) + wxT( "\tF8" ) );
		};
		auto addTestRunMenus = []( wxMenu & menu )
		{
		};
		auto addRendererMenus = []( wxMenu & menu )
		{
			menu.Append( eID_RENDERER_RUN_TESTS_ALL, _( "Run all renderer's tests" ) + wxT( "\tCtrl+F1" ) );
			menu.Append( eID_RENDERER_RUN_TESTS_NOTRUN, _( "Run all <not run> renderer's tests" ) + wxT( "\tCtrl+F2" ) );
			menu.Append( eID_RENDERER_RUN_TESTS_ACCEPTABLE, _( "Run all <acceptable> renderer's tests" ) + wxT( "\tCtrl+F3" ) );
			menu.Append( eID_RENDERER_RUN_TESTS_ALL_BUT_NEGLIGIBLE, _( "Run all but <negligible> renderer's tests" ) + wxT( "\tCtrl+F4" ) );
			menu.Append( eID_RENDERER_RUN_TESTS_OUTDATED, _( "Run all outdated renderer's tests" ) + wxT( "\tFCtrl+5" ) );
			menu.Append( eID_RENDERER_UPDATE_CASTOR, _( "Update renderer's tests Castor3D's date" ) + wxT( "\tCtrl+F6" ) );
			menu.Append( eID_RENDERER_UPDATE_SCENE, _( "Update renderer's tests Scene's date" ) + wxT( "\tCtrl+F7" ) );
		};
		auto addCategoryMenus = []( wxMenu & menu )
		{
			menu.Append( eID_CATEGORY_RUN_TESTS_ALL, _( "Run all category's tests" ) + wxT( "\tAlt+F1" ) );
			menu.Append( eID_CATEGORY_RUN_TESTS_NOTRUN, _( "Run all <not run> category's tests" ) + wxT( "\tAlt+F2" ) );
			menu.Append( eID_CATEGORY_RUN_TESTS_ACCEPTABLE, _( "Run all <acceptable> category's tests" ) + wxT( "\tAlt+F3" ) );
			menu.Append( eID_CATEGORY_RUN_TESTS_ALL_BUT_NEGLIGIBLE, _( "Run all but <negligible> category's tests" ) + wxT( "\tAlt+F4" ) );
			menu.Append( eID_CATEGORY_RUN_TESTS_OUTDATED, _( "Run all outdated category's tests" ) + wxT( "\tAlt+F5" ) );
			menu.Append( eID_CATEGORY_UPDATE_CASTOR, _( "Update category's tests Castor3D's date" ) + wxT( "\tAlt+F6" ) );
			menu.Append( eID_CATEGORY_UPDATE_SCENE, _( "Update category's tests Scene's date" ) + wxT( "\tAlt+F7" ) );
		};
		auto addAllMenus = []( wxMenu & menu )
		{
			menu.Append( eID_ALL_RUN_TESTS_ALL, _( "Run all tests" ) + wxT( "\tCtrl+Alt+F1" ) );
			menu.Append( eID_ALL_RUN_TESTS_NOTRUN, _( "Run all <not run> tests" ) + wxT( "\tCtrl+Alt+F2" ) );
			menu.Append( eID_ALL_RUN_TESTS_ACCEPTABLE, _( "Run all <acceptable> tests" ) + wxT( "\tCtrl+Alt+F3" ) );
			menu.Append( eID_ALL_RUN_TESTS_ALL_BUT_NEGLIGIBLE, _( "Run all but <negligible> tests" ) + wxT( "\tCtrl+Alt+F4" ) );
			menu.Append( eID_ALL_RUN_TESTS_OUTDATED, _( "Run all outdated tests" ) + wxT( "\tCtrl+Alt+F5" ) );
			menu.Append( eID_ALL_UPDATE_CASTOR, _( "Update tests Castor3D's date" ) + wxT( "\tCtrl+Alt+F6" ) );
			menu.Append( eID_ALL_UPDATE_SCENE, _( "Update tests Scene's date" ) + wxT( "\tCtrl+Alt+F7" ) );
		};
		m_testMenu = std::make_unique< wxMenu >();
		m_testMenu->Append( eID_TEST_RUN, _( "Run Test" ) + wxT( "\tF1" ) );
		addTestBaseMenus( *m_testMenu );
		m_testMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );
		m_barTestMenu = new wxMenu;
		m_barTestMenu->Append( eID_TEST_RUN, _( "Run Test" ) + wxT( "\tF1" ) );
		addTestBaseMenus( *m_barTestMenu );
		m_barTestMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );

		m_categoryMenu = std::make_unique< wxMenu >();
		addCategoryMenus( *m_categoryMenu );
		m_categoryMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );
		m_barCategoryMenu = new wxMenu;
		addCategoryMenus( *m_barCategoryMenu );
		m_barCategoryMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );
		
		m_rendererMenu = std::make_unique< wxMenu >();
		addRendererMenus( *m_rendererMenu );
		m_rendererMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );
		m_barRendererMenu = new wxMenu;
		addRendererMenus( *m_barRendererMenu );
		m_barRendererMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );

		m_allMenu = std::make_unique< wxMenu >();
		addAllMenus( *m_allMenu );
		m_allMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );
		m_barAllMenu = new wxMenu;
		addAllMenus( *m_barAllMenu );
		m_barAllMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );

		m_busyMenu = std::make_unique< wxMenu >();
		m_busyMenu->Append( eID_CANCEL, _( "Cancel" ) + wxT( "\tF1" ) );
		addTestBaseMenus( *m_busyMenu );
		m_busyMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );
	}

	void MainFrame::doInitMenuBar()
	{
		wxMenuBar * menuBar{ new wxMenuBar };

		wxMenu * rendererMenu{ new wxMenu };
		rendererMenu->Append( eID_NEW_RENDERER, _( "New Renderer" ) );
		rendererMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onDatabaseMenuOption )
			, nullptr
			, this );

		wxMenu * categoryMenu{ new wxMenu };
		categoryMenu->Append( eID_NEW_CATEGORY, _( "New Category" ) );
		categoryMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onDatabaseMenuOption )
			, nullptr
			, this );

		wxMenu * testMenu{ new wxMenu };
		testMenu->Append( eID_NEW_TEST, _( "New Test" ) );
		testMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onDatabaseMenuOption )
			, nullptr
			, this );

		wxMenu * databaseMenu{ new wxMenu };
		databaseMenu->AppendSubMenu( rendererMenu, _( "Renderer" ) );
		databaseMenu->AppendSubMenu( categoryMenu, _( "Category" ) );
		databaseMenu->AppendSubMenu( testMenu, _( "Test" ) );
		menuBar->Append( databaseMenu, _( "Database" ) );

		wxMenu * testsMenu{ new wxMenu };
		testsMenu->AppendSubMenu( m_barTestMenu, _( "Single" ) );
		testsMenu->AppendSubMenu( m_barCategoryMenu, _( "Category" ) );
		testsMenu->AppendSubMenu( m_barRendererMenu, _( "Renderer" ) );
		testsMenu->AppendSubMenu( m_barAllMenu, _( "All" ) );
		menuBar->Append( testsMenu, _( "Tests" ) );

		SetMenuBar( menuBar );
	}

	void MainFrame::doFillLists( wxProgressDialog & progress
		, int & index )
	{
		uint32_t testCount = 0u;

		for ( auto & test : m_tests.tests )
		{
			testCount += test.second.size();
		}

		castor::Logger::logInfo( "Filling Data View" );
		progress.SetTitle( _( "Filling Data View" ) );
		progress.SetRange( progress.GetRange() + int( testCount * m_database.getRenderers().size() ) );
		progress.Update( index, _( "Filling Data View..." ) );

		for ( auto & renderer : m_tests.runs )
		{
			doFillList( renderer.first, progress, index );
		}
	}

	void MainFrame::doFillList( Renderer renderer
		, wxProgressDialog & progress
		, int & index )
	{
		auto rendIt = m_testsPages.find( renderer );
		assert( rendIt != m_testsPages.end() );
		rendIt->second->listLatestRuns( m_database
			, m_tests.tests
			, *m_tests.counts
			, progress
			, index );
	}

	RendererPage * MainFrame::doGetPage( wxDataViewItem const & item )
	{
		auto node = static_cast< TreeModelNode * >( item.GetID() );
		auto rendIt = m_testsPages.find( node->test->getRenderer() );

		if ( rendIt != m_testsPages.end() )
		{
			return rendIt->second;
		}

		return nullptr;
	}

	uint32_t MainFrame::doGetAllTestsRange()const
	{
		uint32_t range = 0u;

		for ( auto & category : m_tests.tests )
		{
			range += category.second.size();
		}

		return range;
	}

	uint32_t MainFrame::doGetAllRunsRange()const
	{
		uint32_t range = 0u;

		for ( auto & renderer : m_tests.runs )
		{
			for ( auto & category : renderer.second )
			{
				range += category.second.size();
			}
		}

		return range;
	}

	uint32_t MainFrame::doGetSelectedRendererRange()const
	{
		return m_selectedPage->getSelectedRange( m_tests.runs );
	}

	uint32_t MainFrame::doGetSelectedCategoryRange()const
	{
		return m_selectedPage->getSelectedCategoryRange( m_tests.runs );
	}

	void MainFrame::doViewSceneFile( castor::Path const & filePath )
	{
		if ( !castor::File::fileExists( filePath ) )
		{
			FILE * file;

			if ( castor::fileOpen( file, filePath.c_str(), "w" ) )
			{
				fclose( file );
			}
		}

		auto editor = new SceneFileDialog{ m_config
			, makeWxString( filePath )
			, makeWxString( filePath.getFileName( true ) )
			, this };
		editor->Show();
	}

	void MainFrame::doProcessTest()
	{
		auto testNode = m_runningTest.next();

		if ( !m_cancelled
			&& testNode.test )
		{
			auto & test = *testNode.test;
			wxString command = m_config.launcher;
			command << " " << m_config.test / getSceneFile( *test );
			command << " -" << testNode.test->getRenderer()->name;
			test.updateStatusNW( TestStatus::eRunning_Begin );
			auto page = doGetPage( wxDataViewItem{ testNode.node } );

			if ( page )
			{
				page->updateTest( testNode.node );
				m_statusText->SetLabel( _( "Running test: " ) + test.getName() );
				auto result = wxExecute( command
					, ExecMode
					, m_runningTest.genProcess.get() );
#if CTP_UseAsync

				if ( result == 0 )
				{
					castor::Logger::logError( "doProcessTest: " + castor::System::getLastErrorText() );
				}
				else
				{
					m_runningTest.currentProcess = m_runningTest.genProcess.get();
				}

#else
				onTestRunEnd( wxProcessEvent{} );
#endif
				m_testProgress->SetValue( m_testProgress->GetValue() + 1u );
			}
		}
		else
		{
			m_statusText->SetLabel( _( "Idle" ) );
			m_testProgress->Hide();
		}
	}

	void MainFrame::doStartTests()
	{
		m_testProgress->SetRange( m_runningTest.size() );

		if ( !m_runningTest.isRunning() )
		{
			m_testProgress->SetValue( 0 );
			m_testProgress->Show();
			doProcessTest();
		}

		if ( m_selectedPage )
		{
			m_selectedPage->refreshView();
		}
	}

	void MainFrame::doPushTest( wxDataViewItem & item )
	{
		auto node = static_cast< TreeModelNode * >( item.GetID() );
		auto & run = *node->test;
		m_runningTest.push( { &run, run.getStatus(), node } );
		run.updateStatusNW( TestStatus::ePending );
		m_selectedPage->updateTest( node );
	}

	void MainFrame::doClearRunning()
	{
		m_runningTest.clear();
	}

	void MainFrame::doRunTest()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto tests = m_selectedPage->listSelectedTests();

			if ( !tests.empty() )
			{
				for ( auto & item : tests )
				{
					doPushTest( item );
				}

				doStartTests();
			}
		}
	}

	void MainFrame::doCopyTestFileName()
	{
		if ( m_selectedPage )
		{
			m_selectedPage->copyTestFileName();
		}
	}

	void MainFrame::doViewTestSceneFile()
	{
		if ( m_selectedPage )
		{
			m_selectedPage->viewTestSceneFile();
		}
	}

	void MainFrame::doViewTest()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			m_selectedPage->viewTest( m_runningTest.disProcess.get()
				, m_statusText );
		}

		if ( m_runningTest.empty() )
		{
			m_statusText->SetLabel( _( "Idle" ) );
		}
		else
		{
			m_statusText->SetLabel( _( "Running Test" ) );
		}
	}

	void MainFrame::doSetRef()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			m_selectedPage->setTestsReferences( *m_tests.counts );
		}
	}

	void MainFrame::doIgnoreTestResult()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );

		if ( m_selectedPage )
		{
			m_selectedPage->ignoreTestsResult( m_testMenu->IsChecked( eID_TEST_IGNORE_RESULT ) );
		}
	}

	void MainFrame::doUpdateCastorDate()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );

		if ( m_selectedPage )
		{
			m_selectedPage->updateTestsCastorDate();
		}
	}

	void MainFrame::doUpdateSceneDate()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			m_selectedPage->updateTestsSceneDate();
		}
	}

	TreeModelNode * MainFrame::getTestNode( DatabaseTest const & test )
	{
		auto rendIt = m_testsPages.find( test->renderer );
		assert( rendIt != m_testsPages.end() );
		return rendIt->second->getTestNode( test );
	}

	wxDataViewItem MainFrame::getTestItem( DatabaseTest const & test )
	{
		return wxDataViewItem{ getTestNode ( test ) };
	}

	void MainFrame::doRunAllCategoryTests()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( m_tests.runs
				, []( DatabaseTest const & run )
				{
					return true;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doRunCategoryTests( TestStatus filter )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( m_tests.runs
				, [filter]( DatabaseTest const & run )
				{
					return run->status == filter;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doRunAllCategoryTestsBut( TestStatus filter )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( m_tests.runs
				, [filter]( DatabaseTest const & run )
				{
					return run->status != filter;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doRunAllCategoryOutdatedTests()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( m_tests.runs
				, [this]( DatabaseTest const & run )
				{
					return isOutOfDate( m_config, *run )
						|| run->status == TestStatus::eNotRun;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doUpdateCategoryCastorDate()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );
		auto range = doGetSelectedCategoryRange();
		wxProgressDialog progress{ wxT( "Updating tests Castor3D date" )
			, wxT( "Updating tests..." )
			, int( range )
			, this };
		int index = 0;

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( m_tests.runs
				, []( DatabaseTest const & run )
				{
					return run.checkOutOfCastorDate();
				} );

			for ( auto & item : items )
			{
				auto node = reinterpret_cast< TreeModelNode * >( item.GetID() );
				auto & run = *node->test;
				progress.Update( index++
					, _( "Updating tests Castor3D date" )
					+ wxT( "\n" ) + getProgressDetails( run ) );
				progress.Fit();
				run.updateCastorDate( m_config.castorRefDate );
			}

			m_selectedPage->refreshView();
		}
	}

	void MainFrame::doUpdateCategorySceneDate()
	{
		m_cancelled.exchange( false );
		auto range = doGetSelectedCategoryRange();
		wxProgressDialog progress{ wxT( "Updating tests Scene date" )
			, wxT( "Updating tests..." )
			, int( range )
			, this };
		int index = 0;

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( m_tests.runs
				, []( DatabaseTest const & run )
				{
					return run.checkOutOfSceneDate();
				} );

			for ( auto & item : items )
			{
				auto node = reinterpret_cast< TreeModelNode * >( item.GetID() );
				auto & run = *node->test;
				auto sceneDate = getSceneDate( m_config, *run );
				progress.Update( index++
					, _( "Updating tests Scene date" )
					+ wxT( "\n" ) + getProgressDetails( run ) );
				progress.Fit();
				run.updateSceneDate( sceneDate );
			}

			m_selectedPage->refreshView();
		}
	}

	void MainFrame::doRunAllRendererTests()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRendererTests( m_tests.runs
				, []( DatabaseTest const & run )
				{
					return true;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doRunRendererTests( TestStatus filter )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRendererTests( m_tests.runs
				, [filter]( DatabaseTest const & run )
				{
					return run->status == filter;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doRunAllRendererTestsBut( TestStatus filter )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRendererTests( m_tests.runs
				, [filter]( DatabaseTest const & run )
				{
					return run->status != filter;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doRunAllRendererOutdatedTests()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRendererTests( m_tests.runs
				, [this]( DatabaseTest const & run )
				{
					return isOutOfDate( m_config, *run )
						|| run->status == TestStatus::eNotRun;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doUpdateRendererCastorDate()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );
		auto range = doGetSelectedRendererRange();
		wxProgressDialog progress{ wxT( "Updating tests Castor3D date" )
			, wxT( "Updating tests..." )
			, int( range )
			, this };
		int index = 0;

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRendererTests( m_tests.runs
				, [this]( DatabaseTest const & run )
				{
					return run.checkOutOfCastorDate();
				} );

			for ( auto & item : items )
			{
				auto node = reinterpret_cast< TreeModelNode * >( item.GetID() );
				auto & run = *node->test;
				progress.Update( index++
					, _( "Updating tests Castor3D date" )
					+ wxT( "\n" ) + getProgressDetails( run ) );
				progress.Fit();
				run.updateCastorDate( m_config.castorRefDate );
			}

			m_selectedPage->refreshView();
		}
	}

	void MainFrame::doUpdateRendererSceneDate()
	{
		m_cancelled.exchange( false );
		auto range = doGetSelectedRendererRange();
		wxProgressDialog progress{ wxT( "Updating tests Scene date" )
			, wxT( "Updating tests..." )
			, int( range )
			, this };
		int index = 0;

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRendererTests( m_tests.runs
				, [this]( DatabaseTest const & run )
				{
					return run.checkOutOfSceneDate();
				} );

			for ( auto & item : items )
			{
				auto node = reinterpret_cast< TreeModelNode * >( item.GetID() );
				auto & run = *node->test;
				auto sceneDate = getSceneDate( m_config, *run );
				progress.Update( index++
					, _( "Updating tests Scene date" )
					+ wxT( "\n" ) + getProgressDetails( run ) );
				progress.Fit();
				run.updateSceneDate( sceneDate );
			}

			m_selectedPage->refreshView();
		}
	}

	void MainFrame::doRunAllTests()
	{
		m_cancelled.exchange( false );

		for ( auto & renderer : m_tests.runs )
		{
			for ( auto & category : renderer.second )
			{
				for ( auto & run : category.second )
				{
					doPushTest( getTestItem( run ) );
				}
			}
		}

		doStartTests();
	}

	void MainFrame::doRunTests( TestStatus filter )
	{
		m_cancelled.exchange( false );

		for ( auto & renderer : m_tests.runs )
		{
			for ( auto & category : renderer.second )
			{
				for ( auto & run : category.second )
				{
					if ( run->status == filter )
					{
						doPushTest( getTestItem( run ) );
					}
				}
			}
		}

		doStartTests();
	}

	void MainFrame::doRunAllTestsBut( TestStatus filter )
	{
		m_cancelled.exchange( false );

		for ( auto & renderer : m_tests.runs )
		{
			for ( auto & category : renderer.second )
			{
				for ( auto & run : category.second )
				{
					if ( run->status != filter )
					{
						doPushTest( getTestItem( run ) );
					}
				}
			}
		}

		doStartTests();
	}

	void MainFrame::doRunAllOutdatedTests()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );

		for ( auto & renderer : m_tests.runs )
		{
			for ( auto & category : renderer.second )
			{
				for ( auto & run : category.second )
				{
					if ( isOutOfDate( m_config, *run ) )
					{
						doPushTest( getTestItem( run ) );
					}
				}
			}
		}

		doStartTests();
	}

	void MainFrame::doUpdateAllCastorDate()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );
		auto range = doGetAllRunsRange();
		wxProgressDialog progress{ wxT( "Updating tests Castor3D date" )
			, wxT( "Updating tests..." )
			, int( range )
			, this };
		int index = 0;

		for ( auto & renderer : m_tests.runs )
		{
			for ( auto & category : renderer.second )
			{
				for ( auto & run : category.second )
				{
					progress.Update( index++
						, _( "Updating tests Castor3D date" )
						+ wxT( "\n" ) + getProgressDetails( run ) );
					progress.Fit();

					if ( run.checkOutOfCastorDate() )
					{
						run.updateCastorDate( m_config.castorRefDate );
					}
				}
			}
		}

		m_selectedPage->refreshView();
	}

	void MainFrame::doUpdateAllSceneDate()
	{
		m_cancelled.exchange( false );
		auto range = doGetAllRunsRange();
		wxProgressDialog progress{ wxT( "Updating tests Scene date" )
			, wxT( "Updating tests..." )
			, int( range )
			, this };
		int index = 0;

		for ( auto & renderer : m_tests.runs )
		{
			for ( auto & category : renderer.second )
			{
				for ( auto & run : category.second )
				{
					auto sceneDate = getSceneDate( m_config, *run );
					progress.Update( index++
						, _( "Updating tests Scene date" )
						+ wxT( "\n" ) + getProgressDetails( run ) );
					progress.Fit();

					if ( run.checkOutOfSceneDate() )
					{
						run.updateSceneDate( sceneDate );
					}
				}
			}
		}

		m_selectedPage->refreshView();
	}

	void MainFrame::doCancelTest( DatabaseTest & test
		, TestStatus status )
	{
		test.updateStatusNW( status );
		auto node = getTestNode( test );
		wxDataViewItem item = wxDataViewItem{ node };
		auto page = doGetPage( item );

		if ( page )
		{
			m_selectedPage->updateTest( node );
		}

		doClearRunning();
		m_statusText->SetLabel( _( "Idle" ) );
		m_testProgress->Hide();
	}

	void MainFrame::doCancel()
	{
		m_cancelled.exchange( true );
	}

	void MainFrame::doNewRenderer()
	{
		wxTextEntryDialog dialog{ this, _( "Enter the new renderer name" ) };

		if ( dialog.ShowModal() == wxID_OK )
		{
			auto renderer = m_database.createRenderer( makeStdString( dialog.GetValue() ) );
			auto ires = m_tests.runs.emplace( renderer, TestRunCategoryMap{} );

			if ( ires.second )
			{
				auto range = doGetAllTestsRange();
				wxProgressDialog progress{ wxT( "Creating renderer entries" )
					, wxT( "Creating renderer entries..." )
					, int( range )
					, this };
				int index = 0;
				doInitTestsList( renderer );
				doFillList( renderer, progress, index );
			}
		}
	}

	void MainFrame::doNewCategory()
	{
		wxTextEntryDialog dialog{ this
			, _( "Enter the new category name" )
			, _( "Category creation" ) };

		if ( dialog.ShowModal() == wxID_OK )
		{
			auto categoryName = makeStdString( dialog.GetValue() );
			auto catIt = m_database.getCategories().find( makeStdString( categoryName ) );

			if ( catIt != m_database.getCategories().end() )
			{
				wxMessageBox( wxString{} << wxT( "Invalid category name: " ) << categoryName
					, wxT( "Error" )
					, wxICON_ERROR );
				return;
			}

			auto category = m_database.createCategory( makeStdString( categoryName ) );
			m_tests.tests.emplace( category, TestArray{} );

			if ( !castor::File::directoryExists( m_config.test / category->name ) )
			{
				castor::File::directoryCreate( m_config.test / category->name );
			}

			for ( auto & rendererIt : m_database.getRenderers() )
			{
				auto renderer = rendererIt.second.get();
				auto & rendCounts = m_tests.counts->getRenderer( renderer );
				auto & catCounts = rendCounts.addCategory( category, {}, {} );

				auto rendRunIt = m_tests.runs.find( renderer );
				rendRunIt->second.emplace( category, TestRunArray{} );

				auto rendPageIt = m_testsPages.find( renderer );
				rendPageIt->second->addCategory( category, catCounts );
			}
		}
	}

	void MainFrame::doNewTest()
	{
		wxArrayString categories;

		for ( auto & category : m_database.getCategories() )
		{
			categories.push_back( category.first );
		}

		wxSingleChoiceDialog categoryDlg{ this
			, _( "Select the test's category" )
			, _( "Test creation" )
			, categories };

		if ( categoryDlg.ShowModal() == wxID_OK )
		{
			wxString categoryName = categoryDlg.GetStringSelection();
			auto catIt = m_database.getCategories().find( makeStdString( categoryName ) );

			if ( catIt == m_database.getCategories().end() )
			{
				wxMessageBox( wxString{} << wxT( "Invalid category name: " ) << categoryName
					, wxT( "Error" )
					, wxICON_ERROR );
				return;
			}

			auto category = catIt->second.get();
			wxTextEntryDialog dialog{ this
				, _( "Enter the new test name" )
				, _( "Test creation" ) };

			if ( dialog.ShowModal() == wxID_OK )
			{
				auto testName = makeStdString( dialog.GetValue() );
				auto catTestIt = m_tests.tests.find( category );
				catTestIt->second.emplace_back( std::make_unique< Test >( 0
					, testName
					, category ) );
				auto & test = *catTestIt->second.back();
				m_database.insertTest( test, false );

				for ( auto & rendererIt : m_database.getRenderers() )
				{
					auto renderer = rendererIt.second.get();
					auto rendRunIt = m_tests.runs.find( renderer );
					auto & rendCounts = m_tests.counts->getRenderer( renderer );
					auto catRunIt = rendRunIt->second.find( category );
					auto & catCounts = rendCounts.getCounts( category );
					catRunIt->second.emplace_back( m_database
						, TestRun{ &test
							, renderer
							, db::DateTime{}
							, TestStatus::eNotRun
							, db::DateTime{}
							, db::DateTime{} } );
					auto & dbTest = catRunIt->second.back();

					auto rendPageIt = m_testsPages.find( renderer );
					rendPageIt->second->addTest( dbTest );
					catCounts.addTest( dbTest );
				}

				doViewSceneFile( getTestFileName( m_config.test, test ) );
			}
		}
	}

	void MainFrame::doUpdateTestView( DatabaseTest const & test )
	{
		wxDataViewItem testItem{ getTestNode( test ) };
		auto page = doGetPage( testItem );

		if ( page )
		{
			page->updateTestView( test, *m_tests.counts );
		}
	}

	void MainFrame::onTestRunEnd( int status )
	{
		auto testNode = m_runningTest.current();
		auto & run = *testNode.test;

		if ( status < 0 && status != std::numeric_limits< int >::max() )
		{
			castor::Logger::logError( castor::makeStringStream() << "Test run failed (" << status << ")" );
		}

		if ( !m_cancelled )
		{
#if ARIA_UseDiffImageLib
			diffimg::Options options;
			auto file = ( m_config.test / run.getCategory()->name / ( run.getName() + ".cscn" ) );
			options.input = file.getPath() / ( file.getFileName() + cuT( "_ref.png" ) );
			options.outputs.emplace_back( file.getPath() / cuT( "Compare" ) / ( file.getFileName() + cuT( "_" ) + run.getRenderer()->name + cuT( ".png" ) ) );
			diffimg::Config config{ options };

			for ( auto output : options.outputs )
			{
				diffimg::compareImages( options, config, output );
			}

			onTestDiffEnd( 0 );
#else
			wxString command = m_config.differ;
			command << " " << run.getRenderer()->name;
			command << " -f " << ( m_config.test / run.getCategory()->name / ( run.getName() + ".cscn" ) );
			m_runningTest.difProcess->Redirect();
			auto result = wxExecute( command
				, ExecMode
				, m_runningTest.difProcess.get() );
#	if CTP_UseAsync

			if ( result == 0 )
			{
				castor::Logger::logError( "doProcessTest: " + castor::System::getLastErrorText() );
			}
			else
			{
				m_runningTest.currentProcess = m_runningTest.difProcess.get();
			}

#	else
			onTestDiffEnd( wxProcessEvent{} );
#	endif
#endif
		}
		else 
		{
			doCancelTest( run, run.getStatus() );
		}
	}

	void MainFrame::onTestDisplayEnd( int status )
	{
	}

	void MainFrame::onTestDiffEnd( int status )
	{
		TestNode testNode = m_runningTest.current();
		auto & test = *testNode.test;

		if ( !m_cancelled )
		{
			auto matches = castor::File::filterDirectoryFiles( m_config.test / getCompareFolder( *test->test )
				, [&test]( castor::Path const & folder, castor::String name )
				{
					return name.find( test.getName() + "_" + test->renderer->name ) == 0u
						&& castor::Path{ name }.getExtension() == "png"
						&& name.find( "diff.png" ) == castor::String::npos;
				}
			, true );
			assert( matches.size() < 2 );

			if ( !matches.empty() )
			{
				test.createNewRun( matches[0] );
			}
			else
			{
				test.createNewRun( TestStatus::eCrashed
					, makeDbDateTime( wxDateTime::Now() ) );
			}

			auto page = doGetPage( wxDataViewItem{ testNode.node } );

			if ( page )
			{
				page->updateTest( testNode.node );
				page->updateTestView( test, *m_tests.counts );
			}

			doProcessTest();
		}
		else
		{
			doCancelTest( *testNode.test, testNode.status );
		}
	}

	void MainFrame::onTestProcessEnd( int pid, int status )
	{
		auto currentProcess = m_runningTest.currentProcess;
		m_runningTest.currentProcess = nullptr;

		if ( currentProcess )
		{
#if !ARIA_UseDiffImageLib
			if ( currentProcess == m_runningTest.difProcess.get() )
			{
				onTestDiffEnd( status );
			}
			else
#endif
			if ( currentProcess == m_runningTest.disProcess.get() )
			{
				onTestDisplayEnd( status );
			}
			else if ( currentProcess == m_runningTest.genProcess.get() )
			{
				onTestRunEnd( status );
			}
		}
	}

	void MainFrame::onClose( wxCloseEvent & evt )
	{
		m_categoriesUpdater->Stop();
		m_testUpdater->Stop();

		if ( m_runningTest.disProcess )
		{
			if ( wxProcess::Exists( m_runningTest.disProcess->GetPid() ) )
			{
				wxProcess::Kill( m_runningTest.disProcess->GetPid() );
			}

			m_runningTest.disProcess->Disconnect( wxEVT_END_PROCESS );
			m_runningTest.disProcess = nullptr;
		}

#if !ARIA_UseDiffImageLib
		if ( m_runningTest.difProcess )
		{
			if ( wxProcess::Exists( m_runningTest.difProcess->GetPid() ) )
			{
				wxProcess::Kill( m_runningTest.difProcess->GetPid() );
			}

			m_runningTest.difProcess->Disconnect( wxEVT_END_PROCESS );
			m_runningTest.difProcess = nullptr;
		}
#endif

		if ( m_runningTest.genProcess )
		{
			if ( wxProcess::Exists( m_runningTest.genProcess->GetPid() ) )
			{
				wxProcess::Kill( m_runningTest.genProcess->GetPid() );
			}

			m_runningTest.genProcess->Disconnect( wxEVT_END_PROCESS );
			m_runningTest.genProcess = nullptr;
		}

		evt.Skip();
	}

	void MainFrame::onTestsPageChange( wxAuiNotebookEvent & evt )
	{
		m_selectedPage = reinterpret_cast< RendererPage * >( m_testsBook->GetPage( m_testsBook->GetSelection() ) );
	}

	void MainFrame::onTestsMenuOption( wxCommandEvent & evt )
	{
		switch ( evt.GetId() )
		{
		case eID_TEST_RUN:
			doRunTest();
			break;
		case eID_TEST_COPY_FILE_NAME:
			doCopyTestFileName();
			break;
		case eID_TEST_VIEW_FILE:
			doViewTestSceneFile();
			break;
		case eID_TEST_VIEW:
			doViewTest();
			break;
		case eID_TEST_SET_REF:
			doSetRef();
			break;
		case eID_TEST_IGNORE_RESULT:
			doIgnoreTestResult();
			break;
		case eID_TEST_UPDATE_CASTOR:
			doUpdateCastorDate();
			break;
		case eID_TEST_UPDATE_SCENE:
			doUpdateSceneDate();
			break;
		case eID_CATEGORY_RUN_TESTS_ALL:
			doRunAllCategoryTests();
			break;
		case eID_CATEGORY_RUN_TESTS_NOTRUN:
			doRunCategoryTests( TestStatus::eNotRun );
			break;
		case eID_CATEGORY_RUN_TESTS_ACCEPTABLE:
			doRunCategoryTests( TestStatus::eAcceptable );
			break;
		case eID_CATEGORY_RUN_TESTS_ALL_BUT_NEGLIGIBLE:
			doRunAllCategoryTestsBut( TestStatus::eNegligible );
			break;
		case eID_CATEGORY_RUN_TESTS_OUTDATED:
			doRunAllCategoryOutdatedTests();
			break;
		case eID_CATEGORY_UPDATE_CASTOR:
			doUpdateCategoryCastorDate();
			break;
		case eID_CATEGORY_UPDATE_SCENE:
			doUpdateCategorySceneDate();
			break;
		case eID_RENDERER_RUN_TESTS_ALL:
			doRunAllRendererTests();
			break;
		case eID_RENDERER_RUN_TESTS_NOTRUN:
			doRunRendererTests( TestStatus::eNotRun );
			break;
		case eID_RENDERER_RUN_TESTS_ACCEPTABLE:
			doRunRendererTests( TestStatus::eAcceptable );
			break;
		case eID_RENDERER_RUN_TESTS_ALL_BUT_NEGLIGIBLE:
			doRunAllRendererTestsBut( TestStatus::eNegligible );
			break;
		case eID_RENDERER_RUN_TESTS_OUTDATED:
			doRunAllRendererOutdatedTests();
			break;
		case eID_RENDERER_UPDATE_CASTOR:
			doUpdateRendererCastorDate();
			break;
		case eID_RENDERER_UPDATE_SCENE:
			doUpdateRendererSceneDate();
			break;
		case eID_ALL_RUN_TESTS_ALL:
			doRunAllTests();
			break;
		case eID_ALL_RUN_TESTS_NOTRUN:
			doRunTests( TestStatus::eNotRun );
			break;
		case eID_ALL_RUN_TESTS_ACCEPTABLE:
			doRunTests( TestStatus::eAcceptable );
			break;
		case eID_ALL_RUN_TESTS_ALL_BUT_NEGLIGIBLE:
			doRunAllTestsBut( TestStatus::eNegligible );
			break;
		case eID_ALL_RUN_TESTS_OUTDATED:
			doRunAllOutdatedTests();
			break;
		case eID_ALL_UPDATE_CASTOR:
			doUpdateAllCastorDate();
			break;
		case eID_ALL_UPDATE_SCENE:
			doUpdateAllSceneDate();
			break;
		case eID_CANCEL:
			doCancel();
			break;
		}
	}

	void MainFrame::onDatabaseMenuOption( wxCommandEvent & evt )
	{
		switch ( evt.GetId() )
		{
		case eID_NEW_RENDERER:
			doNewRenderer();
			break;
		case eID_NEW_CATEGORY:
			doNewCategory();
			break;
		case eID_NEW_TEST:
			doNewTest();
			break;
		}
	}

	void MainFrame::onProcessEnd( wxProcessEvent & evt )
	{
		castor::Logger::logInfo( castor::makeStringStream() << "Process ended " << evt.GetPid() << "(" << evt.GetExitCode() << ")" );
		onTestProcessEnd( evt.GetPid(), evt.GetExitCode() );
	}

	void MainFrame::onTestUpdateTimer( wxTimerEvent & evt )
	{
		auto testNode = m_runningTest.current();

		if ( !testNode.test )
		{
			return;
		}

		TreeModelNode * node{ testNode.node };
		DatabaseTest * run{ testNode.test};

		if ( isTestNode( *node ) )
		{
			node->test->updateStatusNW( ( node->test->getStatus() == TestStatus::eRunning_End )
				? TestStatus::eRunning_Begin
				: TestStatus( uint32_t( node->test->getStatus() ) + 1 ) );
		}
		else
		{
			node->statusName.status = ( node->statusName.status == TestStatus::eRunning_End )
				? TestStatus::eRunning_Begin
				: TestStatus( uint32_t( node->statusName.status ) + 1 );
		}

		auto page = doGetPage( wxDataViewItem{ node } );

		if ( page )
		{
			page->updateTest( node );
		}

		evt.Skip();
	}

	void MainFrame::onCategoryUpdateTimer( wxTimerEvent & evt )
	{
		evt.Skip();
	}

	void MainFrame::onSize( wxSizeEvent & evt )
	{
		for ( auto & testPageIt : m_testsPages )
		{
			auto page = testPageIt.second;

			if ( page )
			{
				auto size = GetClientSize();
				page->resizeModel( size );
			}
		}

		evt.Skip();
	}

	//*********************************************************************************************
}
