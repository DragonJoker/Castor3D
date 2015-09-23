#include "CameraTranslateEvent.hpp"

#include <SceneNode.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	CameraTranslateEvent::CameraTranslateEvent( SceneNodeSPtr p_node, real p_dx, real p_dy, real p_dz )
		: MouseCameraEvent( p_node, p_dx, p_dy, p_dz )
	{
	}

	CameraTranslateEvent::~CameraTranslateEvent()
	{
	}

	bool CameraTranslateEvent::Apply()
	{
		SceneNodeSPtr l_node = m_node.lock();

		if ( l_node )
		{
			Quaternion l_orientation = l_node->GetOrientation();
			Point3r l_translation( m_dx, m_dy, m_dz );
			l_node->Translate( l_orientation.Transform( l_translation, l_translation ) );
		}

		m_dx = 0;
		m_dy = 0;
		m_dz = 0;
		return true;
	}
}
