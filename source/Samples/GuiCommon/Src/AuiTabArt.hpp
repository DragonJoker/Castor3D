/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
