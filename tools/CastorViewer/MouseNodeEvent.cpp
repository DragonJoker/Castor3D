#include "CastorViewer/MouseNodeEvent.hpp"

namespace CastorViewer
{
	MouseNodeEvent::MouseNodeEvent( castor3d::SceneNodeRPtr node
		, float dx
		, float dy
		, float dz )
		: CpuFrameEvent( castor3d::CpuEventType::ePostCpuStep )
		, m_node( node )
		, m_dx( dx )
		, m_dy( dy )
		, m_dz( dz )
	{
	}
}
