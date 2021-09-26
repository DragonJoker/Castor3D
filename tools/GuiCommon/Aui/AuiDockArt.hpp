/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_DOCK_ART_H___
#define ___GUICOMMON_DOCK_ART_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#pragma warning( push )
#pragma warning( disable: 4365 )
#include <wx/aui/aui.h>
#include <wx/aui/dockart.h>
#pragma warning( pop )

namespace GuiCommon
{
	class AuiDockArt
		: public wxAuiDefaultDockArt
	{
	public:
		AuiDockArt();

		void DrawBackground( wxDC & dc
			, wxWindow * window
			, int orientation
			, const wxRect & rect )override;
	};
}

#endif
