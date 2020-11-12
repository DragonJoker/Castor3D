#include "CastorTestParser/TestDatabase.hpp"

#include "CastorTestParser/Database/DbDateTimeHelpers.hpp"
#include "CastorTestParser/Database/DbResult.hpp"
#include "CastorTestParser/Database/DbStatement.hpp"

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Miscellaneous/Utils.hpp>

#include <wx/progdlg.h>

namespace test_parser
{
	//*********************************************************************************************

	namespace
	{
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
							, category
							, false } );
				}
			}
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
	}

	//*********************************************************************************************

	TestDatabase::TestDatabase( Config config )
		: m_config{ std::move( config ) }
		, m_database{ m_config.work }
	{
	}

	TestDatabase::~TestDatabase()
	{
	}

	void TestDatabase::initialise( wxProgressDialog & progress
		, int & index )
	{
#ifdef NDEBUG
		if ( !m_config.skip )
		{
			doInitDatabase( progress, index );
			m_insertTest = InsertTest{ m_database };
			m_updateTestStatus = UpdateTestStatus{ m_database };
			doFillDatabase( progress, index );
		}
		else
#endif
		{
			m_insertTest = InsertTest{ m_database };
			m_updateTestStatus = UpdateTestStatus{ m_database };
		}

		m_updateTestIgnoreResult = UpdateTestIgnoreResult{ m_database };
		std::string listTests = "SELECT Id, Name, MAX( RunDate ) AS RunDate, Status, Renderer, Category, IgnoreResult\n";
		listTests += "FROM Test\n";
		listTests += "GROUP BY Category, Name\n";
		listTests += "ORDER BY Category, Name;";
		m_listTests = m_database.createStatement( listTests );
		m_listTests->initialise();
	}

	TestMap TestDatabase::listLatestTests( uint32_t & testCount
		, wxProgressDialog & progress
		, int & index )
	{
		testCount = 0;
		TestMap tests;
		auto result = m_listTests->executeSelect();
		std::string prevCategory;
		auto categoryIt = tests.end();
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
				, row.getField( 5u ).getValue< std::string >()
				, row.getField( 6u ).getValue< int32_t >() != 0 };

			if ( prevCategory != test.category )
			{
				categoryIt = tests.emplace( test.category, TestArray{} ).first;
				prevCategory = test.category;
			}

			categoryIt->second.push_back( test );
			++testCount;
			progress.Update( index++, makeWxString( test.category ) + wxT( " - " ) + makeWxString( test.name ) + wxT( "..." ) );
		}

		return tests;
	}

	TestMap TestDatabase::listLatestTests( uint32_t & testCount )
	{
		wxProgressDialog progress{ wxT( "Listing tests" )
			, wxT( "Listing tests..." )
			, int( 1 )
			, nullptr };
		int index = 0;
		return listLatestTests( testCount, progress, index );
	}

	void TestDatabase::insertTest( Test const & test
		, bool moveFiles )
	{
		m_insertTest.name->setValue( test.name );
		m_insertTest.runDate->setValue( test.runDate );
		m_insertTest.status->setValue( int32_t( test.status ) );
		m_insertTest.renderer->setValue( test.renderer );
		m_insertTest.category->setValue( test.category );
		m_insertTest.ignoreResult->setValue( test.ignoreResult ? 1 : 0 );
		m_insertTest.stmt->executeUpdate();

		if ( moveFiles )
		{
			moveTestFile( test, m_config.test, m_config.work );
		}

		castor::Logger::logInfo( "Inserted: " + getDetails( test ) );
	}

	void TestDatabase::updateTestStatus( Test const & test
		, TestStatus newStatus
		, bool useAsReference )
	{
		auto oldStatus = test.status;
		m_updateTestStatus.status->setValue( int32_t( newStatus ) );
		m_updateTestStatus.id->setValue( int32_t( test.id ) );
		m_updateTestStatus.stmt->executeUpdate();
		moveResultFile( test, oldStatus, newStatus, m_config.work );

		if ( useAsReference )
		{
			castor::File::copyFileName( m_config.work / getResultFolder( test, false ) / getFolderName( newStatus ) / getResultName( test )
				, m_config.test / getReferenceFolder( test ) / getReferenceName( test ) );
		}

		castor::Logger::logInfo( "Updated status for: " + getDetails( test ) );
	}

	void TestDatabase::updateTestIgnoreResult( Test & test
		, bool ignore
		, bool useAsReference )
	{
		auto oldStatus = test.status;
		m_updateTestIgnoreResult.status->setValue( int32_t( ignore ? 1 : 0 ) );
		m_updateTestIgnoreResult.id->setValue( int32_t( test.id ) );
		m_updateTestIgnoreResult.stmt->executeUpdate();

		if ( ignore )
		{
			auto newStatus = TestStatus::eNegligible;
			moveResultFile( test, oldStatus, newStatus, m_config.work );

			if ( useAsReference )
			{
				castor::File::copyFileName( m_config.work / getResultFolder( test, false ) / getFolderName( newStatus ) / getResultName( test )
					, m_config.test / getReferenceFolder( test ) / getReferenceName( test ) );
			}
		}

		castor::Logger::logInfo( "Updated ignore result for: " + getDetails( test ) );
	}

	void TestDatabase::doInitDatabase( wxProgressDialog & progress
		, int & index )
	{
		std::string createTableTest = "CREATE TABLE IF NOT EXISTS Test";
		createTableTest += "( Id INTEGER PRIMARY KEY\n";
		createTableTest += "\t, Name VARCHAR(1024)\n";
		createTableTest += "\t, RunDate DATETIME\n";
		createTableTest += "\t, Status INTEGER\n";
		createTableTest += "\t, Renderer VARCHAR(10)\n";
		createTableTest += "\t, Category VARCHAR(50)\n";
		createTableTest += "\t, IgnoreResult INTEGER\n";
		createTableTest += ");";
		m_database.executeUpdate( createTableTest );
	}

	TestMap TestDatabase::doListCategories( wxProgressDialog & progress
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

	void TestDatabase::doPopulateDatabase( wxProgressDialog & progress
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

	void TestDatabase::doFillDatabase( wxProgressDialog & progress
		, int & index )
	{
		uint32_t testCount = 0u;
		auto tests = doListCategories( progress, index, testCount );
		doPopulateDatabase( progress, index, tests, testCount );
	}

	//*********************************************************************************************
}
