/*
See LICENSE file in root folder
*/
#ifndef ___CTP_CategoryPanel_HPP___
#define ___CTP_CategoryPanel_HPP___

#include "Prerequisites.hpp"

#include <wx/panel.h>

namespace test_parser
{
	class CategoryPanel
		: public wxPanel
	{
	public:
		CategoryPanel( wxWindow * parent
			, wxPoint const & position
			, wxSize const & size );

		void setCategory( wxString const & category
			, TestArray const & tests );

	private:
		wxString m_category;
		TestArray const * m_tests{};
	};
}

#endif
