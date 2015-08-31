#include "CameraRotateEvent.hpp"

#include <SceneNode.hpp>
#include <Angle.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	CameraRotateEvent::CameraRotateEvent( SceneNodeSPtr p_pMaterial, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
		: MouseCameraEvent( p_pMaterial, p_rDeltaX, p_rDeltaY, p_rDeltaZ )
	{
	}

	CameraRotateEvent::~CameraRotateEvent()
	{
	}

	bool CameraRotateEvent::Apply()
	{
		SceneNodeSPtr l_node = m_pMaterial.lock();

		if ( l_node )
		{
			l_node->Yaw( Angle::FromDegrees( m_rDeltaX ) );
			l_node->Roll( Angle::FromDegrees( m_rDeltaY ) );
			l_node->Pitch( Angle::FromDegrees( m_rDeltaZ ) );
		}

		m_rDeltaX = 0;
		m_rDeltaY = 0;
		m_rDeltaZ = 0;
		return true;
	}
}
