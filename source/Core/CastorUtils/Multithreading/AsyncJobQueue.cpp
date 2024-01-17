#include "CastorUtils/Multithreading/AsyncJobQueue.hpp"

#include "CastorUtils/Config/MultiThreadConfig.hpp"

namespace castor
{
	AsyncJobQueue::AsyncJobQueue( size_t count )
		: m_pool{ count }
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
		if ( !m_ended )
		{
			m_pending.emplace_back( std::move( job ) );
		}
	}

	void AsyncJobQueue::waitAll()
	{
		while ( !doCheckEnded() )
		{
			std::this_thread::sleep_for( 5_ms );
		}

		m_pool.waitAll( Milliseconds{ Milliseconds::max() } );
	}

	void AsyncJobQueue::finish()
	{
		m_ended = true;
		m_pool.waitAll( Milliseconds{ Milliseconds::max() } );
	}

	void AsyncJobQueue::reset()
	{
		if ( m_ended )
		{
			m_worker.join();
			m_ended = false;
			m_worker = std::thread{ [this]() { doRun(); } };
		}
	}

	void AsyncJobQueue::doRun()
	{
		while ( !m_ended )
		{
			while ( !m_ended && doCheckEnded() )
			{
				std::this_thread::sleep_for( 5_ms );
			}

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

		// Should never happen, but let's do this just in case.
		static Job dummy = []()
		{
			std::this_thread::sleep_for( 5_ms );
		};
		return dummy;
	}

	bool AsyncJobQueue::doCheckEnded()
	{
		auto lock( makeUniqueLock( m_mutex ) );
		return m_pending.empty();
	}
}
