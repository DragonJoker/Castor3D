#include "CameraRotateEvent.hpp"

#include <SceneNode.hpp>
#include <Angle.hpp>

using namespace Castor3D;
using namespace Castor;
double g_cumul = 0;

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
			l_node->Rotate( Quaternion( Angle::FromDegrees( 0.0/*m_dy*/ ), Angle::FromDegrees( m_dx ), Angle::FromDegrees( 0.0/*m_dz*/ ) ) );
			g_cumul += m_dx;
			Logger::LogDebug( StringStream() << cuT( "Cumul: " ) << g_cumul << cuT( " - Dx: " ) << m_dx << cuT( " - Orientation: " ) << l_node->GetOrientation() );
		}

		m_dx = 0;
		m_dy = 0;
		m_dz = 0;
		return true;
	}
}
