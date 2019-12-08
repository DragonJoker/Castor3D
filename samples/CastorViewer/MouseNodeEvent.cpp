#include "CastorViewer/MouseNodeEvent.hpp"

using namespace castor3d;
using namespace castor;

namespace CastorViewer
{
	MouseNodeEvent::MouseNodeEvent( SceneNodeSPtr p_node, float p_dx, float p_dy, float p_dz )
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
}
