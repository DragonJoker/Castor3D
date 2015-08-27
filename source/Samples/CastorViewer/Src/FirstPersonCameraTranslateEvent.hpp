#ifndef ___CV_FIRST_PERSON_CAMERA_TRANSLATE_EVENT_H___
#define ___CV_FIRST_PERSON_CAMERA_TRANSLATE_EVENT_H___

#include "FirstPersonCameraEvent.hpp"

namespace CastorViewer
{
	class FirstPersonCameraTranslateEvent
		: public FirstPersonCameraEvent
	{
	public:
		FirstPersonCameraTranslateEvent( Castor3D::SceneNodeSPtr p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		FirstPersonCameraTranslateEvent( Castor3D::SceneNode * p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~FirstPersonCameraTranslateEvent();

		virtual bool Apply();
	};
}

#endif