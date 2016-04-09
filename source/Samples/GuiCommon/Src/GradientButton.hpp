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
		DECLARE_EVENT_TABLE();
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
