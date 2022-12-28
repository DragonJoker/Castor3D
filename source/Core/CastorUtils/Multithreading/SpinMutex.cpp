/*
See LICENSE file in root folder
*/
#include "CastorUtils/Multithreading/SpinMutex.hpp"

#if defined( CU_CompilerMSVC )
#	include <Windows.h>
#endif

namespace castor
{
	void SpinMutex::lock()noexcept
	{
		for ( ;;)
		{
			if ( !m_lock.exchange( true, std::memory_order_acquire ) )
			{
				return;
			}

			while ( m_lock.load( std::memory_order_relaxed ) )
			{
#if defined( CU_CompilerMSVC )
				_mm_pause();
#elif defined( CU_PlatformAndroid )
#else
				__builtin_ia32_pause();
#endif
			}
		}
	}

	bool SpinMutex::try_lock()noexcept
	{
		return !m_lock.load( std::memory_order_relaxed )
			&& !m_lock.exchange( true, std::memory_order_acquire );
	}

	void SpinMutex::unlock()noexcept
	{
		m_lock.store( false, std::memory_order_release );
	}
}
