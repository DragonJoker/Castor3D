#ifndef ___CV_CAMERA_ROTATE_EVENT_H___
#define ___CV_CAMERA_ROTATE_EVENT_H___

#include "MouseCameraEvent.hpp"

namespace CastorViewer
{
	class CameraRotateEvent
		:	public MouseCameraEvent
	{
	public:
		CameraRotateEvent( Castor3D::SceneNodeSPtr p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		CameraRotateEvent( Castor3D::SceneNode * p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~CameraRotateEvent();

		virtual bool Apply();
	};
}

#endif