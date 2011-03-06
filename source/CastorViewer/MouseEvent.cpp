#include "CastorViewer/PrecompiledHeader.h"

#include "CastorViewer/MouseEvent.h"

using namespace CastorViewer;
using namespace Castor3D;

//********************************************************************************************

MouseCameraEvent :: MouseCameraEvent( CameraPtr p_camera, real p_deltaX, real p_deltaY, real p_deltaZ)
	:	FrameEvent( FrameEvent::ePreRender),
		m_camera( p_camera),
		m_deltaX( p_deltaX),
		m_deltaY( p_deltaY),
		m_deltaZ( p_deltaZ)
{
}

MouseCameraEvent :: ~MouseCameraEvent()
{
}

void MouseCameraEvent :: Add( FrameEventPtr p_pThis, FrameListenerPtr p_pListener, real p_deltaX, real p_deltaY, real p_deltaZ)
{
	shared_ptr<MouseCameraEvent> l_pThis = static_pointer_cast<MouseCameraEvent, FrameEvent>( p_pThis);
	bool l_bToAdd = l_pThis->m_deltaX == 0 && l_pThis->m_deltaY == 0 && l_pThis->m_deltaZ == 0;

	l_pThis->m_deltaX += p_deltaX;
	l_pThis->m_deltaY += p_deltaY;
	l_pThis->m_deltaZ += p_deltaZ;

	if (l_bToAdd)
	{
		p_pListener->PostEvent( p_pThis);
	}
}

//********************************************************************************************

CameraRotateEvent :: CameraRotateEvent( CameraPtr p_camera, real p_deltaX, real p_deltaY, real p_deltaZ)
	:	MouseCameraEvent( p_camera, p_deltaX, p_deltaY, p_deltaZ)
{
}

CameraRotateEvent :: ~CameraRotateEvent()
{
}

bool CameraRotateEvent :: Apply()
{
	m_camera->GetParent()->Yaw( m_deltaX);
	m_camera->GetParent()->Pitch( m_deltaY);
	m_camera->GetParent()->Roll( m_deltaZ);
	m_deltaX = 0;
	m_deltaY = 0;
	m_deltaZ = 0;
	return true;
}

//********************************************************************************************

CameraTranslateEvent :: CameraTranslateEvent( CameraPtr p_camera, real p_deltaX, real p_deltaY, real p_deltaZ)
	:	MouseCameraEvent( p_camera, p_deltaX, p_deltaY, p_deltaZ)
{
}

CameraTranslateEvent :: ~CameraTranslateEvent()
{
}

bool CameraTranslateEvent :: Apply()
{
	m_camera->GetParent()->Translate( Point3r( m_deltaX, m_deltaY, m_deltaZ));
	m_deltaX = 0;
	m_deltaY = 0;
	m_deltaZ = 0;
	return true;
}

//********************************************************************************************
