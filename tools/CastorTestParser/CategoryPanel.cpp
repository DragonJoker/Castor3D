#include "CastorTestParser/CategoryPanel.hpp"

namespace test_parser
{
	CategoryPanel::CategoryPanel( wxWindow * parent
		, wxPoint const & position
		, wxSize const & size )
		: wxPanel{ parent, wxID_ANY, position, size }
	{
		SetBackgroundColour( PANEL_FOREGROUND_COLOUR );
		SetForegroundColour( PANEL_BACKGROUND_COLOUR );
	}

	void CategoryPanel::setCategory( wxString const & category
		, TestArray const & tests )
	{
		m_category = category;
		m_tests = &tests;
	}
}