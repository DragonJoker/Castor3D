#include "CastorViewer/TranslateNodeEvent.hpp"

#include <Castor3D/Scene/SceneNode.hpp>

using namespace castor3d;
using namespace castor;

namespace CastorViewer
{
	TranslateNodeEvent::TranslateNodeEvent( SceneNodeSPtr p_node, float p_dx, float p_dy, float p_dz )
		: MouseNodeEvent( p_node, p_dx, p_dy, p_dz )
	{
	}

	TranslateNodeEvent::~TranslateNodeEvent()
	{
	}

	void TranslateNodeEvent::apply()
	{
		SceneNodeSPtr node = m_node.lock();

		if ( node )
		{
			Quaternion orientation = node->getOrientation();
			castor::Point3f right{ 1.0f, 0.0f, 0.0f };
			castor::Point3f up{ 0.0f, 1.0f, 0.0f };
			castor::Point3f front{ 0.0f, 0.0f, 1.0f };
			orientation.transform( right, right );
			orientation.transform( up, up );
			orientation.transform( front, front );
			node->translate( ( right * m_dx ) + ( up * m_dy ) + ( front * m_dz ) );
		}

		m_dx = 0;
		m_dy = 0;
		m_dz = 0;
	}
}
