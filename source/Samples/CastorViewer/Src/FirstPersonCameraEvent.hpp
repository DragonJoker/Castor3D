#ifndef ___CV_FIRST_PERSON_CAMERA_EVENT_H___
#define ___CV_FIRST_PERSON_CAMERA_EVENT_H___

#include "MouseCameraEvent.hpp"

namespace CastorViewer
{
	class FirstPersonCameraEvent
		: public MouseCameraEvent
	{
	protected:
		Castor3D::SceneNodeSPtr m_pNodeYaw;
		Castor3D::SceneNodeSPtr m_pNodePitch;
		Castor3D::SceneNodeSPtr m_pNodeRoll;
		bool m_bOK;

	public:
		FirstPersonCameraEvent( Castor3D::SceneNodeSPtr p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~FirstPersonCameraEvent();

	private:
		void DoInitialise();
	};
}

#endif