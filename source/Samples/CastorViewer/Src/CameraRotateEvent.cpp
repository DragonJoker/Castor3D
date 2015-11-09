#include "CameraRotateEvent.hpp"

#include <SceneNode.hpp>
#include <Angle.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	CameraRotateEvent::CameraRotateEvent( SceneNodeSPtr p_node, real p_dx, real p_dy, real p_dz )
		: MouseCameraEvent( p_node, p_dx, p_dy, p_dz )
	{
	}

	CameraRotateEvent::~CameraRotateEvent()
	{
	}

	bool CameraRotateEvent::Apply()
	{
		SceneNodeSPtr l_node = m_node.lock();

		if ( l_node )
		{
			StringStream l_stream;
			l_stream << cuT( "Rotate - Yaw Before: " ) << l_node->GetOrientation().GetYaw().Degrees() << cuT( ", Offset: " ) << m_dy;
			l_node->Rotate( Quaternion( Angle::FromDegrees( m_dx ), Angle::FromDegrees( m_dy ), Angle::FromDegrees( m_dz ) ) );
			//l_node->Rotate( Quaternion( Point3r( 0, 1, 0 ), Angle::FromDegrees( m_dy ) ) );
			l_stream << cuT( ", Yaw After: " ) << l_node->GetOrientation().GetYaw().Degrees() << std::endl;
			l_stream << cuT( ", Quaternion: " ) << l_node->GetOrientation();
			Logger::LogDebug( l_stream );
		}

		m_dx = 0;
		m_dy = 0;
		m_dz = 0;
		return true;
	}
}
