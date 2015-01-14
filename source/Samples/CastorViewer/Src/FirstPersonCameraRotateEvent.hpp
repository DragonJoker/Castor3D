#ifndef ___CV_FIRST_PERSON_CAMERA_ROTATE_EVENT_H___
#define ___CV_FIRST_PERSON_CAMERA_ROTATE_EVENT_H___

#include "FirstPersonCameraEvent.hpp"

namespace CastorViewer
{
	class FirstPersonCameraRotateEvent
		:	public FirstPersonCameraEvent
	{
	public:
		FirstPersonCameraRotateEvent( Castor3D::SceneNodeSPtr p_pNode, Castor::real p_rYaw, Castor::real p_rPitch );
		virtual ~FirstPersonCameraRotateEvent();

		virtual bool Apply();
	};
}

#endif