#ifndef ___CS_MouseEvent___
#define ___CS_MouseEvent___

#include "RenderPanel.h"

namespace CastorShape
{
	class MouseCameraEvent : public Castor3D::FrameEvent
	{
	protected:
		Castor3D::CameraPtr m_camera;
		RenderPanel * m_panel;
		real m_deltaX;
		real m_deltaY;
		real m_deltaZ;

	public:
		MouseCameraEvent( Castor3D::CameraPtr p_camera, real p_deltaX, real p_deltaY, real p_deltaZ);
		virtual ~MouseCameraEvent();

		static void Add( Castor3D::FrameEventPtr p_pThis, Castor3D::FrameListenerPtr p_pListener, real p_deltaX, real p_deltaY, real p_deltaZ);
	};

	class CameraRotateEvent : public MouseCameraEvent
	{
	public:
		CameraRotateEvent( Castor3D::CameraPtr p_camera, real p_deltaX, real p_deltaY, real p_deltaZ);
		virtual ~CameraRotateEvent();

		virtual bool Apply();
	};

	class CameraTranslateEvent : public MouseCameraEvent
	{
	public:
		CameraTranslateEvent( Castor3D::CameraPtr p_camera, real p_deltaX, real p_deltaY, real p_deltaZ);
		virtual ~CameraTranslateEvent();

		virtual bool Apply();
	};
}

#endif