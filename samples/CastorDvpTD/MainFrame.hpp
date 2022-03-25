#pragma once

#include "CastorDvpTDPrerequisites.hpp"

#include <wx/frame.h>
#include <wx/windowptr.h>

namespace castortd
{
	class MainFrame
		: public wxFrame
	{
	public:
		MainFrame();

	private:
		void doLoadScene();

		DECLARE_EVENT_TABLE()
		void OnPaint( wxPaintEvent  & event );
		void OnClose( wxCloseEvent  & event );
		void OnEraseBackground( wxEraseEvent & event );
		void OnRenderTimer( wxTimerEvent & event );

	private:
		wxWindowPtr< RenderPanel > m_panel;
		std::unique_ptr< Game > m_game;
		wxTimer * m_timer{ nullptr };
	};
}
