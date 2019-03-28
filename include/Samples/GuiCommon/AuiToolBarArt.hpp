/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_TOOLBAR_ART_H___
#define ___GUICOMMON_TOOLBAR_ART_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <wx/aui/auibar.h>

namespace GuiCommon
{
	class AuiToolBarArt
		: public wxAuiDefaultToolBarArt
	{
	public:
		AuiToolBarArt();

		wxAuiToolBarArt * Clone()override;

		virtual void DrawBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect );
		virtual void DrawPlainBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect );
		virtual void DrawSeparator( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect );
	};
}

#endif
