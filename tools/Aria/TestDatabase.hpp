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
		, castor::Path const & dstName
		, bool force );
	void moveFile( castor::Path const & srcFolder
		, castor::Path const & dstFolder
		, castor::Path const & name
		, bool force );

	class TestDatabase
	{
	public:
		explicit TestDatabase( Config config );
		~TestDatabase();

		void initialise( wxProgressDialog & progress
			, int & index );

		TestMap listLatestTests( uint32_t & testCount );
		TestMap listLatestTests( uint32_t & testCount
			, wxProgressDialog & progress
			, int & index );

		void insertTest( Test const & test
			, bool moveFiles = true );
		void updateTestStatus( Test const & test
			, TestStatus newStatus
			, bool useAsReference );
		void updateTestIgnoreResult( Test & test
			, bool ignore
			, bool useAsReference );
		void updateTestCastorDate( Test & test );
		void updateTestSceneDate( Test & test );

	private:
		void doInitDatabase( wxProgressDialog & progress, int & index );
		TestMap doListCategories( wxProgressDialog & progress, int & index, uint32_t & testCount );
		void doPopulateDatabase( wxProgressDialog & progress, int & index, TestMap const & tests, uint32_t testCount );
		void doFillPerRenderer( wxProgressDialog & progress, int & index, TestMap const & tests, uint32_t testCount );
		void doFillDatabase( wxProgressDialog & progress, int & index );

	private:
		struct InsertTest
		{
			InsertTest() = default;
			explicit InsertTest( db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO Test(Name, RunDate, Status, Renderer, Category, IgnoreResult, CastorDate, SceneDate) VALUES (?, ?, ?, ?, ?, ?, ?, ?);" ) }
				, name{ stmt->createParameter( "Name", db::FieldType::eVarchar, 1024 ) }
				, runDate{ stmt->createParameter( "RunDate", db::FieldType::eDatetime ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, renderer{ stmt->createParameter( "Renderer", db::FieldType::eVarchar, 10 ) }
				, category{ stmt->createParameter( "Category", db::FieldType::eVarchar, 50 ) }
				, ignoreResult{ stmt->createParameter( "IgnoreResult", db::FieldType::eSint32 ) }
				, castorDate{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, sceneDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
			{
				stmt->initialise();
			}

			db::StatementPtr stmt;
			db::Parameter * name{};
			db::Parameter * runDate{};
			db::Parameter * status{};
			db::Parameter * renderer{};
			db::Parameter * category{};
			db::Parameter * ignoreResult{};
			db::Parameter * castorDate{};
			db::Parameter * sceneDate{};
		};
		
		struct UpdateTestStatus
		{
			UpdateTestStatus() = default;
			explicit UpdateTestStatus( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE Test SET Status=?, CastorDate=?, SceneDate=? WHERE Id=?;" ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, castorDate{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, sceneDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				stmt->initialise();
			}

			db::StatementPtr stmt;
			db::Parameter * status{};
			db::Parameter * castorDate{};
			db::Parameter * sceneDate{};
			db::Parameter * id{};
		};

		struct UpdateTestIgnoreResult
		{
			UpdateTestIgnoreResult() = default;
			explicit UpdateTestIgnoreResult( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE Test SET IgnoreResult=?, CastorDate=?, SceneDate=? WHERE Id=?;" ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, castorDate{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, sceneDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				stmt->initialise();
			}

			db::StatementPtr stmt;
			db::Parameter * status{};
			db::Parameter * castorDate{};
			db::Parameter * sceneDate{};
			db::Parameter * id{};
		};

		struct UpdateTestCastorDate
		{
			UpdateTestCastorDate() = default;
			explicit UpdateTestCastorDate( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE Test SET CastorDate=? WHERE Id=?;" ) }
				, castorDate{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				stmt->initialise();
			}

			db::StatementPtr stmt;
			db::Parameter * castorDate{};
			db::Parameter * id{};
		};

		struct UpdateTestSceneDate
		{
			UpdateTestSceneDate() = default;
			explicit UpdateTestSceneDate( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE Test SET SceneDate=? WHERE Id=?;" ) }
				, sceneDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				stmt->initialise();
			}

			db::StatementPtr stmt;
			db::Parameter * sceneDate{};
			db::Parameter * id{};
		};

	private:
		Config m_config;
		db::Connection m_database;
		InsertTest m_insertTest;
		UpdateTestStatus m_updateTestStatus;
		UpdateTestIgnoreResult m_updateTestIgnoreResult;
		UpdateTestCastorDate m_updateTestCastorDate;
		UpdateTestSceneDate m_updateTestSceneDate;
		db::StatementPtr m_listTests;
	};
}

#endif
