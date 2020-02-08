#include "CastorUtils/Multithreading/ThreadPool.hpp"

#include "CastorUtils/Config/MultiThreadConfig.hpp"

namespace castor
{
	namespace
	{
		using LockType = std::unique_lock< std::mutex >;
	}

	ThreadPool::ThreadPool( size_t count )
		: m_count{ count }
	{
		LockType lock{ makeUniqueLock( m_mutex ) };
		m_available.reserve( count );
		m_busy.reserve( count );

		for ( size_t i = 0u; i < count; ++i )
		{
			m_available.push_back( std::make_unique< WorkerThread >() );
			m_endConnections.push_back( m_available.back()->onEnded.connect( [this]( WorkerThread & worker )
				{
					doFreeWorker( worker );
				} ) );
		}
	}

	ThreadPool::~ThreadPool()noexcept
	{
		waitAll( Milliseconds( 0xFFFFFFFF ) );
		LockType lock{ makeUniqueLock( m_mutex ) };
		m_endConnections.clear();
		m_busy.clear();
		m_available.clear();
	}

	bool ThreadPool::isEmpty()const
	{
		LockType lock{ makeUniqueLock( m_mutex ) };
		return m_available.empty();
	}

	bool ThreadPool::isFull()const
	{
		LockType lock{ makeUniqueLock( m_mutex ) };
		return m_busy.empty();
	}

	bool ThreadPool::waitAll( Milliseconds const & timeout )const
	{
		bool result = isFull();

		if ( !result )
		{
			auto begin = std::chrono::high_resolution_clock::now();
			Milliseconds wait{ 0 };

			do
			{
				std::this_thread::sleep_for( Milliseconds( 1 ) );
				result = isFull();
				wait = std::chrono::duration_cast< Milliseconds >( std::chrono::high_resolution_clock::now() - begin );
			}
			while ( wait < timeout && !result );
		}

		return result;
	}

	void ThreadPool::pushJob( WorkerThread::Job p_job )
	{
		auto & worker = doReserveWorker();
		worker.feed( p_job );
	}

	WorkerThread & ThreadPool::doReserveWorker()
	{
		while ( isEmpty() )
		{
			std::this_thread::sleep_for( Milliseconds( 1 ) );
		}

		LockType lock{ makeUniqueLock( m_mutex ) };
		auto it = m_available.rbegin();
		auto & worker = **it;
		m_busy.push_back( std::move( *it ) );
		m_available.erase( ( ++it ).base() );
		return worker;
	}

	void ThreadPool::doFreeWorker( WorkerThread & p_worker )
	{
		LockType lock{ makeUniqueLock( m_mutex ) };
		auto it = std::find_if( m_busy.begin()
			, m_busy.end()
			, [&p_worker]( WorkerPtr const & worker )
			{
				return worker.get() == &p_worker;
			} );
		m_available.push_back( std::move( *it ) );
		m_busy.erase( it );
	}
}
