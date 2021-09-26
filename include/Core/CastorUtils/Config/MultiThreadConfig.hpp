/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_MULTITHREAD_CONFIG_H___
#define ___CASTOR_MULTITHREAD_CONFIG_H___

#include "CastorUtils/Config/Macros.hpp"

#pragma warning( push )
#pragma warning( disable:4365 )
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
}

#endif
