/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GUICOMMON_TAB_ART_H___
#define ___GUICOMMON_TAB_ART_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/aui/tabart.h>

namespace GuiCommon
{
	class AuiTabArt
		: public wxAuiDefaultTabArt
	{
	public:
		AuiTabArt();

		virtual wxAuiTabArt * Clone();

		virtual void DrawBackground( wxDC & p_dc, wxWindow * p_window, wxRect const & p_rect );
		// DrawTab() draws an individual tab.
		//
		// dc       - output dc
		// in_rect  - rectangle the tab should be confined to
		// caption  - tab's caption
		// active   - whether or not the tab is active
		// out_rect - actual output rectangle
		// x_extent - the advance x; where the next tab should start
		void DrawTab( wxDC & dc, wxWindow * wnd, const wxAuiNotebookPage & page, const wxRect & in_rect, int close_button_state, wxRect * out_tab_rect, wxRect * out_button_rect, int * x_extent );
	};
}

#endif
