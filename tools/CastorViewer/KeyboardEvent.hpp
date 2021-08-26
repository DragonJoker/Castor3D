/*
See LICENSE file in root folder
*/
#ifndef ___CV_KEYBOARD_EVENT_H___
#define ___CV_KEYBOARD_EVENT_H___

#include "RenderPanel.hpp"

#include <Castor3D/Event/Frame/CpuFrameEvent.hpp>

namespace CastorViewer
{
	class KeyboardEvent
		: public castor3d::CpuFrameEvent
	{
	public:
		explicit KeyboardEvent( castor3d::RenderWindow & window );
		virtual ~KeyboardEvent();

		void apply()override;

	private:
		castor3d::RenderWindow * m_window;
	};
}

#endif
