#include "WorkerThread.hpp"

#include "Config/MultiThreadConfig.hpp"

namespace castor
{
	WorkerThread::WorkerThread()
		: m_start{ false }
		, m_terminate{ false }
	{
		m_thread = std::make_unique< std::thread >( [this]()
			{
				doRun();
			} );
	}

	WorkerThread::~WorkerThread()noexcept
	{
		m_terminate = true;
		m_thread->join();
		m_thread.reset();
	}

	void WorkerThread::feed( Job p_job )
	{
		CU_Require( m_start == false );
		{
			m_currentJob = p_job;
		}
		m_start = true;
	}

	bool WorkerThread::isEnded()const
	{
		return !m_start;
	}

	bool WorkerThread::wait( Milliseconds const & p_timeout )const
	{
		bool result = isEnded() && !m_terminate;

		if ( !result )
		{
			auto begin = std::chrono::high_resolution_clock::now();
			Milliseconds wait{ 0 };

			do
			{
				std::this_thread::sleep_for( Milliseconds( 1 ) );
				result = isEnded();
				wait = std::chrono::duration_cast< Milliseconds >( std::chrono::high_resolution_clock::now() - begin );
			}
			while ( wait < p_timeout && !result && !m_terminate );
		}

		return result;
	}

	void WorkerThread::doRun()
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
