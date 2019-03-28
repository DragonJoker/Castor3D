#include "CastorUtils/Miscellaneous/PreciseTimer.hpp"

#include "CastorUtils/CastorUtils.hpp"

namespace castor
{
	PreciseTimer::PreciseTimer()
	{
		m_savedTime = doGetElapsed();
	}

	PreciseTimer::~PreciseTimer()
	{
	}

	Nanoseconds PreciseTimer::getElapsed()
	{
		auto current = doGetElapsed();
		auto diff( current - m_savedTime );
		m_savedTime = current;
		return std::chrono::duration_cast< Nanoseconds >( diff );
	}

	PreciseTimer::clock::time_point PreciseTimer::doGetElapsed()const
	{
		return clock::now();
	}
}
