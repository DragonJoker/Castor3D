#ifndef ___CV_ROTATE_NODE_EVENT_H___
#define ___CV_ROTATE_NODE_EVENT_H___

#include "MouseNodeEvent.hpp"

namespace CastorViewer
{
	class RotateNodeEvent
		: public MouseNodeEvent
	{
	public:
		RotateNodeEvent( Castor3D::SceneNodeSPtr p_node, Castor::real p_dx, Castor::real p_dy, Castor::real p_dz );
		virtual ~RotateNodeEvent();

		virtual bool Apply();
	};
}

#endif
