#include "MouseNodeEvent.hpp"

#include <Event/Frame/FrameListener.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	MouseNodeEvent::MouseNodeEvent( SceneNodeSPtr p_node, real p_dx, real p_dy, real p_dz )
		: FrameEvent( eEVENT_TYPE_PRE_RENDER )
		, m_node( p_node )
		, m_dx( p_dx )
		, m_dy( p_dy )
		, m_dz( p_dz )
	{
	}

	MouseNodeEvent::~MouseNodeEvent()
	{
	}

	void MouseNodeEvent::Add( MouseNodeEventSPtr p_event, FrameListenerSPtr p_listener, real p_dx, real p_dy, real p_dz )
	{
		bool l_add = p_event->m_dx == 0 && p_event->m_dy == 0 && p_event->m_dz == 0;
		p_event->m_dx += p_dx;
		p_event->m_dy += p_dy;
		p_event->m_dz += p_dz;

		if ( l_add )
		{
			p_listener->PostEvent( p_event );
		}
	}
}
