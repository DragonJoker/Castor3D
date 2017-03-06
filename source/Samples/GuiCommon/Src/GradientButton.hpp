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
#ifndef ___GC_BUTTON_H___
#define ___GC_BUTTON_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/button.h>

namespace GuiCommon
{
	class GradientButton
		: public wxButton
	{
	public:
		GradientButton();
		GradientButton( wxWindow * p_parent, wxWindowID p_id, wxString const & p_label = wxEmptyString, wxPoint const & p_pos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize, long p_style = 0, wxValidator const & p_validator = wxDefaultValidator, wxString const & p_name = wxButtonNameStr );

	private:
		void DoInitialise();
		DECLARE_EVENT_TABLE()
		void OnPaint( wxPaintEvent & p_event );

	private:
		wxColour m_gradientTopStartColour;
		wxColour m_gradientTopEndColour;
		wxColour m_gradientBottomStartColour;
		wxColour m_gradientBottomEndColour;
		wxColour m_pressedColourTop;
		wxColour m_pressedColourBottom;
	};
}

#endif
