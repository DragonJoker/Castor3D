#include "CastorViewer/TranslateNodeEvent.hpp"

#include <Castor3D/Scene/SceneNode.hpp>

namespace CastorViewer
{
	TranslateNodeEvent::TranslateNodeEvent( castor3d::SceneNodeSPtr node
		, float dx
		, float dy
		, float dz )
		: MouseNodeEvent( node, dx, dy, dz )
	{
	}

	void TranslateNodeEvent::doApply()
	{
		if ( m_node )
		{
			castor::Quaternion orientation = m_node->getOrientation();
			castor::Point3f right{ 1.0f, 0.0f, 0.0f };
			castor::Point3f up{ 0.0f, 1.0f, 0.0f };
			castor::Point3f front{ 0.0f, 0.0f, 1.0f };
			orientation.transform( right, right );
			orientation.transform( up, up );
			orientation.transform( front, front );
			m_node->translate( ( right * m_dx ) + ( up * m_dy ) + ( front * m_dz ) );
		}

		m_dx = 0;
		m_dy = 0;
		m_dz = 0;
	}
}
