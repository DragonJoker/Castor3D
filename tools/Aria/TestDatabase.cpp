#include "Aria/TestDatabase.hpp"

#include "Aria/DatabaseTest.hpp"
#include "Aria/Database/DbDateTimeHelpers.hpp"
#include "Aria/Database/DbResult.hpp"
#include "Aria/Database/DbStatement.hpp"

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Miscellaneous/Utils.hpp>

#include <wx/progdlg.h>

#include <unordered_map>

namespace aria
{
	//*********************************************************************************************

	namespace
	{
		template< typename HashT >
		IdValue * getIdValue( std::string const & name
			, std::unordered_map< std::string, IdValuePtr, HashT > & map
			, TestDatabase::InsertIdValue & insertIdValue )
		{
			auto ires = map.emplace( name, nullptr );

			if ( ires.second )
			{
				ires.first->second = std::make_unique< IdValue >( 0, name );
			}

			auto idValue = ires.first->second.get();

			if ( idValue->id == 0 )
			{
				idValue->id = insertIdValue.insert( idValue->name );
			}

			return idValue;
		}

		template< typename HashT >
		IdValue * getIdValue( int32_t id
			, std::unordered_map< std::string, IdValuePtr, HashT > & map )
		{
			auto it = std::find_if( map.begin()
				, map.end()
				, [id]( RendererMap::value_type const & lookup )
				{
					return lookup.second->id == id;
				} );

			if ( it == map.end() )
			{
				return nullptr;
			}

			return it->second.get();
		}

		Renderer getRenderer( std::string const & name
			, RendererMap & renderers
			, TestDatabase::InsertRenderer & insertRenderer )
		{
			return getIdValue( name, renderers, insertRenderer );
		}

		Renderer getRenderer( int32_t id
			, RendererMap & renderers )
		{
			return getIdValue( id, renderers );
		}

		Category getCategory( std::string const & name
			, CategoryMap & categories
			, TestDatabase::InsertCategory & insertCategory )
		{
			return getIdValue( name, categories, insertCategory );
		}

		IdValue * getCategory( int32_t id
			, CategoryMap & categories )
		{
			return getIdValue( id, categories );
		}

		Keyword getKeyword( std::string const & name
			, KeywordMap & keywords
			, TestDatabase::InsertKeyword & insertCategory )
		{
			return getIdValue( name, keywords, insertCategory );
		}

		Keyword getKeyword( int32_t id
			, KeywordMap & keywords )
		{
			return getIdValue( id, keywords );
		}

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

		TestArray::iterator findTest( TestArray & result
			, std::string const & name )
		{
			return std::find_if( result.begin()
				, result.end()
				, [&name]( TestPtr const & lookup )
				{
					return lookup->name == name;
				} );
		}

		void makeTestRun( Config const & config
			, TestDatabase::InsertRenderer & insertRenderer
			, TestDatabase::InsertTest & insertTest
			, TestDatabase::InsertRun & insertRun
			, IdValue * category
			, TestArray & categoryTests
			, castor::Path const & imgPath
			, TestStatus status
			, RendererMap & renderers )
		{
			auto name = castor::string::stringCast< char >( imgPath.getFileName() );
			auto prevDotIdx = name.find_last_of( "." );

			if ( prevDotIdx == std::string::npos
				|| name.substr( prevDotIdx + 1 ) != "diff" )
			{
				auto rendererIdx = name.find_last_of( "_" );

				if ( rendererIdx != std::string::npos )
				{
					auto renderer = getRenderer( name.substr( rendererIdx + 1 )
						, renderers
						, insertRenderer );
					name = name.substr( 0, rendererIdx );
					auto runDate = getFileDate( imgPath );

					if ( db::date_time::isValid( runDate ) )
					{
						auto it = findTest( categoryTests, name );

						if ( it == categoryTests.end() )
						{
							it = categoryTests.emplace( it
								, std::make_unique< Test >( 0
									, name
									, category ) );
							( *it )->id = insertTest.insert( category->id, name );
						}

						Test & test = *( *it );
						insertRun.insert( test.id
							, renderer->id
							, runDate
							, status
							, runDate
							, runDate );
					}
				}
			}
		}

		void listAllResults( Config const & config
			, TestDatabase::InsertRenderer & insertRenderer
			, TestDatabase::InsertTest & insertTest
			, TestDatabase::InsertRun & insertRun
			, castor::Path const & categoryPath
			, castor::PathArray const & folders
			, Category category
			, TestArray & categoryTests
			, RendererMap & renderers )
		{
			for ( auto & status : folders )
			{
				auto testStatus = getStatus( status.getFileName() );
				castor::File::filterDirectoryFiles( status
					, [&config, &insertRenderer, &insertTest, &insertRun, &renderers, &status, &categoryTests, category, testStatus]( castor::Path const & folder, castor::String const & name )
					{
						auto result = castor::Path{ name }.getExtension() == cuT( "png" );

						if ( result )
						{
							makeTestRun( config
								, insertRenderer
								, insertTest
								, insertRun
								, category
								, categoryTests
								, folder / name
								, testStatus
								, renderers );
						}

						return result;
					} );
			}
		}

		TestArray listCategoryTestFiles( Config const & config
			, TestDatabase::InsertRenderer & insertRenderer
			, TestDatabase::InsertTest & insertTest
			, TestDatabase::InsertRun & insertRun
			, castor::Path const & categoryPath
			, Category category
			, RendererMap & renderers )
		{
			auto renderer = getRenderer( "vk", renderers, insertRenderer );
			auto compareFolder = categoryPath / cuT( "Compare" );
			castor::PathArray folders
			{
				compareFolder / "Negligible",
				compareFolder / "Acceptable",
				compareFolder / "Unacceptable",
				compareFolder / "Unprocessed",
			};
			auto scenes = listScenes( categoryPath );
			TestArray result;

			for ( auto & testScene : scenes )
			{
				auto sceneName = testScene.getFileName();
				auto it = findTest( result, sceneName );

				if ( it == result.end() )
				{
					auto test = std::make_unique< Test >( 0
						, sceneName
						, category );
					test->id = insertTest.insert( test->category->id, test->name );
					result.push_back( std::move( test ) );
				}
			}

			listAllResults( config
				, insertRenderer
				, insertTest
				, insertRun
				, categoryPath
				, folders
				, category
				, result
				, renderers );

			return result;
		}

		void moveTestFile( TestRun const & test
			, castor::Path srcFolder
			, castor::Path dstFolder )
		{
			if ( test.status == TestStatus::eNotRun )
			{
				return;
			}

			srcFolder /= getCompareFolder( test );
			dstFolder /= getResultFolder( test );
			moveFile( srcFolder
				, dstFolder
				, getCompareName( test )
				, getResultName( test )
				, false );
		}
	}

	//*********************************************************************************************

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

	//*********************************************************************************************

	int32_t TestDatabase::InsertIdValue::insert( std::string const & inName )
	{
		name->setValue( inName );
		sName->setValue( inName );

		if ( !stmt->executeUpdate() )
		{
			return -1;
		}

		auto result = select->executeSelect();

		if ( !result || result->empty() )
		{
			return -1;
		}

		return result->begin()->getField( 0 ).getValue< int32_t >();
	}

	//*********************************************************************************************

	void TestDatabase::InsertIdId::insert( int32_t lhs, int32_t rhs )
	{
		lhsId->setValue( lhs );
		rhsId->setValue( rhs );

		if ( !stmt->executeUpdate() )
		{
			throw std::runtime_error{ "Couldn't insert entry" };
		}
	}

	//*********************************************************************************************

	int32_t TestDatabase::InsertTest::insert( int32_t inCategoryId
		, std::string const & inName
		, bool inIgnoreResult )
	{
		categoryId->setValue( inCategoryId );
		sCategoryId->setValue( inCategoryId );
		name->setValue( inName );
		sName->setValue( inName );
		ignoreResult->setValue( inIgnoreResult ? 1 : 0 );

		if ( !stmt->executeUpdate() )
		{
			return -1;
		}

		auto result = select->executeSelect();

		if ( !result || result->empty() )
		{
			return -1;
		}

		return result->begin()->getField( 0 ).getValue< int32_t >();
	}

	//*********************************************************************************************

	int32_t TestDatabase::InsertRun::insert( int32_t id
		, int32_t inRendererId
		, db::DateTime dateRun
		, TestStatus inStatus
		, db::DateTime const & dateCastor
		, db::DateTime const & dateScene )
	{
		testId->setValue( id );
		sTestId->setValue( id );
		rendererId->setValue( inRendererId );
		sRendererId->setValue( inRendererId );
		runDate->setValue( dateRun );
		sRunDate->setValue( dateRun );
		status->setValue( int32_t( inStatus ) );
		sStatus->setValue( int32_t( inStatus ) );
		castorDate->setValue( dateCastor );
		sCastorDate->setValue( dateCastor );
		sceneDate->setValue( dateScene );
		sSceneDate->setValue( dateScene );

		if ( !stmt->executeUpdate() )
		{
			return -1;
		}

		auto result = select->executeSelect();

		if ( !result || result->empty() )
		{
			return -1;
		}

		return result->begin()->getField( 0 ).getValue< int32_t >();
	}

	//*********************************************************************************************

	bool TestDatabase::CheckTableExists::checkTable( std::string const & name )
	{
		tableName->setValue( name );
		auto result = stmt->executeSelect();
		return result && !result->empty();
	}

	//*********************************************************************************************

	TestMap TestDatabase::ListTests::listTests( CategoryMap & categories
		, wxProgressDialog & progress
		, int & index )
	{
		TestMap result;
		listTests( categories, result, progress, index );
		return result;
	}

	void TestDatabase::ListTests::listTests( CategoryMap & categories
		, TestMap & result
		, wxProgressDialog & progress
		, int & index )
	{
		if ( auto res = stmt->executeSelect() )
		{
			progress.SetRange( int( progress.GetRange() + res->size() ) );

			for ( auto & row : *res )
			{
				auto id = row.getField( 0 ).getValue< int32_t >();
				auto catId = row.getField( 1 ).getValue< int32_t >();
				auto name = row.getField( 2 ).getValue< std::string >();
				auto category = getCategory( catId, categories );
				auto catIt = result.emplace( category, TestArray{} ).first;
				catIt->second.emplace_back( std::make_unique< Test >( id, name, category ) );
				progress.Update( index++
					, _( "Listing tests" )
					+ wxT( "\n" ) + getDetails( *catIt->second.back() ) );
			}
		}
		else
		{
			throw std::runtime_error{ "Couldn't list tests" };
		}
	}

	//*********************************************************************************************

	db::ResultPtr TestDatabase::ListLatestTestRun::listTests( int32_t inId )
	{
		testId->setValue( inId );
		return stmt->executeSelect();
	}

	//*********************************************************************************************

	TestRunCategoryMap TestDatabase::ListLatestRendererTests::listTests( TestMap const & tests
		, CategoryMap & categories
		, Renderer renderer
		, wxProgressDialog & progress
		, int & index )
	{
		TestRunCategoryMap result;
		listTests( tests, categories, renderer, result, progress, index );
		return result;
	}

	void TestDatabase::ListLatestRendererTests::listTests( TestMap const & tests
		, CategoryMap & categories
		, Renderer renderer
		, TestRunCategoryMap & result
		, wxProgressDialog & progress
		, int & index )
	{
		// Prefill result with "not run" entries.
		for ( auto & cat : tests )
		{
			auto catIt = result.emplace( cat.first, TestRunArray{} ).first;

			for ( auto & test : cat.second )
			{
				catIt->second.emplace_back( *database
					, TestRun{ test.get()
						, renderer
						, db::DateTime{}
						, TestStatus::eNotRun
						, db::DateTime{}
						, db::DateTime{} } );
			}
		}

		rendererId->setValue( renderer->id );

		if ( auto res = stmt->executeSelect() )
		{
			progress.SetRange( int( progress.GetRange() + res->size() ) );

			for ( auto & row : *res )
			{
				auto catId = row.getField( 0 ).getValue< int32_t >();
				auto catIt = tests.find( getCategory( catId, categories ) );

				if ( catIt != tests.end() )
				{
					auto testId = row.getField( 1 ).getValue< int32_t >();
					auto testIt = std::find_if( catIt->second.begin()
						, catIt->second.end()
						, [testId]( TestPtr const & lookup )
						{
							return lookup->id == testId;
						} );

					if ( testIt != catIt->second.end() )
					{
						auto & test = *testIt->get();
						auto rendIt = tests.find( getCategory( catId, categories ) );
						auto runId = row.getField( 2 ).getValue< int32_t >();
						auto runDate = row.getField( 3 ).getValue< db::DateTime >();
						auto status = TestStatus( row.getField( 4 ).getValue< int32_t >() );
						auto castorDate = row.getField( 5 ).getValue< db::DateTime >();
						auto sceneDate = row.getField( 6 ).getValue< db::DateTime >();
						auto runIt = result.find( catIt->first );
						assert( runIt != result.end() );
						auto it = std::find_if( runIt->second.begin()
							, runIt->second.end()
							, [testId]( DatabaseTest const & lookup )
							{
								return lookup->test->id == testId;
							} );

						if ( it == runIt->second.end() )
						{
							assert( false );
							runIt->second.emplace_back( *database
								, TestRun{ &test
									, renderer
									, runDate
									, status
									, castorDate
									, sceneDate } );
							runIt->second.back().update( runId );
							progress.Update( index++
								, _( "Listing latest runs" )
								+ wxT( "\n" ) + getProgressDetails( runIt->second.back() ) );
						}
						else
						{
							assert( it->getStatus() == TestStatus::eNotRun );
							it->update( runId
								, runDate
								, status
								, castorDate
								, sceneDate );
							progress.Update( index++
								, _( "Listing latest runs" )
								+ wxT( "\n" ) + getProgressDetails( *it ) );
						}
					}
				}
			}
		}
		else
		{
			throw std::runtime_error{ "Couldn't list tests runs" };
		}
	}

	//*********************************************************************************************

	TestDatabase::TestDatabase( Config config )
		: m_config{ std::move( config ) }
		, m_database{ m_config.database }
	{
	}

	TestDatabase::~TestDatabase()
	{
	}

	void TestDatabase::initialise( wxProgressDialog & progress
		, int & index )
	{
		// Necessary database initialisation
		m_checkTableExists = CheckTableExists{ m_database };
		auto catRenInit = false;

		if ( !m_checkTableExists.checkTable( "Test" ) )
		{
			doCreateV1( progress, index );
		}

		if ( !m_checkTableExists.checkTable( "TestsDatabase" ) )
		{
			doCreateV2( progress, index );
			catRenInit = true;
		}
		else
		{
			m_insertTest = InsertTest{ m_database };
			m_insertRun = InsertRun{ m_database };
			m_insertCategory = InsertCategory{ m_database };
			m_insertRenderer = InsertRenderer{ m_database };
		}

		if ( !m_checkTableExists.checkTable( "Keyword" ) )
		{
			doCreateV3( progress, index );
		}
		else
		{
			m_insertKeyword = InsertKeyword{ m_database };
			m_insertTestKeyword = InsertTestKeyword{ m_database };
			m_insertCategoryKeyword = InsertCategoryKeyword{ m_database };
		}

		m_updateRunStatus = UpdateRunStatus{ m_database };
		m_updateTestIgnoreResult = UpdateTestIgnoreResult{ m_database };
		m_updateRunDates = UpdateRunDates{ m_database };
		m_updateRunCastorDate = UpdateRunCastorDate{ m_database };
		m_updateRunSceneDate = UpdateRunSceneDate{ m_database };
		m_listTests = ListTests{ m_database };
		m_listLatestRun = ListLatestTestRun{ m_database };
		m_listLatestRendererRuns = ListLatestRendererTests{ this };

		if ( m_config.initFromFolder )
		{
			doFillDatabase( progress, index );
			catRenInit = true;
		}

		if ( !catRenInit )
		{
			if ( auto result = m_database.executeSelect( "SELECT Id, Name FROM Category;" ) )
			{
				for ( auto & row : *result )
				{
					auto id = row.getField( 0 ).getValue< int32_t >();
					auto name = row.getField( 1 ).getValue< std::string >();
					m_categories.emplace( name, std::make_unique< IdValue >( id, name ) );
				}
			}
			else
			{
				throw std::runtime_error{ "Couldn't list categories" };
			}

			if ( auto result = m_database.executeSelect( "SELECT Id, Name FROM Renderer;" ) )
			{
				for ( auto & row : *result )
				{
					auto id = row.getField( 0 ).getValue< int32_t >();
					auto name = row.getField( 1 ).getValue< std::string >();
					m_renderers.emplace( name, std::make_unique< IdValue >( id, name ) );
				}
			}
			else
			{
				throw std::runtime_error{ "Couldn't list renderers" };
			}
		}
	}

	Renderer TestDatabase::createRenderer( std::string const & name )
	{
		return getRenderer( name, m_renderers, m_insertRenderer );
	}

	Category TestDatabase::createCategory( std::string const & name )
	{
		return getCategory( name, m_categories, m_insertCategory );
	}

	Keyword TestDatabase::createKeyword( std::string const & name )
	{
		return getKeyword( name, m_keywords, m_insertKeyword );
	}

	TestMap TestDatabase::listTests()
	{
		TestMap result;
		listTests( result );
		return result;
	}

	TestMap TestDatabase::listTests( wxProgressDialog & progress
		, int & index )
	{
		TestMap result;
		listTests( result, progress, index );
		return result;
	}

	void TestDatabase::listTests( TestMap & result )
	{
		wxProgressDialog progress{ wxT( "Listing tests" )
			, wxT( "Listing tests..." )
			, int( 1 )
			, nullptr };
		int index = 0;
		listTests( result, progress, index );
	}

	void TestDatabase::listTests( TestMap & result
		, wxProgressDialog & progress
		, int & index )
	{
		castor::Logger::logInfo( "Listing tests" );
		progress.SetTitle( _( "Listing tests" ) );
		progress.Update( index
			, _( "Listing tests" )
			+ wxT( "\n" ) + _( "..." ) );
		m_listTests.listTests( m_categories, result, progress, index );
	}

	TestRunMap TestDatabase::listLatestRuns( TestMap const & tests )
	{
		TestRunMap result;
		listLatestRuns( tests, result );
		return result;
	}

	TestRunMap TestDatabase::listLatestRuns( TestMap const & tests
		, wxProgressDialog & progress
		, int & index )
	{
		TestRunMap result;
		listLatestRuns( tests, result, progress, index );
		return result;
	}

	void TestDatabase::listLatestRuns( TestMap const & tests
		, TestRunMap & result )
	{
		wxProgressDialog progress{ wxT( "Listing latest runs" )
			, wxT( "Listing latest runs..." )
			, int( 1 )
			, nullptr };
		int index = 0;
		listLatestRuns( tests, result, progress, index );
	}

	void TestDatabase::listLatestRuns( TestMap const & tests
		, TestRunMap & result
		, wxProgressDialog & progress
		, int & index )
	{
		castor::Logger::logInfo( "Listing latest runs" );
		progress.SetTitle( _( "Listing latest runs" ) );
		progress.Update( index, _( "Listing latest runs\n..." ) );

		for ( auto & renderer : m_renderers )
		{
			result[renderer.second.get()] = listLatestRuns( renderer.second.get(), tests, progress, index );
		}
	}

	TestRunCategoryMap TestDatabase::listLatestRuns( Renderer renderer
		, TestMap const & tests )
	{
		TestRunCategoryMap result;
		listLatestRuns( renderer, tests, result );
		return result;
	}

	TestRunCategoryMap TestDatabase::listLatestRuns( Renderer renderer
		, TestMap const & tests
		, wxProgressDialog & progress
		, int & index )
	{
		TestRunCategoryMap result;
		listLatestRuns( renderer, tests, result, progress, index );
		return result;
	}

	void TestDatabase::listLatestRuns( Renderer renderer
		, TestMap const & tests
		, TestRunCategoryMap & result )
	{
		wxProgressDialog progress{ wxT( "Listing latest renderer runs" )
			, wxT( "Listing latest renderer runs..." )
			, int( 1 )
			, nullptr };
		int index = 0;
		listLatestRuns( renderer, tests, result, progress, index );
	}

	void TestDatabase::listLatestRuns( Renderer renderer
		, TestMap const & tests
		, TestRunCategoryMap & result
		, wxProgressDialog & progress
		, int & index )
	{
		castor::Logger::logInfo( "Listing latest renderer runs" );
		progress.SetTitle( _( "Listing latest renderer runs" ) );
		progress.Update( index, _( "Listing latest renderer runs\n..." ) );
		m_listLatestRendererRuns.listTests( tests, m_categories, renderer, result, progress, index );
	}

	void TestDatabase::insertTest( Test & test
		, bool moveFiles )
	{
		test.id = m_insertTest.insert( test.category->id
			, test.name );
		castor::Logger::logInfo( "Inserted: " + getDetails( test ) );
	}

	void TestDatabase::insertRun( TestRun & run
		, bool moveFiles )
	{
		run.id = m_insertRun.insert( run.test->id
			, run.renderer->id
			, run.runDate
			, run.status
			, run.castorDate
			, run.sceneDate );

		if ( moveFiles )
		{
			moveTestFile( run, m_config.test, m_config.work );
		}

		castor::Logger::logInfo( "Inserted: " + getDetails( run ) );
	}

	void TestDatabase::updateTestIgnoreResult( Test const & test
		, bool ignore )
	{
		m_updateTestIgnoreResult.ignore->setValue( int32_t( ignore ? 1 : 0 ) );
		m_updateTestIgnoreResult.id->setValue( int32_t( test.id ) );
		m_updateTestIgnoreResult.stmt->executeUpdate();
		castor::Logger::logInfo( "Updated ignore result for: " + getDetails( test ) );
	}

	void TestDatabase::updateRunStatus( TestRun const & run )
	{
		m_updateRunStatus.status->setValue( int32_t( run.status ) );
		m_updateRunStatus.castorDate->setValue( run.castorDate );
		m_updateRunStatus.sceneDate->setValue( run.sceneDate );
		m_updateRunStatus.id->setValue( int32_t( run.id ) );
		m_updateRunStatus.stmt->executeUpdate();
		castor::Logger::logInfo( "Updated status for: " + getDetails( run ) );
	}

	void TestDatabase::updateRunCastorDate( TestRun const & run )
	{
		m_updateRunCastorDate.castorDate->setValue( run.castorDate );
		m_updateRunCastorDate.id->setValue( int32_t( run.id ) );
		m_updateRunCastorDate.stmt->executeUpdate();
		castor::Logger::logInfo( "Updated Castor3D date for: " + getDetails( run ) );
	}

	void TestDatabase::updateRunSceneDate( TestRun const & run )
	{
		m_updateRunSceneDate.sceneDate->setValue( getSceneDate( m_config, run ) );
		m_updateRunSceneDate.id->setValue( int32_t( run.id ) );
		m_updateRunSceneDate.stmt->executeUpdate();
		castor::Logger::logInfo( "Updated Scene date for: " + getDetails( run ) );
	}

	void TestDatabase::doCreateV1( wxProgressDialog & progress, int & index )
	{
		auto saveRange = progress.GetRange();
		auto saveIndex = index;
		progress.SetTitle( _( "Creating tests database" ) + wxT( " V3" ) );
		index = 0;
		auto transaction = m_database.beginTransaction( "DatabaseUpdate" );

		if ( !transaction )
		{
			throw std::runtime_error{ "Couldn't begin a transaction." };
		}

		try
		{
			std::string createTableTest = "CREATE TABLE Test";
			createTableTest += "( Id INTEGER PRIMARY KEY\n";
			createTableTest += "\t, Name VARCHAR(1024)\n";
			createTableTest += "\t, RunDate DATETIME\n";
			createTableTest += "\t, Status INTEGER\n";
			createTableTest += "\t, Renderer VARCHAR(10)\n";
			createTableTest += "\t, Category VARCHAR(50)\n";
			createTableTest += "\t, IgnoreResult INTEGER\n";
			createTableTest += "\t, CastorDate DATETIME\n";
			createTableTest += "\t, SceneDate DATETIME\n";
			createTableTest += ");";
			m_database.executeUpdate( createTableTest );

			progress.Update( index++
				, _( "Creating tests database" )
				+ wxT( "\n" ) + _( "Validating changes" ) );
			transaction.commit();
			progress.SetRange( saveRange );
			index = saveIndex;
		}
		catch ( std::exception & )
		{
			transaction.rollback();
			progress.SetRange( saveRange );
			index = saveIndex;
			throw;
		}
	}

	void TestDatabase::doCreateV2( wxProgressDialog & progress, int & index )
	{
		static int constexpr NonTestsCount = 7;
		auto saveRange = progress.GetRange();
		auto saveIndex = index;
		progress.SetTitle( _( "Updating tests database to V3" ) );
		index = 0;
		auto transaction = m_database.beginTransaction( "DatabaseUpdate" );

		if ( !transaction )
		{
			throw std::runtime_error{ "Couldn't begin a transaction." };
		}

		try
		{
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating tests database" ) );
			progress.SetRange( NonTestsCount );
			std::string query = "CREATE TABLE TestsDatabase( Id INTEGER PRIMARY KEY, Version INTEGER );";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create TestsDatabase table." };
			}

			query = "INSERT INTO TestsDatabase (Version) VALUES (2);";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create version 2 entry." };
			}

			{
				// Renderer table
				progress.Update( index++
					, _( "Updating tests database" )
					+ wxT( "\n" ) + _( "Creating Renderer table" ) );
				query = "CREATE TABLE Renderer";
				query += "( Id INTEGER PRIMARY KEY\n";
				query += "\t, Name VARCHAR(10)\n";
				query += ");";

				if ( !m_database.executeUpdate( query ) )
				{
					throw std::runtime_error{ "Couldn't create Renderer table." };
				}

				m_insertRenderer = InsertRenderer{ m_database };
			}

			{
				// Category table
				progress.Update( index++
					, _( "Updating tests database" )
					+ wxT( "\n" ) + _( "Creating Category table" ) );
				query = "CREATE TABLE Category";
				query += "( Id INTEGER PRIMARY KEY\n";
				query += "\t, Name VARCHAR(50)\n";
				query += ");";

				if ( !m_database.executeUpdate( query ) )
				{
					throw std::runtime_error{ "Couldn't create Category table." };
				}

				m_insertCategory = InsertCategory{ m_database };
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating Test table" ) );

			query = "ALTER TABLE Test\n";
			query += "RENAME TO TestOld;";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't rename Test table." };
			}

			query = "CREATE TABLE Test";
			query += "( Id INTEGER PRIMARY KEY\n";
			query += "\t, CategoryId INTEGER\n";
			query += "\t, Name VARCHAR(1024)\n";
			query += "\t, IgnoreResult INTEGER\n";
			query += ");";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't re-create Test table." };
			}

			query = "CREATE TABLE TestRun";
			query += "( Id INTEGER PRIMARY KEY\n";
			query += "\t, TestId INTEGER\n";
			query += "\t, RendererId INTEGER\n";
			query += "\t, RunDate DATETIME\n";
			query += "\t, Status INTEGER\n";
			query += "\t, CastorDate DATETIME\n";
			query += "\t, SceneDate DATETIME\n";
			query += ");";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create TestRun table." };
			}

			m_insertTest = InsertTest{ m_database };
			m_insertRun = InsertRun{ m_database };
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Listing tests" ) );

			query = "SELECT Category, Name, Renderer, RunDate, Status, CastorDate, SceneDate\n";
			query += "FROM TestOld\n";
			query += "ORDER BY Category, Name, Renderer, RunDate;";
			auto testsList = m_database.executeSelect( query );

			if ( !testsList )
			{
				throw std::runtime_error{ "Couldn't list existing tests." };
			}

			progress.SetRange( NonTestsCount + testsList->size() );
			std::vector< TestPtr > tests;
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( " - " ) + _( "Listing tests" )
				+ wxT( "\n" ) + _( "..." ) );
			std::string prvCatName;
			std::string prvTestName;
			int testId = 0;

			for ( auto & testInstance : *testsList )
			{
				auto catName = testInstance.getField( 0u ).getValue< std::string >();
				auto testName = testInstance.getField( 1u ).getValue< std::string >();
				auto rendName = testInstance.getField( 2u ).getValue< std::string >();
				auto runDate = testInstance.getField( 3u ).getValue< db::DateTime >();
				progress.Update( index++
					, _( "Updating tests database" )
					+ wxT( " - " ) + _( "Listing tests" )
					+ wxT( "\n" ) + getProgressDetails( catName, testName, rendName, runDate ) );

				if ( catName != prvCatName
					|| testName != prvTestName )
				{
					prvCatName = catName;
					prvTestName = testName;
					auto category = getCategory( catName, m_categories, m_insertCategory );
					testId = m_insertTest.insert( category->id, testName );
				}

				auto status = TestStatus( testInstance.getField( 4u ).getValue< int32_t >() );
				auto castorDate = testInstance.getField( 5u ).getValue< db::DateTime >();
				auto sceneDate = testInstance.getField( 6u ).getValue < db::DateTime >();
				auto renderer = getRenderer( rendName, m_renderers, m_insertRenderer );
				m_insertRun.insert( testId, renderer->id, runDate, status, castorDate, sceneDate );
			}

			query = "DROP TABLE TestOld;";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't drop TestOld table." };
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Validating changes" ) );
			transaction.commit();
			progress.SetRange( saveRange );
			index = saveIndex;
		}
		catch ( std::exception & )
		{
			transaction.rollback();
			progress.SetRange( saveRange );
			index = saveIndex;
			throw;
		}
	}

	void TestDatabase::doCreateV3( wxProgressDialog & progress, int & index )
	{
		static int constexpr NonTestsCount = 6;
		auto saveRange = progress.GetRange();
		auto saveIndex = index;
		progress.SetTitle( _( "Updating tests database to V3" ) );
		index = 0;
		auto transaction = m_database.beginTransaction( "DatabaseUpdate" );

		if ( !transaction )
		{
			throw std::runtime_error{ "Couldn't begin a transaction." };
		}

		try
		{
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating Keyword table" ) );
			progress.SetRange( NonTestsCount );
			std::string query = "CREATE TABLE Keyword( Id INTEGER PRIMARY KEY, Name VARCHAR(50) );";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create Keyword table." };
			}

			m_insertKeyword = InsertKeyword{ m_database };
			std::set< std::string, LessNoCase > keywords
			{
				"Light",
				"Directional",
				"Point",
				"Spot",
				"Shadow",
				"PCF",
				"VSM",
				"Reflection",
				"Refraction",
				"Phong",
				"PBR",
				"GlobalIllumination",
				"LPV",
				"Texturing",
				"Albedo",
				"Diffuse",
				"Roughness",
				"Glossiness",
				"Shininess",
				"Metallic",
				"Specular",
				"Emissive",
				"Opacity",
				"Normal",
				"Occlusion",
				"Height",
			};

			for ( auto & keyword : keywords )
			{
				getKeyword( keyword, m_keywords, m_insertKeyword )->id;
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating CategoryKeyword table" ) );
			query = "CREATE TABLE CategoryKeyword( CategoryId INTEGER, KeywordId INTEGER );";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create CategoryKeyword table." };
			}

			m_insertCategoryKeyword = InsertCategoryKeyword{ m_database };
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating TestKeyword table" ) );
			query = "CREATE TABLE TestKeyword( TestId INTEGER, KeywordId INTEGER );";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create TestKeyword table." };
			}

			m_insertTestKeyword = InsertTestKeyword{ m_database };
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Listing test names" ) );
			query = "SELECT Id, Name FROM Test ORDER BY Name;";
			auto testNames = m_database.executeSelect( query );

			if ( !testNames )
			{
				throw std::runtime_error{ "Couldn't list test names." };
			}

			progress.SetRange( int( progress.GetRange() + testNames->size() ) );
			auto findSubstr = []( const std::string & str1
				, const std::string & str2 )
			{
				auto it = std::search( str1.begin()
					, str1.end()
					, str2.begin()
					, str2.end()
					, EqualNoCase{} );
				int result = -1;

				if ( it != str1.end() )
				{
					result = it - str1.begin();
				}

				return result;
			};

			for ( auto & test : *testNames )
			{
				auto id = test.getField( 0 ).getValue< int32_t >();
				auto name = test.getField( 1 ).getValue< std::string >();
				progress.Update( index++
					, _( "Updating tests database" )
					+ wxT( "\n" ) + _( "Listing test names" )
					+ wxT( "\n" ) + _( "- Test:" ) + name );

				for ( auto & keyword : m_keywords )
				{
					if ( findSubstr( name, keyword.first ) != -1 )
					{
						m_insertTestKeyword.insert( id, keyword.second->id );
					}
				}
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Updating database version number" ) );
			query = "UPDATE TestsDatabase SET Version=3;";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't update database version number to 3." };
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Validating changes" ) );
			transaction.commit();
			progress.SetRange( saveRange );
			index = saveIndex;
		}
		catch ( std::exception & )
		{
			transaction.rollback();
			progress.SetRange( saveRange );
			index = saveIndex;
			throw;
		}
	}

	void TestDatabase::doUpdateCategories()
	{
		for ( auto & category : m_categories )
		{
			category.second->id = m_insertCategory.insert( category.first );
		}
	}

	void TestDatabase::doUpdateRenderers()
	{
		for ( auto & renderer : m_renderers )
		{
			renderer.second->id = m_insertRenderer.insert( renderer.first );
		}
	}

	void TestDatabase::doListCategories( wxProgressDialog & progress
		, int & index )
	{
		TestMap result;
		auto categoryPaths = listTestCategories( m_config.test );
		doUpdateCategories();
		castor::Logger::logInfo( "Listing Test files" );
		progress.SetTitle( _( "Listing Test files" ) );
		progress.SetRange( progress.GetRange() + int( categoryPaths.size() ) );
		progress.Update( index, _( "Listing Test files\n..." ) );

		for ( auto & categoryPath : categoryPaths )
		{
			auto categoryName = categoryPath.getFileName();
			progress.Update( index++
				, _( "Listing Test files" )
				+ wxT( "\n" ) + wxT( "- Category: " ) + makeWxString( categoryName ) + wxT( "..." ) );
			auto category = getCategory( categoryName, m_categories, m_insertCategory );
			auto iresult = result.emplace( category
				, listCategoryTestFiles( m_config
					, m_insertRenderer
					, m_insertTest
					, m_insertRun
					, categoryPath
					, category
					, m_renderers ) );
		}
	}

	void TestDatabase::doFillDatabase( wxProgressDialog & progress
		, int & index )
	{
		doListCategories( progress, index );
	}

	//*********************************************************************************************
}
