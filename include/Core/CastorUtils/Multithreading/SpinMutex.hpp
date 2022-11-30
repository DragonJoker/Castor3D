/*
See LICENSE file in root folder
*/
#ifndef ___CU_SpinMutex_H___
#define ___CU_SpinMutex_H___

#include "CastorUtils/Multithreading/MultithreadingModule.hpp"

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <atomic>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor
{
	class SpinMutex
	{
	public:
		CU_API void lock()noexcept;
		CU_API bool try_lock()noexcept;
		CU_API void unlock()noexcept;

	private:
		std::atomic_bool m_lock{ false };
	};
}

#endif
