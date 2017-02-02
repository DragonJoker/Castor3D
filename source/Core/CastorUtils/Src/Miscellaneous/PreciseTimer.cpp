#include "PreciseTimer.hpp"
#include "CastorUtils.hpp"

#include <chrono>

namespace Castor
{
	PreciseTimer::PreciseTimer()
	{
		m_savedTime = DoGetTime();
	}

	PreciseTimer::~PreciseTimer()
	{
	}

	std::chrono::nanoseconds PreciseTimer::Time()
	{
		auto l_current = DoGetTime();
		auto l_diff( l_current - m_savedTime );
		m_savedTime = l_current;
		return std::chrono::duration_cast< std::chrono::nanoseconds >( l_diff );
	}

	PreciseTimer::clock::time_point PreciseTimer::DoGetTime()const
	{
		return clock::now();
	}
}
