/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestDatabase_HPP___
#define ___CTP_TestDatabase_HPP___

#include "Prerequisites.hpp"
#include "Database/DbConnection.hpp"
#include "Database/DbStatement.hpp"

#include <list>
#include <map>

class wxProgressDialog;

namespace aria
{
	void moveFile( castor::Path const & srcFolder
		, castor::Path const & dstFolder
		, castor::Path const & srcName
		, castor::Path const & dstName );
	void moveFile( castor::Path const & srcFolder
		, castor::Path const & dstFolder
		, castor::Path const & name );

	class TestDatabase
	{
		friend class DatabaseTest;

	public:
		explicit TestDatabase( Config config );
		~TestDatabase();

		void initialise( wxProgressDialog & progress
			, int & index );

		RendererMap const & getRenderers()const
		{
			return m_renderers;
		}

		CategoryMap const & getCategories()const
		{
			return m_categories;
		}

		Renderer createRenderer( std::string const & name );
		Category createCategory( std::string const & name );
		Keyword createKeyword( std::string const & name );

		TestMap listTests();
		TestMap listTests( wxProgressDialog & progress
			, int & index );
		void listTests( TestMap & result );
		void listTests( TestMap & result
			, wxProgressDialog & progress
			, int & index );

		TestRunMap listLatestRuns( TestMap const & tests );
		TestRunMap listLatestRuns( TestMap const & tests
			, wxProgressDialog & progress
			, int & index );
		void listLatestRuns( TestMap const & tests
			, TestRunMap & result );
		void listLatestRuns( TestMap const & tests
			, TestRunMap & result
			, wxProgressDialog & progress
			, int & index );
		TestRunCategoryMap listLatestRuns( Renderer renderer
			, TestMap const & tests );
		TestRunCategoryMap listLatestRuns( Renderer renderer
			, TestMap const & tests
			, wxProgressDialog & progress
			, int & index );
		void listLatestRuns( Renderer renderer
			, TestMap const & tests
			, TestRunCategoryMap & result );
		void listLatestRuns( Renderer renderer
			, TestMap const & tests
			, TestRunCategoryMap & result
			, wxProgressDialog & progress
			, int & index );

		void insertTest( Test & test
			, bool moveFiles = true );
		void updateRunsCastorDate( db::DateTime const & date );

	public:
		struct InsertIdValue
		{
		protected:
			InsertIdValue( InsertIdValue const & ) = default;
			InsertIdValue & operator=( InsertIdValue const & ) = default;
			InsertIdValue( InsertIdValue && ) = default;
			InsertIdValue & operator=( InsertIdValue && ) = default;
			InsertIdValue() = default;
			explicit InsertIdValue( std::string const & tableName
				, uint32_t nameSize
				, db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO " + tableName + " (Name) VALUES (?);" ) }
				, select{ connection.createStatement( "SELECT Id FROM " + tableName + " WHERE Name=?;" ) }
				, name{ stmt->createParameter( "Name", db::FieldType::eVarchar, nameSize ) }
				, sName{ select->createParameter( "Name", db::FieldType::eVarchar, nameSize ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create Insert" + tableName + " INSERT statement." };
				}

				if ( !select->initialise() )
				{
					throw std::runtime_error{ "Couldn't create Insert" + tableName + " SELECT statement." };
				}
			}

		public:
			int32_t insert( std::string const & name );

		private:
			db::StatementPtr stmt;
			db::StatementPtr select;
			db::Parameter * name{};
			db::Parameter * sName{};
		};

		struct InsertIdId
		{
		protected:
			InsertIdId( InsertIdId const & ) = default;
			InsertIdId & operator=( InsertIdId const & ) = default;
			InsertIdId( InsertIdId && ) = default;
			InsertIdId & operator=( InsertIdId && ) = default;
			InsertIdId() = default;
			explicit InsertIdId( std::string const & tableName
				, std::string const & lhsIdName
				, std::string const & rhsIdName
				, db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO " + tableName + " (" + lhsIdName + ", " + rhsIdName + ") VALUES (?, ?);" ) }
				, lhsId{ stmt->createParameter( lhsIdName, db::FieldType::eSint32 ) }
				, rhsId{ stmt->createParameter( rhsIdName, db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create Insert" + tableName + " INSERT statement." };
				}
			}

		public:
			void insert( int32_t lhs, int32_t rhs );

		private:
			db::StatementPtr stmt;
			db::Parameter * lhsId{};
			db::Parameter * rhsId{};
		};

		struct InsertRenderer
			: InsertIdValue
		{
			InsertRenderer( InsertRenderer const & ) = default;
			InsertRenderer & operator=( InsertRenderer const & ) = default;
			InsertRenderer( InsertRenderer && ) = default;
			InsertRenderer & operator=( InsertRenderer && ) = default;
			InsertRenderer() = default;
			explicit InsertRenderer( db::Connection & connection )
				: InsertIdValue{ "Renderer", 10u, connection }
			{
			}
		};

		struct InsertCategory
			: InsertIdValue
		{
			InsertCategory( InsertCategory const & ) = default;
			InsertCategory & operator=( InsertCategory const & ) = default;
			InsertCategory( InsertCategory && ) = default;
			InsertCategory & operator=( InsertCategory && ) = default;
			InsertCategory() = default;
			explicit InsertCategory( db::Connection & connection )
				: InsertIdValue{ "Category", 50u, connection }
			{
			}
		};

		struct InsertKeyword
			: InsertIdValue
		{
			InsertKeyword( InsertKeyword const & ) = default;
			InsertKeyword & operator=( InsertKeyword const & ) = default;
			InsertKeyword( InsertKeyword && ) = default;
			InsertKeyword & operator=( InsertKeyword && ) = default;
			InsertKeyword() = default;
			explicit InsertKeyword( db::Connection & connection )
				: InsertIdValue{ "Keyword", 50u, connection }
			{
			}
		};

		struct InsertCategoryKeyword
			: InsertIdId
		{
			InsertCategoryKeyword( InsertCategoryKeyword const & ) = default;
			InsertCategoryKeyword & operator=( InsertCategoryKeyword const & ) = default;
			InsertCategoryKeyword( InsertCategoryKeyword && ) = default;
			InsertCategoryKeyword & operator=( InsertCategoryKeyword && ) = default;
			InsertCategoryKeyword() = default;
			explicit InsertCategoryKeyword( db::Connection & connection )
				: InsertIdId{ "CategoryKeyword", "CategoryId", "KeywordId", connection }
			{
			}
		};

		struct InsertTestKeyword
			: InsertIdId
		{
			InsertTestKeyword( InsertTestKeyword const & ) = default;
			InsertTestKeyword & operator=( InsertTestKeyword const & ) = default;
			InsertTestKeyword( InsertTestKeyword && ) = default;
			InsertTestKeyword & operator=( InsertTestKeyword && ) = default;
			InsertTestKeyword() = default;
			explicit InsertTestKeyword( db::Connection & connection )
				: InsertIdId{ "TestKeyword", "TestId", "KeywordId", connection }
			{
			}
		};

		struct InsertTest
		{
			InsertTest( InsertTest const & ) = default;
			InsertTest & operator=( InsertTest const & ) = default;
			InsertTest( InsertTest && ) = default;
			InsertTest & operator=( InsertTest && ) = default;
			InsertTest() = default;
			explicit InsertTest( db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO Test (CategoryId, Name, IgnoreResult) VALUES (?, ?, ?);" ) }
				, select{ connection.createStatement( "SELECT Id FROM Test WHERE CategoryId=? AND Name=?;" ) }
				, categoryId{ stmt->createParameter( "CategoryId", db::FieldType::eSint32 ) }
				, sCategoryId{ select->createParameter( "CategoryId", db::FieldType::eSint32 ) }
				, name{ stmt->createParameter( "Name", db::FieldType::eVarchar, 1024 ) }
				, sName{ select->createParameter( "Name", db::FieldType::eVarchar, 1024 ) }
				, ignoreResult{ stmt->createParameter( "IgnoreResult", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create InsertTest INSERT statement." };
				}

				if ( !select->initialise() )
				{
					throw std::runtime_error{ "Couldn't create InsertTest SELECT statement." };
				}
			}

			int32_t insert( int32_t categoryId
				, std::string const & name
				, bool ignoreResult = false );

		private:
			db::StatementPtr stmt;
			db::StatementPtr select;
			db::Parameter * categoryId{};
			db::Parameter * sCategoryId{};
			db::Parameter * name{};
			db::Parameter * sName{};
			db::Parameter * ignoreResult{};
		};

		struct InsertRun
		{
			InsertRun( InsertRun const & ) = default;
			InsertRun & operator=( InsertRun const & ) = default;
			InsertRun( InsertRun && ) = default;
			InsertRun & operator=( InsertRun && ) = default;
			InsertRun() = default;
			explicit InsertRun( db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO TestRun (TestId, RendererId, RunDate, Status, CastorDate, SceneDate) VALUES (?, ?, ?, ?, ?, ?);" ) }
				, select{ connection.createStatement( "SELECT Id FROM TestRun WHERE TestId=? AND RendererId=? AND RunDate=? AND Status=? AND CastorDate=? AND SceneDate=?;" ) }
				, testId{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
				, sTestId{ select->createParameter( "TestId", db::FieldType::eSint32 ) }
				, rendererId{ stmt->createParameter( "RendererId", db::FieldType::eSint32 ) }
				, sRendererId{ select->createParameter( "RendererId", db::FieldType::eSint32 ) }
				, runDate{ stmt->createParameter( "RunDate", db::FieldType::eDatetime ) }
				, sRunDate{ select->createParameter( "RunDate", db::FieldType::eDatetime ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, sStatus{ select->createParameter( "Status", db::FieldType::eSint32 ) }
				, castorDate{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, sCastorDate{ select->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, sceneDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, sSceneDate{ select->createParameter( "SceneDate", db::FieldType::eDatetime ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create InsertRun INSERT statement." };
				}

				if ( !select->initialise() )
				{
					throw std::runtime_error{ "Couldn't create InsertRun SELECT statement." };
				}
			}

			int32_t insert( int32_t id
				, int32_t inRendererId
				, db::DateTime dateRun
				, TestStatus status
				, db::DateTime const & dateCastor
				, db::DateTime const & dateScene );

		private:
			db::StatementPtr stmt;
			db::StatementPtr select;
			db::Parameter * testId{};
			db::Parameter * sTestId{};
			db::Parameter * rendererId{};
			db::Parameter * sRendererId{};
			db::Parameter * runDate{};
			db::Parameter * sRunDate{};
			db::Parameter * status{};
			db::Parameter * sStatus{};
			db::Parameter * castorDate{};
			db::Parameter * sCastorDate{};
			db::Parameter * sceneDate{};
			db::Parameter * sSceneDate{};
		};

		struct UpdateTestIgnoreResult
		{
			UpdateTestIgnoreResult() = default;
			explicit UpdateTestIgnoreResult( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE Test SET IgnoreResult=? WHERE Id=?;" ) }
				, ignore{ stmt->createParameter( "IgnoreResult", db::FieldType::eSint32 ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateTestIgnoreResult UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * ignore{};
			db::Parameter * id{};
		};

		struct UpdateRunStatus
		{
			UpdateRunStatus() = default;
			explicit UpdateRunStatus( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET Status=?, CastorDate=?, SceneDate=? WHERE Id=?;" ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, castorDate{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, sceneDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunStatus UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * status{};
			db::Parameter * castorDate{};
			db::Parameter * sceneDate{};
			db::Parameter * id{};
		};

		struct UpdateRunDates
		{
			UpdateRunDates() = default;
			explicit UpdateRunDates( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET CastorDate=?, SceneDate=? WHERE Id=?;" ) }
				, castorDate{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, sceneDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunDates UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * castorDate{};
			db::Parameter * sceneDate{};
			db::Parameter * id{};
		};

		struct UpdateRunCastorDate
		{
			UpdateRunCastorDate() = default;
			explicit UpdateRunCastorDate( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET CastorDate=? WHERE Id=?;" ) }
				, castorDate{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunCastorDate UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * castorDate{};
			db::Parameter * id{};
		};

		struct UpdateRunSceneDate
		{
			UpdateRunSceneDate() = default;
			explicit UpdateRunSceneDate( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET SceneDate=? WHERE Id=?;" ) }
				, sceneDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunSceneDate UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * sceneDate{};
			db::Parameter * id{};
		};

		struct CheckTableExists
		{
			CheckTableExists( CheckTableExists const & ) = default;
			CheckTableExists & operator=( CheckTableExists const & ) = default;
			CheckTableExists( CheckTableExists && ) = default;
			CheckTableExists & operator=( CheckTableExists && ) = default;
			CheckTableExists() = default;
			explicit CheckTableExists( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT name FROM sqlite_master WHERE type = 'table' AND name=?;" ) }
				, tableName{ stmt->createParameter( "TableName", db::FieldType::eVarchar, 255 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create CheckTableExists SELECT statement." };
				}
			}

			bool checkTable( std::string const & name );

		private:
			db::StatementPtr stmt;
			db::Parameter * tableName{};
		};

		struct ListTests
		{
			ListTests( ListTests const & ) = default;
			ListTests & operator=( ListTests const & ) = default;
			ListTests( ListTests && ) = default;
			ListTests & operator=( ListTests && ) = default;
			ListTests() = default;
			explicit ListTests( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT Id, CategoryId, Name, IgnoreResult FROM Test ORDER BY CategoryId, Name" ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListTests SELECT statement." };
				}
			}

			TestMap listTests( CategoryMap & categories
				, wxProgressDialog & progress
				, int & index );
			void listTests( CategoryMap & categories
				, TestMap & result
				, wxProgressDialog & progress
				, int & index );

		private:
			db::StatementPtr stmt;
			db::Parameter * testId{};
		};

		struct ListLatestTestRun
		{
			ListLatestTestRun( ListLatestTestRun const & ) = default;
			ListLatestTestRun & operator=( ListLatestTestRun const & ) = default;
			ListLatestTestRun( ListLatestTestRun && ) = default;
			ListLatestTestRun & operator=( ListLatestTestRun && ) = default;
			ListLatestTestRun() = default;
			explicit ListLatestTestRun( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT Id, RendererId, MAX(RunDate) AS RunDate, Status, CastorDate, SceneDate FROM TestRun WHERE TestId=?;" ) }
				, testId{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListLatestTestRun SELECT statement." };
				}
			}

			db::ResultPtr listTests( int32_t id );

		private:
			db::StatementPtr stmt;
			db::Parameter * testId{};
		};

		struct ListLatestRendererTests
		{
			ListLatestRendererTests( ListLatestRendererTests const & ) = default;
			ListLatestRendererTests & operator=( ListLatestRendererTests const & ) = default;
			ListLatestRendererTests( ListLatestRendererTests && ) = default;
			ListLatestRendererTests & operator=( ListLatestRendererTests && ) = default;
			ListLatestRendererTests() = default;
			explicit ListLatestRendererTests( TestDatabase * database )
				: database{ database }
				, stmt{ database->m_database.createStatement( "SELECT CategoryId, TestId, TestRun.Id, MAX(RunDate) AS RunDate, Status, CastorDate, SceneDate FROM Test, TestRun WHERE Test.Id=TestRun.TestId AND RendererId=? GROUP BY CategoryId, TestId ORDER BY CategoryId, TestId; " ) }
				, rendererId{ stmt->createParameter( "RendererId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListLatestRendererTests SELECT statement." };
				}
			}

			TestRunCategoryMap listTests( TestMap const & tests
				, CategoryMap & categories
				, Renderer renderer
				, wxProgressDialog & progress
				, int & index );
			void listTests( TestMap const & tests
				, CategoryMap & categories
				, Renderer renderer
				, TestRunCategoryMap & result
				, wxProgressDialog & progress
				, int & index );

		private:
			TestDatabase * database;
			db::StatementPtr stmt;
			db::Parameter * rendererId{};
		};

		struct UpdateRunsCastorDate
		{
			UpdateRunsCastorDate() = default;
			explicit UpdateRunsCastorDate( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET CastorDate=? WHERE Id IN (SELECT MAX(Id) FROM TestRun GROUP BY TestId, RendererId);" ) }
				, castorDate{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunsCastorDate UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * castorDate;
		};

	private:
		void insertRun( TestRun & run
			, bool moveFiles = true );
		void updateTestIgnoreResult( Test const & test
			, bool ignore );
		void updateRunStatus( TestRun const & run );
		void updateRunCastorDate( TestRun const & run );
		void updateRunSceneDate( TestRun const & run );
		void doCreateV1( wxProgressDialog & progress, int & index );
		void doCreateV2( wxProgressDialog & progress, int & index );
		void doCreateV3( wxProgressDialog & progress, int & index );
		void doUpdateCategories();
		void doUpdateRenderers();
		void doListCategories( wxProgressDialog & progress, int & index );
		void doFillDatabase( wxProgressDialog & progress, int & index );

	private:
		Config m_config;
		db::Connection m_database;
		InsertRenderer m_insertRenderer;
		InsertCategory m_insertCategory;
		InsertKeyword m_insertKeyword;
		InsertCategoryKeyword m_insertCategoryKeyword;
		InsertTestKeyword m_insertTestKeyword;
		RendererMap m_renderers;
		CategoryMap m_categories;
		KeywordMap m_keywords;
		InsertTest m_insertTest;
		InsertRun m_insertRun;
		CheckTableExists m_checkTableExists;
		UpdateRunStatus m_updateRunStatus;
		UpdateTestIgnoreResult m_updateTestIgnoreResult;
		UpdateRunDates m_updateRunDates;
		UpdateRunCastorDate m_updateRunCastorDate;
		UpdateRunSceneDate m_updateRunSceneDate;
		ListTests m_listTests;
		ListLatestTestRun m_listLatestRun;
		ListLatestRendererTests m_listLatestRendererRuns;
		UpdateRunsCastorDate m_updateRunsCastorDate;
	};
}

#endif
