#include "TranslateNodeEvent.hpp"

#include <Scene/SceneNode.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	TranslateNodeEvent::TranslateNodeEvent( SceneNodeSPtr p_node, real p_dx, real p_dy, real p_dz )
		: MouseNodeEvent( p_node, p_dx, p_dy, p_dz )
	{
	}

	TranslateNodeEvent::~TranslateNodeEvent()
	{
	}

	bool TranslateNodeEvent::Apply()
	{
		SceneNodeSPtr l_node = m_node.lock();

		if ( l_node )
		{
			Quaternion l_orientation = l_node->GetOrientation();
			Point3r l_right, l_up, l_front;
			l_orientation.to_axes( l_right, l_up, l_front );
			l_node->Translate( ( l_right * m_dx ) + ( l_up * m_dy ) + ( l_front * m_dz ) );
		}

		m_dx = 0;
		m_dy = 0;
		m_dz = 0;
		return true;
	}
}
