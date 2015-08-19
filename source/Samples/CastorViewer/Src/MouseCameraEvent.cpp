#include "MouseCameraEvent.hpp"

#include <FrameListener.hpp>

using namespace Castor3D;
using namespace Castor;

namespace CastorViewer
{
	MouseCameraEvent::MouseCameraEvent( SceneNodeSPtr p_pNode, real p_rDeltaX, real p_rDeltaY, real p_rDeltaZ )
		: FrameEvent( eEVENT_TYPE_PRE_RENDER )
		, m_pNode( p_pNode )
		, m_rDeltaX( p_rDeltaX )
		, m_rDeltaY( p_rDeltaY )
		, m_rDeltaZ( p_rDeltaZ )
	{
	}

	MouseCameraEvent::~MouseCameraEvent()
	{
	}

	void MouseCameraEvent::Add( MouseCameraEventSPtr p_event, FrameListenerSPtr p_listener, real p_deltaX, real p_deltaY, real p_deltaZ )
	{
		bool l_add = p_event->m_rDeltaX == 0 && p_event->m_rDeltaY == 0 && p_event->m_rDeltaZ == 0;
		p_event->m_rDeltaX += p_deltaX;
		p_event->m_rDeltaY += p_deltaY;
		p_event->m_rDeltaZ += p_deltaZ;

		if ( l_add )
		{
			p_listener->PostEvent( p_event );
		}
	}
}
