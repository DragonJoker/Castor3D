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
			, TestsCounts & counts );

	private:
		Config const & m_config;
		TestsCounts * m_counts{};
		wxString m_name;
		std::array< wxStaticText *, TestsCounts::Type::eCount > m_values{};
#if CTP_UseCountedValue
		std::array< CountedUIntConnection, TestsCounts::Type::eCount > m_connections{};
#endif
	};
}

#endif
