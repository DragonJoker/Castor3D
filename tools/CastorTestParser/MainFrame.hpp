/*
See LICENSE file in root folder
*/
#ifndef ___CTP_MainFrame_HPP___
#define ___CTP_MainFrame_HPP___

#include "Prerequisites.hpp"
#include "Database/DbConnection.hpp"
#include "Database/DbStatement.hpp"

#include <wx/frame.h>
#include <wx/dataview.h>
#include <wx/process.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>

#include <list>
#include <map>

class wxProgressDialog;
class wxStaticText;

namespace test_parser
{
	class MainFrame
		: public wxFrame
	{
	public:
		explicit MainFrame( Config config );
		~MainFrame();

		void initialise();

		void insertTest( Test const & test
			, bool moveFiles = true );
		void updateTestStatus( Test & test
			, TestStatus newStatus
			, bool reference );
		TreeModelNode * getTestNode( Test const & test );

	private:
		wxWindow * doInitTestsList();
		wxWindow * doInitDetailsView();
		void doInitGui();
		void doInitMenus();
		void doInitDatabase( wxProgressDialog & progress, int & index );
		TestMap doListCategories( wxProgressDialog & progress, int & index, uint32_t & testCount );
		void doPopulateDatabase( wxProgressDialog & progress, int & index, TestMap const & tests, uint32_t testCount );
		void doFillDatabase( wxProgressDialog & progress, int & index );
		uint32_t doListTests( wxProgressDialog & progress, int & index );
		void doFillList( wxProgressDialog & progress, int & index, uint32_t testCount );

		void doProcessTest();
		void doRunTest();
		void doViewTest();
		void doSetRef();
		void doRunAllCategoryTests();
		void doRunCategoryTests( TestStatus filter );
		void doRunAllCategoryTestsBut( TestStatus filter );
		void doRunAllTests();
		void doRunTests( TestStatus filter );
		void doRunAllTestsBut( TestStatus filter );
		void doCancelTest( Test & test
			, TestStatus status );
		void doCancel();

		void onSelectionChange( wxDataViewEvent & evt );
		void onItemContextMenu( wxDataViewEvent & evt );
		void onMenuOption( wxCommandEvent & evt );
		void onTestRunEnd( wxProcessEvent & evt );
		void onTestDisplayEnd( wxProcessEvent & evt );
		void onTestDiffEnd( wxProcessEvent & evt );

	private:
		struct InsertTest
		{
			InsertTest() = default;
			InsertTest( db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO Test(Name, RunDate, Status, Renderer, Category) VALUES (?, ?, ?, ?, ?);" ) }
				, name{ stmt->createParameter( "Name", db::FieldType::eVarchar, 1024 ) }
				, runDate{ stmt->createParameter( "RunDate", db::FieldType::eDatetime ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, renderer{ stmt->createParameter( "Renderer", db::FieldType::eVarchar, 10 ) }
				, category{ stmt->createParameter( "Category", db::FieldType::eVarchar, 50 ) }
			{
				stmt->initialise();
			}

			db::StatementPtr stmt;
			db::Parameter * name{};
			db::Parameter * runDate{};
			db::Parameter * status{};
			db::Parameter * renderer{};
			db::Parameter * category{};
		};
		
		struct UpdateTestStatus
		{
			UpdateTestStatus() = default;
			UpdateTestStatus( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE Test SET Status=? WHERE Id=?;" ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				stmt->initialise();
			}

			db::StatementPtr stmt;
			db::Parameter * status{};
			db::Parameter * id{};
		};

	private:
		wxAuiManager m_auiManager;
		Config m_config;
		db::Connection m_database;
		InsertTest m_insertTest;
		UpdateTestStatus m_updateTestStatus;
		TestMap m_tests;
		wxDataViewCtrl * m_view{};
		wxObjectDataPtr< TreeModel > m_model;
		std::map< uint32_t, TreeModelNode * > m_modelNodes;
		LayeredPanel * m_detailViews{};
		TestPanel * m_testView{};
		CategoryPanel * m_categoryView{};
		wxStaticText * m_statusBar{};
		bool m_hasPage{ false };
		std::unique_ptr< wxMenu > m_testMenu{};
		std::unique_ptr< wxMenu > m_categoryMenu{};
		std::unique_ptr< wxMenu > m_allMenu{};
		std::unique_ptr< wxMenu > m_busyMenu{};
		struct Selection
		{
			wxDataViewItemArray items;
			bool allTests{};
			bool allCategories{};
		};
		Selection m_selected;
		struct TestNode
		{
			Test * test;
			TestStatus status;
			TreeModelNode * node;
		};
		struct RunningTest
		{
			std::unique_ptr< wxProcess > genProcess{};
			std::unique_ptr< wxProcess > difProcess{};
			std::unique_ptr< wxProcess > disProcess{};
			std::list< TestNode > tests{};
		};
		RunningTest m_runningTest;
		std::atomic_bool m_cancelled;
	};
}

#endif
