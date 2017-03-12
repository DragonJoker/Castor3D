#include "WorkerThread.hpp"

#include "Config/MultiThreadConfig.hpp"
#include "Exception/Assertion.hpp"

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

	bool WorkerThread::Wait( std::chrono::milliseconds const & p_timeout )const
	{
		bool l_return = IsEnded() && !m_terminate;

		if ( !l_return )
		{
			auto l_begin = std::chrono::high_resolution_clock::now();
			std::chrono::milliseconds l_wait{ 0 };

			do
			{
				std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
				l_return = IsEnded();
				l_wait = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::high_resolution_clock::now() - l_begin );
			}
			while ( l_wait < p_timeout && !l_return && !m_terminate );
		}

		return l_return;
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
				std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
			}
		}
	}
}
