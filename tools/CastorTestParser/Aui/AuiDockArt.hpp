/*
See LICENSE file in root folder
*/
#ifndef ___CTP_AuiDockArt_HPP___
#define ___CTP_AuiDockArt_HPP___

#include "CastorTestParser/Prerequisites.hpp"

#include <wx/aui/aui.h>
#include <wx/aui/dockart.h>

namespace test_parser
{
	class AuiDockArt
		: public wxAuiDefaultDockArt
	{
	public:
		AuiDockArt();

		void DrawBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect );
	};
}

#endif
