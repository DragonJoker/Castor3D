#include "RotateNodeEvent.hpp"

#include <Scene/SceneNode.hpp>
#include <Angle.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	RotateNodeEvent::RotateNodeEvent( SceneNodeSPtr p_node, real p_dx, real p_dy, real p_dz )
		: MouseNodeEvent( p_node, p_dx, p_dy, p_dz )
	{
	}

	RotateNodeEvent::~RotateNodeEvent()
	{
	}

	bool RotateNodeEvent::Apply()
	{
		SceneNodeSPtr l_node = m_node.lock();

		if ( l_node )
		{
			l_node->Rotate( Quaternion( Angle::from_degrees( m_dx ), Angle::from_degrees( m_dy ), Angle::from_degrees( m_dz ) ) );
		}

		m_dx = 0;
		m_dy = 0;
		m_dz = 0;
		return true;
	}
}
