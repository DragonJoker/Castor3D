#include "Castor3D/Event/Frame/FrameListener.hpp"

#include "Castor3D/Event/Frame/CpuFrameEvent.hpp"
#include "Castor3D/Event/Frame/GpuFrameEvent.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"

namespace castor3d
{
	namespace frmevtlstr
	{
		template< typename EventT, typename ... ParamsT >
		static bool doFireEvents( std::vector< castor::UniquePtr< EventT > > & arrayEvents
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
				log::error << cuT( "Encountered exception while processing events: " ) << castor::string::stringCast< xchar >( exc.getFullDescription() ) << std::endl;
				result = false;
			}
			catch ( std::exception & exc )
			{
				log::error << cuT( "Encountered exception while processing events: " ) << castor::string::stringCast< xchar >( exc.what() ) << std::endl;
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

	FrameListener::~FrameListener()
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

	void FrameListener::postEvent( CpuFrameEventUPtr event )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		m_cpuEvents[size_t( event->getType() )].push_back( std::move( event ) );
	}

	void FrameListener::postEvent( GpuFrameEventUPtr event )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		m_gpuEvents[size_t( event->getType() )].push_back( std::move( event ) );
	}

	bool FrameListener::fireEvents( EventType type )
	{
		CpuFrameEventPtrArray arrayEvents;
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );
			std::swap( arrayEvents, m_cpuEvents[size_t( type )] );
		}
		return frmevtlstr::doFireEvents( arrayEvents );
	}

	bool FrameListener::fireEvents( EventType type
		, RenderDevice const & device
		, QueueData const & queueData )
	{
		GpuFrameEventPtrArray arrayEvents;
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );
			std::swap( arrayEvents, m_gpuEvents[size_t( type )] );
		}
		return frmevtlstr::doFireEvents( arrayEvents, device, queueData );
	}

	void FrameListener::flushEvents( EventType type )
	{
		GpuFrameEventPtrArray gpuEvents;
		CpuFrameEventPtrArray cpuEvents;

		auto lock( castor::makeUniqueLock( m_mutex ) );
		std::swap( gpuEvents, m_gpuEvents[size_t( type )] );
		std::swap( cpuEvents, m_cpuEvents[size_t( type )] );
	}
}
