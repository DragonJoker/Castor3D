#include "FrameListener.hpp"
#include "FrameEvent.hpp"

using namespace Castor;

namespace Castor3D
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

	void FrameListener::Flush()
	{
		for ( auto & list : m_events )
		{
			list.clear();
		}

		DoFlush();
	}

	void FrameListener::PostEvent( FrameEventUPtr && p_event )
	{
		auto lock = Castor::make_unique_lock( m_mutex );
		m_events[size_t( p_event->GetType() )].push_back( std::move( p_event ) );
	}

	bool FrameListener::FireEvents( EventType p_type )
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
					result = event->Apply();
				}
			}
		}
		catch ( Exception & p_exc )
		{
			Logger::LogError( StringStream() << cuT( "Encountered exception while processing events: " ) << string::string_cast< xchar >( p_exc.GetFullDescription() ) );
			result = false;
		}
		catch ( std::exception & p_exc )
		{
			Logger::LogError( StringStream() << cuT( "Encountered exception while processing events: " ) << string::string_cast< xchar >( p_exc.what() ) );
			result = false;
		}
		catch ( ... )
		{
			Logger::LogError( StringStream() << cuT( "Encountered exception while processing events" ) );
			result = false;
		}

		return result;
	}

	void FrameListener::FlushEvents( EventType p_type )
	{
		m_mutex.lock();
		FrameEventPtrArray arrayEvents;
		std::swap( arrayEvents, m_events[size_t( p_type )] );
		m_mutex.unlock();
	}
}
