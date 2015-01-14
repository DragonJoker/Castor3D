#include "CameraRotateEvent.hpp"

#include <SceneNode.hpp>
#include <Angle.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	CameraRotateEvent::CameraRotateEvent( SceneNodeSPtr p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
		:	MouseCameraEvent( p_pNode, p_rDeltaX, p_rDeltaY, p_rDeltaZ )
	{
	}

	CameraRotateEvent::~CameraRotateEvent()
	{
	}

	bool CameraRotateEvent::Apply()
	{
		m_pNode->Yaw( Angle::FromDegrees( m_rDeltaX ) );
		m_pNode->Roll( Angle::FromDegrees( m_rDeltaY ) );
		m_pNode->Pitch( Angle::FromDegrees( m_rDeltaZ ) );
		m_rDeltaX = 0;
		m_rDeltaY = 0;
		m_rDeltaZ = 0;
		return true;
	}
}
