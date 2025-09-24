#pragma once

#include "CastorDvpTDPrerequisites.hpp"

#include <wx/defs.h>
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
#pragma clang diagnostic ignored "-Wsuggest-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void onPaint( wxPaintEvent  & event );
		void onClose( wxCloseEvent  & event );
		void onEraseBackground( wxEraseEvent & event );
		void onRenderTimer( wxTimerEvent & event );
		void onKeyDown( wxKeyEvent & event );
		void onKeyUp( wxKeyEvent & event );
		void onMouseLDown( wxMouseEvent & event );
		void onMouseLUp( wxMouseEvent & event );
		void onMouseRUp( wxMouseEvent & event );
		void onMouseWheel( wxMouseEvent & event );

	private:
		wxWindowPtr< RenderPanel > m_panel;
		c3d::RawUniquePtr< Game > m_game;
		c3d::RawUniquePtr< wxTimer > m_timer{};
	};
}
