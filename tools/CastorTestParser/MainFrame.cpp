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

#include <castor.xpm>

#include <wx/progdlg.h>
#include <wx/sizer.h>

namespace test_parser
{
	//*********************************************************************************************

	namespace
	{
		enum ID
		{
			eID_GRID,
			eID_DETAIL,
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

		std::map< castor::String, Test > listAllResults( castor::Path const & categoryPath
			, std::map< castor::Path, TestStatus > const & folders )
		{
			std::map< castor::String, Test > results;
			auto category = castor::string::stringCast< char >( categoryPath.getFileName() );

			for ( auto & status : folders )
			{
				castor::File::filterDirectoryFiles( status.first
					, [&results, &status, &category]( castor::Path const & folder, castor::String const & name )
					{
						auto result = castor::Path{ name }.getExtension() == cuT( "png" );

						if ( result )
						{
							makeTest( status.second, category, folder / name, results );
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
			std::map< castor::Path, TestStatus > folders
			{
				{ compareFolder / "Negligible", TestStatus::eNegligible },
				{ compareFolder / "Acceptable", TestStatus::eAcceptable },
				{ compareFolder / "Unacceptable", TestStatus::eUnacceptable },
				{ compareFolder / "Unprocessed", TestStatus::eUnprocessed },
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
	}

	//*********************************************************************************************

	std::string getFolderName( TestStatus value )
	{
		switch ( value )
		{
		case TestStatus::eNotRun:
			return "NotRun";
		case TestStatus::eNegligible:
			return "Negligible";
		case TestStatus::eAcceptable:
			return "Acceptable";
		case TestStatus::eUnacceptable:
			return "Unacceptable";
		case TestStatus::eUnprocessed:
			return "Unprocessed";
		default:
			assert( false );
			return std::string{};
		}
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
		Show( true );

		if ( config.init )
		{
			doInitDatabase();
			doFillDatabase();
		}
		else
		{
			doListTests();
		}

		doFillList();
	}

	MainFrame::~MainFrame()
	{
		m_auiManager.UnInit();
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
			, 0 };
		// Intently inverted back and fore.
		m_view->SetBackgroundColour( PANEL_FOREGROUND_COLOUR );
		m_view->SetForegroundColour( PANEL_BACKGROUND_COLOUR );
		sizerList->Add( m_view, wxSizerFlags( 1 ).Expand() );
		listPanel->SetSizer( sizerList );
		sizerList->SetSizeHints( listPanel );
		m_model = new TreeModel;
		m_view->AssociateModel( m_model.get() );
		m_view->Connect( wxEVT_DATAVIEW_ITEM_ACTIVATED
			, wxDataViewEventHandler( MainFrame::onItemActivated )
			, nullptr
			, this );
		uint32_t flags = wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE;
		auto renCategory = new wxDataViewTextRenderer{ wxDataViewTextRenderer::GetDefaultType() };
		wxDataViewColumn * colCategory = new wxDataViewColumn( _( "Category" ), renCategory, int( TreeModel::Column::eCategory ), 150, wxALIGN_LEFT, flags );
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
		auto renStatus = new wxDataViewBitmapRenderer{ wxDataViewBitmapRenderer::GetDefaultType() };
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

	void MainFrame::doInitDatabase()
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

	void MainFrame::doFillDatabase()
	{
		uint32_t testCount = 0u;
		castor::PathArray categories;
		{
			categories = listTestCategories( m_config.test );
			wxProgressDialog progress{ wxT( "Listing Test files" )
				, wxT( "Listing Test files..." )
				, int( categories.size() - 1 )
				, this };
			int index = 0;

			for ( auto & categoryPath : categories )
			{
				progress.Update( index++, wxT( "Category " ) + makeWxString( categoryPath.getFileName() ) + wxT( "..." ) );
				testCount += listTests( categoryPath, m_tests );
			}
		}

		{
			wxProgressDialog progress{ wxT( "Populating database" )
				, wxT( "Populating database..." )
				, int( testCount - 1 )
				, this };
			int index = 0;
			auto updater = m_database.createStatement( "UPDATE Test SET RunDate=?, Status=?, Renderer=?, Category=? WHERE Id=?;" );
			auto updateRunDate = updater->createParameter( "RunDate", db::FieldType::eDatetime );
			auto updateStatus = updater->createParameter( "Status", db::FieldType::eSint32 );
			auto updateRenderer = updater->createParameter( "Renderer", db::FieldType::eVarchar, 10 );
			auto updateCategory = updater->createParameter( "Category", db::FieldType::eVarchar, 50 );
			auto updateId = updater->createParameter( "Id", db::FieldType::eSint32 );
			updater->initialise();
			
			auto inserter = m_database.createStatement( "INSERT INTO Test(Name, RunDate, Status, Renderer, Category) VALUES (?, ?, ?, ?, ?);" );
			auto insertName = inserter->createParameter( "Name", db::FieldType::eVarchar, 1024 );
			auto insertRunDate = inserter->createParameter( "RunDate", db::FieldType::eDatetime );
			auto insertStatus = inserter->createParameter( "Status", db::FieldType::eSint32 );
			auto insertRenderer = inserter->createParameter( "Renderer", db::FieldType::eVarchar, 10 );
			auto insertCategory = inserter->createParameter( "Category", db::FieldType::eVarchar, 50 );
			inserter->initialise();

			for ( auto & tests : m_tests )
			{
				for ( auto test : tests.second )
				{
					progress.Update( index++, makeWxString( test.category ) + wxT( " - " ) + makeWxString( test.name ) + wxT( "..." ) );
					std::string findTest = "SELECT Id\n";
					findTest += "FROM Test\n";
					findTest += "WHERE Name=" + m_database.writeText( test.name ) + "\n";
					findTest += "	AND Category=" + m_database.writeText( test.category ) + ";";
					auto result = m_database.executeSelect( findTest );

					if ( !result->empty() )
					{
						auto & firstRow = *result->begin();
						std::string updateTest = "UPDATE Test\n";
						updateRunDate->setValue( test.runDate );
						updateStatus->setValue( int( test.status ) );
						updateRenderer->setValue( test.renderer );
						updateCategory->setValue( test.category );
						updateId->setValue( firstRow.getField( 0u ).getValue< int32_t >() );
						updater->executeUpdate();
						castor::Logger::logInfo( "Updated: " + test.category + " - " + test.renderer + " - " + test.name );
					}
					else
					{
						insertName->setValue( test.name );
						insertRunDate->setValue( test.runDate );
						insertStatus->setValue( int( test.status ) );
						insertRenderer->setValue( test.renderer );
						insertCategory->setValue( test.category );
						inserter->executeUpdate();
						castor::Logger::logInfo( "Inserted: " + test.category + " - " + test.renderer + " - " + test.name );
					}
				}
			}
		}
	}

	void MainFrame::doListTests()
	{
		std::string listTests = "SELECT Id, Name, MAX( RunDate ) AS RunDate, Status, Renderer, Category\n";
		listTests += "FROM Test\n";
		listTests += "GROUP BY Category, Name, RunDate\n";
		listTests += "ORDER BY Category, Name;";
		auto result = m_database.executeSelect( listTests );
		std::string prevCategory;
		auto categoryIt = m_tests.end();

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
		}
	}

	void MainFrame::doFillList()
	{
		for ( auto & tests : m_tests )
		{
			auto node = addCategory( *m_model, tests.first );

			for ( auto & test : tests.second )
			{
				addTest( *m_model, node, test );
			}
		}

		m_view->Expand( wxDataViewItem{ m_model->GetRootNode() } );
	}

	void MainFrame::onItemActivated( wxDataViewEvent & evt )
	{
		auto node = static_cast< TreeModelNode * >( evt.GetItem().GetID() );

		if ( node )
		{
			if ( node->test )
			{
				m_testView->setTest( *node->test );
				m_detailViews->showLayer( 1 );
			}
			else
			{
				auto it = m_tests.find( makeStdString( node->category ) );
				assert( it != m_tests.end() );
				m_categoryView->setCategory( node->category, it->second );
				m_detailViews->showLayer( 2 );
			}
		}
		else
		{
			m_detailViews->hideLayers();
		}

		m_auiManager.Update();
	}

	//*********************************************************************************************
}
