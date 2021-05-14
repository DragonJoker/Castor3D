/*
See LICENSE file in root folder
*/
#ifndef ___GC_BUTTON_H___
#define ___GC_BUTTON_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

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
		void doInitialise();
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
