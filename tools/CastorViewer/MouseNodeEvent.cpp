#include "CastorViewer/MouseNodeEvent.hpp"

namespace CastorViewer
{
	MouseNodeEvent::MouseNodeEvent( castor3d::SceneNodeSPtr p_node, float p_dx, float p_dy, float p_dz )
		: CpuFrameEvent( castor3d::EventType::ePostRender )
		, m_node( p_node )
		, m_dx( p_dx )
		, m_dy( p_dy )
		, m_dz( p_dz )
	{
	}
}
