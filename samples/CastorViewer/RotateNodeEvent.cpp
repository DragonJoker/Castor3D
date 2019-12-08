#include "CastorViewer/RotateNodeEvent.hpp"

#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Math/Angle.hpp>

using namespace castor3d;
using namespace castor;

namespace CastorViewer
{
	RotateNodeEvent::RotateNodeEvent( SceneNodeSPtr p_node, float p_dx, float p_dy, float p_dz )
		: MouseNodeEvent( p_node, p_dx, p_dy, p_dz )
	{
	}

	RotateNodeEvent::~RotateNodeEvent()
	{
	}

	bool RotateNodeEvent::apply()
	{
		SceneNodeSPtr node = m_node.lock();

		if ( node && ( m_dx || m_dy || m_dz ) )
		{
			Quaternion x{ Quaternion::fromAxisAngle( castor::Point3f{ 1.0, 0.0, 0.0 }, Angle::fromDegrees( m_dx ) ) };
			Quaternion y{ Quaternion::fromAxisAngle( castor::Point3f{ 0.0, 1.0, 0.0 }, Angle::fromDegrees( m_dy ) ) };
			Quaternion z{ Quaternion::fromAxisAngle( castor::Point3f{ 0.0, 0.0, 1.0 }, Angle::fromDegrees( m_dz ) ) };
			node->rotate( x );
			node->rotate( y );
			node->rotate( z );
		}

		m_dx = 0;
		m_dy = 0;
		m_dz = 0;
		return true;
	}
}
