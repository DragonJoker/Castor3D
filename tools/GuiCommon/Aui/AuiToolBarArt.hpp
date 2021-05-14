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

		void DrawBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect )override;
		void DrawPlainBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect )override;
		void DrawSeparator( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect )override;
	};
}

#endif
