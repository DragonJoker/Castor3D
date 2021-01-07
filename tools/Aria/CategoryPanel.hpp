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

		void refresh();
		void update( wxString const & name
			, TestsCounts & counts );

	private:
		Config const & m_config;
		TestsCounts * m_counts{};
		wxStaticText * m_status;
		wxStaticText * m_negligible;
		wxStaticText * m_acceptable;
		wxStaticText * m_unacceptable;
		wxStaticText * m_unprocessed;
		wxStaticText * m_pending;
		wxStaticText * m_running;
		wxStaticText * m_outdated;
		wxStaticText * m_ignored;
	};
}

#endif
