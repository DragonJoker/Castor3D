#include "FirstPersonCameraRotateEvent.hpp"

#include <SceneNode.hpp>
#include <Angle.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	FirstPersonCameraRotateEvent::FirstPersonCameraRotateEvent( SceneNodeSPtr p_pNode, real p_rYaw, real p_rPitch )
		:	FirstPersonCameraEvent( p_pNode.get(), p_rYaw, p_rPitch, 0	)
	{
	}

	FirstPersonCameraRotateEvent::FirstPersonCameraRotateEvent( SceneNode * p_pNode, real p_rYaw, real p_rPitch )
		:	FirstPersonCameraEvent( p_pNode, p_rYaw, p_rPitch, 0 )
	{
	}

	FirstPersonCameraRotateEvent::~FirstPersonCameraRotateEvent()
	{
	}

	bool FirstPersonCameraRotateEvent::Apply()
	{
		if ( m_bOK )
		{
			// Yaws the camera according to the mouse relative movement.
			m_pNodeYaw->Yaw( Angle::FromDegrees( m_rDeltaX ) );
			real l_rPitch = m_pNodePitch->GetOrientation().GetPitch().Degrees();

			if ( l_rPitch + m_rDeltaY >= -90 && l_rPitch + m_rDeltaY <= 90 )
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
}
