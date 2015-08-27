#ifndef ___CV_CAMERA_TRANSLATE_EVENT_H___
#define ___CV_CAMERA_TRANSLATE_EVENT_H___

#include "MouseCameraEvent.hpp"

namespace CastorViewer
{
	class CameraTranslateEvent
		:	public MouseCameraEvent
	{
	public:
		CameraTranslateEvent( Castor3D::SceneNodeSPtr p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		CameraTranslateEvent( Castor3D::SceneNode * p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~CameraTranslateEvent();

		virtual bool Apply();
	};
}

#endif