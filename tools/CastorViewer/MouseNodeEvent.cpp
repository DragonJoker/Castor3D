#include "CastorViewer/MouseNodeEvent.hpp"

namespace CastorViewer
{
	MouseNodeEvent::MouseNodeEvent( castor3d::SceneNodeSPtr node
		, float dx
		, float dy
		, float dz )
		: CpuFrameEvent( castor3d::EventType::ePostRender )
		, m_node( node )
		, m_dx( dx )
		, m_dy( dy )
		, m_dz( dz )
	{
	}
}
