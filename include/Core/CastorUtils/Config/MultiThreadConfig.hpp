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
	template< typename Lockable >
	std::unique_lock< Lockable > makeUniqueLock( Lockable & lockable )
	{
		return std::unique_lock< Lockable >( lockable );
	}

	template< typename MutexT >
	struct CheckedMutexT
	{
		void lock()const
		{
			assert( this->doCheckUnlocked() );
			m_mutex.lock();
		}

		void unlock()const
		{
			assert( this->doCheckLocked() );
			m_mutex.unlock();
		}

	private:
		bool doCheckUnlocked()const
		{
			auto lock = makeUniqueLock( this->m_lockedMtx );
			auto it = m_locked.emplace( std::this_thread::get_id(), false ).first;
			auto result = it->second;
			it->second = true;
			return !result;
		}

		bool doCheckLocked()const
		{
			auto lock = makeUniqueLock( this->m_lockedMtx );
			auto it = m_locked.find( std::this_thread::get_id() );
			assert( it != m_locked.end() );
			auto result = it->second;
			it->second = false;
			return result;
		}

	private:
		mutable std::map< std::thread::id, bool > m_locked;
		mutable MutexT m_lockedMtx;
		mutable MutexT m_mutex;
	};

	using CheckedMutex = CheckedMutexT< std::mutex >;
}

#endif
