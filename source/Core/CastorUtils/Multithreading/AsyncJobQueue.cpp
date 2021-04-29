#include "CastorUtils/Multithreading/AsyncJobQueue.hpp"

#include "CastorUtils/Config/MultiThreadConfig.hpp"

namespace castor
{
	AsyncJobQueue::AsyncJobQueue( size_t count )
		: m_ended{ false }
		, m_pool{ count }
		, m_worker{ [this](){ doRun(); } }
	{
	}

	AsyncJobQueue::~AsyncJobQueue()noexcept
	{
		m_ended = true;
		m_pool.waitAll( Milliseconds( 0xFFFFFFFFFFFFFFFF ) );
		m_worker.join();
	}

	void AsyncJobQueue::pushJob( Job job )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		m_pending.push_back( job );
	}

	void AsyncJobQueue::doRun()
	{
		while ( !m_ended )
		{
			m_pool.pushJob( doPopJob() );
		}
	}

	AsyncJobQueue::Job AsyncJobQueue::doPopJob()
	{
		auto lock( makeUniqueLock( m_mutex ) );

		if ( !m_pending.empty() )
		{
			auto job = m_pending.front();
			m_pending.erase( m_pending.begin() );
			return job;
		}

		static Job dummy = []()
		{
			std::this_thread::sleep_for( 5_ms );
		};
		return dummy;
	}
}
