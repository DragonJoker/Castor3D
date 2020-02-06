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
		for ( auto & l_it : m_events )
		{
			l_it.clear();
		}
	}

	void FrameListener::Flush()
	{
		for ( auto & l_list : m_events )
		{
			l_list.clear();
		}

		DoFlush();
	}

	void FrameListener::PostEvent( FrameEventUPtr && p_event )
	{
		auto l_lock = Castor::make_unique_lock( m_mutex );
		m_events[size_t( p_event->GetType() )].push_back( std::move( p_event ) );
	}

	bool FrameListener::FireEvents( EventType p_type )
	{
		m_mutex.lock();
		FrameEventPtrArray l_arrayEvents;
		std::swap( l_arrayEvents, m_events[size_t( p_type )] );
		m_mutex.unlock();
		bool l_return = true;

		try
		{
			for ( auto & l_event : l_arrayEvents )
			{
				if ( l_return )
				{
					l_return = l_event->Apply();
				}
			}
		}
		catch ( Exception & p_exc )
		{
			Logger::LogError( StringStream() << cuT( "Encountered exception while processing events: " ) << string::string_cast< xchar >( p_exc.GetFullDescription() ) );
			l_return = false;
		}
		catch ( std::exception & p_exc )
		{
			Logger::LogError( StringStream() << cuT( "Encountered exception while processing events: " ) << string::string_cast< xchar >( p_exc.what() ) );
			l_return = false;
		}
		catch ( ... )
		{
			Logger::LogError( StringStream() << cuT( "Encountered exception while processing events" ) );
			l_return = false;
		}

		return l_return;
	}

	void FrameListener::FlushEvents( EventType p_type )
	{
		m_mutex.lock();
		FrameEventPtrArray l_arrayEvents;
		std::swap( l_arrayEvents, m_events[size_t( p_type )] );
		m_mutex.unlock();
	}
}
