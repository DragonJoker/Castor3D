#ifndef ___CV_MOUSE_CAMERA_EVENT_H___
#define ___CV_MOUSE_CAMERA_EVENT_H___

#include "RenderPanel.hpp"

#include <FrameEvent.hpp>

namespace CastorViewer
{
	class MouseCameraEvent
		:	public Castor3D::FrameEvent
	{
	protected:
		Castor3D::SceneNodeWPtr m_pMaterial;
		RenderPanel * m_pPanel;
		Castor::real m_rDeltaX;
		Castor::real m_rDeltaY;
		Castor::real m_rDeltaZ;

	public:
		MouseCameraEvent( Castor3D::SceneNodeSPtr p_pMaterial, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~MouseCameraEvent();

		static void Add( MouseCameraEventSPtr p_pThis, Castor3D::FrameListenerSPtr p_pListener, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
	};
}

#endif
