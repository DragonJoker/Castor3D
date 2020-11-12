#include "CastorTestParser/MainFrame.hpp"

#include "CastorTestParser/CategoryPanel.hpp"
#include "CastorTestParser/LayeredPanel.hpp"
#include "CastorTestParser/TestPanel.hpp"
#include "CastorTestParser/Aui/AuiDockArt.hpp"
#include "CastorTestParser/Aui/AuiTabArt.hpp"
#include "CastorTestParser/Database/DbDateTimeHelpers.hpp"
#include "CastorTestParser/Database/DbResult.hpp"
#include "CastorTestParser/Database/DbStatement.hpp"
#include "CastorTestParser/Model/TreeModel.hpp"
#include "CastorTestParser/Model/TreeModelNode.hpp"

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Miscellaneous/Utils.hpp>

#include <castor.xpm>

#include <wx/dc.h>
#include <wx/menu.h>
#include <wx/progdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#define CTP_UseAsync 1

namespace test_parser
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
			eID_RUN_TEST,
			eID_VIEW_TEST,
			eID_SET_REF,
			eID_IGNORE_RESULT,
			eID_RUN_CATEGORY_TESTS_ALL,
			eID_RUN_CATEGORY_TESTS_NOTRUN,
			eID_RUN_CATEGORY_TESTS_ACCEPTABLE,
			eID_RUN_CATEGORY_TESTS_ALL_BUT_NEGLIGIBLE,
			eID_RUN_TESTS_ALL,
			eID_RUN_TESTS_NOTRUN,
			eID_RUN_TESTS_ACCEPTABLE,
			eID_RUN_TESTS_ALL_BUT_NEGLIGIBLE,
			eID_CANCEL,
		};

		TreeModelNode * addCategory( TreeModel & model
			, std::string const & category )
		{
			TreeModelNode * node = new TreeModelNode{ nullptr, makeWxString( category ) };
			model.GetRootNode()->Append( node );
			return node;
		}

		TreeModelNode * addTest( TreeModel & model
			, TreeModelNode * category
			, Test & test )
		{
			TreeModelNode * node = new TreeModelNode{ category, test };
			category->Append( node );
			return node;
		}

		class DataViewAnimatedBitmapRenderer : public wxDataViewRenderer
		{
		public:
			static wxString GetDefaultType()
			{
				return wxS( "wxBitmap" );
			}

			DataViewAnimatedBitmapRenderer( const wxString & varianttype = GetDefaultType(),
				wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
				int align = wxDVR_DEFAULT_ALIGNMENT )
				: wxDataViewRenderer{ varianttype, mode, align }
			{
			}

			bool SetValue( const wxVariant & value ) override
			{
				m_bitmap << value;
				return true;
			}

			bool GetValue( wxVariant & value ) const override
			{
				value << m_bitmap;
				return true;
			}

			bool Render( wxRect cell, wxDC * dc, int state ) override
			{
				dc->DrawBitmap( m_bitmap
					, cell.x
					, cell.y
					, true );
				return true;
			}

			wxSize GetSize() const override
			{
				return m_bitmap.GetSize();
			}

		private:
			wxIcon m_icon;
			wxBitmap m_bitmap;
		};
	}

	//*********************************************************************************************

	MainFrame::MainFrame( Config config )
		: wxFrame{ nullptr, wxID_ANY, wxT( "Castor3D Tests Parser" ), wxDefaultPosition, wxSize( 800, 700 ) }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
		, m_config{ std::move( config ) }
		, m_database{ m_config }
	{
		SetClientSize( 800, 600 );
		doInitGui();
		doInitMenus();
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
		m_auiManager.UnInit();
	}

	void MainFrame::initialise()
	{
		m_statusBar->SetLabel( _( "Initialising..." ) );
		{
			wxProgressDialog progress{ wxT( "Initialising" )
				, wxT( "Initialising..." )
				, int( 1 )
				, this };
			int index = 0;
			m_database.initialise( progress, index );
			uint32_t testCount;
			m_tests = m_database.listLatestTests( testCount, progress, index );
			doFillList( progress, index, testCount );
		}

		m_runningTest.genProcess = std::make_unique< wxProcess >( wxPROCESS_DEFAULT );
		m_runningTest.genProcess->Connect( wxEVT_END_PROCESS
			, wxProcessEventHandler( MainFrame::onTestRunEnd )
			, nullptr
			, this );
		m_runningTest.difProcess = std::make_unique< wxProcess >( wxPROCESS_DEFAULT );
		m_runningTest.difProcess->Connect( wxEVT_END_PROCESS
			, wxProcessEventHandler( MainFrame::onTestDiffEnd )
			, nullptr
			, this );
		m_runningTest.disProcess = std::make_unique< wxProcess >( wxPROCESS_DEFAULT );
		m_runningTest.disProcess->Connect( wxEVT_END_PROCESS
			, wxProcessEventHandler( MainFrame::onTestDisplayEnd )
			, nullptr
			, this );
		m_statusBar->SetLabel( _( "Idle" ) );
	}

	void MainFrame::updateTestStatus( Test & test
		, TestStatus newStatus
		, bool reference )
	{
		auto node = getTestNode( test );
		m_database.updateTestStatus( test, newStatus, reference );
		test.status = newStatus;
		m_model->ItemChanged( wxDataViewItem{ node } );

		if ( m_detailViews->isLayerShown( 1 )
			&& m_testView->getTest() == &test )
		{
			m_testView->setTest( test );
		}
	}

	wxWindow * MainFrame::doInitTestsList()
	{
		wxPanel * listPanel = new wxPanel{ this
			, wxID_ANY
			, wxDefaultPosition
			, wxDefaultSize };
		listPanel->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		listPanel->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxBoxSizer * sizerList = new wxBoxSizer{ wxVERTICAL };
		m_view = new wxDataViewCtrl{ listPanel
			, eID_GRID
			, wxDefaultPosition
			, wxDefaultSize
			, wxDV_MULTIPLE };
		// Intently inverted back and fore.
		m_view->SetBackgroundColour( PANEL_FOREGROUND_COLOUR );
		m_view->SetForegroundColour( PANEL_BACKGROUND_COLOUR );
		sizerList->Add( m_view, wxSizerFlags( 1 ).Expand() );
		listPanel->SetSizer( sizerList );
		sizerList->SetSizeHints( listPanel );
		m_model = new TreeModel;
		m_view->AssociateModel( m_model.get() );
		m_view->Connect( wxEVT_DATAVIEW_SELECTION_CHANGED
			, wxDataViewEventHandler( MainFrame::onSelectionChange )
			, nullptr
			, this );
		m_view->Connect( wxEVT_DATAVIEW_ITEM_CONTEXT_MENU
			, wxDataViewEventHandler( MainFrame::onItemContextMenu )
			, nullptr
			, this );
		uint32_t flags = wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE;
		auto renCategory = new wxDataViewTextRenderer{ wxDataViewTextRenderer::GetDefaultType() };
		wxDataViewColumn * colCategory = new wxDataViewColumn( _( "Category" ), renCategory, int( TreeModel::Column::eCategory ), 200, wxALIGN_LEFT, flags );
		colCategory->SetMinWidth( 150 );
		m_view->AppendColumn( colCategory );
		auto renScene = new wxDataViewTextRenderer{ wxDataViewTextRenderer::GetDefaultType() };
		wxDataViewColumn * colScene = new wxDataViewColumn( _( "Name" ), renScene, int( TreeModel::Column::eName ), 400, wxALIGN_LEFT, flags );
		colScene->SetMinWidth( 400 );
		m_view->AppendColumn( colScene );
		auto renRenderer = new wxDataViewTextRenderer{ wxDataViewTextRenderer::GetDefaultType() };
		wxDataViewColumn * colRenderer = new wxDataViewColumn( _( "Renderer" ), renRenderer, int( TreeModel::Column::eRenderer ), 40, wxALIGN_LEFT, flags );
		colRenderer->SetMinWidth( 40 );
		m_view->AppendColumn( colRenderer );
		auto renRunDate = new wxDataViewDateRenderer{ wxDataViewDateRenderer::GetDefaultType() };
		wxDataViewColumn * colRunDate = new wxDataViewColumn( _( "Run Date" ), renRunDate, int( TreeModel::Column::eRunDate ), 100, wxALIGN_LEFT, flags );
		colRunDate->SetMinWidth( 100 );
		m_view->AppendColumn( colRunDate );
		auto renStatus = new DataViewAnimatedBitmapRenderer{ wxDataViewBitmapRenderer::GetDefaultType() };
		wxDataViewColumn * colStatus = new wxDataViewColumn( _( "Status" ), renStatus, int( TreeModel::Column::eStatus ), 100, wxALIGN_LEFT, flags );
		colStatus->SetMinWidth( 100 );
		m_view->AppendColumn( colStatus );

		return listPanel;
	}

	wxWindow * MainFrame::doInitDetailsView()
	{
		m_detailViews = new LayeredPanel{ this
			, wxDefaultPosition
			, wxDefaultSize };
		m_detailViews->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_detailViews->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		auto layer = new wxPanel{ m_detailViews, wxID_ANY, wxDefaultPosition, wxSize{ 800, 600 } };
		layer->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		layer->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_detailViews->addLayer( layer );
		m_testView = new TestPanel{ m_detailViews, m_config };
		m_detailViews->addLayer( m_testView );
		m_categoryView = new CategoryPanel{ m_detailViews, wxDefaultPosition, wxSize{ 800, 600 } };
		m_detailViews->addLayer( m_categoryView );
		m_detailViews->showLayer( 0 );

		return m_detailViews;
	}

	void MainFrame::doInitGui()
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		auto testsList = doInitTestsList();
		auto detailsView = doInitDetailsView();

		auto statusBar = CreateStatusBar();
		statusBar->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		statusBar->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_statusBar = new wxStaticText{ statusBar, wxID_ANY, _( "Idle" ), wxPoint( 10, 5 ), wxDefaultSize, 0 };
		m_statusBar->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		m_statusBar->SetForegroundColour( PANEL_FOREGROUND_COLOUR );


		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( testsList
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

	void MainFrame::doInitMenus()
	{
		auto addTestMenus = []( wxMenu & menu )
		{
			menu.Append( eID_VIEW_TEST, _( "View Test" ) + wxT( "\tF5" ) );
			menu.Append( eID_SET_REF, _( "Set Reference" ) + wxT( "\tF6" ) );
			menu.Append( eID_RUN_TEST, _( "Run Test" ) + wxT( "\tF7" ) );
			menu.Append( eID_IGNORE_RESULT, _( "Ignore test results" ) + wxT( "\tF8" ), wxEmptyString, wxITEM_CHECK );
		};
		auto addCategoryMenus = []( wxMenu & menu )
		{
			menu.Append( eID_RUN_CATEGORY_TESTS_ALL, _( "Run all category's tests" ) + wxT( "\tCtrl+F7" ) );
			menu.Append( eID_RUN_CATEGORY_TESTS_NOTRUN, _( "Run all <not run> category's tests" ) + wxT( "\tCtrl+F8" ) );
			menu.Append( eID_RUN_CATEGORY_TESTS_ACCEPTABLE, _( "Run all <acceptable> category's tests" ) + wxT( "\tCtrl+F9" ) );
			menu.Append( eID_RUN_CATEGORY_TESTS_ALL_BUT_NEGLIGIBLE, _( "Run all but <negligible> category's tests" ) + wxT( "\tCtrl+F10" ) );
		};
		auto addAllMenus = []( wxMenu & menu )
		{
			menu.Append( eID_RUN_TESTS_ALL, _( "Run all tests" ) + wxT( "\tCtrl+Alt+F7" ) );
			menu.Append( eID_RUN_TESTS_NOTRUN, _( "Run all <not run> tests" ) + wxT( "\tCtrl+Alt+F8" ) );
			menu.Append( eID_RUN_TESTS_ACCEPTABLE, _( "Run all <acceptable> tests" ) + wxT( "\tCtrl+Alt+F9" ) );
			menu.Append( eID_RUN_TESTS_ALL_BUT_NEGLIGIBLE, _( "Run all but <negligible> tests" ) + wxT( "\tCtrl+F10" ) );
		};
		m_testMenu = std::make_unique< wxMenu >();
		addTestMenus( *m_testMenu );
		m_testMenu->AppendSeparator();
		addCategoryMenus( *m_testMenu );
		m_testMenu->AppendSeparator();
		addAllMenus( *m_testMenu );
		m_testMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onMenuOption )
			, nullptr
			, this );

		m_categoryMenu = std::make_unique< wxMenu >();
		addCategoryMenus( *m_categoryMenu );
		m_categoryMenu->AppendSeparator();
		addAllMenus( *m_categoryMenu );
		m_categoryMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onMenuOption )
			, nullptr
			, this );

		m_allMenu = std::make_unique< wxMenu >();
		addAllMenus( *m_allMenu );
		m_allMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onMenuOption )
			, nullptr
			, this );

		m_busyMenu = std::make_unique< wxMenu >();
		m_busyMenu->Append( eID_VIEW_TEST, "View Test\tF5" );
		m_busyMenu->Append( eID_CANCEL, "Cancel\tF11" );
		m_busyMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onMenuOption )
			, nullptr
			, this );
	}

	void MainFrame::doFillList( wxProgressDialog & progress
		, int & index
		, uint32_t testCount )
	{
		castor::Logger::logInfo( "Filling Data View" );
		progress.SetTitle( _( "Filling Data View" ) );
		progress.SetRange( progress.GetRange() + int( testCount ) );
		progress.Update( index, _( "Filling Data View..." ) );

		for ( auto & tests : m_tests )
		{
			auto node = addCategory( *m_model, tests.first );

			for ( auto & test : tests.second )
			{
				auto testNode = addTest( *m_model, node, test );
				m_modelNodes[test.id] = testNode;
				progress.Update( index++, makeWxString( test.category ) + wxT( " - " ) + makeWxString( test.name ) + wxT( "..." ) );
			}
		}

		m_view->Expand( wxDataViewItem{ m_model->GetRootNode() } );
	}

	void MainFrame::doProcessTest()
	{
		if ( !m_cancelled
			&& !m_runningTest.tests.empty() )
		{
			auto & testNode = *m_runningTest.tests.begin();
			auto & test = *testNode.test;
			wxString command = m_config.launcher;
			command << " " << ( m_config.test / test.category / ( test.name + ".cscn" ) );
			command << " -" << test.renderer;
			test.status = TestStatus::eRunning;
			m_model->ItemChanged( wxDataViewItem{ testNode.node } );
			m_statusBar->SetLabel( _( "Running test: " ) + test.name );
			auto result = wxExecute( command
				, ExecMode
				, m_runningTest.genProcess.get() );
#if CTP_UseAsync

			if ( result == 0 )
			{
				castor::Logger::logError( "doProcessTest: " + castor::System::getLastErrorText() );
			}

#else
			onTestRunEnd( wxProcessEvent{} );
#endif
		}
		else
		{
			m_statusBar->SetLabel( _( "Idle" ) );
		}
	}

	void MainFrame::doRunTest()
	{
		m_cancelled.exchange( false );

		if ( !m_selected.items.empty()
			&& m_runningTest.tests.empty() )
		{
			for ( auto & item : m_selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );
				assert( node->test );
				m_runningTest.tests.push_back( { node->test, node->test->status, node } );
			}

			doProcessTest();
		}
	}

	void MainFrame::doViewTest()
	{
		m_cancelled.exchange( false );

		if ( m_selected.items.size() == 1 )
		{
			auto & item = *m_selected.items.begin();
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			wxString command = m_config.viewer;
			command << " " << ( m_config.test / node->test->category / ( node->test->name + ".cscn" ) );
			command << " -" << node->test->renderer;
			auto result = wxExecute( command
				, wxEXEC_SYNC
				, m_runningTest.disProcess.get() );

			if ( result != 0 )
			{
				castor::Logger::logError( "doViewTest: " + castor::System::getLastErrorText() );
			}
		}
	}

	void MainFrame::doSetRef()
	{
		m_cancelled.exchange( false );

		if ( !m_selected.items.empty() )
		{
			for ( auto & item : m_selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );
				assert( node->test );
				m_statusBar->SetLabel( _( "Setting reference: " ) + node->test->name );
				updateTestStatus( *node->test, TestStatus::eNegligible, true );
				m_model->ItemChanged( item );
			}
		}

		m_statusBar->SetLabel( _( "Idle" ) );
	}

	void MainFrame::doIgnoreTestResult()
	{
		m_cancelled.exchange( false );

		if ( !m_selected.items.empty() )
		{
			for ( auto & item : m_selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );
				assert( node->test );
				m_statusBar->SetLabel( _( "Setting reference: " ) + node->test->name );
				node->test->ignoreResult = m_testMenu->IsChecked( eID_IGNORE_RESULT );
				m_database.updateTestIgnoreResult( *node->test, node->test->ignoreResult, true );
				m_model->ItemChanged( item );
			}
		}
	}

	TreeModelNode * MainFrame::getTestNode( Test const & test )
	{
		return m_modelNodes[test.id];
	}

	void MainFrame::doRunAllCategoryTests()
	{
		m_cancelled.exchange( false );

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			assert( !node->category.empty() );
			auto it = m_tests.find( makeStdString( node->category ) );

			if ( it != m_tests.end() )
			{
				auto & tests = *it;

				for ( auto & test : tests.second )
				{
					m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
				}
			}
		}

		doProcessTest();
	}

	void MainFrame::doRunCategoryTests( TestStatus filter )
	{
		m_cancelled.exchange( false );

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			assert( !node->category.empty() );
			auto it = m_tests.find( makeStdString( node->category ) );

			if ( it != m_tests.end() )
			{
				auto & tests = *it;

				for ( auto & test : tests.second )
				{
					if ( test.status == filter )
					{
						m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
					}
				}
			}
		}

		doProcessTest();
	}

	void MainFrame::doRunAllCategoryTestsBut( TestStatus filter )
	{
		m_cancelled.exchange( false );

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			assert( !node->category.empty() );
			auto it = m_tests.find( makeStdString( node->category ) );

			if ( it != m_tests.end() )
			{
				auto & tests = *it;

				for ( auto & test : tests.second )
				{
					if ( test.status != filter )
					{
						m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
					}
				}
			}
		}

		doProcessTest();
	}

	void MainFrame::doRunAllTests()
	{
		m_cancelled.exchange( false );

		for ( auto & tests : m_tests )
		{
			for ( auto & test : tests.second )
			{
				m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
			}
		}

		doProcessTest();
	}

	void MainFrame::doRunTests( TestStatus filter )
	{
		m_cancelled.exchange( false );

		for ( auto & tests : m_tests )
		{
			for ( auto & test : tests.second )
			{
				if ( test.status == filter )
				{
					m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
				}
			}
		}

		doProcessTest();
	}

	void MainFrame::doRunAllTestsBut( TestStatus filter )
	{
		m_cancelled.exchange( false );

		for ( auto & tests : m_tests )
		{
			for ( auto & test : tests.second )
			{
				if ( test.status != filter )
				{
					m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
				}
			}
		}

		doProcessTest();
	}

	void MainFrame::doCancelTest( Test & test
		, TestStatus status )
	{
		test.status = status;
		m_model->ItemChanged( wxDataViewItem{ getTestNode( test ) } );
		m_runningTest.tests.clear();
		m_statusBar->SetLabel( _( "Idle" ) );
	}

	void MainFrame::doCancel()
	{
		m_cancelled.exchange( true );
	}

	void MainFrame::onSelectionChange( wxDataViewEvent & evt )
	{
		auto wasDisplayingTest = m_detailViews->isLayerShown( 1 );
		auto wasDisplayingCategory = m_detailViews->isLayerShown( 2 );
		m_selected.allTests = true;
		m_selected.allCategories = true;
		m_view->GetSelections( m_selected.items );
		bool displayTest = false;
		bool displayCategory = false;

		if ( m_selected.items.size() == 1 )
		{
			auto node = static_cast< TreeModelNode * >( m_selected.items[0].GetID() );

			if ( node )
			{
				wxString category;
				Test * test{};

				if ( node->test )
				{
					test = node->test;
				}
				else
				{
					category = node->category;
				}

				if ( test )
				{
					m_testView->setTest( *test );
					m_detailViews->showLayer( 1 );
					displayTest = true;
					m_selected.allTests = true;
				}
				else if ( !category.empty() )
				{
					auto it = m_tests.find( makeStdString( category ) );
					assert( it != m_tests.end() );
					m_categoryView->setCategory( category, it->second );
					m_detailViews->showLayer( 2 );
					displayCategory = true;
				}
			}
		}

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			m_selected.allCategories = ( !node->category.empty() )
				&& m_selected.allCategories;
			m_selected.allTests = node->test
				&& m_selected.allTests;
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
		if ( ( !m_selected.items.empty() )
			&& m_runningTest.tests.empty() )
		{
			if ( m_selected.allTests )
			{
				if ( m_selected.items.size() <= 1 )
				{
					m_testMenu->Enable( eID_IGNORE_RESULT, true );
					m_testMenu->Check( eID_IGNORE_RESULT, static_cast< TreeModelNode * >( m_selected.items.front().GetID() )->test->ignoreResult );
				}
				else
				{
					m_testMenu->Enable( eID_IGNORE_RESULT, false );
					m_testMenu->Check( eID_IGNORE_RESULT, false );
				}

				PopupMenu( m_testMenu.get() );
			}
			else if ( m_selected.allCategories )
			{
				PopupMenu( m_categoryMenu.get() );
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

	void MainFrame::onMenuOption( wxCommandEvent & evt )
	{
		switch ( evt.GetId() )
		{
		case eID_RUN_TEST:
			doRunTest();
			break;
		case eID_VIEW_TEST:
			doViewTest();
			break;
		case eID_SET_REF:
			doSetRef();
			break;
		case eID_IGNORE_RESULT:
			doIgnoreTestResult();
			break;
		case eID_RUN_CATEGORY_TESTS_ALL:
			doRunAllCategoryTests();
			break;
		case eID_RUN_CATEGORY_TESTS_NOTRUN:
			doRunCategoryTests( TestStatus::eNotRun );
			break;
		case eID_RUN_CATEGORY_TESTS_ACCEPTABLE:
			doRunCategoryTests( TestStatus::eAcceptable );
			break;
		case eID_RUN_CATEGORY_TESTS_ALL_BUT_NEGLIGIBLE:
			doRunAllCategoryTestsBut( TestStatus::eNegligible );
			break;
		case eID_RUN_TESTS_ALL:
			doRunAllTests();
			break;
		case eID_RUN_TESTS_NOTRUN:
			doRunTests( TestStatus::eNotRun );
			break;
		case eID_RUN_TESTS_ACCEPTABLE:
			doRunTests( TestStatus::eAcceptable );
			break;
		case eID_RUN_TESTS_ALL_BUT_NEGLIGIBLE:
			doRunAllTestsBut( TestStatus::eNegligible );
			break;
		case eID_CANCEL:
			doCancel();
			break;
		}
	}

	void MainFrame::onTestRunEnd( wxProcessEvent & evt )
	{
		auto & testNode = *m_runningTest.tests.begin();
		auto & test = *testNode.test;

		if ( !m_cancelled )
		{
			wxString command = m_config.differ;
			command << " " << test.renderer;
			command << " -f " << ( m_config.test / test.category / ( test.name + ".cscn" ) );
			m_runningTest.difProcess->Redirect();
			auto result = wxExecute( command
				, ExecMode
				, m_runningTest.difProcess.get() );
#if CTP_UseAsync

			if ( result == 0 )
			{
				castor::Logger::logError( "doProcessTest: " + castor::System::getLastErrorText() );
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

	void MainFrame::onTestDisplayEnd( wxProcessEvent & evt )
	{
	}

	void MainFrame::onTestDiffEnd( wxProcessEvent & evt )
	{
		auto testNode = *m_runningTest.tests.begin();
		m_runningTest.tests.erase( m_runningTest.tests.begin() );
		auto & test = *testNode.test;

		if ( !m_cancelled )
		{
			auto matches = castor::File::filterDirectoryFiles( m_config.test / getCompareFolder( test, false )
				, [&test]( castor::Path const & folder, castor::String name )
				{
					return name.find( test.name ) == 0u
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
			auto path = match.getPath();
			test.runDate = getFileDate( match );
			test.status = getStatus( path.getFileName() );
			m_database.insertTest( test );

			if ( test.ignoreResult )
			{
				updateTestStatus( test, TestStatus::eNegligible, true );
			}
			else
			{
				m_model->ItemChanged( wxDataViewItem{ testNode.node } );

				if ( m_detailViews->isLayerShown( 1 )
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

	//*********************************************************************************************
}
