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
#include <wx/process.h>
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

		castor::PathArray listTestCategories( castor::Path const & folder )
		{
			castor::PathArray result;
			castor::File::traverseDirectory( folder
				, [&result]( castor::Path const & folder )
				{
					result.push_back( folder );
					return true;
				}
				, []( castor::Path const & folder
					, castor::String const & name )
				{
				} );
			return result;
		}

		castor::PathArray listScenes( castor::Path const & categoryPath )
		{
			return castor::File::filterDirectoryFiles( categoryPath
				, []( castor::Path const & folder, castor::String const & name )
				{
					return castor::Path{ name }.getExtension() == cuT( "cscn" );
				} );
		}

		db::DateTime getFileDate( castor::Path const & imgPath )
		{
			wxStructStat strucStat;
			wxStat( makeWxString( imgPath ), &strucStat );
			return makeDbDateTime( wxDateTime{ strucStat.st_mtime } );
		}

		void makeTest( TestStatus status
			, std::string const & category
			, castor::Path const & imgPath
			, std::map< castor::String, Test > & tests )
		{
			auto name = castor::string::stringCast< char >( imgPath.getFileName() );
			auto prevDotIdx = name.find_last_of( "." );

			if ( prevDotIdx == std::string::npos
				|| name.substr( prevDotIdx + 1 ) != "diff" )
			{
				auto rendererIdx = name.find_last_of( "_" );

				if ( rendererIdx != std::string::npos )
				{
					auto renderer = name.substr( rendererIdx  + 1 );
					name = name.substr( 0, rendererIdx );
					tests.emplace( name
						, Test{ 0u
							, name
							, getFileDate( imgPath )
							, status
							, renderer
							, category } );
				}
			}
		}

		TestStatus getStatus( std::string const & name )
		{
			static std::map< std::string, TestStatus > const folders
			{
				{ "Negligible", TestStatus::eNegligible },
				{ "Acceptable", TestStatus::eAcceptable },
				{ "Unacceptable", TestStatus::eUnacceptable },
				{ "Unprocessed", TestStatus::eUnprocessed },
			};
			auto it = folders.find( name );
			assert( it != folders.end() );
			return it->second;
		}

		std::map< castor::String, Test > listAllResults( castor::Path const & categoryPath
			, castor::PathArray const & folders )
		{
			std::map< castor::String, Test > results;
			auto category = castor::string::stringCast< char >( categoryPath.getFileName() );

			for ( auto & status : folders )
			{
				auto testStatus = getStatus( status.getFileName() );
				castor::File::filterDirectoryFiles( status
					, [&results, &status, &category, testStatus]( castor::Path const & folder, castor::String const & name )
					{
						auto result = castor::Path{ name }.getExtension() == cuT( "png" );

						if ( result )
						{
							makeTest( testStatus, category, folder / name, results );
						}

						return result;
					} );
			}

			return results;
		}

		uint32_t listTests( castor::Path const & categoryPath
			, TestMap & tests )
		{
			auto category = castor::string::stringCast< char >( categoryPath.getFileName() );
			auto compareFolder = categoryPath / cuT( "Compare" );
			castor::PathArray folders
			{
				compareFolder / "Negligible",
				compareFolder / "Acceptable",
				compareFolder / "Unacceptable",
				compareFolder / "Unprocessed",
			};
			auto allResults = listAllResults( categoryPath, folders );
			auto scenes = listScenes( categoryPath );
			auto & categoryIt = tests.emplace( category, TestArray{} ).first;

			for ( auto & testScene : scenes )
			{
				auto sceneName = testScene.getFileName();
				auto it = allResults.find( sceneName );

				if ( it == allResults.end() )
				{
					categoryIt->second.push_back( Test{ 0
						, sceneName
						, getFileDate( testScene )
						, TestStatus::eNotRun
						, "vk"
						, category } );
				}
				else
				{
					categoryIt->second.push_back( it->second );
				}
			}

			return uint32_t( categoryIt->second.size() );
		}

		void moveFile( castor::Path const & srcFolder
			, castor::Path const & dstFolder
			, castor::Path const & srcName
			, castor::Path const & dstName
			, bool force )
		{
			auto src = srcFolder / srcName;
			auto dst = dstFolder / dstName;

			if ( castor::File::fileExists( src ) )
			{
				if ( !castor::File::directoryExists( dstFolder ) )
				{
					if ( !castor::File::directoryCreate( dstFolder ) )
					{
						castor::Logger::logError( "Couldn't create folder [" + dstFolder + "]: " + castor::System::getLastErrorText() );
						return;
					}
				}

				if ( force || !castor::File::fileExists( dst ) )
				{
					if ( !castor::File::copyFileName( src, dst ) )
					{
						castor::Logger::logError( "Couldn't copy image [" + src + "]: " + castor::System::getLastErrorText() );
						return;
					}
				}

				castor::File::deleteFile( src );
			}
		}

		void moveFile( castor::Path const & srcFolder
			, castor::Path const & dstFolder
			, castor::Path const & name
			, bool force )
		{
			moveFile( srcFolder
				, dstFolder
				, name
				, name
				, force );
		}

		void moveTestFile( Test const & test
			, castor::Path srcFolder
			, castor::Path dstFolder )
		{
			if ( test.status == TestStatus::eNotRun )
			{
				return;
			}

			srcFolder /= getCompareFolder( test, true );
			dstFolder /= getResultFolder( test );
			moveFile( srcFolder
				, dstFolder
				, getCompareName( test )
				, getResultName( test )
				, false );
		}

		void moveResultFile( Test const & test
			, TestStatus oldStatus
			, TestStatus newStatus
			, castor::Path work )
		{
			if ( oldStatus == TestStatus::eNotRun
				|| oldStatus == newStatus )
			{
				return;
			}

			auto resultFolder = work / getResultFolder( test, false );
			moveFile( resultFolder / getFolderName( oldStatus )
				, resultFolder / getFolderName( newStatus )
				, getResultName( test )
				, false );
		}

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

		static const std::string FOLDER_DATETIME = "%Y-%m-%d_%H-%M-%S";
		static constexpr size_t FOLDER_DATETIME_SIZE = 4u + 3u + 3u + 3u + 3u + 3u;

		castor::Path getFolderName( db::DateTime const & value )
		{
			return castor::Path{ db::date_time::format( value, FOLDER_DATETIME ) };
		}

		bool isDateTime( castor::String const & value
			, db::DateTime & result )
		{
			return db::date_time::isDateTime( value, FOLDER_DATETIME, result );
		}

		std::string getDetails( Test const & test )
		{
			return test.category
				+ " - " + test.renderer
				+ " - " + test.name
				+ " - " + db::date_time::format( test.runDate, FOLDER_DATETIME );
		}

		castor::PathArray findTestResults( Test const & test
			, castor::Path const & work )
		{
			std::vector< TestStatus > statuses
			{
				TestStatus::eNegligible,
				TestStatus::eAcceptable,
				TestStatus::eUnacceptable,
				TestStatus::eUnprocessed,
			};
			castor::File::FilterFunction filterFile = [&test]( castor::Path const & folder
				, castor::String const & name )
			{
				return name.find( test.name ) == ( FOLDER_DATETIME_SIZE + 1u );
			};
			castor::PathArray result;
			auto baseFolder = work / getResultFolder( test, false );

			for ( auto & status : statuses )
			{
				auto matches = castor::File::filterDirectoryFiles( baseFolder / getFolderName( status )
					, filterFile
					, true );
				result.insert( result.end(), matches.begin(), matches.end() );
			}

			return result;
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

	castor::Path getFolderName( TestStatus value )
	{
		switch ( value )
		{
		case TestStatus::eNotRun:
			return castor::Path{ "NotRun" };
		case TestStatus::eNegligible:
			return castor::Path{ "Negligible" };
		case TestStatus::eAcceptable:
			return castor::Path{ "Acceptable" };
		case TestStatus::eUnacceptable:
			return castor::Path{ "Unacceptable" };
		case TestStatus::eUnprocessed:
			return castor::Path{ "Unprocessed" };
		default:
			assert( false );
			return castor::Path{};
		}
	}

	castor::Path getResultFolder( Test const & test, bool useStatus )
	{
		auto result = castor::Path{ "Result" } / test.category;
		return useStatus
			? result / getFolderName( test.status )
			: result;
	}

	castor::Path getResultName( Test const & test )
	{
		return castor::Path{ getFolderName( test.runDate ) + "_" + test.name + "_" + test.renderer + ".png" };
	}

	castor::Path getCompareFolder( Test const & test, bool useStatus )
	{
		auto result = castor::Path{ test.category } / cuT( "Compare" );
		return useStatus
			? result / getFolderName( test.status )
			: result;
	}

	castor::Path getCompareName( Test const & test )
	{
		return castor::Path{ test.name + "_" + test.renderer + ".png" };
	}

	castor::Path getReferenceFolder( Test const & test )
	{
		return castor::Path{ test.category };
	}

	castor::Path getReferenceName( Test const & test )
	{
		return castor::Path{ test.name + "_ref.png" };
	}

	wxString makeWxString( std::string const & in )
	{
		return wxString{ in.c_str(), wxMBConvUTF8{} };
	}

	std::string makeStdString( wxString const & in )
	{
		return in.char_str( wxMBConvUTF8{} ).data();
	}

	wxDateTime makeWxDateTime( db::DateTime const & in )
	{
		int year = int( in.date().year() );
		int month = int( in.date().month() ) - 1;
		int day = int( in.date().day() );
		int hour = int( in.time_of_day().hours() );
		int minute = int( in.time_of_day().minutes() );
		int second = int( in.time_of_day().seconds() );
		return wxDateTime{ wxDateTime::wxDateTime_t( day )
			, wxDateTime::Month( month )
			, wxDateTime::wxDateTime_t( year )
			, wxDateTime::wxDateTime_t( hour )
			, wxDateTime::wxDateTime_t( minute )
			, wxDateTime::wxDateTime_t( second ) };
	}

	db::DateTime makeDbDateTime( wxDateTime const & in )
	{
		int monthDay = in.GetDay();
		int month = in.GetMonth() + 1;
		int year = in.GetYear();
		int hour = in.GetHour();
		int min = in.GetMinute();
		int sec = in.GetSecond();
		return db::DateTime{ db::Date( year, month, monthDay )
			, db::Time( hour, min, sec ) };

	}

	//*********************************************************************************************

	MainFrame::MainFrame( Config config )
		: wxFrame{ nullptr, wxID_ANY, wxT( "Castor3D Tests Parser" ), wxDefaultPosition, wxSize( 800, 700 ) }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
		, m_config{ std::move( config ) }
		, m_database{ m_config.work }
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
		Show( true );
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

			if ( !m_config.skip )
			{
				doInitDatabase( progress, index );
				m_insertTest = { m_database };
				m_updateTestStatus = { m_database };
				doFillDatabase( progress, index );
			}
			else
			{
				m_insertTest = { m_database };
				m_updateTestStatus = { m_database };
			}

			auto testCount = doListTests( progress, index );
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

	void MainFrame::insertTest( Test const & test
		, bool moveFiles )
	{
		m_insertTest.name->setValue( test.name );
		m_insertTest.runDate->setValue( test.runDate );
		m_insertTest.status->setValue( int32_t( test.status ) );
		m_insertTest.renderer->setValue( test.renderer );
		m_insertTest.category->setValue( test.category );
		m_insertTest.stmt->executeUpdate();

		if ( moveFiles )
		{
			moveTestFile( test, m_config.test, m_config.work );
		}

		castor::Logger::logInfo( "Inserted: " + getDetails( test ) );
	}

	void MainFrame::updateTestStatus( Test & test
		, TestStatus newStatus
		, bool reference )
	{
		auto node = getTestNode( test );
		auto oldStatus = test.status;
		test.status = TestStatus::eRunning;
		m_model->ItemChanged( wxDataViewItem{ node } );
		m_updateTestStatus.status->setValue( int32_t( newStatus ) );
		m_updateTestStatus.id->setValue( int32_t( test.id ) );
		m_updateTestStatus.stmt->executeUpdate();
		moveResultFile( test, oldStatus, newStatus, m_config.work );

		if ( reference )
		{
			castor::File::copyFileName( m_config.work / getResultFolder( test, false ) / getFolderName( newStatus ) / getResultName( test )
				, m_config.test / getReferenceFolder( test ) / getReferenceName( test ) );
		}

		test.status = newStatus;
		m_model->ItemChanged( wxDataViewItem{ node } );

		if ( m_detailViews->isLayerShown( 1 )
			&& m_testView->getTest() == &test )
		{
			m_testView->setTest( test );
		}

		castor::Logger::logInfo( "Updated status for: " + getDetails( test ) );
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
			menu.Append( eID_VIEW_TEST, "View Test\tF5" );
			menu.Append( eID_SET_REF, "Set Reference\tF6" );
			menu.Append( eID_RUN_TEST, "Run Test\tF7" );
		};
		auto addCategoryMenus = []( wxMenu & menu )
		{
			menu.Append( eID_RUN_CATEGORY_TESTS_ALL, "Run all category's tests\tCtrl+F7" );
			menu.Append( eID_RUN_CATEGORY_TESTS_NOTRUN, "Run all <not run> category's tests\tCtrl+F8" );
			menu.Append( eID_RUN_CATEGORY_TESTS_ACCEPTABLE, "Run all <acceptable> category's tests\tCtrl+F9" );
			menu.Append( eID_RUN_CATEGORY_TESTS_ALL_BUT_NEGLIGIBLE, "Run all but <negligible> category's tests\tCtrl+F10" );
		};
		auto addAllMenus = []( wxMenu & menu )
		{
			menu.Append( eID_RUN_TESTS_ALL, "Run all tests\tCtrl+Alt+F7" );
			menu.Append( eID_RUN_TESTS_NOTRUN, "Run all <not run> tests\tCtrl+Alt+F8" );
			menu.Append( eID_RUN_TESTS_ACCEPTABLE, "Run all <acceptable> tests\tCtrl+Alt+F9" );
			menu.Append( eID_RUN_TESTS_ALL_BUT_NEGLIGIBLE, "Run all but <negligible> tests\tCtrl+F10" );
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

	void MainFrame::doInitDatabase( wxProgressDialog & progress
		, int & index )
	{
		std::string createTableTest = "CREATE TABLE IF NOT EXISTS Test";
		createTableTest += "( Id INTEGER PRIMARY KEY\n";
		createTableTest += "\t, Name VARCHAR(1024)\n";
		createTableTest += "\t, RunDate DATETIME\n";
		createTableTest += "\t, Status INTEGER\n";
		createTableTest += "\t, Renderer VARCHAR(10)\n";
		createTableTest += "\t, Category VARCHAR(50)\n";
		createTableTest += ");";
		m_database.executeUpdate( createTableTest );
	}

	TestMap MainFrame::doListCategories( wxProgressDialog & progress
		, int & index
		, uint32_t & testCount )
	{
		TestMap result;
		auto categories = listTestCategories( m_config.test );
		castor::Logger::logInfo( "Listing Test files" );
		progress.SetTitle( _( "Listing Test files" ) );
		progress.SetRange( progress.GetRange() + int( categories.size() ) );
		progress.Update( index, _( "isting Test files..." ) );

		for ( auto & categoryPath : categories )
		{
			progress.Update( index++, wxT( "Category " ) + makeWxString( categoryPath.getFileName() ) + wxT( "..." ) );
			testCount += listTests( categoryPath, result );
		}

		return result;
	}

	void MainFrame::doPopulateDatabase( wxProgressDialog & progress
		, int & index
		, TestMap const & tests
		, uint32_t testCount )
	{
		castor::Logger::logInfo( "Populating database" );
		progress.SetTitle( _( "Populating database" ) );
		progress.SetRange( progress.GetRange() + int( testCount ) );
		progress.Update( index, _( "Populating database..." ) );

		auto finderDate = m_database.createStatement( "SELECT Id, Status FROM Test WHERE Name=? AND Category=? AND RunDate=?;" );
		auto findDateName = finderDate->createParameter( "Name", db::FieldType::eVarchar, 1024 );
		auto findDateCategory = finderDate->createParameter( "Category", db::FieldType::eVarchar, 50 );
		auto findDateRunDate = finderDate->createParameter( "RunDate", db::FieldType::eDatetime );
		finderDate->initialise();

		auto finderName = m_database.createStatement( "SELECT Id FROM Test WHERE Name=? AND Category=?;" );
		auto findNameName = finderName->createParameter( "Name", db::FieldType::eVarchar, 1024 );
		auto findNameCategory = finderName->createParameter( "Category", db::FieldType::eVarchar, 50 );
		finderName->initialise();

		auto updater = m_database.createStatement( "UPDATE Test SET Status=?, Renderer=? WHERE Id=?;" );
		auto updateStatus = updater->createParameter( "Status", db::FieldType::eSint32 );
		auto updateRenderer = updater->createParameter( "Renderer", db::FieldType::eVarchar, 10 );
		auto updateId = updater->createParameter( "Id", db::FieldType::eSint32 );
		updater->initialise();


		for ( auto & tests : tests )
		{
			for ( auto test : tests.second )
			{
				progress.Update( index++, makeWxString( test.category ) + wxT( " - " ) + makeWxString( test.name ) + wxT( "..." ) );
				db::ResultPtr result;

				if ( test.status != TestStatus::eNotRun )
				{
					findDateName->setValue( test.name );
					findDateCategory->setValue( test.category );
					findDateRunDate->setValue( test.runDate );
					result = finderDate->executeSelect();

					if ( result->empty() )
					{
						insertTest( test );
					}
					else
					{
						auto & row = *result->begin();
						auto status = row.getField( 1u ).getValue< int32_t >();

						if ( test.status != TestStatus( status ) )
						{
							m_updateTestStatus.id->setValue( row.getField( 0u ).getValue< int32_t >() );
							m_updateTestStatus.status->setValue( int32_t( test.status ) );
							m_updateTestStatus.stmt->executeUpdate();
							moveTestFile( test, m_config.test, m_config.work );
						}
					}
				}
				else
				{
					findNameName->setValue( test.name );
					findNameCategory->setValue( test.category );
					result = finderName->executeSelect();

					if ( result->empty() )
					{
						castor::PathArray files = findTestResults( test, m_config.work );

						for ( auto & file : files )
						{
							// file = .../Status/YY-MM-DD_HH-MM-SS_Name_Renderer.png
							auto fullName = file.getFileName();
							size_t index = 0;

							//Status
							test.status = getStatus( file.getPath().getFileName() );

							// Date
							// YY-MM-DD
							index = fullName.find( '_', index );
							assert( index != castor::String::npos );
							// YY-MM-DD_HH-MM-SS
							index = fullName.find( '_', index );
							assert( index != castor::String::npos );
							auto isDate = isDateTime( fullName.substr( 0, index ), test.runDate );
							assert( isDate );

							// Renderer
							auto lastIndex = fullName.find_last_of( '_' );
							assert( index != lastIndex );
							test.renderer = fullName.substr( lastIndex );

							insertTest( test, false );
						}
					}
				}
			}
		}
	}

	void MainFrame::doFillDatabase( wxProgressDialog & progress
		, int & index )
	{
		uint32_t testCount = 0u;
		auto tests = doListCategories( progress, index, testCount );
		doPopulateDatabase( progress, index, tests, testCount );
	}

	uint32_t MainFrame::doListTests( wxProgressDialog & progress
		, int & index )
	{
		uint32_t testCount{};
		std::string listTests = "SELECT Id, Name, MAX( RunDate ) AS RunDate, Status, Renderer, Category\n";
		listTests += "FROM Test\n";
		listTests += "GROUP BY Category, Name\n";
		listTests += "ORDER BY Category, Name;";
		auto result = m_database.executeSelect( listTests );
		std::string prevCategory;
		auto categoryIt = m_tests.end();
		castor::Logger::logInfo( "Listing tests" );
		progress.SetTitle( _( "Listing tests" ) );
		progress.SetRange( progress.GetRange() + int( result->size() ) );
		progress.Update( index, _( "Listing tests..." ) );

		for ( auto & row : *result )
		{
			Test test{ uint32_t( row.getField( 0u ).getValue< int32_t >() )
				, row.getField( 1u ).getValue< std::string >()
				, row.getField( 2u ).getValue< db::DateTime >()
				, TestStatus( row.getField( 3u ).getValue< int32_t >() )
				, row.getField( 4u ).getValue< std::string >()
				, row.getField( 5u ).getValue< std::string >() };

			if ( prevCategory != test.category )
			{
				categoryIt = m_tests.emplace( test.category, TestArray{} ).first;
				prevCategory = test.category;
			}

			categoryIt->second.push_back( test );
			++testCount;
			progress.Update( index++, makeWxString( test.category ) + wxT( " - " ) + makeWxString( test.name ) + wxT( "..." ) );
		}

		return testCount;
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
#if !CTP_UseAsync
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
				castor::Logger::logError( castor::System::getLastErrorText() );
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
		bool wasAllTests = m_selected.allTests;
		bool wasAllCategories = m_selected.allCategories;
		m_selected.allTests = true;
		m_selected.allCategories = true;
		m_view->GetSelections( m_selected.items );
		bool displayTest = false;
		bool displayCategory = false;

		if ( m_selected.items.size() == 1 )
		{
			auto node = static_cast< TreeModelNode * >( m_selected.items[0].GetID() );
			Test * test{};
			wxString category;

			if ( node )
			{
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
#if !CTP_UseAsync
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
		auto & testNode = *m_runningTest.tests.begin();
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
			insertTest( test );
			m_model->ItemChanged( wxDataViewItem{ testNode.node } );

			if ( m_detailViews->isLayerShown( 1 )
				&& m_testView->getTest() == &test )
			{
				m_testView->setTest( test );
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
