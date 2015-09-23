#ifndef ___CV_CAMERA_ROTATE_EVENT_H___
#define ___CV_CAMERA_ROTATE_EVENT_H___

#include "MouseCameraEvent.hpp"

namespace CastorViewer
{
	class CameraRotateEvent
		:	public MouseCameraEvent
	{
	public:
		CameraRotateEvent( Castor3D::SceneNodeSPtr p_node, Castor::real p_dx, Castor::real p_dy, Castor::real p_dz );
		virtual ~CameraRotateEvent();

		virtual bool Apply();
	};
}

#endif