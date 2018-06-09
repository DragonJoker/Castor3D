#include "FrameListener.hpp"
#include "FrameEvent.hpp"

using namespace castor;

namespace castor3d
{
	FrameListener::FrameListener( String const & name )
		: Named( name )
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

	void FrameListener::postEvent( FrameEventUPtr && event )
	{
		auto lock = castor::makeUniqueLock( m_mutex );
		m_events[size_t( event->getType() )].push_back( std::move( event ) );
	}

	bool FrameListener::fireEvents( EventType type )
	{
		m_mutex.lock();
		FrameEventPtrArray arrayEvents;
		std::swap( arrayEvents, m_events[size_t( type )] );
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
		catch ( Exception & exc )
		{
			Logger::logError( castor::makeStringStream() << cuT( "Encountered exception while processing events: " ) << string::stringCast< xchar >( exc.getFullDescription() ) );
			result = false;
		}
		catch ( std::exception & exc )
		{
			Logger::logError( castor::makeStringStream() << cuT( "Encountered exception while processing events: " ) << string::stringCast< xchar >( exc.what() ) );
			result = false;
		}
		catch ( ... )
		{
			Logger::logError( castor::makeStringStream() << cuT( "Encountered exception while processing events" ) );
			result = false;
		}

		return result;
	}

	void FrameListener::flushEvents( EventType type )
	{
		m_mutex.lock();
		FrameEventPtrArray arrayEvents;
		std::swap( arrayEvents, m_events[size_t( type )] );
		m_mutex.unlock();
	}
}
