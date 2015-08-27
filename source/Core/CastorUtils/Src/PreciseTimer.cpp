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

	double PreciseTimer::TimeS()
	{
		auto l_current = DoGetTime();
		auto l_diff( l_current - m_savedTime );
		m_savedTime = l_current;
		return std::chrono::duration_cast< std::chrono::milliseconds >( l_diff ).count() / 1000.0;
	}

	double PreciseTimer::TimeMs()
	{
		auto l_current = DoGetTime();
		auto l_diff( l_current - m_savedTime );
		m_savedTime = l_current;
		return std::chrono::duration_cast< std::chrono::microseconds >( l_diff ).count() / 1000.0;
	}

	PreciseTimer::clock::time_point PreciseTimer::DoGetTime()const
	{
		return clock::now();
	}
}
