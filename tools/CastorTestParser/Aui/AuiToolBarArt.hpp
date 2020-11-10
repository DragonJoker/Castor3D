/*
See LICENSE file in root folder
*/
#ifndef ___CTP_AuiToolBarArt_H___
#define ___CTP_AuiToolBarArt_H___

#include "CastorTestParser/Prerequisites.hpp"

#include <wx/bitmap.h>
#include <wx/aui/auibar.h>

namespace test_parser
{
	class AuiToolBarArt
		: public wxAuiDefaultToolBarArt
	{
	public:
		AuiToolBarArt();

		wxAuiToolBarArt * Clone()override;

		void DrawBackground( wxDC & dc
			, wxWindow * window
			, wxRect const & rect )override;
		void DrawPlainBackground( wxDC & dc
			, wxWindow * window
			, wxRect const & rect )override;
		void DrawSeparator( wxDC & dc
			, wxWindow * window
			, wxRect const & rect )override;
	};
}

#endif
