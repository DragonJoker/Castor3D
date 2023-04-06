/*
See LICENSE file in root folder
*/
#ifndef ___CV_MOUSE_CAMERA_EVENT_H___
#define ___CV_MOUSE_CAMERA_EVENT_H___

#include "RenderPanel.hpp"

#include <Castor3D/Event/Frame/CpuFrameEvent.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace CastorViewer
{
	class MouseNodeEvent
		: public castor3d::CpuFrameEvent
	{
	public:
		MouseNodeEvent( castor3d::SceneNodeRPtr node
			, float dx
			, float dy
			, float dz );

	protected:
		castor3d::SceneNodeRPtr m_node{};
		float m_dx{};
		float m_dy{};
		float m_dz{};
	};
}

#endif
