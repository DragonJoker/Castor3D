/*
See LICENSE file in root folder
*/
#ifndef ___CV_ROTATE_NODE_EVENT_H___
#define ___CV_ROTATE_NODE_EVENT_H___

#include "MouseNodeEvent.hpp"

namespace CastorViewer
{
	class RotateNodeEvent
		: public MouseNodeEvent
	{
	public:
		RotateNodeEvent( castor3d::SceneNodeSPtr p_node, castor::real p_dx, castor::real p_dy, castor::real p_dz );
		virtual ~RotateNodeEvent();

		bool apply()override;
	};
}

#endif
