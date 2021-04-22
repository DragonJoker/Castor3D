/*
See LICENSE file in root folder
*/
#ifndef ___CTP_CategoryPanel_HPP___
#define ___CTP_CategoryPanel_HPP___

#include "TestsCounts.hpp"

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

		void refresh();
		void update( wxString const & name
			, AllTestsCounts & counts );
		void update( wxString const & name
			, RendererTestsCounts & counts );
		void update( wxString const & name
			, CategoryTestsCounts & counts );

	private:
		Config const & m_config;
		AllTestsCounts * m_allCounts{};
		RendererTestsCounts * m_rendererCounts{};
		CategoryTestsCounts * m_categoryCounts{};
		wxString m_name;
		std::array< wxStaticText *, TestsCountsType::eCount > m_values{};
#if CTP_UseCountedValue
		std::array< CountedUIntConnection, TestsCountsType::eCount > m_connections{};
#endif
	};
}

#endif
