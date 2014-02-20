#ifndef ___CV_MouseEvent___
#define ___CV_MouseEvent___

#include "RenderPanel.hpp"

namespace CastorViewer
{
	class MouseCameraEvent : public Castor3D::FrameEvent
	{
	protected:
		Castor3D::SceneNode * m_pNode;
		RenderPanel * m_pPanel;
		Castor::real m_rDeltaX;
		Castor::real m_rDeltaY;
		Castor::real m_rDeltaZ;

	public:
		MouseCameraEvent( Castor3D::SceneNode * p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ);
		virtual ~MouseCameraEvent();
		
		static void Add( Castor3D::FrameEventSPtr p_pThis, Castor3D::FrameListenerSPtr p_pListener, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
	};

	class CameraRotateEvent : public MouseCameraEvent
	{
	public:
		CameraRotateEvent( Castor3D::SceneNodeSPtr p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ);
		CameraRotateEvent( Castor3D::SceneNode * p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ);
		virtual ~CameraRotateEvent();

		virtual bool Apply();
	};

	class CameraTranslateEvent : public MouseCameraEvent
	{
	public:
		CameraTranslateEvent( Castor3D::SceneNodeSPtr p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		CameraTranslateEvent( Castor3D::SceneNode * p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~CameraTranslateEvent();

		virtual bool Apply();
	};

	class FPCameraEvent : public MouseCameraEvent
	{
	protected:
		Castor3D::SceneNode * m_pNodeYaw;
		Castor3D::SceneNode * m_pNodePitch;
		Castor3D::SceneNode * m_pNodeRoll;
		bool m_bOK;

	public:
		FPCameraEvent( Castor3D::SceneNode * p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~FPCameraEvent();

	private:
		void DoInitialise();
	};

	class FPCameraRotateEvent : public FPCameraEvent
	{
	public:
		FPCameraRotateEvent( Castor3D::SceneNodeSPtr p_pNode, Castor::real p_rYaw, Castor::real p_rPitch );
		FPCameraRotateEvent( Castor3D::SceneNode * p_pNode, Castor::real p_rYaw, Castor::real p_rPitch );
		virtual ~FPCameraRotateEvent();

		virtual bool Apply();
	};

	class FPCameraTranslateEvent : public FPCameraEvent
	{
	public:
		FPCameraTranslateEvent( Castor3D::SceneNodeSPtr p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		FPCameraTranslateEvent( Castor3D::SceneNode * p_pNode, Castor::real p_rDeltaX, Castor::real p_rDeltaY, Castor::real p_rDeltaZ );
		virtual ~FPCameraTranslateEvent();

		virtual bool Apply();
	};

	class KeyboardEvent : public Castor3D::FrameEvent
	{
	private:
		Castor3D::RenderWindowWPtr m_pWindow;
		wxFrame * m_pMainFrame;

	public:
		KeyboardEvent( Castor3D::RenderWindowSPtr p_pWindow, wxFrame * p_pMainFrame );
		virtual ~KeyboardEvent();
		
		static void Add( Castor3D::FrameEventSPtr p_pThis, Castor3D::FrameListenerSPtr p_pListener );

		virtual bool Apply();
	};
}

#endif