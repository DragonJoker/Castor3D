#include "PreciseTimer.hpp"
#include "CastorUtils.hpp"

#if defined( _WIN32 )
#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#	endif
#	include <windows.h>
#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
#else
#	include <sys/time.h>
#endif

namespace Castor
{
	int64_t PreciseTimer::sm_i64Frequency = 0;

	PreciseTimer::PreciseTimer()
	{
		if ( sm_i64Frequency == 0 )
		{
#if defined( _WIN32 )
			LARGE_INTEGER l_liFrequency;
			::QueryPerformanceFrequency( & l_liFrequency );
			sm_i64Frequency = l_liFrequency.QuadPart;
#else
			sm_i64Frequency = 1000000;
#endif
		}

		m_i64PreviousTime = DoGetTime();
	}

	PreciseTimer::~PreciseTimer()
	{
	}

	double PreciseTimer::TimeS()
	{
		long long l_i64Current = DoGetTime();
		double l_dReturn = double( l_i64Current - m_i64PreviousTime ) / double( sm_i64Frequency );
		m_i64PreviousTime = l_i64Current;
		return l_dReturn;
	}

	double PreciseTimer::TimeMs()
	{
		long long l_i64Current = DoGetTime();
		double l_dReturn = 1000 * double( l_i64Current - m_i64PreviousTime ) / double( sm_i64Frequency );
		m_i64PreviousTime = l_i64Current;
		return l_dReturn;
	}

	double PreciseTimer::TimeUs()
	{
		long long l_i64Current = DoGetTime();
		double l_dReturn = 1000000 * double( l_i64Current - m_i64PreviousTime ) / double( sm_i64Frequency );
		m_i64PreviousTime = l_i64Current;
		return l_dReturn;
	}

	long long PreciseTimer::DoGetTime()const
	{
		long long l_i64Return;
#if defined( _WIN32 )
		LARGE_INTEGER l_liCurrentTime;
		::QueryPerformanceCounter( & l_liCurrentTime );
		l_i64Return = l_liCurrentTime.QuadPart;
#else
		timeval l_time;
		gettimeofday( & l_time, nullptr );
		l_i64Return = l_time.tv_sec * sm_i64Frequency + l_time.tv_usec;
#endif
		return l_i64Return;
	}
}
