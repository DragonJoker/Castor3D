#include "FrameListener.hpp"
#include "FrameEvent.hpp"

using namespace Castor;

namespace Castor3D
{
	FrameListener::FrameListener()
	{
	}

	FrameListener::~FrameListener()
	{
		for ( auto && l_it : m_events )
		{
			l_it.clear();
		}
	}

	void FrameListener::Flush()
	{
		for ( auto && l_list : m_events )
		{
			l_list.clear();
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

		try
		{
			for ( auto && l_it = l_arrayEvents.begin(); l_it != l_arrayEvents.end() && l_bReturn; ++l_it )
			{
				l_bReturn = ( *l_it )->Apply();
			}
		}
		catch ( Exception & p_exc )
		{
			Logger::LogError( StringStream() << cuT( "Encountered exception while processing events: " ) << str_utils::from_str( p_exc.GetFullDescription() ) );
			l_bReturn = false;
		}
		catch ( std::exception & p_exc )
		{
			Logger::LogError( StringStream() << cuT( "Encountered exception while processing events: " ) << str_utils::from_str( p_exc.what() ) );
			l_bReturn = false;
		}
		catch ( ... )
		{
			Logger::LogError( StringStream() << cuT( "Encountered exception while processing events" ) );
			l_bReturn = false;
		}

		return l_bReturn;
	}
}
