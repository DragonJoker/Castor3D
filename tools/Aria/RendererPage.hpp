/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_RendererPage_HPP___
#define ___ARIA_RendererPage_HPP___

#include "Prerequisites.hpp"

#include <wx/aui/framemanager.h>
#include <wx/dataview.h>
#include <wx/panel.h>

#include <functional>
#include <map>

class wxMenu;
class wxProgressDialog;
class wxStaticText;

namespace aria
{
	class RendererPage
		: public wxPanel
	{
	public:
		struct Selection
		{
			wxDataViewItemArray items;
			bool allTests{};
			bool allCategories{};
			bool allRenderers{};
		};

	public:
		using FilterFunc = std::function< bool( DatabaseTest const & ) >;

	public:
		RendererPage( RendererPage && ) = default;
		RendererPage & operator=( RendererPage && ) = default;
		RendererPage( Config const & config
			, Renderer renderer
			, TestRunCategoryMap & runs
			, RendererTestsCounts & counts
			, wxWindow * parent
			, wxWindow * frame
			, wxMenu * testMenu
			, wxMenu * categoryMenu
			, wxMenu * rendererMenu
			, wxMenu * allMenu
			, wxMenu * busyMenu );
		~RendererPage();

		TreeModelNode * getTestNode( DatabaseTest const & test )const;
		void refreshView()const;
		void resizeModel( wxSize const & size );
		void listLatestRuns( TestDatabase & database
			, TestMap const & tests
			, AllTestsCounts & counts
			, wxProgressDialog & progress
			, int & index );
		uint32_t getSelectedRange( TestRunMap const & runs )const;
		uint32_t getSelectedCategoryRange( TestRunMap const & runs )const;
		void updateTest( TreeModelNode * node );
		std::vector< wxDataViewItem > listRendererTests( TestRunMap const & runs
			, FilterFunc filter )const;
		std::vector< wxDataViewItem > listCategoryTests( TestRunMap const & runs
			, FilterFunc filter )const;
		std::vector< wxDataViewItem > listSelectedTests()const;
		void copyTestFileName()const;
		void viewTestSceneFile();
		void viewTest( wxProcess * process
			, wxStaticText * statusText )const;
		void setTestsReferences( AllTestsCounts & counts );
		void ignoreTestsResult( bool ignore );
		void updateTestsCastorDate();
		void updateTestsSceneDate();
		void addCategory( Category category
			, CategoryTestsCounts & catCounts );
		void addTest( DatabaseTest & dbTest );
		void updateTestView( DatabaseTest const & test
			, AllTestsCounts & counts );

	private:
		void doInitLayout( wxWindow * frame );
		void doUpdateTestStatus( DatabaseTest & test
			, AllTestsCounts & counts
			, TestStatus newStatus
			, bool reference );
		void onSelectionChange( wxDataViewEvent & evt );
		void onItemContextMenu( wxDataViewEvent & evt );

	private:
		Config const & m_config;
		Renderer m_renderer;
		wxMenu * m_testMenu;
		wxMenu * m_categoryMenu;
		wxMenu * m_rendererMenu;
		wxMenu * m_allMenu;
		wxMenu * m_busyMenu;
		wxAuiManager m_auiManager;
		TestRunCategoryMap * m_runs;
		RendererTestsCounts & m_counts;
		wxObjectDataPtr< TreeModel > m_model;
		wxDataViewCtrl * m_view{};
		LayeredPanel * m_generalViews{};
		LayeredPanel * m_detailViews{};
		TestPanel * m_testView{};
		CategoryPanel * m_allView{};
		CategoryPanel * m_categoryView{};
		std::map< uint32_t, TreeModelNode * > m_modelNodes;
		Selection m_selected;
	};
}

#endif
