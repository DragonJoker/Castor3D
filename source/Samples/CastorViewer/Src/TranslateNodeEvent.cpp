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
		SceneNodeSPtr node = m_node.lock();

		if ( node )
		{
			Quaternion orientation = node->GetOrientation();
			Point3r right{ 1.0_r, 0.0_r, 0.0_r };
			Point3r up{ 0.0_r, 1.0_r, 0.0_r };
			Point3r front{ 0.0_r, 0.0_r, 1.0_r };
			orientation.transform( right, right );
			orientation.transform( up, up );
			orientation.transform( front, front );
			node->Translate( ( right * m_dx ) + ( up * m_dy ) + ( front * m_dz ) );
		}

		m_dx = 0;
		m_dy = 0;
		m_dz = 0;
		return true;
	}
}
