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

		void updateTestStatus( DatabaseTest & test
			, TestStatus newStatus
			, bool reference );
		TreeModelNode * getTestNode( DatabaseTest const & test );

	private:
		struct TestsPage;

		wxWindow * doInitTestsLists();
		void doInitTestsList( Renderer renderer );
		void doInitGui();
		void doInitMenus();
		void doInitMenuBar();
		void doFillLists( wxProgressDialog & progress, int & index );
		void doFillList( Renderer renderer
			, wxProgressDialog & progress
			, int & index );
		TestsPage & doGetPage( wxDataViewItem const & item );

		void doProcessTest();
		void doPushTest( TreeModelNode * node );
		void doClearRunning();
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
		void doCancelTest( DatabaseTest & test
			, TestStatus status );
		void doCancel();
		void doNewRenderer();
		void doNewCategory();
		void doNewTest();
		void doUpdateTestView( DatabaseTest const & test );
		void onTestRunEnd( int status );
		void onTestDisplayEnd( int status );
		void onTestDiffEnd( int status );
		void onTestProcessEnd( int pid, int status );

		void onClose( wxCloseEvent & evt );
		void onTestsPageChange( wxAuiNotebookEvent & evt );
		void onSelectionChange( wxDataViewEvent & evt );
		void onItemContextMenu( wxDataViewEvent & evt );
		void onTestsMenuOption( wxCommandEvent & evt );
		void onDatabaseMenuOption( wxCommandEvent & evt );
		void onProcessEnd( wxProcessEvent & evt );
		void onTestUpdateTimer( wxTimerEvent & evt );
		void onCategoryUpdateTimer( wxTimerEvent & evt );

		struct Selection
		{
			wxDataViewItemArray items;
			bool allTests{};
			bool allCategories{};
			bool allRenderers{};
		};

		struct TestsPage
		{
			TestsPage( TestsPage && ) = default;
			TestsPage & operator=( TestsPage && ) = default;
			TestsPage( Config const & config
				, Renderer renderer
				, TestRunCategoryMap & runs
				, RendererTestsCounts & counts
				, wxWindow * list );
			~TestsPage();
			wxAuiManager auiManager;
			TestRunCategoryMap * runs;
			TestsCountsCategoryMap * counts;
			wxObjectDataPtr< TreeModel > model;
			wxDataViewCtrl * view{};
			LayeredPanel * generalViews{};
			LayeredPanel * detailViews{};
			TestPanel * testView{};
			CategoryPanel * allView{};
			CategoryPanel * categoryView{};
			std::map< uint32_t, TreeModelNode * > modelNodes;
			Selection selected;
		};

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

	public:
		struct RunningTest
		{
			std::unique_ptr< wxProcess > genProcess{};
			std::unique_ptr< wxProcess > difProcess{};
			std::unique_ptr< wxProcess > disProcess{};
			std::list< TestNode > pending{};
			std::list< TestNode > running{};
			wxProcess * currentProcess{};
		};

	private:
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

	private:
		wxAuiManager m_auiManager;
		Config m_config;
		TestDatabase m_database;
		Tests m_tests;
		std::map< Renderer, std::unique_ptr< TestsPage > > m_testsPages;
		wxAuiNotebook * m_testsBook{};
		TestsPage * m_selectedPage{};
		wxStaticText * m_statusText{};
		wxGauge * m_testProgress{};
		bool m_hasPage{ false };
		std::unique_ptr< wxMenu > m_testMenu{};
		std::unique_ptr< wxMenu > m_categoryMenu{};
		std::unique_ptr< wxMenu > m_rendererMenu{};
		std::unique_ptr< wxMenu > m_allMenu{};
		std::unique_ptr< wxMenu > m_busyMenu{};
		wxMenu * m_barTestMenu{};
		wxMenu * m_barCategoryMenu{};
		wxMenu * m_barRendererMenu{};
		wxMenu * m_barAllMenu{};
		RunningTest m_runningTest;
		std::atomic_bool m_cancelled;
		TestProcessChecker m_processChecker;
		wxTimer * m_testUpdater;
		wxTimer * m_categoriesUpdater;
	};
}

#endif
