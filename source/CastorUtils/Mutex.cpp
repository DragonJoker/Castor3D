#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/Mutex.h"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor::MultiThreading;

//*************************************************************************************************

Mutex :: Mutex()
{
}

Mutex :: ~Mutex()
{
}

void Mutex :: Lock()const
{
	m_mutex.lock();
}

bool Mutex :: TryLock()const
{
	return m_mutex.try_lock();
}

bool Mutex :: TimeLock( unsigned long p_ulTimeOutMilliseconds)const
{
	boost::posix_time::time_duration time = boost::posix_time::millisec( p_ulTimeOutMilliseconds);
	return m_mutex.timed_lock( time);
}

void Mutex :: Unlock()const
{
	m_mutex.unlock();
}

//*************************************************************************************************

RecursiveMutex :: RecursiveMutex()
{
}

RecursiveMutex :: ~RecursiveMutex()
{
}

void RecursiveMutex :: Lock()const
{
	m_mutex.lock();
}

bool RecursiveMutex :: TryLock()const
{
	return m_mutex.try_lock();
}

bool RecursiveMutex :: TimeLock( unsigned long p_ulTimeOutMilliseconds)const
{
	boost::posix_time::time_duration time = boost::posix_time::millisec( p_ulTimeOutMilliseconds);
	return m_mutex.timed_lock( time);
}

void RecursiveMutex :: Unlock()const
{
	m_mutex.unlock();
}

//*************************************************************************************************
/*
CriticalSection :: CriticalSection()
{
	m_pSection = (void *) new CRITICAL_SECTION;
	memset( m_pSection, 0, sizeof( CRITICAL_SECTION));
	InitializeCriticalSection( static_cast <LPCRITICAL_SECTION>( m_pSection));
}

CriticalSection :: ~CriticalSection()
{
	DeleteCriticalSection( static_cast <LPCRITICAL_SECTION>( m_pSection));
	delete m_pSection;
}

void CriticalSection :: Lock()const
{
	BOOL bReturn = TRUE;
	EnterCriticalSection( static_cast <LPCRITICAL_SECTION>( m_pSection));
	return bReturn;
}

bool CriticalSection :: TryLock()const
{
	BOOL bReturn = TRUE;
	bReturn = TryEnterCriticalSection( static_cast <LPCRITICAL_SECTION>( m_pSection));
	return bReturn;
}

bool CriticalSection :: TimeLock( unsigned long p_ulTimeOutMilliseconds)const
{
	BOOL bReturn = FALSE;

	CPreciseTimer timer;
	timer.Time();
	double dTimeOut = p_ulTimeOutMilliseconds / 1000.0;
	double dTime = 0.0;

	while (dTime <= dTimeOut && ! bReturn)
	{
		bReturn = TryEnterCriticalSection( static_cast <LPCRITICAL_SECTION>( m_pSection));
		Sleep( 10);
		dTime += timer.Time();
	}

	return bReturn;
}

void CriticalSection :: Unlock()const
{
	BOOL bReturn = TRUE;
	LeaveCriticalSection( static_cast <LPCRITICAL_SECTION>( m_pSection));
	return bReturn;
}
*/
//*************************************************************************************************
