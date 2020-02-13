/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_DOCK_ART_H___
#define ___GUICOMMON_DOCK_ART_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <wx/aui/aui.h>
#include <wx/aui/dockart.h>

namespace GuiCommon
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
