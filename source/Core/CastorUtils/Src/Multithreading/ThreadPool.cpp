#include "ThreadPool.hpp"

#include "Config/MultiThreadConfig.hpp"

namespace Castor
{
	ThreadPool::ThreadPool( size_t p_count )
		: m_count{ p_count }
	{
		auto l_lock = make_unique_lock( m_mutex );
		m_available.reserve( p_count );
		m_busy.reserve( p_count );

		for ( size_t i = 0u; i < p_count; ++i )
		{
			m_available.push_back( std::make_unique< WorkerThread >() );
			m_endConnections.push_back( m_available.back()->onEnded.connect( [this]( WorkerThread & p_worker )
			{
				DoFreeWorker( p_worker );
			} ) );
		}
	}

	ThreadPool::~ThreadPool()noexcept
	{
		WaitAll( std::chrono::milliseconds( 0xFFFFFFFF ) );
		auto l_lock = make_unique_lock( m_mutex );
		m_endConnections.clear();
		m_busy.clear();
		m_available.clear();
	}

	bool ThreadPool::IsEmpty()const
	{
		auto l_lock = make_unique_lock( m_mutex );
		return m_available.empty();
	}

	bool ThreadPool::IsFull()const
	{
		auto l_lock = make_unique_lock( m_mutex );
		return m_busy.empty();
	}

	bool ThreadPool::WaitAll( std::chrono::milliseconds const & p_timeout )const
	{
		bool l_result = IsFull();

		if ( !l_result )
		{
			auto l_begin = std::chrono::high_resolution_clock::now();
			std::chrono::milliseconds l_wait{ 0 };

			do
			{
				std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
				l_result = IsFull();
				l_wait = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::high_resolution_clock::now() - l_begin );
			}
			while ( l_wait < p_timeout && !l_result );
		}

		return l_result;
	}

	void ThreadPool::PushJob( WorkerThread::Job p_job )
	{
		auto & l_worker = DoReserveWorker();
		l_worker.Feed( p_job );
	}

	WorkerThread & ThreadPool::DoReserveWorker()
	{
		while ( IsEmpty() )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
		}

		auto l_lock = make_unique_lock( m_mutex );
		auto l_it = m_available.rbegin();
		auto & l_worker = **l_it;
		m_busy.push_back( std::move( *l_it ) );
		m_available.erase( ( ++l_it ).base() );
		return l_worker;
	}

	void ThreadPool::DoFreeWorker( WorkerThread & p_worker )
	{
		auto l_lock = make_unique_lock( m_mutex );
		auto l_it = std::find_if( m_busy.begin(), m_busy.end(), [&p_worker]( WorkerPtr const & l_worker )
		{
			return l_worker.get() == &p_worker;
		} );
		m_available.push_back( std::move( *l_it ) );
		m_busy.erase( l_it );
	}
}
