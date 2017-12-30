/* See LICENSE file in root folder */
#ifndef ___CS_MOUSE_EVENT_H___
#define ___CS_MOUSE_EVENT_H___

#include "RenderPanel.hpp"

#include <Event/Frame/FrameEvent.hpp>

namespace CastorShape
{
	class MouseCameraEvent
		: public Castor3D::FrameEvent
	{
	protected:
		Castor3D::SceneNode * m_pNode;
		RenderPanel * m_panel;
		Castor::real m_rDeltaX;
		Castor::real m_rDeltaY;
		Castor::real m_rDeltaZ;

	public:
		MouseCameraEvent( Castor3D::SceneNode * p_node, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~MouseCameraEvent();

		static void Add( Castor3D::FrameEventUPtr && p_pThis, Castor3D::FrameListenerSPtr p_pListener, Castor::real p_deltaX, Castor::real p_deltaY, Castor::real p_deltaZ );
	};

	class CameraRotateEvent : public MouseCameraEvent
	{
	public:
		CameraRotateEvent( Castor3D::SceneNodeSPtr p_node, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		CameraRotateEvent( Castor3D::SceneNode * p_node, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~CameraRotateEvent();

		virtual bool Apply();
	};

	class CameraTranslateEvent : public MouseCameraEvent
	{
	public:
		CameraTranslateEvent( Castor3D::SceneNodeSPtr p_node, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		CameraTranslateEvent( Castor3D::SceneNode * p_node, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~CameraTranslateEvent();

		virtual bool Apply();
	};
}

#endif
