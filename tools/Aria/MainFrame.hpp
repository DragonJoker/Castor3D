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
#include <thread>

class wxGauge;
class wxStaticText;

namespace aria
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
		void doInitMenuBar();
		void doFillList( wxProgressDialog & progress, int & index, uint32_t testCount );

		void doProcessTest();
		void doRunTest();
		void doCopyTestFileName();
		void doViewTest();
		void doSetRef();
		void doIgnoreTestResult();
		void doUpdateCastorDate();
		void doUpdateSceneDate();
		void doRunAllCategoryTests();
		void doRunCategoryTests( TestStatus filter );
		void doRunAllCategoryTestsBut( TestStatus filter );
		void doRunAllCategoryOutdatedTests();
		void doUpdateCategoryCastorDate();
		void doUpdateCategorySceneDate();
		void doRunAllRendererTests();
		void doRunRendererTests( TestStatus filter );
		void doRunAllRendererTestsBut( TestStatus filter );
		void doRunAllRendererOutdatedTests();
		void doUpdateRendererCastorDate();
		void doUpdateRendererSceneDate();
		void doRunAllTests();
		void doRunTests( TestStatus filter );
		void doRunAllTestsBut( TestStatus filter );
		void doRunAllOutdatedTests();
		void doUpdateAllCastorDate();
		void doUpdateAllSceneDate();
		void doCancelTest( Test & test
			, TestStatus status );
		void doCancel();
		void onTestRunEnd( int status );
		void onTestDisplayEnd( int status );
		void onTestDiffEnd( int status );
		void onTestProcessEnd( int pid, int status );

		void onSelectionChange( wxDataViewEvent & evt );
		void onItemContextMenu( wxDataViewEvent & evt );
		void onMenuOption( wxCommandEvent & evt );
		void onProcessEnd( wxProcessEvent & evt );

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
		wxStaticText * m_statusText{};
		wxGauge * m_testProgress{};
		wxGauge * m_statusProgress{};
		bool m_hasPage{ false };
		std::unique_ptr< wxMenu > m_testMenu{};
		std::unique_ptr< wxMenu > m_categoryMenu{};
		std::unique_ptr< wxMenu > m_rendererMenu{};
		std::unique_ptr< wxMenu > m_allMenu{};
		std::unique_ptr< wxMenu > m_busyMenu{};
		struct Selection
		{
			wxDataViewItemArray items;
			bool allTests{};
			bool allCategories{};
			bool allRenderers{};
		};
		Selection m_selected;
		class TestProcess
			: public wxProcess
		{
		public:
			TestProcess( MainFrame * mainframe
				, int flags );

			void OnTerminate( int pid, int status )override;

		private:
			MainFrame * m_mainframe;
		};
		struct RunningTest
		{
			std::unique_ptr< wxProcess > genProcess{};
			std::unique_ptr< wxProcess > difProcess{};
			std::unique_ptr< wxProcess > disProcess{};
			std::list< TestNode > tests{};
			wxProcess * currentProcess{};
		};
		RunningTest m_runningTest;
		std::atomic_bool m_cancelled;

		struct TestProcessChecker
		{
			explicit TestProcessChecker( MainFrame * mainFrame );
			void checkProcess( int pid );
			void stop();

		private:
			int get();

			std::atomic_int pid{};
			std::atomic_bool isStopped{ false };
			std::thread thread;
		};
		TestProcessChecker m_processChecker;

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
