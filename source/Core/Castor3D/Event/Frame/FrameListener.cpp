#include "Castor3D/Event/Frame/FrameListener.hpp"

#include "Castor3D/Event/Frame/CpuFrameEvent.hpp"
#include "Castor3D/Event/Frame/GpuFrameEvent.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"

namespace castor3d
{
	namespace frmevtlstr
	{
		template< typename EventT, typename ... ParamsT >
		static bool doFireEvents( castor::Vector< castor::UniquePtr< EventT > > & arrayEvents
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
			catch ( castor::Exception & exc )
			{
				log::error << cuT( "Encountered exception while processing events: " ) << castor::makeString( exc.getFullDescription() ) << std::endl;
				result = false;
			}
			catch ( std::exception & exc )
			{
				log::error << cuT( "Encountered exception while processing events: " ) << castor::makeString( exc.what() ) << std::endl;
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

	FrameListener::FrameListener( castor::String const & name )
		: castor::Named( name )
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
		auto lock( castor::makeUniqueLock( m_mutex ) );
		auto result = event.get();
		m_cpuEvents[size_t( event->getType() )].push_back( castor::move( event ) );
		return result;
	}

	GpuFrameEvent * FrameListener::postEvent( GpuFrameEventUPtr event )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		auto result = event.get();
		m_gpuEvents[size_t( event->getType() )].push_back( castor::move( event ) );
		return result;
	}

	bool FrameListener::fireEvents( CpuEventType type )
	{
		CpuFrameEventPtrArray arrayEvents;
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );
			castor::swap( arrayEvents, m_cpuEvents[size_t( type )] );
		}
		return frmevtlstr::doFireEvents( arrayEvents );
	}

	bool FrameListener::fireEvents( GpuEventType type
		, RenderDevice const & device
		, QueueData const & queueData )
	{
		GpuFrameEventPtrArray arrayEvents;
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );
			castor::swap( arrayEvents, m_gpuEvents[size_t( type )] );
		}
		return frmevtlstr::doFireEvents( arrayEvents, device, queueData );
	}

	void FrameListener::flushEvents( CpuEventType type )
	{
		CpuFrameEventPtrArray cpuEvents;
		auto lock( castor::makeUniqueLock( m_mutex ) );
		castor::swap( cpuEvents, m_cpuEvents[size_t( type )] );
	}

	void FrameListener::flushEvents( GpuEventType type )
	{
		GpuFrameEventPtrArray gpuEvents;
		auto lock( castor::makeUniqueLock( m_mutex ) );
		castor::swap( gpuEvents, m_gpuEvents[size_t( type )] );
	}
}
