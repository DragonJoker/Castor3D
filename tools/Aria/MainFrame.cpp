#include "Aria/MainFrame.hpp"

#include "Aria/CategoryPanel.hpp"
#include "Aria/LayeredPanel.hpp"
#include "Aria/TestPanel.hpp"
#include "Aria/Aui/AuiDockArt.hpp"
#include "Aria/Aui/AuiTabArt.hpp"
#include "Aria/Database/DbDateTimeHelpers.hpp"
#include "Aria/Database/DbResult.hpp"
#include "Aria/Database/DbStatement.hpp"
#include "Aria/Model/TreeModel.hpp"
#include "Aria/Model/TreeModelNode.hpp"

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Design/BlockGuard.hpp>
#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Miscellaneous/Utils.hpp>

#include <castor.xpm>

#include <wx/clipbrd.h>
#include <wx/dc.h>
#include <wx/gauge.h>
#include <wx/menu.h>
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
		enum ID
		{
			eID_GRID,
			eID_DETAIL,
			eID_TEST_RUN,
			eID_TEST_VIEW,
			eID_TEST_SET_REF,
			eID_TEST_IGNORE_RESULT,
			eID_TEST_UPDATE_CASTOR,
			eID_TEST_UPDATE_SCENE,
			eID_TEST_COPY_FILE_NAME,
			eID_CATEGORY_RUN_TESTS_ALL,
			eID_CATEGORY_RUN_TESTS_NOTRUN,
			eID_CATEGORY_RUN_TESTS_ACCEPTABLE,
			eID_CATEGORY_RUN_TESTS_ALL_BUT_NEGLIGIBLE,
			eID_CATEGORY_RUN_TESTS_OUTDATED,
			eID_CATEGORY_UPDATE_CASTOR,
			eID_CATEGORY_UPDATE_SCENE,
			eID_RENDERER_RUN_TESTS_ALL,
			eID_RENDERER_RUN_TESTS_NOTRUN,
			eID_RENDERER_RUN_TESTS_ACCEPTABLE,
			eID_RENDERER_RUN_TESTS_ALL_BUT_NEGLIGIBLE,
			eID_RENDERER_RUN_TESTS_OUTDATED,
			eID_RENDERER_UPDATE_CASTOR,
			eID_RENDERER_UPDATE_SCENE,
			eID_ALL_RUN_TESTS_ALL,
			eID_ALL_RUN_TESTS_NOTRUN,
			eID_ALL_RUN_TESTS_ACCEPTABLE,
			eID_ALL_RUN_TESTS_ALL_BUT_NEGLIGIBLE,
			eID_ALL_RUN_TESTS_OUTDATED,
			eID_ALL_UPDATE_CASTOR,
			eID_ALL_UPDATE_SCENE,
			eID_CANCEL,
			eID_TESTS_BOOK,
			eID_NEW_RENDERER,
			eID_NEW_CATEGORY,
			eID_NEW_TEST,
		};

		struct View
		{
			enum Type : size_t
			{
				eNone,
				eTest,
				eCategory,
			};
		};

		bool isTestNode( TreeModelNode const & node )
		{
			return node.test != nullptr;
		}

		bool isCategoryNode( TreeModelNode const & node )
		{
			return node.category
				&& node.renderer
				&& !node.isRootNode();
		}

		bool isRendererNode( TreeModelNode const & node )
		{
			return node.renderer
				&& node.isRootNode();
		}

		class DataViewTestBoolBitmapRenderer
			: public wxDataViewCustomRenderer
		{
			static wxString GetDefaultType()
			{
				return wxS( "bool" );
			}

		public:
			DataViewTestBoolBitmapRenderer( char const * const * xpmTrue
				, char const * const * xpmFalse
				, const wxString & varianttype = GetDefaultType()
				, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT
				, int align = wxDVR_DEFAULT_ALIGNMENT )
				: wxDataViewCustomRenderer{ varianttype, mode, align }
				, m_size{ 20, 20 }
				, m_bitmaps{ createImage( xpmFalse )
					, createImage( xpmTrue ) }
			{
			}

			bool SetValue( const wxVariant & value ) override
			{
				m_value = value.GetBool();
				return true;
			}

			bool GetValue( wxVariant & value ) const override
			{
				value = wxVariant{ m_value };
				return true;
			}

			bool Render( wxRect cell, wxDC * dc, int state ) override
			{
				dc->DrawBitmap( m_bitmaps[m_value ? 1u : 0u]
					, cell.x
					, cell.y
					, true );
				return false;
			}

			wxSize GetSize() const override
			{
				return m_size;
			}

		private:
			wxImage createImage( char const * const * xpmData )
			{
				wxImage result{ xpmData };
				return result.Scale( m_size.x, m_size.y );
			}

		private:
			wxSize m_size;
			std::array< wxBitmap, 2 > m_bitmaps;
			bool m_value{ false };
		};

		castor::Path getTestFileName( castor::Path const & folder
			, Test const & test )
		{
			return folder / test.category->name / ( test.name + ".cscn" );
		}

		castor::Path getTestFileName( castor::Path const & folder
			, DatabaseTest const & test )
		{
			return getTestFileName( folder, *test->test );
		}

		void doAddTests( TestRunArray & runs
			, MainFrame::RunningTest & runningTests
			, MainFrame & mainFrame )
		{
			for ( auto & run : runs )
			{
				runningTests.tests.push_back( { &run, run->status, mainFrame.getTestNode( run ) } );
			}
		}

		void doAddTests( TestRunCategoryMap & runs
			, MainFrame::RunningTest & runningTests
			, MainFrame & mainFrame )
		{
			for ( auto & run : runs )
			{
				doAddTests( run.second, runningTests, mainFrame );
			}
		}

		void doAddTests( TestRunMap & runs
			, MainFrame::RunningTest & runningTests
			, MainFrame & mainFrame )
		{
			for ( auto & run : runs )
			{
				doAddTests( run.second, runningTests, mainFrame );
			}
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
		m_mainframe->onTestProcessEnd( pid, status );
	}

	//*********************************************************************************************

	MainFrame::TestProcessChecker::TestProcessChecker( MainFrame * mainFrame )
		: thread{ [this, mainFrame]()
			{
				while ( !isStopped )
				{
					auto current = get();

					if ( current )
					{
						if ( !wxProcess::Exists( current ) )
						{
							castor::Logger::logInfo( castor::makeStringStream() << "Finishing process " << current );
							mainFrame->onTestProcessEnd( current, std::numeric_limits< int >::max() );
						}
					}

					std::this_thread::sleep_for( 1000_ms );
				}
			} }
	{
	}

	void MainFrame::TestProcessChecker::stop()
	{
		isStopped = true;
		thread.join();
	}

	void MainFrame::TestProcessChecker::checkProcess( int pid )
	{
		this->pid = pid;
	}

	int MainFrame::TestProcessChecker::get()
	{
		return pid;
	}

	//*********************************************************************************************

	MainFrame::TestUpdater::TestUpdater( wxObjectDataPtr< TreeModel > & model )
		: thread{ [this, &model]()
			{
				while ( !isStopped )
				{
					auto current = get();
					auto it = current.begin();

					while ( it != current.end() )
					{
						auto node = *it;

						if ( !isRunning( node->test->getStatus() ) )
						{
							it = current.erase( it );
						}
						else
						{
							node->test->updateStatusNW( ( node->test->getStatus() == TestStatus::eRunning_End )
								? TestStatus::eRunning_Begin
								: TestStatus( uint32_t( node->test->getStatus() ) + 1 ) );
							++it;
						}

						model->ItemChanged( wxDataViewItem{ node } );
					}

					set( current );
					std::this_thread::sleep_for( 100_ms );
				}
			} }
	{
	}

	void MainFrame::TestUpdater::stop()
	{
		isStopped = true;
		thread.join();
	}

	void MainFrame::TestUpdater::addTest( TreeModelNode & test )
	{
		auto lock( castor::makeUniqueLock( mutex ) );
		running.push_back( &test );
	}

	std::vector< TreeModelNode * > MainFrame::TestUpdater::get()
	{
		auto lock( castor::makeUniqueLock( mutex ) );
		return running;
	}

	void MainFrame::TestUpdater::set( std::vector< TreeModelNode * > current )
	{
		auto lock( castor::makeUniqueLock( mutex ) );
		running = current;
	}

	//*********************************************************************************************

	MainFrame::TestsPage::TestsPage( Config const & config
		, Renderer renderer
		, TestRunCategoryMap &runs )
		: runs{ &runs }
		, model{ new TreeModel{ config, renderer } }
		, updater{ model }
	{
	}

	MainFrame::TestsPage::~TestsPage()
	{
		updater.stop();
	}

	//*********************************************************************************************

	MainFrame::MainFrame( Config config )
		: wxFrame{ nullptr, wxID_ANY, wxT( "Aria" ), wxDefaultPosition, wxSize( 800, 700 ) }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
		, m_config{ std::move( config ) }
		, m_database{ m_config }
		, m_processChecker{ this }
	{
		SetClientSize( 900, 600 );
		{
			wxProgressDialog progress{ wxT( "Initialising" )
				, wxT( "Initialising..." )
				, int( 1 )
				, this };
			int index = 0;
			m_database.initialise( progress, index );
			m_tests.tests = m_database.listTests( progress, index );

			for ( auto & renderer : m_database.getRenderers() )
			{
				m_tests.runs.emplace( renderer.second.get(), TestRunCategoryMap{} ).first;
			}

			doInitGui();
		}
		doInitMenus();
		doInitMenuBar();
		Bind( wxEVT_CLOSE_WINDOW
			, [this]( wxCloseEvent & evt )
			{
				if ( m_runningTest.difProcess )
				{
					if ( wxProcess::Exists( m_runningTest.disProcess->GetPid() ) )
					{
						wxProcess::Kill( m_runningTest.disProcess->GetPid() );
					}

					if ( wxProcess::Exists( m_runningTest.difProcess->GetPid() ) )
					{
						wxProcess::Kill( m_runningTest.difProcess->GetPid() );
					}

					if ( wxProcess::Exists( m_runningTest.genProcess->GetPid() ) )
					{
						wxProcess::Kill( m_runningTest.genProcess->GetPid() );
					}

					m_runningTest.disProcess->Disconnect( wxEVT_END_PROCESS );
					m_runningTest.difProcess->Disconnect( wxEVT_END_PROCESS );
					m_runningTest.genProcess->Disconnect( wxEVT_END_PROCESS );
					m_runningTest.disProcess = nullptr;
					m_runningTest.difProcess = nullptr;
					m_runningTest.genProcess = nullptr;
				}

				evt.Skip();
			} );
	}

	MainFrame::~MainFrame()
	{
		m_processChecker.stop();
		m_testsPages.clear();
		m_auiManager.UnInit();
	}

	void MainFrame::initialise()
	{
		m_statusText->SetLabel( _( "Initialising..." ) );
		{
			wxProgressDialog progress{ wxT( "Initialising" )
				, wxT( "Initialising..." )
				, int( 1 )
				, this };
			int index = 0;
			doFillLists( progress, index );
		}

		m_runningTest.genProcess = std::make_unique< TestProcess >( this, wxPROCESS_DEFAULT );
		m_runningTest.genProcess->Connect( wxEVT_END_PROCESS
			, wxProcessEventHandler( MainFrame::onProcessEnd )
			, nullptr
			, this );
		m_runningTest.difProcess = std::make_unique< TestProcess >( this, wxPROCESS_DEFAULT );
		m_runningTest.difProcess->Connect( wxEVT_END_PROCESS
			, wxProcessEventHandler( MainFrame::onProcessEnd )
			, nullptr
			, this );
		m_runningTest.disProcess = std::make_unique< TestProcess >( this, wxPROCESS_DEFAULT );
		m_runningTest.disProcess->Connect( wxEVT_END_PROCESS
			, wxProcessEventHandler( MainFrame::onProcessEnd )
			, nullptr
			, this );
		m_statusText->SetLabel( _( "Idle" ) );

		m_categoryView->setAll( m_tests.tests, m_tests.runs, m_runningTest.tests );
		m_detailViews->showLayer( View::eCategory );
	}

	void MainFrame::updateTestStatus( DatabaseTest & test
		, TestStatus newStatus
		, bool reference )
	{
		auto node = getTestNode( test );
		test.updateStatus( newStatus, reference );
		auto & page = doGetPage( wxDataViewItem{ node } );
		page.model->ItemChanged( wxDataViewItem{ node } );

		if ( m_detailViews->isLayerShown( View::eTest )
			&& m_testView->getTest() == &test )
		{
			m_testView->refresh();
		}
	}

	void MainFrame::doInitTestsList( Renderer renderer )
	{
		auto testsIt = m_tests.runs.find( renderer );
		auto it = m_testsPages.find( renderer );

		if ( it == m_testsPages.end() )
		{
			auto page = std::make_unique< TestsPage >( m_config
				, renderer
				, testsIt->second );
			m_testsPages.emplace( renderer
				, std::move( page ) );
			it = m_testsPages.find( renderer );
		}

		auto & testsPage = *it->second;

		wxPanel * listPanel = new wxPanel{ m_testsBook
			, wxID_ANY
			, wxDefaultPosition
			, wxDefaultSize };
		listPanel->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		listPanel->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxBoxSizer * sizerList = new wxBoxSizer{ wxVERTICAL };

		testsPage.view = new wxDataViewCtrl{ listPanel
			, eID_GRID
			, wxDefaultPosition
			, wxDefaultSize
			, wxDV_MULTIPLE };
		// Intently inverted back and fore.
		testsPage.view->SetBackgroundColour( PANEL_FOREGROUND_COLOUR );
		testsPage.view->SetForegroundColour( PANEL_BACKGROUND_COLOUR );
		sizerList->Add( testsPage.view, wxSizerFlags( 1 ).Expand() );
		listPanel->SetSizer( sizerList );
		sizerList->SetSizeHints( listPanel );

		testsPage.view->AssociateModel( testsPage.model.get() );
		testsPage.view->Connect( wxEVT_DATAVIEW_SELECTION_CHANGED
			, wxDataViewEventHandler( MainFrame::onSelectionChange )
			, nullptr
			, this );
		testsPage.view->Connect( wxEVT_DATAVIEW_ITEM_CONTEXT_MENU
			, wxDataViewEventHandler( MainFrame::onItemContextMenu )
			, nullptr
			, this );
		testsPage.model->instantiate( testsPage.view );
		listPanel->SetClientData( &testsPage );
		m_testsBook->AddPage( listPanel, renderer->name );
	}

	void MainFrame::doInitTestsLists()
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

		for ( auto & renderer : m_database.getRenderers() )
		{
			m_tests.runs.emplace( renderer.second.get(), TestRunCategoryMap{} );
		}

		for ( auto & renderer : m_database.getRenderers() )
		{
			doInitTestsList( renderer.second.get() );
		}

		m_testsBook->SetSelection( 0u );
	}

	wxWindow * MainFrame::doInitDetailsView()
	{
		auto size = GetClientSize();
		m_detailViews = new LayeredPanel{ this
			, wxDefaultPosition
			, wxDefaultSize };
		m_detailViews->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_detailViews->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		auto layer = new wxPanel{ m_detailViews, wxID_ANY, wxDefaultPosition, size };
		layer->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		layer->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_detailViews->addLayer( layer );
		m_testView = new TestPanel{ m_detailViews, m_config };
		m_detailViews->addLayer( m_testView );
		m_categoryView = new CategoryPanel{ m_config, m_detailViews, wxDefaultPosition, size };
		m_detailViews->addLayer( m_categoryView );
		m_detailViews->showLayer( View::eNone );

		return m_detailViews;
	}

	void MainFrame::doInitGui()
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		doInitTestsLists();
		auto detailsView = doInitDetailsView();

		auto statusBar = CreateStatusBar();
		statusBar->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		statusBar->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_statusProgress = new wxGauge{ statusBar, wxID_ANY, 100, wxPoint( 10, 3 ), wxSize( 100, statusBar->GetSize().GetHeight() - 6 ), wxGA_SMOOTH, wxDefaultValidator };
		m_statusProgress->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		m_statusProgress->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_statusProgress->SetValue( 0 );
		m_statusProgress->Hide();
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
			.Caption( _( "Tests List" ) )
			.CaptionVisible( true )
			.CloseButton( false )
			.PaneBorder( false )
			.Top()
			.Movable( true )
			.Dockable( true )
			.BottomDockable( true )
			.TopDockable( true ) );
		m_auiManager.AddPane( detailsView
			, wxAuiPaneInfo()
			.Layer( 0 )
			.Caption( _( "Details View" ) )
			.CaptionVisible( true )
			.CloseButton( false )
			.PaneBorder( false )
			.Center()
			.Movable( true )
			.Dockable( true )
			.BottomDockable( true )
			.TopDockable( true ) );
		m_auiManager.Update();
	}

	void MainFrame::doInitMenus( )
	{
		auto addTestBaseMenus = []( wxMenu & menu )
		{
			menu.Append( eID_TEST_COPY_FILE_NAME, _( "Copy test file path" ) + wxT( "\tF2" ) );
			menu.Append( eID_TEST_VIEW, _( "View Test" ) + wxT( "\tF3" ) );
			menu.Append( eID_TEST_SET_REF, _( "Set Reference" ) + wxT( "\tF4" ) );
			menu.Append( eID_TEST_IGNORE_RESULT, _( "Ignore result" ) + wxT( "\tF5" ), wxEmptyString, true );
			menu.Append( eID_TEST_UPDATE_CASTOR, _( "Update Castor3D's date" ) + wxT( "\tF6" ) );
			menu.Append( eID_TEST_UPDATE_SCENE, _( "Update Scene's date" ) + wxT( "\tF7" ) );
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
		auto runsIt = m_tests.runs.find( renderer );
		auto rendIt = m_testsPages.find( renderer );
		m_database.listLatestRuns( renderer
			, m_tests.tests
			, *rendIt->second->runs
			, progress
			, index );
		assert( rendIt != m_testsPages.end() );
		auto & testsPage = *rendIt->second;
		testsPage.runs = &runsIt->second;

		for ( auto & category : *testsPage.runs )
		{
			testsPage.model->addCategory( category.first );

			for ( auto & run : category.second )
			{
				auto testNode = testsPage.model->addTest( run );
				testsPage.modelNodes[run->test->id] = testNode;
				progress.Update( index++
					, _( "Filling tests list" )
					+ wxT( "\n" ) + getProgressDetails( run ) );
			}
		}

		testsPage.model->expandRoots( testsPage.view );
	}

	MainFrame::TestsPage & MainFrame::doGetPage( wxDataViewItem const & item )
	{
		auto node = static_cast< TreeModelNode * >( item.GetID() );
		auto rendIt = m_testsPages.find( node->test->getRenderer() );

		if ( rendIt != m_testsPages.end() )
		{
			return *rendIt->second;
		}

		static TestRunCategoryMap runs;
		static TestsPage dummy{ m_config, nullptr, runs };
		return dummy;
	}

	void MainFrame::doProcessTest()
	{
		if ( !m_cancelled
			&& !m_runningTest.tests.empty() )
		{
			auto & testNode = *m_runningTest.tests.begin();
			auto & test = *testNode.test;
			wxString command = m_config.launcher;
			command << " " << m_config.test / getSceneFile( *test );
			command << " -" << test->renderer->name;
			test.updateStatusNW( TestStatus::eRunning_Begin );
			auto & page = doGetPage( wxDataViewItem{ testNode.node } );
			page.updater.addTest( *testNode.node );
			page.model->ItemChanged( wxDataViewItem{ testNode.node } );
			m_categoryView->update( m_runningTest.tests );
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
				m_processChecker.checkProcess( result );
				m_runningTest.currentProcess = m_runningTest.genProcess.get();
			}

#else
			onTestRunEnd( wxProcessEvent{} );
#endif
			m_testProgress->SetValue( m_testProgress->GetValue() + 1u );
		}
		else
		{
			m_statusText->SetLabel( _( "Idle" ) );
			m_testProgress->Hide();
		}
	}

	void MainFrame::doRunTest()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage
			&& !m_selectedPage->selected.items.empty()
			&& m_runningTest.tests.empty() )
		{
			m_testProgress->SetRange( int( m_selectedPage->selected.items.size() ) );
			m_testProgress->SetValue( 0 );
			m_testProgress->Show( m_selectedPage->selected.items.size() > 1 );

			for ( auto & item : m_selectedPage->selected.items )
			{
				TreeModelNode * node = static_cast< TreeModelNode * >( item.GetID() );

				if ( isTestNode( *node ) )
				{
					m_runningTest.tests.push_back( { node->test, node->test->getStatus(), node } );
				}
			}

			doProcessTest();
		}
	}

	void MainFrame::doCopyTestFileName()
	{
		if ( m_selectedPage
			&& m_selectedPage->selected.items.size() == 1 )
		{
			auto & item = *m_selectedPage->selected.items.begin();
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isTestNode( *node ) && wxTheClipboard->Open() )
			{
				auto guard = castor::makeBlockGuard( [](){ wxTheClipboard->Close(); } );
				wxTheClipboard->SetData( new wxTextDataObject( makeWxString( getTestFileName( m_config.test, *node->test ) ) ) );
			}
		}
	}

	void MainFrame::doViewTest()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage
			&& m_selectedPage->selected.items.size() == 1 )
		{
			auto & item = *m_selectedPage->selected.items.begin();
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isTestNode( *node ) )
			{
				wxString command = m_config.viewer;
				command << " " << makeWxString( getTestFileName( m_config.test, *node->test ) );
				command << " -" << node->test->getRenderer()->name;
				m_statusText->SetLabel( _( "Viewing: " ) + node->test->getName() );
				auto result = wxExecute( command
					, wxEXEC_SYNC
					, m_runningTest.disProcess.get() );

				if ( result != 0 )
				{
					castor::Logger::logError( "doViewTest: " + castor::System::getLastErrorText() );
				}
			}
		}

		m_statusText->SetLabel( _( "Idle" ) );
	}

	void MainFrame::doSetRef()
	{
		m_cancelled.exchange( false );
		wxProgressDialog progress{ wxT( "Updating tests reference" )
			, wxT( "Updating tests reference..." )
			, int( 1 )
			, this };
		int index = 0;

		if ( m_selectedPage
			&& !m_selectedPage->selected.items.empty() )
		{
			progress.SetRange( m_selectedPage->selected.items.size() );

			for ( auto & item : m_selectedPage->selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );

				if ( isTestNode( *node ) )
				{
					auto & run = *node->test;
					progress.Update( index++
						, _( "Setting reference\n" )
						+ wxT( "\n" ) + getProgressDetails( run ) );
					updateTestStatus( *node->test, TestStatus::eNegligible, true );
					auto & page = doGetPage( item );
					page.model->ItemChanged( item );
				}
			}
		}
	}

	void MainFrame::doIgnoreTestResult()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );
		wxProgressDialog progress{ wxT( "Ignoring tests results" )
			, wxT( "Ignoring tests results..." )
			, int( 1 )
			, this };
		int index = 0;

		if ( m_selectedPage
			&& !m_selectedPage->selected.items.empty() )
		{
			progress.SetRange( m_selectedPage->selected.items.size() );

			for ( auto & item : m_selectedPage->selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );

				if ( isTestNode( *node ) )
				{
					auto & run = *node->test;
					progress.Update( index++
						, _( "Ignoring" )
						+ wxT( "\n" ) + getProgressDetails( run ) );
					run.updateIgnoreResult( m_testMenu->IsChecked( eID_TEST_IGNORE_RESULT )
						, m_config.castorRefDate
						, true );
					auto & page = doGetPage( item );
					page.model->ItemChanged( item );
				}
			}
		}
	}

	void MainFrame::doUpdateCastorDate()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );
		wxProgressDialog progress{ wxT( "Setting Castor Date" )
			, wxT( "Setting Castor Date..." )
			, int( 1 )
			, this };
		int index = 0;

		if ( m_selectedPage
			&& !m_selectedPage->selected.items.empty() )
		{
			progress.SetRange( m_selectedPage->selected.items.size() );

			for ( auto & item : m_selectedPage->selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );

				if ( isTestNode( *node ) )
				{
					auto & run = *node->test;
					progress.Update( index++
						, _( "Setting reference" )
						+ wxT( "\n" ) + getProgressDetails( run ) );
					run.updateCastorDate( m_config.castorRefDate );
				}
			}
		}
	}

	void MainFrame::doUpdateSceneDate()
	{
		m_cancelled.exchange( false );
		wxProgressDialog progress{ wxT( "Setting Scene Date" )
			, wxT( "Setting Scene Date..." )
			, int( 1 )
			, this };
		int index = 0;

		if ( m_selectedPage
			&& !m_selectedPage->selected.items.empty() )
		{
			progress.SetRange( m_selectedPage->selected.items.size() );

			for ( auto & item : m_selectedPage->selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );

				if ( isTestNode( *node ) )
				{
					auto & run = *node->test;
					progress.Update( index++
						, _( "Setting reference" )
						+ wxT( "\n" ) + getProgressDetails( run ) );
					run.updateSceneDate();
				}
			}
		}
	}

	TreeModelNode * MainFrame::getTestNode( DatabaseTest const & test )
	{
		auto rendIt = m_testsPages.find( test->renderer );
		assert( rendIt != m_testsPages.end() );
		return rendIt->second->modelNodes[test->test->id];
	}

	void MainFrame::doRunAllCategoryTests()
	{
		m_cancelled.exchange( false );

		for ( auto & item : m_selectedPage->selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isCategoryNode( *node ) )
			{
				auto rendIt = m_tests.runs.find( node->renderer );

				if ( rendIt != m_tests.runs.end() )
				{
					auto catIt = rendIt->second.find( node->category );

					if ( catIt != rendIt->second.end() )
					{
						doAddTests( catIt->second, m_runningTest, *this );
					}
				}
			}
		}

		m_testProgress->SetRange( m_runningTest.tests.size() );
		m_testProgress->SetValue( 0 );
		m_testProgress->Show();
		doProcessTest();
	}

	void MainFrame::doRunCategoryTests( TestStatus filter )
	{
		m_cancelled.exchange( false );

		for ( auto & item : m_selectedPage->selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isCategoryNode( *node ) )
			{
				auto rendIt = m_tests.runs.find( node->renderer );

				if ( rendIt != m_tests.runs.end() )
				{
					auto catIt = rendIt->second.find( node->category );

					if ( catIt != rendIt->second.end() )
					{
						for ( auto & run : catIt->second )
						{
							if ( run->status == filter )
							{
								m_runningTest.tests.push_back( { &run, run->status, getTestNode( run ) } );
							}
						}
					}
				}
			}
		}

		m_testProgress->SetRange( m_runningTest.tests.size() );
		m_testProgress->SetValue( 0 );
		m_testProgress->Show();
		doProcessTest();
	}

	void MainFrame::doRunAllCategoryTestsBut( TestStatus filter )
	{
		m_cancelled.exchange( false );

		for ( auto & item : m_selectedPage->selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isCategoryNode( *node ) )
			{
				auto rendIt = m_tests.runs.find( node->renderer );

				if ( rendIt != m_tests.runs.end() )
				{
					auto catIt = rendIt->second.find( node->category );

					if ( catIt != rendIt->second.end() )
					{
						for ( auto & run : catIt->second )
						{
							if ( run->status != filter )
							{
								m_runningTest.tests.push_back( { &run, run->status, getTestNode( run ) } );
							}
						}
					}
				}
			}
		}

		m_testProgress->SetRange( m_runningTest.tests.size() );
		m_testProgress->SetValue( 0 );
		m_testProgress->Show();
		doProcessTest();
	}

	void MainFrame::doRunAllCategoryOutdatedTests()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );

		for ( auto & item : m_selectedPage->selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isCategoryNode( *node ) )
			{
				auto rendIt = m_tests.runs.find( node->renderer );

				if ( rendIt != m_tests.runs.end() )
				{
					auto catIt = rendIt->second.find( node->category );

					if ( catIt != rendIt->second.end() )
					{
						for ( auto & run : catIt->second )
						{
							if ( isOutOfDate( m_config, *run )
								|| run->status == TestStatus::eNotRun )
							{
								m_runningTest.tests.push_back( { &run, run->status, getTestNode( run ) } );
							}
						}
					}
				}
			}
		}

		m_testProgress->SetRange( m_runningTest.tests.size() );
		m_testProgress->SetValue( 0 );
		m_testProgress->Show();
		doProcessTest();
	}

	void MainFrame::doUpdateCategoryCastorDate()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );
		wxProgressDialog progress{ wxT( "Updating tests Castor3D date" )
			, wxT( "Updating tests..." )
			, int( 1 )
			, this };
		int index = 0;

		for ( auto & item : m_selectedPage->selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isCategoryNode( *node ) )
			{
				auto rendIt = m_tests.runs.find( node->renderer );

				if ( rendIt != m_tests.runs.end() )
				{
					auto catIt = rendIt->second.find( node->category );

					if ( catIt != rendIt->second.end() )
					{
						progress.SetRange( progress.GetRange() + catIt->second.size() );

						for ( auto & run : catIt->second )
						{
							progress.Update( index++
								, _( "Updating tests Castor3D date" )
								+ wxT( "\n" ) + getProgressDetails( run ) );

							if ( isOutOfCastorDate( m_config, *run ) )
							{
								run.updateCastorDate( m_config.castorRefDate );
							}
						}
					}
				}
			}
		}
	}

	void MainFrame::doUpdateCategorySceneDate()
	{
		m_cancelled.exchange( false );
		wxProgressDialog progress{ wxT( "Updating tests Scene date" )
			, wxT( "Updating tests..." )
			, int( 1 )
			, this };
		int index = 0;

		for ( auto & item : m_selectedPage->selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isCategoryNode( *node ) )
			{
				auto rendIt = m_tests.runs.find( node->renderer );

				if ( rendIt != m_tests.runs.end() )
				{
					auto catIt = rendIt->second.find( node->category );

					if ( catIt != rendIt->second.end() )
					{
						progress.SetRange( progress.GetRange() + catIt->second.size() );

						for ( auto & run : catIt->second )
						{
							auto sceneDate = getSceneDate( m_config, *run );
							progress.Update( index++
								, _( "Updating tests Scene date" )
								+ wxT( "\n" ) + getProgressDetails( run ) );

							if ( isOutOfSceneDate( m_config, *run ) )
							{
								run.updateSceneDate( sceneDate );
							}
						}
					}
				}
			}
		}
	}

	void MainFrame::doRunAllRendererTests()
	{
		m_cancelled.exchange( false );

		for ( auto & item : m_selectedPage->selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isRendererNode( *node ) )
			{
				auto rendIt = m_tests.runs.find( node->renderer );

				if ( rendIt != m_tests.runs.end() )
				{
					for ( auto & category : rendIt->second )
					{
						for ( auto & run : category.second )
						{
							m_runningTest.tests.push_back( { &run, run->status, getTestNode( run ) } );
						}
					}
				}
			}
		}

		m_testProgress->SetRange( m_runningTest.tests.size() );
		m_testProgress->SetValue( 0 );
		m_testProgress->Show();
		doProcessTest();
	}

	void MainFrame::doRunRendererTests( TestStatus filter )
	{
		m_cancelled.exchange( false );

		for ( auto & item : m_selectedPage->selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isRendererNode( *node ) )
			{
				auto rendIt = m_tests.runs.find( node->renderer );

				if ( rendIt != m_tests.runs.end() )
				{
					for ( auto & category : rendIt->second )
					{
						for ( auto & run : category.second )
						{
							if ( run->status == filter )
							{
								m_runningTest.tests.push_back( { &run, run->status, getTestNode( run ) } );
							}
						}
					}
				}
			}
		}

		m_testProgress->SetRange( m_runningTest.tests.size() );
		m_testProgress->SetValue( 0 );
		m_testProgress->Show();
		doProcessTest();
	}

	void MainFrame::doRunAllRendererTestsBut( TestStatus filter )
	{
		m_cancelled.exchange( false );

		for ( auto & item : m_selectedPage->selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isRendererNode( *node ) )
			{
				auto rendIt = m_tests.runs.find( node->renderer );

				if ( rendIt != m_tests.runs.end() )
				{
					for ( auto & category : rendIt->second )
					{
						for ( auto & run : category.second )
						{
							if ( run->status != filter )
							{
								m_runningTest.tests.push_back( { &run, run->status, getTestNode( run ) } );
							}
						}
					}
				}
			}
		}

		m_testProgress->SetRange( m_runningTest.tests.size() );
		m_testProgress->SetValue( 0 );
		m_testProgress->Show();
		doProcessTest();
	}

	void MainFrame::doRunAllRendererOutdatedTests()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );

		for ( auto & item : m_selectedPage->selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isRendererNode( *node ) )
			{
				auto rendIt = m_tests.runs.find( node->renderer );

				if ( rendIt != m_tests.runs.end() )
				{
					for ( auto & category : rendIt->second )
					{
						for ( auto & run : category.second )
						{
							if ( isOutOfDate( m_config, *run )
								|| run->status == TestStatus::eNotRun )
							{
								m_runningTest.tests.push_back( { &run, run->status, getTestNode( run ) } );
							}
						}
					}
				}
			}
		}

		m_testProgress->SetRange( m_runningTest.tests.size() );
		m_testProgress->SetValue( 0 );
		m_testProgress->Show();
		doProcessTest();
	}

	void MainFrame::doUpdateRendererCastorDate()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );
		wxProgressDialog progress{ wxT( "Updating tests Castor3D date" )
			, wxT( "Updating tests..." )
			, int( 1 )
			, this };
		int index = 0;

		for ( auto & item : m_selectedPage->selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isRendererNode( *node ) )
			{
				auto rendIt = m_tests.runs.find( node->renderer );

				if ( rendIt != m_tests.runs.end() )
				{
					for ( auto & category : rendIt->second )
					{
						progress.SetRange( progress.GetRange() + category.second.size() );

						for ( auto & run : category.second )
						{
							progress.Update( index++
								, _( "Updating tests Castor3D date" )
								+ wxT( "\n" ) + getProgressDetails( run ) );

							if ( isOutOfCastorDate( m_config, *run ) )
							{
								run.updateCastorDate( m_config.castorRefDate );
							}
						}
					}
				}
			}
		}
	}

	void MainFrame::doUpdateRendererSceneDate()
	{
		m_cancelled.exchange( false );
		wxProgressDialog progress{ wxT( "Updating tests Scene date" )
			, wxT( "Updating tests..." )
			, int( 1 )
			, this };
		int index = 0;

		for ( auto & item : m_selectedPage->selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( isRendererNode( *node ) )
			{
				auto rendIt = m_tests.runs.find( node->renderer );

				if ( rendIt != m_tests.runs.end() )
				{
					for ( auto & category : rendIt->second )
					{
						progress.SetRange( progress.GetRange() + category.second.size() );

						for ( auto & run : category.second )
						{
							auto sceneDate = getSceneDate( m_config, *run );
							progress.Update( index++
								, _( "Updating tests Scene date" )
								+ wxT( "\n" ) + getProgressDetails( run ) );

							if ( isOutOfSceneDate( m_config, *run ) )
							{
								run.updateSceneDate( sceneDate );
							}
						}
					}
				}
			}
		}
	}

	void MainFrame::doRunAllTests()
	{
		m_cancelled.exchange( false );
		doAddTests( m_tests.runs, m_runningTest, *this );

		m_testProgress->SetRange( m_runningTest.tests.size() );
		m_testProgress->SetValue( 0 );
		m_testProgress->Show();
		doProcessTest();
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
						m_runningTest.tests.push_back( { &run, run->status, getTestNode( run ) } );
					}
				}
			}
		}

		m_testProgress->SetRange( m_runningTest.tests.size() );
		m_testProgress->SetValue( 0 );
		m_testProgress->Show();
		doProcessTest();
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
						m_runningTest.tests.push_back( { &run, run->status, getTestNode( run ) } );
					}
				}
			}
		}

		m_testProgress->SetRange( m_runningTest.tests.size() );
		m_testProgress->SetValue( 0 );
		m_testProgress->Show();
		doProcessTest();
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
						m_runningTest.tests.push_back( { &run, run->status, getTestNode( run ) } );
					}
				}
			}
		}

		m_testProgress->SetRange( m_runningTest.tests.size() );
		m_testProgress->SetValue( 0 );
		m_testProgress->Show();
		doProcessTest();
	}

	void MainFrame::doUpdateAllCastorDate()
	{
		m_cancelled.exchange( false );
		updateCastorRefDate( m_config );
		wxProgressDialog progress{ wxT( "Updating tests Castor3D date" )
			, wxT( "Updating tests..." )
			, int( 1 )
			, this };
		int index = 0;

		for ( auto & renderer : m_tests.runs )
		{
			for ( auto & category : renderer.second )
			{
				progress.SetRange( progress.GetRange() + category.second.size() );

				for ( auto & run : category.second )
				{
					progress.Update( index++
						, _( "Updating tests Castor3D date" )
						+ wxT( "\n" ) + getProgressDetails( run ) );

					if ( isOutOfCastorDate( m_config, *run ) )
					{
						run.updateCastorDate( m_config.castorRefDate );
					}
				}
			}
		}
	}

	void MainFrame::doUpdateAllSceneDate()
	{
		m_cancelled.exchange( false );
		wxProgressDialog progress{ wxT( "Updating tests Scene date" )
			, wxT( "Updating tests..." )
			, int( 1 )
			, this };
		int index = 0;

		for ( auto & renderer : m_tests.runs )
		{
			for ( auto & category : renderer.second )
			{
				progress.SetRange( progress.GetRange() + category.second.size() );

				for ( auto & run : category.second )
				{
					auto sceneDate = getSceneDate( m_config, *run );
					progress.Update( index++
						, _( "Updating tests Scene date" )
						+ wxT( "\n" ) + getProgressDetails( run ) );

					if ( isOutOfSceneDate( m_config, *run ) )
					{
						run.updateSceneDate( sceneDate );
					}
				}
			}
		}
	}

	void MainFrame::doCancelTest( DatabaseTest & test
		, TestStatus status )
	{
		test.updateStatusNW( status );
		wxDataViewItem item = wxDataViewItem{ getTestNode( test ) };
		auto & page = doGetPage( item );
		page.model->ItemChanged( item );
		m_runningTest.tests.clear();
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
				wxProgressDialog progress{ wxT( "Creating renderer entries" )
					, wxT( "Creating renderer entries..." )
					, int( 1 )
					, this };
				int index = 0;
				doInitTestsList( renderer );
				doFillList( renderer, progress, index );
			}
		}
	}

	void MainFrame::doNewCategory()
	{
	}

	void MainFrame::doNewTest()
	{
	}

	void MainFrame::onTestRunEnd( int status )
	{
		auto & testNode = *m_runningTest.tests.begin();
		auto & run = *testNode.test;

		if ( status < 0 && status != std::numeric_limits< int >::max() )
		{
			m_runningTest.tests.erase( m_runningTest.tests.begin() );
			castor::Logger::logError( castor::makeStringStream() << "Test run failed (" << status << ")" );
			//doCancel();
		}

		if ( !m_cancelled )
		{
			wxString command = m_config.differ;
			command << " " << run->renderer->name;
			command << " -f " << ( m_config.test / run.getCategory()->name / ( run.getName() + ".cscn" ) );
			m_runningTest.difProcess->Redirect();
			auto result = wxExecute( command
				, ExecMode
				, m_runningTest.difProcess.get() );
#if CTP_UseAsync

			if ( result == 0 )
			{
				castor::Logger::logError( "doProcessTest: " + castor::System::getLastErrorText() );
			}
			else
			{
				m_processChecker.checkProcess( result );
				m_runningTest.currentProcess = m_runningTest.difProcess.get();
			}

#else
			onTestDiffEnd( wxProcessEvent{} );
#endif
		}
		else
		{
			doCancelTest( *testNode.test, testNode.status );
		}
	}

	void MainFrame::onTestDisplayEnd( int status )
	{
	}

	void MainFrame::onTestDiffEnd( int status )
	{
		auto testNode = *m_runningTest.tests.begin();
		m_runningTest.tests.erase( m_runningTest.tests.begin() );
		auto & test = *testNode.test;

		if ( !m_cancelled )
		{
			auto matches = castor::File::filterDirectoryFiles( m_config.test / getCompareFolder( *test->test )
				, [&test]( castor::Path const & folder, castor::String name )
				{
					return name.find( test.getName() ) == 0u
						&& castor::Path{ name }.getExtension() == "png"
						&& name.find( "diff.png" ) == castor::String::npos;
				}
			, true );
			assert( matches.size() < 2 );

			if ( matches.empty() )
			{
				return;
			}

			auto & match = matches[0];
			test.createNewRun( match );

			if ( !test->test->ignoreResult )
			{
				wxDataViewItem item{ testNode.node };
				auto & page = doGetPage( item );
				page.model->ItemChanged( item );

				if ( m_detailViews->isLayerShown( View::eTest )
					&& m_testView->getTest() == &test )
				{
					m_testView->setTest( test );
				}
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
		m_processChecker.checkProcess( 0 );
		auto currentProcess = m_runningTest.currentProcess;
		m_runningTest.currentProcess = nullptr;

		if ( currentProcess )
		{
			if ( currentProcess == m_runningTest.difProcess.get() )
			{
				onTestDiffEnd( status );
			}
			else if ( currentProcess == m_runningTest.disProcess.get() )
			{
				onTestDisplayEnd( status );
			}
			else if ( currentProcess == m_runningTest.genProcess.get() )
			{
				onTestRunEnd( status );
			}
		}
	}

	void MainFrame::onTestsPageChange( wxAuiNotebookEvent & evt )
	{
		m_selectedPage = reinterpret_cast< TestsPage * >( m_testsBook->GetPage( m_testsBook->GetSelection() )->GetClientData() );
	}

	void MainFrame::onSelectionChange( wxDataViewEvent & evt )
	{
		auto wasDisplayingTest = m_detailViews->isLayerShown( View::eTest );
		auto wasDisplayingCategory = m_detailViews->isLayerShown( View::eCategory );
		m_selectedPage->selected.allTests = true;
		m_selectedPage->selected.allCategories = true;
		m_selectedPage->selected.allRenderers = true;
		m_selectedPage->view->GetSelections( m_selectedPage->selected.items );
		bool displayTest = false;
		bool displayCategory = false;

		if ( m_selectedPage->selected.items.size() == 1 )
		{
			TreeModelNode * node = static_cast< TreeModelNode * >( m_selectedPage->selected.items[0].GetID() );

			if ( node )
			{
				Category category{};
				Renderer renderer{};
				DatabaseTest * test{};

				if ( node->test )
				{
					test = node->test;
				}
				else if ( node->isRootNode() )
				{
					renderer = node->renderer;
				}
				else if ( node->GetParent() )
				{
					renderer = node->renderer;
					category = node->category;
				}

				if ( test )
				{
					m_testView->setTest( *test );
					m_detailViews->showLayer( View::eTest );
					displayTest = true;
					m_selectedPage->selected.allTests = true;
				}
				else if ( category )
				{
					auto rendRunIt = m_tests.runs.find( renderer );

					if ( rendRunIt != m_tests.runs.end() )
					{
						auto catTestIt = m_tests.tests.find( category );
						auto catRunIt = rendRunIt->second.find( category );

						if ( catRunIt != rendRunIt->second.end() )
						{
							m_categoryView->setCategory( category->name
								, catTestIt->second
								, catRunIt->second
								, m_runningTest.tests );
						}
						else
						{
							m_categoryView->setRenderer( renderer->name
								, m_tests.tests
								, rendRunIt->second
								, m_runningTest.tests );
						}
					}
					else
					{
						m_categoryView->setAll( m_tests.tests
							, m_tests.runs
							, m_runningTest.tests );
					}

					m_detailViews->showLayer( View::eCategory );
					displayCategory = true;
				}
				else if ( renderer )
				{
					auto rendRunIt = m_tests.runs.find( renderer );

					if ( rendRunIt != m_tests.runs.end() )
					{
						m_categoryView->setRenderer( renderer->name
							, m_tests.tests
							, rendRunIt->second
							, m_runningTest.tests );
					}
					else
					{
						m_categoryView->setAll( m_tests.tests
							, m_tests.runs
							, m_runningTest.tests );
					}

					m_detailViews->showLayer( View::eCategory );
					displayCategory = true;
				}
				else
				{
					m_categoryView->setAll( m_tests.tests
						, m_tests.runs
						, m_runningTest.tests );
					m_detailViews->showLayer( View::eCategory );
					displayCategory = true;
				}
			}
		}

		for ( auto & item : m_selectedPage->selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			m_selectedPage->selected.allRenderers = isRendererNode( *node )
				&& m_selectedPage->selected.allRenderers;
			m_selectedPage->selected.allCategories = isCategoryNode( *node )
				&& m_selectedPage->selected.allCategories;
			m_selectedPage->selected.allTests = node->test
				&& m_selectedPage->selected.allTests;
		}

		if ( !displayTest && !displayCategory )
		{
			m_detailViews->hideLayers();
		}

		if ( wasDisplayingCategory != displayCategory
			|| wasDisplayingTest != displayTest )
		{
			m_auiManager.Update();
		}
	}

	void MainFrame::onItemContextMenu( wxDataViewEvent & evt )
	{
		if ( m_selectedPage
			&& ( !m_selectedPage->selected.items.empty() )
			&& m_runningTest.tests.empty() )
		{
			if ( m_selectedPage->selected.allTests )
			{
				if ( m_selectedPage->selected.items.size() <= 1 )
				{
					m_testMenu->Enable( eID_TEST_IGNORE_RESULT, true );
					m_testMenu->Check( eID_TEST_IGNORE_RESULT, static_cast< TreeModelNode * >( m_selectedPage->selected.items.front().GetID() )->test->getIgnoreResult() );
				}
				else
				{
					m_testMenu->Enable( eID_TEST_IGNORE_RESULT, false );
					m_testMenu->Check( eID_TEST_IGNORE_RESULT, false );
				}

				PopupMenu( m_testMenu.get() );
			}
			else if ( m_selectedPage->selected.allCategories )
			{
				PopupMenu( m_categoryMenu.get() );
			}
			else if ( m_selectedPage->selected.allRenderers )
			{
				PopupMenu( m_rendererMenu.get() );
			}
			else
			{
				PopupMenu( m_allMenu.get() );
			}
		}
		else
		{
			PopupMenu( m_busyMenu.get() );
		}
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

	//*********************************************************************************************
}
