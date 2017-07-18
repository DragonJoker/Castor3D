#include "PreciseTimer.hpp"
#include "CastorUtils.hpp"

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
		auto current = DoGetTime();
		auto diff( current - m_savedTime );
		m_savedTime = current;
		return std::chrono::duration_cast< std::chrono::nanoseconds >( diff );
	}

	PreciseTimer::clock::time_point PreciseTimer::DoGetTime()const
	{
		return clock::now();
	}
}
