#ifndef ___CV_KEYBOARD_EVENT_H___
#define ___CV_KEYBOARD_EVENT_H___

#include "RenderPanel.hpp"

#include <FrameEvent.hpp>

namespace CastorViewer
{
	class MainFrame;

	class KeyboardEvent
		:	public Castor3D::FrameEvent
	{
	public:
		KeyboardEvent( Castor3D::RenderWindowSPtr p_pWindow, MainFrame * p_pMainFrame );
		virtual ~KeyboardEvent();

		static void Add( Castor3D::FrameEventSPtr p_pThis, Castor3D::FrameListenerSPtr p_pListener );

		virtual bool Apply();

	private:
		Castor3D::RenderWindowWPtr m_pWindow;
		MainFrame * m_pMainFrame;
	};
}

#endif
