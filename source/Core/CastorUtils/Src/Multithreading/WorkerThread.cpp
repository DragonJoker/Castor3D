#include "WorkerThread.hpp"

#include "Config/MultiThreadConfig.hpp"

namespace Castor
{
	WorkerThread::WorkerThread()
		: m_start{ false }
		, m_terminate{ false }
	{
		m_thread = std::make_unique< std::thread >( std::bind( &WorkerThread::DoRun, this ) );
	}

	WorkerThread::~WorkerThread()noexcept
	{
		m_terminate = true;
		m_thread->join();
		m_thread.reset();
	}

	void WorkerThread::Feed( Job p_job )
	{
		REQUIRE( m_start == false );
		{
			m_currentJob = p_job;
		}
		m_start = true;
	}

	bool WorkerThread::IsEnded()const
	{
		return !m_start;
	}

	bool WorkerThread::Wait( Milliseconds const & p_timeout )const
	{
		bool result = IsEnded() && !m_terminate;

		if ( !result )
		{
			auto begin = std::chrono::high_resolution_clock::now();
			Milliseconds wait{ 0 };

			do
			{
				std::this_thread::sleep_for( Milliseconds( 1 ) );
				result = IsEnded();
				wait = std::chrono::duration_cast< Milliseconds >( std::chrono::high_resolution_clock::now() - begin );
			}
			while ( wait < p_timeout && !result && !m_terminate );
		}

		return result;
	}

	void WorkerThread::DoRun()
	{
		while ( !m_terminate )
		{
			if ( m_start )
			{
				m_currentJob();
				m_start = false;
				onEnded( *this );
			}
			else
			{
				std::this_thread::sleep_for( Milliseconds( 1 ) );
			}
		}
	}
}
