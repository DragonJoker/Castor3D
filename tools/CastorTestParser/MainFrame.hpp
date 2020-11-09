/*
See LICENSE file in root folder
*/
#ifndef ___CTP_MainFrame_HPP___
#define ___CTP_MainFrame_HPP___

#include "Prerequisites.hpp"
#include "Database/DbConnection.hpp"

#include <wx/frame.h>
#include <wx/dataview.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>

#include <map>

namespace test_parser
{
	class MainFrame
		: public wxFrame
	{
	public:
		explicit MainFrame( Config config );
		~MainFrame();

	private:
		wxWindow * doInitTestsList();
		wxWindow * doInitDetailsView();
		void doInitGui();
		void doInitDatabase();
		void doFillDatabase();
		void doListTests();
		void doFillList();

		void onItemActivated( wxDataViewEvent & evt );

	private:
		wxAuiManager m_auiManager;
		Config m_config;
		db::Connection m_database;
		TestMap m_tests;
		wxDataViewCtrl * m_view{};
		wxObjectDataPtr< TreeModel > m_model;
		LayeredPanel * m_detailViews{};
		TestPanel * m_testView{};
		CategoryPanel * m_categoryView{};
		bool m_hasPage{ false };
	};
}

#endif
