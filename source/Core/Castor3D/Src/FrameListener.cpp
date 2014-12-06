#include "FrameListener.hpp"
#include "FrameEvent.hpp"

namespace Castor3D
{
	FrameListener::FrameListener()
	{
	}

	FrameListener::~FrameListener()
	{
		for ( std::array< FrameEventPtrArray, eEVENT_TYPE_COUNT >::iterator l_it = m_events.begin(); l_it != m_events.end(); ++l_it )
		{
			l_it->clear();
		}
	}

	void FrameListener::PostEvent( FrameEventSPtr p_event )
	{
		CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
		m_events[p_event->GetType()].push_back( p_event );
	}

	bool FrameListener::FireEvents( eEVENT_TYPE p_type )
	{
		m_mutex.lock();
		FrameEventPtrArray l_arrayEvents;
		std::swap( l_arrayEvents, m_events[p_type] );
		m_mutex.unlock();
		bool l_bReturn = true;

		for ( FrameEventPtrArrayIt l_it = l_arrayEvents.begin(); l_it != l_arrayEvents.end() && l_bReturn; ++l_it )
		{
			l_bReturn = ( *l_it )->Apply();
		}

		return l_bReturn;
	}
}
