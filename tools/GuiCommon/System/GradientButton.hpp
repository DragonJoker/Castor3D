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
		GradientButton( wxWindow * parent, wxWindowID id, wxString const & label = wxEmptyString, wxPoint const & pos = wxDefaultPosition, wxSize const & size = wxDefaultSize, long style = 0, wxValidator const & validator = wxDefaultValidator, wxString const & name = wxButtonNameStr );

	private:
		void doInitialise();
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void OnPaint( wxPaintEvent & event );

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
