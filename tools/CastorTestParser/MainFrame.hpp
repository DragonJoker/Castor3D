/*
See LICENSE file in root folder
*/
#ifndef ___CTP_MainFrame_HPP___
#define ___CTP_MainFrame_HPP___

#include "Prerequisites.hpp"
#include "TestDatabase.hpp"
#include "Database/DbConnection.hpp"
#include "Database/DbStatement.hpp"

#include <wx/frame.h>
#include <wx/dataview.h>
#include <wx/menu.h>
#include <wx/process.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>

#include <list>
#include <map>

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

		void updateTestStatus( Test & test
			, TestStatus newStatus
			, bool reference );
		TreeModelNode * getTestNode( Test const & test );

	private:
		wxWindow * doInitTestsList();
		wxWindow * doInitDetailsView();
		void doInitGui();
		void doInitMenus();
		void doFillList( wxProgressDialog & progress, int & index, uint32_t testCount );

		void doProcessTest();
		void doRunTest();
		void doViewTest();
		void doSetRef();
		void doIgnoreTestResult();
		void doUpdateCastorDate();
		void doRunAllCategoryTests();
		void doRunCategoryTests( TestStatus filter );
		void doRunAllCategoryTestsBut( TestStatus filter );
		void doRunAllCategoryOutdatedTests();
		void doUpdateCategoryCastorDate();
		void doRunAllTests();
		void doRunTests( TestStatus filter );
		void doRunAllTestsBut( TestStatus filter );
		void doRunAllOutdatedTests();
		void doUpdateAllCastorDate();
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
		wxAuiManager m_auiManager;
		Config m_config;
		TestDatabase m_database;
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
		struct RunningTest
		{
			std::unique_ptr< wxProcess > genProcess{};
			std::unique_ptr< wxProcess > difProcess{};
			std::unique_ptr< wxProcess > disProcess{};
			std::list< TestNode > tests{};
		};
		RunningTest m_runningTest;
		std::atomic_bool m_cancelled;

		struct TestUpdater
		{
			explicit TestUpdater( wxObjectDataPtr< TreeModel > & model );
			void addTest( TreeModelNode & test );
			void stop();

		private:
			std::vector< TreeModelNode * > get();
			void set( std::vector< TreeModelNode * > current );

			std::mutex mutex;
			std::vector< TreeModelNode * > running;
			std::atomic_bool isStopped{ false };
			std::thread thread;
		};
		TestUpdater m_updater;
	};
}

#endif
