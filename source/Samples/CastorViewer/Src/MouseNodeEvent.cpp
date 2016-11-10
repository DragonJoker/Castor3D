#include "MouseNodeEvent.hpp"

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	MouseNodeEvent::MouseNodeEvent( SceneNodeSPtr p_node, real p_dx, real p_dy, real p_dz )
		: FrameEvent( EventType::ePostRender )
		, m_node( p_node )
		, m_dx( p_dx )
		, m_dy( p_dy )
		, m_dz( p_dz )
	{
	}

	MouseNodeEvent::~MouseNodeEvent()
	{
	}

	void MouseNodeEvent::Modify( real p_dx, real p_dy, real p_dz )
	{
		m_dx += p_dx;
		m_dy += p_dy;
		m_dz += p_dz;
	}
}
