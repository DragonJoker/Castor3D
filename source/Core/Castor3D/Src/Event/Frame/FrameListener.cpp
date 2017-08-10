#include "FrameListener.hpp"
#include "FrameEvent.hpp"

using namespace castor;

namespace castor3d
{
	FrameListener::FrameListener( String const & p_name )
		: Named( p_name )
	{
	}

	FrameListener::~FrameListener()
	{
		for ( auto & it : m_events )
		{
			it.clear();
		}
	}

	void FrameListener::flush()
	{
		for ( auto & list : m_events )
		{
			list.clear();
		}

		doFlush();
	}

	void FrameListener::postEvent( FrameEventUPtr && p_event )
	{
		auto lock = castor::makeUniqueLock( m_mutex );
		m_events[size_t( p_event->getType() )].push_back( std::move( p_event ) );
	}

	bool FrameListener::fireEvents( EventType p_type )
	{
		m_mutex.lock();
		FrameEventPtrArray arrayEvents;
		std::swap( arrayEvents, m_events[size_t( p_type )] );
		m_mutex.unlock();
		bool result = true;

		try
		{
			for ( auto & event : arrayEvents )
			{
				if ( result )
				{
					result = event->apply();
				}
			}
		}
		catch ( Exception & p_exc )
		{
			Logger::logError( StringStream() << cuT( "Encountered exception while processing events: " ) << string::stringCast< xchar >( p_exc.getFullDescription() ) );
			result = false;
		}
		catch ( std::exception & p_exc )
		{
			Logger::logError( StringStream() << cuT( "Encountered exception while processing events: " ) << string::stringCast< xchar >( p_exc.what() ) );
			result = false;
		}
		catch ( ... )
		{
			Logger::logError( StringStream() << cuT( "Encountered exception while processing events" ) );
			result = false;
		}

		return result;
	}

	void FrameListener::flushEvents( EventType p_type )
	{
		m_mutex.lock();
		FrameEventPtrArray arrayEvents;
		std::swap( arrayEvents, m_events[size_t( p_type )] );
		m_mutex.unlock();
	}
}
