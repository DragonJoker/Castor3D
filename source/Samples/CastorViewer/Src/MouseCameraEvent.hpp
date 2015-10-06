#ifndef ___CV_MOUSE_CAMERA_EVENT_H___
#define ___CV_MOUSE_CAMERA_EVENT_H___

#include "RenderPanel.hpp"

#include <FrameEvent.hpp>

namespace CastorViewer
{
	class MouseCameraEvent
		: public Castor3D::FrameEvent
	{
	public:
		MouseCameraEvent( Castor3D::SceneNodeSPtr p_node, Castor::real p_dx, Castor::real p_dy, Castor::real p_dz );
		virtual ~MouseCameraEvent();

		static void Add( MouseCameraEventSPtr p_this, Castor3D::FrameListenerSPtr p_listener, Castor::real p_dx, Castor::real p_dy, Castor::real p_dz );

	protected:
		Castor3D::SceneNodeWPtr m_node;
		RenderPanel * m_panel;
		Castor::real m_dx;
		Castor::real m_dy;
		Castor::real m_dz;
	};
}

#endif
