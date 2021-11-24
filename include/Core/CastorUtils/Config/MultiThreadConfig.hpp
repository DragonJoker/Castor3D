/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_MULTITHREAD_CONFIG_H___
#define ___CASTOR_MULTITHREAD_CONFIG_H___

#include "CastorUtils/Config/Macros.hpp"

#pragma warning( push )
#pragma warning( disable:4365 )
#include <cassert>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#pragma warning( pop )

namespace castor
{
	template< typename MutexT >
	struct CheckedMutexT
	{
		void lock()const
		{
			assert( !m_locked.exchange( true ) );
			m_mutex.lock();
		}

		void unlock()const
		{
			assert( m_locked.exchange( false ) );
			m_mutex.unlock();
		}

	private:
		mutable std::atomic_bool m_locked;
		mutable MutexT m_mutex;
	};

	using CheckedMutex = CheckedMutexT< std::mutex >;

	template< typename Lockable >
	std::unique_lock< Lockable > makeUniqueLock( Lockable & lockable )
	{
		return std::unique_lock< Lockable >( lockable );
	}
}

#endif
