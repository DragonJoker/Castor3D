/*
See LICENSE file in root folder
*/
#ifndef ___CTP_CategoryPanel_HPP___
#define ___CTP_CategoryPanel_HPP___

#include "Prerequisites.hpp"

#include <wx/panel.h>

class wxStaticText;

namespace aria
{
	class CategoryPanel
		: public wxPanel
	{
	public:
		CategoryPanel( Config const & config
			, wxWindow * parent
			, wxPoint const & position
			, wxSize const & size );

		void setAll( TestMap const & tests
			, TestRunMap const & runs
			, std::list< TestNode > const & running );
		void setRenderer( wxString const & renderer
			, TestMap const & tests
			, TestRunCategoryMap const & runs
			, std::list< TestNode > const & running );
		void setCategory( wxString const & category
			, TestArray const & tests
			, TestRunArray const & runs
			, std::list< TestNode > const & running );

		void update( std::list< TestNode > const & running );

	public:
		struct Counts
		{
			wxString name;
			uint32_t all{};
			uint32_t negligible{};
			uint32_t acceptable{};
			uint32_t unacceptable{};
			uint32_t unprocessed{};
			uint32_t ignored{};
			uint32_t outdated{};
			uint32_t running{};
		};

	private:
		Config const & m_config;
		Counts m_counts;
		TestMap const * m_allTests{};
		TestRunMap const * m_allRuns{};
		TestMap const * m_rendererTests{};
		TestRunCategoryMap const * m_rendererRuns{};
		TestArray const * m_categoryTests{};
		TestRunArray const * m_categoryRuns{};
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
