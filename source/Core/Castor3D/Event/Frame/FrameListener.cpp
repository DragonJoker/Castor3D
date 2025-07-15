#include "Castor3D/Event/Frame/FrameListener.hpp"

#include "Castor3D/Event/Frame/CpuFrameEvent.hpp"
#include "Castor3D/Event/Frame/GpuFrameEvent.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"

namespace c3d
{
	namespace frmevtlstr
	{
		template< typename EventT, typename ... ParamsT >
		static bool doFireEvents( Vector< UniquePtr< EventT > > & arrayEvents
			, ParamsT & ... params )
		{
			bool result = true;

			try
			{
				for ( auto & event : arrayEvents )
				{
					event->apply( params... );
				}
			}
			catch ( Exception & exc )
			{
				log::error << cuT( "Encountered exception while processing events: " ) << makeString( exc.getFullDescription() ) << std::endl;
				result = false;
			}
			catch ( std::exception & exc )
			{
				log::error << cuT( "Encountered exception while processing events: " ) << makeString( exc.what() ) << std::endl;
				result = false;
			}
			catch ( ... )
			{
				log::error << cuT( "Encountered exception while processing events" ) << std::endl;
				result = false;
			}

			return result;
		}
	}

	FrameListener::FrameListener( String const & name )
		: Named( name )
	{
	}

	FrameListener::~FrameListener()noexcept
	{
		for ( auto & list : m_cpuEvents )
		{
			list.clear();
		}

		for ( auto & list : m_gpuEvents )
		{
			list.clear();
		}
	}

	void FrameListener::flush()
	{
		for ( auto & list : m_cpuEvents )
		{
			list.clear();
		}
		
		for ( auto & list : m_gpuEvents )
		{
			list.clear();
		}

		doFlush();
	}

	CpuFrameEvent * FrameListener::postEvent( CpuFrameEventUPtr event )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		auto result = event.get();
		m_cpuEvents[size_t( event->getType() )].push_back( c3d::move( event ) );
		return result;
	}

	GpuFrameEvent * FrameListener::postEvent( GpuFrameEventUPtr event )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		auto result = event.get();
		m_gpuEvents[size_t( event->getType() )].push_back( c3d::move( event ) );
		return result;
	}

	bool FrameListener::fireEvents( CpuEventType type )
	{
		CpuFrameEventPtrArray arrayEvents;
		{
			auto lock( makeUniqueLock( m_mutex ) );
			c3d::swap( arrayEvents, m_cpuEvents[size_t( type )] );
		}
		return frmevtlstr::doFireEvents( arrayEvents );
	}

	bool FrameListener::fireEvents( GpuEventType type
		, RenderDevice const & device
		, QueueData const & queueData )
	{
		GpuFrameEventPtrArray arrayEvents;
		{
			auto lock( makeUniqueLock( m_mutex ) );
			c3d::swap( arrayEvents, m_gpuEvents[size_t( type )] );
		}
		return frmevtlstr::doFireEvents( arrayEvents, device, queueData );
	}

	void FrameListener::flushEvents( CpuEventType type )
	{
		CpuFrameEventPtrArray cpuEvents;
		auto lock( makeUniqueLock( m_mutex ) );
		c3d::swap( cpuEvents, m_cpuEvents[size_t( type )] );
	}

	void FrameListener::flushEvents( GpuEventType type )
	{
		GpuFrameEventPtrArray gpuEvents;
		auto lock( makeUniqueLock( m_mutex ) );
		c3d::swap( gpuEvents, m_gpuEvents[size_t( type )] );
	}
}
