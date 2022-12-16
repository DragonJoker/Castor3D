#include "CastorViewer/RotateNodeEvent.hpp"

#include <Castor3D/Scene/SceneNode.hpp>

#include <CastorUtils/Math/Angle.hpp>

namespace CastorViewer
{
	namespace rotevt
	{
		static bool isNonZero( float v )
		{
			return std::abs( v ) < std::numeric_limits< float >::epsilon();
		}
	}

	RotateNodeEvent::RotateNodeEvent( castor3d::SceneNodeSPtr node
		, float dx
		, float dy
		, float dz )
		: MouseNodeEvent( node, dx, dy, dz )
	{
	}

	void RotateNodeEvent::doApply()
	{
		castor3d::SceneNodeSPtr node = m_node.lock();

		if ( node && ( rotevt::isNonZero( m_dx ) || rotevt::isNonZero( m_dy ) || rotevt::isNonZero( m_dz ) ) )
		{
			auto x = castor::Quaternion::fromAxisAngle( castor::Point3f{ 1.0, 0.0, 0.0 }, castor::Angle::fromDegrees( m_dx ) );
			auto y = castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0, 1.0, 0.0 }, castor::Angle::fromDegrees( m_dy ) );
			auto z = castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0, 0.0, 1.0 }, castor::Angle::fromDegrees( m_dz ) );
			node->rotate( x );
			node->rotate( y );
			node->rotate( z );
		}

		m_dx = 0;
		m_dy = 0;
		m_dz = 0;
	}
}
