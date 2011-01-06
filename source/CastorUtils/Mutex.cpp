#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/Mutex.h"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor::MultiThreading;

#if CASTOR_MT_USE_MFC
#	ifndef WINVER
#		define WINVER 0x0501
#	endif
#	define _AFXDLL
#	include <afxmt.h>
#elif ! CASTOR_MT_USE_BOOST && ! defined( __GNUG__)
#	include <Windows.h>
#endif

//*************************************************************************************************

Mutex :: Mutex()
#if CASTOR_MT_USE_BOOST
	:	m_pSemaphore( (void *)new boost::mutex)
#elif CASTOR_MT_USE_MFC
	:	m_pSemaphore( (void *)new CSemaphore())
#elif CASTOR_MT_USE_WIN32
	:	m_pSemaphore( ::CreateSemaphore( NULL, 1, 1, NULL))
#endif
{
}

Mutex :: ~Mutex()
{
#if CASTOR_MT_USE_MFC || CASTOR_MT_USE_BOOST
	delete m_pSemaphore;
#elif CASTOR_MT_USE_WIN32
	::CloseHandle( m_pSemaphore);
#endif
}

void Mutex :: Lock()const
{
#if CASTOR_MT_USE_BOOST
	((boost::mutex *)m_pSemaphore)->lock();
#elif CASTOR_MT_USE_MFC
	::WaitForSingleObject( ((CSemaphore *)m_pSemaphore)->m_hObject, INFINITE);
#elif CASTOR_MT_USE_WIN32
	::WaitForSingleObject( m_pSemaphore, INFINITE);
#endif
}

bool Mutex :: TryLock()const
{
	bool l_bReturn = true;
#if CASTOR_MT_USE_BOOST
	l_bReturn = ((boost::mutex *)m_pSemaphore)->try_lock();
#elif CASTOR_MT_USE_MFC
	l_bReturn = ::WaitForSingleObject( ((CSemaphore *)m_pSemaphore)->m_hObject, 1) != WAIT_FAILED;
#elif CASTOR_MT_USE_WIN32
	l_bReturn = ::WaitForSingleObject( m_pSemaphore, 1) != WAIT_FAILED;
#endif
	return l_bReturn;
}

bool Mutex :: TimeLock( unsigned long p_ulTimeOutMilliseconds)const
{
	bool l_bReturn = true;
#if CASTOR_MT_USE_BOOST
	boost::posix_time::time_duration time = boost::posix_time::millisec( p_ulTimeOutMilliseconds);
	l_bReturn = ((boost::mutex *)m_pSemaphore)->timed_lock( time);
#elif CASTOR_MT_USE_MFC
	l_bReturn = ::WaitForSingleObject( ((CSemaphore *)m_pSemaphore)->m_hObject, p_ulTimeOutMilliseconds) != WAIT_FAILED;
#elif CASTOR_MT_USE_WIN32
	l_bReturn = ::WaitForSingleObject( m_pSemaphore, p_ulTimeOutMilliseconds) != WAIT_FAILED;
#endif
	return l_bReturn;
}

void Mutex :: Unlock()const
{
#if CASTOR_MT_USE_BOOST
	((boost::mutex *)m_pSemaphore)->unlock();
#elif CASTOR_MT_USE_MFC
	long l_lresult;
	::ReleaseSemaphore( ((CSemaphore *)m_pSemaphore)->m_hObject, 1, & l_lresult);
#elif CASTOR_MT_USE_WIN32
	long l_lresult;
	::ReleaseSemaphore( m_pSemaphore, 1, & l_lresult);
#endif
}

//*************************************************************************************************

RecursiveMutex :: RecursiveMutex()
#if CASTOR_MT_USE_BOOST
	:	m_pMutex( (void *)new boost::recursive_mutex)
#elif CASTOR_MT_USE_MFC
	:	m_pMutex( (void *)new CMutex)
#elif CASTOR_MT_USE_WIN32
	:	m_pMutex( ::CreateMutex( false, NULL, NULL))
#endif
{
}

RecursiveMutex :: ~RecursiveMutex()
{
#if CASTOR_MT_USE_MFC || CASTOR_MT_USE_BOOST
	delete m_pMutex;
#elif CASTOR_MT_USE_WIN32
	::CloseHandle( m_pMutex);
#endif
}

void RecursiveMutex :: Lock()const
{
#if CASTOR_MT_USE_BOOST
	((boost::recursive_mutex *)m_pMutex)->lock();
#elif CASTOR_MT_USE_MFC
	::WaitForSingleObject( ((CMutex *)m_pMutex)->m_hObject, INFINITE);
#elif CASTOR_MT_USE_WIN32
	::WaitForSingleObject( m_pMutex, INFINITE);
#endif
}

bool RecursiveMutex :: TryLock()const
{
	bool l_bReturn = true;
#if CASTOR_MT_USE_BOOST
	l_bReturn = ((boost::recursive_mutex *)m_pMutex)->try_lock();
#elif CASTOR_MT_USE_MFC
	l_bReturn = ::WaitForSingleObject( ((CMutex *)m_pMutex)->m_hObject, 1) != WAIT_FAILED;
#elif CASTOR_MT_USE_WIN32
	l_bReturn = ::WaitForSingleObject( m_pMutex, 1) != WAIT_FAILED;
#endif
	return l_bReturn;
}

bool RecursiveMutex :: TimeLock( unsigned long p_ulTimeOutMilliseconds)const
{
	bool l_bReturn = true;
#if CASTOR_MT_USE_BOOST
	boost::posix_time::time_duration time = boost::posix_time::millisec( p_ulTimeOutMilliseconds);
	l_bReturn = ((boost::recursive_mutex *)m_pMutex)->timed_lock( time);
#elif CASTOR_MT_USE_MFC
	l_bReturn = ::WaitForSingleObject( ((CMutex *)m_pMutex)->m_hObject, p_ulTimeOutMilliseconds) != WAIT_FAILED;
#elif ! defined( __GNUG__)
	l_bReturn = ::WaitForSingleObject( m_pMutex, p_ulTimeOutMilliseconds) != WAIT_FAILED;
#endif
	return l_bReturn;
}

void RecursiveMutex :: Unlock()const
{
#if CASTOR_MT_USE_BOOST
	((boost::recursive_mutex *)m_pMutex)->unlock();
#elif CASTOR_MT_USE_MFC
	::ReleaseMutex( ((CMutex *)m_pMutex)->m_hObject);
#elif CASTOR_MT_USE_WIN32
	::ReleaseMutex( m_pMutex);
#endif
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