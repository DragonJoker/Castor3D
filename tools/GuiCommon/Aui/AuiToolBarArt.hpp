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

		void DrawBackground( wxDC & dc, wxWindow * window, wxRect const & rect )override;
		void DrawPlainBackground( wxDC & dc, wxWindow * window, wxRect const & rect )override;
		void DrawSeparator( wxDC & dc, wxWindow * window, wxRect const & rect )override;
	};
}

#endif
