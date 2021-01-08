/*
See LICENSE file in root folder
*/
#ifndef ___CTP_AuiDockArt_HPP___
#define ___CTP_AuiDockArt_HPP___

#include "Aria/Prerequisites.hpp"

#include <wx/aui/aui.h>
#include <wx/aui/dockart.h>

namespace aria
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
