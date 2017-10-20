/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_MULTITHREAD_CONFIG_H___
#define ___CASTOR_MULTITHREAD_CONFIG_H___

#include "Macros.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>

namespace castor
{
	template< typename Lockable >
	std::unique_lock< Lockable > makeUniqueLock( Lockable & p_lockable )
	{
		return std::unique_lock< Lockable >( p_lockable );
	}
}

#endif
