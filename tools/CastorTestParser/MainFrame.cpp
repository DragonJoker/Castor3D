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
#include <CastorUtils/Design/BlockGuard.hpp>
#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Miscellaneous/Utils.hpp>

#include <castor.xpm>

#include "CastorTestParser/xpms/acceptable.xpm"
#include "CastorTestParser/xpms/ignored.xpm"
#include "CastorTestParser/xpms/negligible.xpm"
#include "CastorTestParser/xpms/none.xpm"
#include "CastorTestParser/xpms/notrun.xpm"
#include "CastorTestParser/xpms/outofdate.xpm"
#include "CastorTestParser/xpms/unacceptable.xpm"
#include "CastorTestParser/xpms/unprocessed.xpm"
#include "CastorTestParser/xpms/progress_1.xpm"
#include "CastorTestParser/xpms/progress_2.xpm"
#include "CastorTestParser/xpms/progress_3.xpm"
#include "CastorTestParser/xpms/progress_4.xpm"
#include "CastorTestParser/xpms/progress_5.xpm"
#include "CastorTestParser/xpms/progress_6.xpm"
#include "CastorTestParser/xpms/progress_7.xpm"
#include "CastorTestParser/xpms/progress_8.xpm"
#include "CastorTestParser/xpms/progress_9.xpm"
#include "CastorTestParser/xpms/progress_10.xpm"
#include "CastorTestParser/xpms/progress_11.xpm"
#include "CastorTestParser/xpms/progress_12.xpm"

#include <wx/clipbrd.h>
#include <wx/dc.h>
#include <wx/gauge.h>
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
			eID_UPDATE_CASTOR,
			eID_COPY_FILE_NAME,
			eID_RUN_CATEGORY_TESTS_ALL,
			eID_RUN_CATEGORY_TESTS_NOTRUN,
			eID_RUN_CATEGORY_TESTS_ACCEPTABLE,
			eID_RUN_CATEGORY_TESTS_ALL_BUT_NEGLIGIBLE,
			eID_RUN_CATEGORY_TESTS_OUTDATED,
			eID_CATEGORY_UPDATE_CASTOR,
			eID_RUN_RENDERER_TESTS_ALL,
			eID_RUN_RENDERER_TESTS_NOTRUN,
			eID_RUN_RENDERER_TESTS_ACCEPTABLE,
			eID_RUN_RENDERER_TESTS_ALL_BUT_NEGLIGIBLE,
			eID_RUN_RENDERER_TESTS_OUTDATED,
			eID_RENDERER_UPDATE_CASTOR,
			eID_RUN_TESTS_ALL,
			eID_RUN_TESTS_NOTRUN,
			eID_RUN_TESTS_ACCEPTABLE,
			eID_RUN_TESTS_ALL_BUT_NEGLIGIBLE,
			eID_RUN_TESTS_OUTDATED,
			eID_ALL_UPDATE_CASTOR,
			eID_CANCEL,
		};

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

		class DataViewTestStatusRenderer
			: public wxDataViewCustomRenderer
		{
		private:
			using Clock = std::chrono::high_resolution_clock;

		public:
			static wxString GetDefaultType()
			{
				return wxS( "long" );
			}

			DataViewTestStatusRenderer( const wxString & varianttype = GetDefaultType(),
				wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
				int align = wxDVR_DEFAULT_ALIGNMENT )
				: wxDataViewCustomRenderer{ varianttype, mode, align }
				, m_size{ 20, 20 }
				, m_bitmaps{ createImage( notrun_xpm )
					, createImage( negligible_xpm )
					, createImage( acceptable_xpm )
					, createImage( unacceptable_xpm )
					, createImage( unprocessed_xpm )
					, createImage( progress_1_xpm )
					, createImage( progress_2_xpm )
					, createImage( progress_3_xpm )
					, createImage( progress_4_xpm )
					, createImage( progress_5_xpm )
					, createImage( progress_6_xpm )
					, createImage( progress_7_xpm )
					, createImage( progress_8_xpm )
					, createImage( progress_9_xpm )
					, createImage( progress_10_xpm )
					, createImage( progress_11_xpm )
					, createImage( progress_12_xpm ) }
			{
			}

			bool SetValue( const wxVariant & value ) override
			{
				m_status = TestStatus( value.GetLong() );
				return true;
			}

			bool GetValue( wxVariant & value ) const override
			{
				value = wxVariant{ long( m_status ) };
				return true;
			}

			bool Render( wxRect cell, wxDC * dc, int state ) override
			{
				dc->DrawBitmap( m_bitmaps[size_t( m_status )]
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
			std::array< wxBitmap, size_t( TestStatus::eCount ) > m_bitmaps;
			TestStatus m_status{ TestStatus::eNegligible };
		};

		castor::Path getTestFileName( castor::Path const & folder
			, Test const & test )
		{
			return folder / test.category / ( test.name + ".cscn" );
		}
	}

	//*********************************************************************************************

	MainFrame::MainFrame( Config config )
		: wxFrame{ nullptr, wxID_ANY, wxT( "Castor3D Tests Parser" ), wxDefaultPosition, wxSize( 800, 700 ) }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
		, m_config{ std::move( config ) }
		, m_database{ m_config }
		, m_updater{ m_model }
	{
		SetClientSize( 900, 600 );
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
		m_updater.stop();
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
		m_statusText->SetLabel( _( "Idle" ) );

		m_categoryView->setAll( m_tests, m_runningTest.tests );
		m_detailViews->showLayer( 2u );
	}

	void MainFrame::updateTestStatus( Test & test
		, TestStatus newStatus
		, bool reference )
	{
		auto node = getTestNode( test );
		test.castorDate = getFileDate( m_config.castor );
		assert( db::date_time::isValid( test.castorDate ) );
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
		uint32_t flags = wxCOL_SORTABLE | wxCOL_RESIZABLE;
		auto renCategory = new wxDataViewTextRenderer{ wxT( "string" ) };
		wxDataViewColumn * colCategory = new wxDataViewColumn( _( "Category" ), renCategory, int( TreeModel::Column::eCategory ), 200, wxALIGN_LEFT, flags );
		colCategory->SetMinWidth( 150 );
		m_view->AppendColumn( colCategory );
		auto renScene = new wxDataViewTextRenderer{ wxT( "string" ) };
		wxDataViewColumn * colScene = new wxDataViewColumn( _( "Name" ), renScene, int( TreeModel::Column::eName ), 400, wxALIGN_LEFT, flags );
		colScene->SetMinWidth( 400 );
		m_view->AppendColumn( colScene );
		auto renRenderer = new wxDataViewTextRenderer{ wxT( "string" ) };
		wxDataViewColumn * colRenderer = new wxDataViewColumn( _( "Renderer" ), renRenderer, int( TreeModel::Column::eRenderer ), 100, wxALIGN_LEFT, flags );
		colRenderer->SetMinWidth( 100 );
		m_view->AppendColumn( colRenderer );
		auto renRunDate = new wxDataViewDateRenderer{ wxT( "datetime" ) };
		wxDataViewColumn * colRunDate = new wxDataViewColumn( _( "Run Date" ), renRunDate, int( TreeModel::Column::eRunDate ), 100, wxALIGN_LEFT, flags );
		colRunDate->SetMinWidth( 100 );
		m_view->AppendColumn( colRunDate );
		auto renStatus = new DataViewTestStatusRenderer;
		wxDataViewColumn * colStatus = new wxDataViewColumn( _( "A" ), renStatus, int( TreeModel::Column::eStatus ), 30, wxALIGN_LEFT, flags );
		colStatus->SetMinWidth( 30 );
		m_view->AppendColumn( colStatus );
		auto renIgnored = new DataViewTestBoolBitmapRenderer{ ignored_xpm, none_xpm };
		wxDataViewColumn * colIgnored = new wxDataViewColumn( _( "I" ), renIgnored, int( TreeModel::Column::eIgnored ), 30, wxALIGN_LEFT, flags );
		colIgnored->SetMinWidth( 30 );
		m_view->AppendColumn( colIgnored );
		auto renCastorOOD = new DataViewTestBoolBitmapRenderer{ outofdate_xpm, none_xpm };
		wxDataViewColumn * colCastorOOD = new wxDataViewColumn( _( "C" ), renCastorOOD, int( TreeModel::Column::eCastorDate ), 30, wxALIGN_LEFT, flags );
		colCastorOOD->SetMinWidth( 30 );
		m_view->AppendColumn( colCastorOOD );
		auto renSceneOOD = new DataViewTestBoolBitmapRenderer{ outofdate_xpm, none_xpm };
		wxDataViewColumn * colSceneOOD = new wxDataViewColumn( _( "S" ), renSceneOOD, int( TreeModel::Column::eSceneDate ), 30, wxALIGN_LEFT, flags );
		colSceneOOD->SetMinWidth( 30 );
		m_view->AppendColumn( colSceneOOD );

		return listPanel;
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
		m_statusText = new wxStaticText{ statusBar, wxID_ANY, _( "Idle" ), wxPoint( 10, 5 ), wxDefaultSize, 0 };
		m_statusText->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		m_statusText->SetForegroundColour( PANEL_FOREGROUND_COLOUR );


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
			menu.Append( eID_COPY_FILE_NAME, _( "Copy test file path" ) + wxT( "\tF4" ) );
			menu.Append( eID_VIEW_TEST, _( "View Test" ) + wxT( "\tF5" ) );
			menu.Append( eID_SET_REF, _( "Set Reference" ) + wxT( "\tF6" ) );
			menu.Append( eID_RUN_TEST, _( "Run Test" ) + wxT( "\tF7" ) );
			menu.Append( eID_IGNORE_RESULT, _( "Ignore test results" ) + wxT( "\tF8" ), wxEmptyString, wxITEM_CHECK );
			menu.Append( eID_UPDATE_CASTOR, _( "Update Castor3D's date" ) + wxT( "\tF9" ) );
		};
		auto addRendererMenus = []( wxMenu & menu )
		{
			menu.Append( eID_RUN_RENDERER_TESTS_ALL, _( "Run all renderer's tests" ) + wxT( "\tCtrl+F7" ) );
			menu.Append( eID_RUN_RENDERER_TESTS_NOTRUN, _( "Run all <not run> renderer's tests" ) + wxT( "\tCtrl+F8" ) );
			menu.Append( eID_RUN_RENDERER_TESTS_ACCEPTABLE, _( "Run all <acceptable> renderer's tests" ) + wxT( "\tCtrl+F9" ) );
			menu.Append( eID_RUN_RENDERER_TESTS_ALL_BUT_NEGLIGIBLE, _( "Run all but <negligible> renderer's tests" ) + wxT( "\tCtrl+F10" ) );
			menu.Append( eID_RUN_RENDERER_TESTS_OUTDATED, _( "Run all outdated renderer's tests" ) + wxT( "\tCtrl+F11" ) );
			menu.Append( eID_RENDERER_UPDATE_CASTOR, _( "Update renderer's tests Castor3D's date" ) + wxT( "\tCtrl+F12" ) );
		};
		auto addCategoryMenus = []( wxMenu & menu )
		{
			menu.Append( eID_RUN_CATEGORY_TESTS_ALL, _( "Run all category's tests" ) + wxT( "\tCtrl+F7" ) );
			menu.Append( eID_RUN_CATEGORY_TESTS_NOTRUN, _( "Run all <not run> category's tests" ) + wxT( "\tCtrl+F8" ) );
			menu.Append( eID_RUN_CATEGORY_TESTS_ACCEPTABLE, _( "Run all <acceptable> category's tests" ) + wxT( "\tCtrl+F9" ) );
			menu.Append( eID_RUN_CATEGORY_TESTS_ALL_BUT_NEGLIGIBLE, _( "Run all but <negligible> category's tests" ) + wxT( "\tCtrl+F10" ) );
			menu.Append( eID_RUN_CATEGORY_TESTS_OUTDATED, _( "Run all outdated category's tests" ) + wxT( "\tCtrl+F11" ) );
			menu.Append( eID_CATEGORY_UPDATE_CASTOR, _( "Update category's tests Castor3D's date" ) + wxT( "\tCtrl+F12" ) );
		};
		auto addAllMenus = []( wxMenu & menu )
		{
			menu.Append( eID_RUN_TESTS_ALL, _( "Run all tests" ) + wxT( "\tCtrl+Alt+F7" ) );
			menu.Append( eID_RUN_TESTS_NOTRUN, _( "Run all <not run> tests" ) + wxT( "\tCtrl+Alt+F8" ) );
			menu.Append( eID_RUN_TESTS_ACCEPTABLE, _( "Run all <acceptable> tests" ) + wxT( "\tCtrl+Alt+F9" ) );
			menu.Append( eID_RUN_TESTS_ALL_BUT_NEGLIGIBLE, _( "Run all but <negligible> tests" ) + wxT( "\tCtrl+Alt+F10" ) );
			menu.Append( eID_RUN_TESTS_OUTDATED, _( "Run all outdated tests" ) + wxT( "\tCtrl+Alt+F11" ) );
			menu.Append( eID_ALL_UPDATE_CASTOR, _( "Update tests Castor3D's date" ) + wxT( "\tCtrl+Alt+F12" ) );
		};
		m_testMenu = std::make_unique< wxMenu >();
		addTestMenus( *m_testMenu );
		m_testMenu->AppendSeparator();
		addCategoryMenus( *m_testMenu );
		m_testMenu->AppendSeparator();
		addRendererMenus( *m_testMenu );
		m_testMenu->AppendSeparator();
		addAllMenus( *m_testMenu );
		m_testMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onMenuOption )
			, nullptr
			, this );

		m_categoryMenu = std::make_unique< wxMenu >();
		addCategoryMenus( *m_categoryMenu );
		m_categoryMenu->AppendSeparator();
		addRendererMenus( *m_categoryMenu );
		m_categoryMenu->AppendSeparator();
		addAllMenus( *m_categoryMenu );
		m_categoryMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onMenuOption )
			, nullptr
			, this );
		
		m_rendererMenu = std::make_unique< wxMenu >();
		addRendererMenus( *m_rendererMenu );
		m_rendererMenu->AppendSeparator();
		addAllMenus( *m_rendererMenu );
		m_rendererMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
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
		m_busyMenu->Append( eID_SET_REF, _( "Set Reference" ) + wxT( "\tF6" ) );
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

		for ( auto & renderer : m_tests )
		{
			m_model->addRenderer( renderer.first );

			for ( auto & category : renderer.second )
			{
				m_model->addCategory( renderer.first, category.first );

				for ( auto & test : category.second )
				{
					auto testNode = m_model->addTest( test );
					m_modelNodes[test.id] = testNode;
					progress.Update( index++, makeWxString( test.category ) + wxT( " - " ) + makeWxString( test.name ) + wxT( "..." ) );
				}
			}
		}

		m_model->expandRoots( m_view );
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
			test.status = TestStatus::eRunning_Begin;
			m_updater.addTest( *testNode.node );
			m_model->ItemChanged( wxDataViewItem{ testNode.node } );
			m_statusText->SetLabel( _( "Running test: " ) + test.name );
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

	void MainFrame::doCopyTestFileName()
	{
		if ( m_selected.items.size() == 1 )
		{
			auto & item = *m_selected.items.begin();
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( wxTheClipboard->Open() )
			{
				auto guard = castor::makeBlockGuard( [](){ wxTheClipboard->Close(); } );
				wxTheClipboard->SetData( new wxTextDataObject( makeWxString( getTestFileName( m_config.test, *node->test ) ) ) );
			}
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
			command << " " << makeWxString( getTestFileName( m_config.test, *node->test ) );
			command << " -" << node->test->renderer;
			m_statusText->SetLabel( _( "Viewing: " ) + node->test->name );
			auto result = wxExecute( command
				, wxEXEC_SYNC
				, m_runningTest.disProcess.get() );

			if ( result != 0 )
			{
				castor::Logger::logError( "doViewTest: " + castor::System::getLastErrorText() );
			}
		}

		m_statusText->SetLabel( _( "Idle" ) );
	}

	void MainFrame::doSetRef()
	{
		m_cancelled.exchange( false );

		if ( !m_selected.items.empty() )
		{
			m_statusProgress->SetRange( m_selected.items.size() );
			m_statusProgress->SetValue( 0 );
			m_statusProgress->Show();

			for ( auto & item : m_selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );
				assert( node->test );
				m_statusText->SetLabel( _( "Setting reference: " ) + node->test->name );
				updateTestStatus( *node->test, TestStatus::eNegligible, true );
				m_model->ItemChanged( item );
				m_statusProgress->SetValue( m_statusProgress->GetValue() + 1 );
			}

			m_statusProgress->Hide();
		}

		m_statusText->SetLabel( _( "Idle" ) );
	}

	void MainFrame::doIgnoreTestResult()
	{
		m_cancelled.exchange( false );

		if ( !m_selected.items.empty() )
		{
			m_statusProgress->SetRange( m_selected.items.size() );
			m_statusProgress->SetValue( 0 );
			m_statusProgress->Show();

			for ( auto & item : m_selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );
				assert( node->test );
				m_statusText->SetLabel( _( "Ignoring: " ) + node->test->name );
				node->test->ignoreResult = m_testMenu->IsChecked( eID_IGNORE_RESULT );
				node->test->castorDate = getFileDate( m_config.castor );
				assert( db::date_time::isValid( node->test->castorDate ) );
				m_database.updateTestIgnoreResult( *node->test, node->test->ignoreResult, true );
				m_model->ItemChanged( item );
				m_statusProgress->SetValue( m_statusProgress->GetValue() + 1 );
			}

			m_statusProgress->Hide();
		}

		m_statusText->SetLabel( _( "Idle" ) );
	}

	void MainFrame::doUpdateCastorDate()
	{
		m_cancelled.exchange( false );
		auto date = getFileDate( m_config.castor );
		assert( db::date_time::isValid( date ) );

		if ( !m_selected.items.empty() )
		{
			m_statusProgress->SetRange( m_selected.items.size() );
			m_statusProgress->SetValue( 0 );
			m_statusProgress->Show();

			for ( auto & item : m_selected.items )
			{
				auto node = static_cast< TreeModelNode * >( item.GetID() );
				assert( node->test );

				if ( node->test->castorDate != date )
				{
					node->test->castorDate = date;
					m_database.updateTestCastorDate( *node->test );
				}

				m_statusProgress->SetValue( m_statusProgress->GetValue() + 1 );
			}

			m_statusProgress->Hide();
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
			auto rendererNode = node->getRenderer();
			assert( !rendererNode->category.empty() && rendererNode->isRenderer() );
			auto rendIt = m_tests.find( makeStdString( rendererNode->category ) );

			if ( rendIt != m_tests.end() )
			{
				auto & renderer = *rendIt;
				auto it = renderer.second.find( makeStdString( node->category ) );

				if ( it != renderer.second.end() )
				{
					auto & category = * it;

					for ( auto & test : category.second )
					{
						m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
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

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			assert( !node->category.empty() );
			auto rendererNode = node->getRenderer();
			assert( !rendererNode->category.empty() && rendererNode->isRenderer() );
			auto rendIt = m_tests.find( makeStdString( rendererNode->category ) );

			if ( rendIt != m_tests.end() )
			{
				auto & renderer = *rendIt;
				auto it = renderer.second.find( makeStdString( node->category ) );

				if ( it != renderer.second.end() )
				{
					auto & category = *it;

					for ( auto & test : category.second )
					{
						if ( test.status == filter )
						{
							m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
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

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			assert( !node->category.empty() );
			auto rendererNode = node->getRenderer();
			assert( !rendererNode->category.empty() && rendererNode->isRenderer() );
			auto rendIt = m_tests.find( makeStdString( rendererNode->category ) );

			if ( rendIt != m_tests.end() )
			{
				auto & renderer = *rendIt;
				auto it = renderer.second.find( makeStdString( node->category ) );

				if ( it != renderer.second.end() )
				{
					auto & category = *it;

					for ( auto & test : category.second )
					{
						if ( test.status != filter )
						{
							m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
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
		auto date = getFileDate( m_config.castor );
		assert( db::date_time::isValid( date ) );

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			assert( !node->category.empty() );
			auto rendererNode = node->getRenderer();
			assert( !rendererNode->category.empty() && rendererNode->isRenderer() );
			auto rendIt = m_tests.find( makeStdString( rendererNode->category ) );

			if ( rendIt != m_tests.end() )
			{
				auto & renderer = *rendIt;
				auto it = renderer.second.find( makeStdString( node->category ) );

				if ( it != renderer.second.end() )
				{
					auto & category = *it;

					for ( auto & test : category.second )
					{
						if ( test.castorDate != date
							|| test.status == TestStatus::eNotRun )
						{
							m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
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
		auto date = getFileDate( m_config.castor );
		assert( db::date_time::isValid( date ) );

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
					auto node = static_cast< TreeModelNode * >( item.GetID() );
					assert( node->test );

					if ( node->test->castorDate != date )
					{
						node->test->castorDate = date;
						m_database.updateTestCastorDate( *node->test );
					}
				}
			}
		}
	}

	void MainFrame::doRunAllRendererTests()
	{
		m_cancelled.exchange( false );

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			assert( !node->category.empty() );
			auto rendererIt = m_tests.find( makeStdString(  node->category ) );

			if ( rendererIt != m_tests.end() )
			{
				for ( auto & category : rendererIt->second )
				{
					auto & tests = category.second;

					for ( auto & test : tests )
					{
						m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
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

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			assert( !node->category.empty() );
			auto rendererIt = m_tests.find( makeStdString( node->category ) );

			if ( rendererIt != m_tests.end() )
			{
				for ( auto & category : rendererIt->second )
				{
					auto & tests = category.second;

					for ( auto & test : tests )
					{
						if ( test.status == filter )
						{
							m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
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

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			assert( !node->category.empty() );
			auto rendererIt = m_tests.find( makeStdString( node->category ) );

			if ( rendererIt != m_tests.end() )
			{
				for ( auto & category : rendererIt->second )
				{
					auto & tests = category.second;

					for ( auto & test : tests )
					{
						if ( test.status != filter )
						{
							m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
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
		auto date = getFileDate( m_config.castor );
		assert( db::date_time::isValid( date ) );

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			assert( !node->category.empty() );
			auto rendererIt = m_tests.find( makeStdString( node->category ) );

			if ( rendererIt != m_tests.end() )
			{
				for ( auto & category : rendererIt->second )
				{
					auto & tests = category.second;

					for ( auto & test : tests )
					{
						if ( test.castorDate != date
							|| test.status == TestStatus::eNotRun )
						{
							m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
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
		auto date = getFileDate( m_config.castor );
		assert( db::date_time::isValid( date ) );

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			assert( !node->category.empty() );

			for ( auto & renderer : m_tests )
			{
				auto it = renderer.second.find( makeStdString( node->category ) );

				if ( it != renderer.second.end() )
				{
					auto & tests = *it;

					for ( auto & test : tests.second )
					{
						auto node = static_cast< TreeModelNode * >( item.GetID() );
						assert( node->test );

						if ( node->test->castorDate != date )
						{
							node->test->castorDate = date;
							m_database.updateTestCastorDate( *node->test );
						}
					}
				}
			}
		}
	}

	void MainFrame::doRunAllTests()
	{
		m_cancelled.exchange( false );

		for ( auto & renderer : m_tests )
		{
			for ( auto & category : renderer.second )
			{
				for ( auto & test : category.second )
				{
					m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
				}
			}
		}

		m_testProgress->SetRange( m_runningTest.tests.size() );
		m_testProgress->SetValue( 0 );
		m_testProgress->Show();
		doProcessTest();
	}

	void MainFrame::doRunTests( TestStatus filter )
	{
		m_cancelled.exchange( false );

		for ( auto & renderer : m_tests )
		{
			for ( auto & category : renderer.second )
			{
				for ( auto & test : category.second )
				{
					if ( test.status == filter )
					{
						m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
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

		for ( auto & renderer : m_tests )
		{
			for ( auto & category : renderer.second )
			{
				for ( auto & test : category.second )
				{
					if ( test.status != filter )
					{
						m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
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
		auto date = getFileDate( m_config.castor );
		assert( db::date_time::isValid( date ) );

		for ( auto & renderer : m_tests )
		{
			for ( auto & category : renderer.second )
			{
				for ( auto & test : category.second )
				{
					if ( test.castorDate != date )
					{
						m_runningTest.tests.push_back( { &test, test.status, getTestNode( test ) } );
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
		auto date = getFileDate( m_config.castor );
		assert( db::date_time::isValid( date ) );

		for ( auto & renderer : m_tests )
		{
			for ( auto & category : renderer.second )
			{
				for ( auto & test : category.second )
				{
					if ( test.castorDate != date )
					{
						test.castorDate = date;
						m_database.updateTestCastorDate( test );
					}
				}
			}
		}
	}

	void MainFrame::doCancelTest( Test & test
		, TestStatus status )
	{
		test.status = status;
		m_model->ItemChanged( wxDataViewItem{ getTestNode( test ) } );
		m_runningTest.tests.clear();
		m_statusText->SetLabel( _( "Idle" ) );
		m_testProgress->Hide();
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
		m_selected.allRenderers = true;
		m_view->GetSelections( m_selected.items );
		bool displayTest = false;
		bool displayCategory = false;

		if ( m_selected.items.size() == 1 )
		{
			auto node = static_cast< TreeModelNode * >( m_selected.items[0].GetID() );

			if ( node )
			{
				wxString category;
				wxString renderer;
				Test * test{};

				if ( node->test )
				{
					test = node->test;
				}
				else if ( node->isRenderer() )
				{
					renderer = node->category;
				}
				else
				{
					renderer = node->getRenderer()->category;
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
					auto rendIt = m_tests.find( makeStdString( renderer ) );

					if ( rendIt != m_tests.end() )
					{
						auto it = rendIt->second.find( makeStdString( category ) );

						if ( it != rendIt->second.end() )
						{
							m_categoryView->setCategory( category, it->second );
						}
						else
						{
							m_categoryView->setRenderer( renderer, rendIt->second );
						}
					}
					else
					{
						m_categoryView->setAll( m_tests, m_runningTest.tests );
					}

					m_detailViews->showLayer( 2 );
					displayCategory = true;
				}
				else if ( !renderer.empty() )
				{
					auto rendIt = m_tests.find( makeStdString( renderer ) );

					if ( rendIt != m_tests.end() )
					{
						m_categoryView->setRenderer( renderer, rendIt->second );
					}
					else
					{
						m_categoryView->setAll( m_tests, m_runningTest.tests );
					}

					m_detailViews->showLayer( 2 );
					displayCategory = true;
				}
			}
		}

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );
			m_selected.allRenderers = ( !node->category.empty() && node->isRenderer() )
				&& m_selected.allRenderers;
			m_selected.allCategories = ( !node->category.empty() && !node->isRenderer() )
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
			else if ( m_selected.allRenderers )
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

	void MainFrame::onMenuOption( wxCommandEvent & evt )
	{
		switch ( evt.GetId() )
		{
		case eID_RUN_TEST:
			doRunTest();
			break;
		case eID_COPY_FILE_NAME:
			doCopyTestFileName();
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
		case eID_UPDATE_CASTOR:
			doUpdateCastorDate();
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
		case eID_RUN_CATEGORY_TESTS_OUTDATED:
			doRunAllCategoryOutdatedTests();
			break;
		case eID_CATEGORY_UPDATE_CASTOR:
			doUpdateCategoryCastorDate();
			break;
		case eID_RUN_RENDERER_TESTS_ALL:
			doRunAllRendererTests();
			break;
		case eID_RUN_RENDERER_TESTS_NOTRUN:
			doRunRendererTests( TestStatus::eNotRun );
			break;
		case eID_RUN_RENDERER_TESTS_ACCEPTABLE:
			doRunRendererTests( TestStatus::eAcceptable );
			break;
		case eID_RUN_RENDERER_TESTS_ALL_BUT_NEGLIGIBLE:
			doRunAllRendererTestsBut( TestStatus::eNegligible );
			break;
		case eID_RUN_RENDERER_TESTS_OUTDATED:
			doRunAllRendererOutdatedTests();
			break;
		case eID_RENDERER_UPDATE_CASTOR:
			doUpdateRendererCastorDate();
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
		case eID_RUN_TESTS_OUTDATED:
			doRunAllOutdatedTests();
			break;
		case eID_ALL_UPDATE_CASTOR:
			doUpdateAllCastorDate();
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
			assert( db::date_time::isValid( test.runDate ) );
			test.status = getStatus( path.getFileName() );
			test.castorDate = getFileDate( m_config.castor );
			assert( db::date_time::isValid( test.castorDate ) );
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

						if ( !isRunning( node->test->status ) )
						{
							it = current.erase( it );
						}
						else
						{
							node->test->status = ( node->test->status == TestStatus::eRunning_End )
								? TestStatus::eRunning_Begin
								: TestStatus( uint32_t( node->test->status ) + 1 );
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
}
