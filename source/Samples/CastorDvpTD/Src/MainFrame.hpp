#pragma once

#include <wx/frame.h>
#include <wx/windowptr.h>

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class MainFrame
		: public wxFrame
	{
	public:
		MainFrame();
		~MainFrame();

		void Initialise();

	private:
		DECLARE_EVENT_TABLE()
		void OnPaint( wxPaintEvent  & p_event );
		void OnClose( wxCloseEvent  & p_event );
		void OnEraseBackground( wxEraseEvent & p_event );

	private:
		wxWindowPtr< RenderPanel > m_panel;
	};
}
