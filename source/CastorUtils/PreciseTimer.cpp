#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/PreciseTimer.hpp"

#include "CastorUtils/CastorUtils.hpp"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.hpp"
using namespace Castor::Utils;
#endif

#ifndef __GNUG__
#	include "CastorUtils/Utils.hpp"
#	include "CastorUtils/Exception.hpp"

using namespace Castor::Utils;

long long PreciseTimer :: sm_frequency = 0;
long long RepeatTimer :: sm_frequency = 0;

#else
#	include <sys/time.h>
//#	define nullptr 0

using namespace Castor::Utils;

long long PreciseTimer :: sm_frequency = 1000000;
long long RepeatTimer :: sm_frequency = 1000000;

#endif

PreciseTimer :: PreciseTimer()
{
#ifndef __GNUG__
	if (sm_frequency == 0)
	{
		long long l_i64Frequency;
		QueryPerformanceFrequency( & l_i64Frequency);
		sm_frequency = l_i64Frequency;//l_liFrequency.QuadPart;

		if (sm_frequency == 0)
		{
			CASTOR_EXCEPTION( "Could not access the high precision timer");
		}
	}

	long long l_i64CurrentTime;
	QueryPerformanceCounter( & l_i64CurrentTime);
	m_previousTime = l_i64CurrentTime;//l_liCurrentTime.QuadPart;
#else
	timeval l_time;
	gettimeofday( & l_time, nullptr);
	m_previousTime = l_time.tv_sec * 1000000 + l_time.tv_usec;
#endif
}

PreciseTimer :: ~PreciseTimer()
{
}

double PreciseTimer :: Time()
{
	long long l_currentTime;
#ifndef __GNUG__
	long long l_i64CurrentTime;
	QueryPerformanceCounter( & l_i64CurrentTime);
	l_currentTime = l_i64CurrentTime;//l_liCurrentTime.QuadPart;
#else
	timeval l_time;
	gettimeofday( & l_time, nullptr);
	l_currentTime = l_time.tv_sec * 1000000 + l_time.tv_usec;
#endif
	double l_diff = static_cast <double> ( l_currentTime - m_previousTime) / static_cast <double> ( sm_frequency);
 
	m_previousTime = l_currentTime;

	return l_diff;
}

double PreciseTimer :: TimeDiff( long long p_time)
{
	return static_cast <double> ( m_previousTime - p_time) / static_cast <double> ( sm_frequency);
}


RepeatTimer :: RepeatTimer( double p_time)
	:	m_repeatTime( p_time)
{
#ifndef __GNUG__
	if (sm_frequency == 0)
	{
		long long l_i64Frequency;
		QueryPerformanceFrequency( & l_i64Frequency);
		sm_frequency = l_i64Frequency;//l_liFrequency.QuadPart;

		if (sm_frequency == 0)
		{
			CASTOR_EXCEPTION( "Could not access the high precision timer");
		}
	}

	long long l_i64CurrentTime;
	QueryPerformanceCounter( & l_i64CurrentTime);
	m_previousTime = l_i64CurrentTime;//l_liCurrentTime.QuadPart;
#else
	timeval l_time;
	gettimeofday( & l_time, nullptr);
	m_previousTime = l_time.tv_sec * 1000000 + l_time.tv_usec;
#endif
}

RepeatTimer :: ~RepeatTimer()
{
}

bool RepeatTimer :: Time()
{
	long long l_currentTime;
#ifndef __GNUG__
	long long l_i64CurrentTime;
	QueryPerformanceCounter( & l_i64CurrentTime);
	l_currentTime = l_i64CurrentTime;//l_liCurrentTime.QuadPart;
#else
	timeval l_time;
	gettimeofday( & l_time, nullptr);
	l_currentTime = l_time.tv_sec * 1000000 + l_time.tv_usec;
#endif
	double l_diff = static_cast <double> ( l_currentTime - m_previousTime) / static_cast <double> ( sm_frequency);

	if (l_diff >= m_repeatTime)
	{
		m_previousTime = l_currentTime;
		return true;
	}

	return false;
}
