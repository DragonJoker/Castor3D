/*
See LICENSE file in root folder
*/
#ifndef ___CV_MOUSE_CAMERA_EVENT_H___
#define ___CV_MOUSE_CAMERA_EVENT_H___

#include "RenderPanel.hpp"

#include <Event/Frame/FrameEvent.hpp>
#include <Math/RangedValue.hpp>

namespace CastorViewer
{
	class MouseNodeEvent
		: public castor3d::FrameEvent
	{
	public:
		MouseNodeEvent( castor3d::SceneNodeSPtr p_node, castor::real p_dx, castor::real p_dy, castor::real p_dz );
		virtual ~MouseNodeEvent();

	protected:
		castor3d::SceneNodeWPtr m_node;
		castor::real m_dx;
		castor::real m_dy;
		castor::real m_dz;
	};
}

#endif
