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
		SceneNodeSPtr node = m_node.lock();

		if ( node && ( m_dx || m_dy || m_dz ) )
		{
			Quaternion x{ Quaternion::from_axis_angle( Point3r{ 1.0, 0.0, 0.0 }, Angle::from_degrees( m_dx ) ) };
			Quaternion y{ Quaternion::from_axis_angle( Point3r{ 0.0, 1.0, 0.0 }, Angle::from_degrees( m_dy ) ) };
			Quaternion z{ Quaternion::from_axis_angle( Point3r{ 0.0, 0.0, 1.0 }, Angle::from_degrees( m_dz ) ) };
			node->Rotate( x );
			node->Rotate( y );
			node->Rotate( z );
		}

		m_dx = 0;
		m_dy = 0;
		m_dz = 0;
		return true;
	}
}
