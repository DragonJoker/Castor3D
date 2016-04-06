#ifndef ___CV_KEYBOARD_EVENT_H___
#define ___CV_KEYBOARD_EVENT_H___

#include "RenderPanel.hpp"

#include <FrameEvent.hpp>

namespace CastorViewer
{
	class KeyboardEvent
		: public Castor3D::FrameEvent
	{
	public:
		KeyboardEvent( Castor3D::RenderWindowSPtr p_window );
		virtual ~KeyboardEvent();

		virtual bool Apply();

	private:
		Castor3D::RenderWindowWPtr m_window;
	};
}

#endif
