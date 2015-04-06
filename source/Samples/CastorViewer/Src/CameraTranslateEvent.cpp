#include "CameraTranslateEvent.hpp"

#include <SceneNode.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	CameraTranslateEvent::CameraTranslateEvent( SceneNodeSPtr p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
		:	MouseCameraEvent( p_pNode, p_rDeltaX, p_rDeltaY, p_rDeltaZ )
	{
	}

	CameraTranslateEvent::~CameraTranslateEvent()
	{
	}

	bool CameraTranslateEvent::Apply()
	{
		SceneNodeSPtr l_node = m_pNode.lock();

		if ( l_node )
		{
			l_node->Translate( Point3r( m_rDeltaX, m_rDeltaY, m_rDeltaZ ) );
		}

		m_rDeltaX = 0;
		m_rDeltaY = 0;
		m_rDeltaZ = 0;
		return true;
	}
}
