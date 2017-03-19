#include "RotateNodeEvent.hpp"

#include <Scene/SceneNode.hpp>
#include <Math/Angle.hpp>

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

		if ( l_node && ( m_dx || m_dy || m_dz ) )
		{
			Quaternion l_x{ Point3r{ 1.0, 0.0, 0.0 }, Angle::from_degrees( m_dx ) };
			Quaternion l_y{ Point3r{ 0.0, 1.0, 0.0 }, Angle::from_degrees( m_dy ) };
			Quaternion l_z{ Point3r{ 0.0, 0.0, 1.0 }, Angle::from_degrees( m_dz ) };
			l_node->Rotate( l_x );
			l_node->Rotate( l_y );
			l_node->Rotate( l_z );
		}

		m_dx = 0;
		m_dy = 0;
		m_dz = 0;
		return true;
	}
}
