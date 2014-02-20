#include "CastorViewer/PrecompiledHeader.hpp"

#include "CastorViewer/MouseEvent.hpp"

using namespace CastorViewer;
using namespace Castor3D;
using namespace Castor;

//********************************************************************************************

MouseCameraEvent :: MouseCameraEvent( SceneNode * p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ)
	:	FrameEvent	( eEVENT_TYPE_PRE_RENDER	)
	,	m_pNode		( p_pNode					)
	,	m_rDeltaX	( p_rDeltaX					)
	,	m_rDeltaY	( p_rDeltaY					)
	,	m_rDeltaZ	( p_rDeltaZ					)
{
}

MouseCameraEvent :: ~MouseCameraEvent()
{
}

void MouseCameraEvent :: Add( FrameEventSPtr p_pThis, FrameListenerSPtr p_pListener, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
{
	std::shared_ptr< MouseCameraEvent > l_pThis = std::static_pointer_cast< MouseCameraEvent >( p_pThis );
	bool l_bToAdd = l_pThis->m_rDeltaX == 0 && l_pThis->m_rDeltaY == 0 && l_pThis->m_rDeltaZ == 0;

	l_pThis->m_rDeltaX += p_rDeltaX;
	l_pThis->m_rDeltaY += p_rDeltaY;
	l_pThis->m_rDeltaZ += p_rDeltaZ;

	if( l_bToAdd )
	{
		p_pListener->PostEvent( p_pThis );
	}
}

//********************************************************************************************

CameraRotateEvent :: CameraRotateEvent( SceneNodeSPtr p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
	:	MouseCameraEvent( p_pNode.get(), p_rDeltaX, p_rDeltaY, p_rDeltaZ )
{
}

CameraRotateEvent :: CameraRotateEvent( SceneNode * p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
	:	MouseCameraEvent( p_pNode, p_rDeltaX, p_rDeltaY, p_rDeltaZ )
{
}

CameraRotateEvent :: ~CameraRotateEvent()
{
}

bool CameraRotateEvent :: Apply()
{
	m_pNode->Yaw(	Angle::FromDegrees( m_rDeltaX ) );
	m_pNode->Roll(	Angle::FromDegrees( m_rDeltaY ) );
	m_pNode->Pitch(	Angle::FromDegrees( m_rDeltaZ ) );
	m_rDeltaX = 0;
	m_rDeltaY = 0;
	m_rDeltaZ = 0;
	return true;
}

//********************************************************************************************

FPCameraEvent :: FPCameraEvent( SceneNode * p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
	:	MouseCameraEvent	( p_pNode, p_rDeltaX, p_rDeltaY, p_rDeltaZ	)
	,	m_pNodeRoll			( NULL										)
	,	m_pNodePitch		( NULL										)
	,	m_pNodeYaw			( NULL										)
	,	m_bOK				( false										)
{
	DoInitialise();
}

FPCameraEvent :: ~FPCameraEvent()
{
}

void FPCameraEvent :: DoInitialise()
{
	m_pNodeRoll = m_pNode;

	if( m_pNodeRoll )
	{
		m_pNodePitch = m_pNodeRoll->GetParent();
	}

	if( m_pNodePitch )
	{
		m_pNodeYaw = m_pNodePitch->GetParent();
	}

	if( m_pNodeYaw && m_pNodeYaw->GetParent() )
	{
		m_pNode = m_pNodeYaw->GetParent();
		m_bOK = true;
	}

	if( !m_bOK )
	{
		// Create the needed nodes ?
	}
}

//********************************************************************************************

FPCameraRotateEvent :: FPCameraRotateEvent( SceneNodeSPtr p_pNode, real p_rYaw, real p_rPitch )
	:	FPCameraEvent	( p_pNode.get(), p_rYaw, p_rPitch, 0	)
{
}

FPCameraRotateEvent :: FPCameraRotateEvent( SceneNode * p_pNode, real p_rYaw, real p_rPitch )
	:	FPCameraEvent	( p_pNode, p_rYaw, p_rPitch, 0 )
{
}

FPCameraRotateEvent :: ~FPCameraRotateEvent()
{
}

bool FPCameraRotateEvent :: Apply()
{
	if( m_bOK )
	{
		// Yaws the camera according to the mouse relative movement.
		m_pNodeYaw->Yaw( Angle::FromDegrees( m_rDeltaX ) );

		real l_rPitch = m_pNodePitch->GetOrientation().GetPitch().Degrees();
 
		if( l_rPitch + m_rDeltaY >= -90 && l_rPitch + m_rDeltaY <= 90 )
		{
			// Pitches the camera according to the mouse relative movement.
			m_pNodePitch->Pitch( Angle::FromDegrees( m_rDeltaY ) );
		}
	}

	m_rDeltaX = 0;
	m_rDeltaY = 0;
	m_rDeltaZ = 0;
	return true;
}

//********************************************************************************************

FPCameraTranslateEvent :: FPCameraTranslateEvent( SceneNodeSPtr p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
	:	FPCameraEvent	( p_pNode.get(), p_rDeltaX, p_rDeltaY, p_rDeltaZ	)
{
}

FPCameraTranslateEvent :: FPCameraTranslateEvent( SceneNode * p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
	:	FPCameraEvent	( p_pNode, p_rDeltaX, p_rDeltaY, p_rDeltaZ	)
{
}

FPCameraTranslateEvent :: ~FPCameraTranslateEvent()
{
}

bool FPCameraTranslateEvent :: Apply()
{
	if( m_bOK )
	{
		// Translates the camera according to the translate vector which is
		// controlled by the keyboard arrows.
		Point3r l_ptTranslate( m_rDeltaX, m_rDeltaY, m_rDeltaZ );
		m_pNodeRoll->GetDerivedOrientation().Transform( l_ptTranslate, l_ptTranslate );
		m_pNode->Translate( l_ptTranslate );
	}

	m_rDeltaX = 0;
	m_rDeltaY = 0;
	m_rDeltaZ = 0;
	return true;
}

//********************************************************************************************

CameraTranslateEvent :: CameraTranslateEvent( SceneNodeSPtr p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ)
	:	MouseCameraEvent( p_pNode.get(), p_rDeltaX, p_rDeltaY, p_rDeltaZ)
{
}

CameraTranslateEvent :: CameraTranslateEvent( SceneNode * p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ)
	:	MouseCameraEvent( p_pNode, p_rDeltaX, p_rDeltaY, p_rDeltaZ)
{
}

CameraTranslateEvent :: ~CameraTranslateEvent()
{
}

bool CameraTranslateEvent :: Apply()
{
	m_pNode->Translate( Point3r( m_rDeltaX, m_rDeltaY, m_rDeltaZ ) );
	m_rDeltaX = 0;
	m_rDeltaY = 0;
	m_rDeltaZ = 0;
	return true;
}

//********************************************************************************************

KeyboardEvent :: KeyboardEvent( RenderWindowSPtr p_pWindow, wxFrame * p_pMainFrame )
	:	FrameEvent		( eEVENT_TYPE_PRE_RENDER	)
	,	m_pWindow		( p_pWindow					)
	,	m_pMainFrame	( p_pMainFrame				)
{
}

KeyboardEvent :: ~KeyboardEvent()
{
}

bool KeyboardEvent :: Apply()
{
	RenderWindowSPtr l_pWindow = m_pWindow.lock();

	if( l_pWindow )
	{
		m_pMainFrame->ShowFullScreen( !l_pWindow->IsFullscreen(), wxFULLSCREEN_ALL );
		Logger::LogDebug( cuT( "Main frame switched fullscreen" ) );
		l_pWindow->UpdateFullScreen( !l_pWindow->IsFullscreen() );
		Logger::LogDebug( cuT( "Render window switched fullscreen" ) );
	}

	return true;
}

//********************************************************************************************
