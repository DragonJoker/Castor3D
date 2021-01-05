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

		void doInitTestsLists();
		void doInitTestsList( Renderer renderer );
		wxWindow * doInitDetailsView();
		void doInitGui();
		void doInitMenus();
		void doInitMenuBar();
		void doFillLists( wxProgressDialog & progress, int & index );
		void doFillList( Renderer renderer
			, wxProgressDialog & progress
			, int & index );
		TestsPage & doGetPage( wxDataViewItem const & item );

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
		void doCancelTest( DatabaseTest & test
			, TestStatus status );
		void doCancel();
		void doNewRenderer();
		void doNewCategory();
		void doNewTest();
		void onTestRunEnd( int status );
		void onTestDisplayEnd( int status );
		void onTestDiffEnd( int status );
		void onTestProcessEnd( int pid, int status );

		void onTestsPageChange( wxAuiNotebookEvent & evt );
		void onSelectionChange( wxDataViewEvent & evt );
		void onItemContextMenu( wxDataViewEvent & evt );
		void onTestsMenuOption( wxCommandEvent & evt );
		void onDatabaseMenuOption( wxCommandEvent & evt );
		void onProcessEnd( wxProcessEvent & evt );

	private:
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
				, TestRunCategoryMap & runs );
			~TestsPage();
			TestRunCategoryMap * runs;
			wxObjectDataPtr< TreeModel > model;
			wxDataViewCtrl * view{};
			std::map< uint32_t, TreeModelNode * > modelNodes;
			TestUpdater updater;
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
			std::list< TestNode > tests{};
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
		LayeredPanel * m_detailViews{};
		TestPanel * m_testView{};
		CategoryPanel * m_categoryView{};
		std::map< Renderer, std::unique_ptr< TestsPage > > m_testsPages;
		wxAuiNotebook * m_testsBook{};
		TestsPage * m_selectedPage{};
		wxStaticText * m_statusText{};
		wxGauge * m_testProgress{};
		wxGauge * m_statusProgress{};
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
	};
}

#endif
