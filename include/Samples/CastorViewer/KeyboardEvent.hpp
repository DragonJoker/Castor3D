/*
See LICENSE file in root folder
*/
#ifndef ___CV_KEYBOARD_EVENT_H___
#define ___CV_KEYBOARD_EVENT_H___

#include "RenderPanel.hpp"

#include <Castor3D/Event/Frame/FrameEvent.hpp>

namespace CastorViewer
{
	class KeyboardEvent
		: public castor3d::FrameEvent
	{
	public:
		explicit KeyboardEvent( castor3d::RenderWindowSPtr p_window );
		virtual ~KeyboardEvent();

		virtual bool apply();

	private:
		castor3d::RenderWindowWPtr m_window;
	};
}

#endif
