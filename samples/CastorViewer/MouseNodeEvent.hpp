/*
See LICENSE file in root folder
*/
#ifndef ___CV_MOUSE_CAMERA_EVENT_H___
#define ___CV_MOUSE_CAMERA_EVENT_H___

#include "RenderPanel.hpp"

#include <Castor3D/Event/Frame/FrameEvent.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace CastorViewer
{
	class MouseNodeEvent
		: public castor3d::FrameEvent
	{
	public:
		MouseNodeEvent( castor3d::SceneNodeSPtr p_node, float p_dx, float p_dy, float p_dz );
		virtual ~MouseNodeEvent();

	protected:
		castor3d::SceneNodeWPtr m_node;
		float m_dx;
		float m_dy;
		float m_dz;
	};
}

#endif
