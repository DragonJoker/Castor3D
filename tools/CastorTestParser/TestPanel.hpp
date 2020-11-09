/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestPanel_HPP___
#define ___CTP_TestPanel_HPP___

#include "Prerequisites.hpp"

#include <wx/panel.h>

namespace test_parser
{
	class wxImagePanel;

	class TestPanel
		: public wxPanel
	{
	public:
		TestPanel( wxWindow * parent
			, Config const & config );

		void setTest( Test & test );

	private:
		Config const & m_config;
		Test * m_test{};
		wxImagePanel * m_ref{};
		wxImagePanel * m_result{};
	};
}

#endif
