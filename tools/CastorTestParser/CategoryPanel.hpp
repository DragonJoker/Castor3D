/*
See LICENSE file in root folder
*/
#ifndef ___CTP_CategoryPanel_HPP___
#define ___CTP_CategoryPanel_HPP___

#include "Prerequisites.hpp"

#include <wx/panel.h>

class wxStaticText;

namespace test_parser
{
	class CategoryPanel
		: public wxPanel
	{
	public:
		CategoryPanel( Config const & config
			, wxWindow * parent
			, wxPoint const & position
			, wxSize const & size );

		void setRenderer( wxString const & renderer
			, TestCategoryMap const & tests );
		void setCategory( wxString const & category
			, TestArray const & tests );
		void setAll( TestMap const & tests
			, std::list< TestNode > const & running );

	private:
		Config const & m_config;
		wxString m_renderer;
		wxString m_category;
		TestCategoryMap const * m_rendererTests{};
		TestArray const * m_categoryTests{};
		wxStaticText * m_status;
		wxStaticText * m_negligible;
		wxStaticText * m_acceptable;
		wxStaticText * m_unacceptable;
		wxStaticText * m_unprocessed;
		wxStaticText * m_ignored;
		wxStaticText * m_outdated;
		wxStaticText * m_running;
	};
}

#endif
