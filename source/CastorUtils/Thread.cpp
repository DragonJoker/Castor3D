#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/Thread.h"
#include "CastorUtils/Assertion.h"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor;
using namespace Castor::MultiThreading;

//*********************************************************************************************

#include <boost/function.hpp>
#include <boost/thread/thread.hpp>
#include <Windows.h>

Thread :: Thread()
	:	m_thread( NULL),
		m_bSuspended( false),
		m_dwID( 0)
{
}

Thread :: Thread( PDWThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
	:	m_thread( NULL),
		m_bSuspended( false),
		m_dwID( 0)
{
	CreateThread( lpStartAddress, lpParameter, lpThreadAttributes, dwStackSize, eCreationFlags);
}

Thread :: Thread( PUIThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
	:	m_thread( NULL),
		m_bSuspended( false),
		m_dwID( 0)
{
	CreateThread( lpStartAddress, lpParameter, lpThreadAttributes, dwStackSize, eCreationFlags);
}

Thread :: ~Thread()
{
#if CASTOR_USE_MULTITHREADING
	if (m_thread != NULL)
	{
		::TerminateThread( m_thread, 0);
	}
#endif
}

void Thread :: CreateThread( PDWThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
{
	m_bSuspended = (eCreationFlags & eCreateSuspended ? true : false);
#if CASTOR_USE_MULTITHREADING
	m_thread = new boost::thread( CASTOR_THREAD_FUNCTOR( lpStartAddress, lpParameter));
//	m_thread = ::CreateThread( (LPSECURITY_ATTRIBUTES)lpThreadAttributes, dwStackSize, LPTHREAD_START_ROUTINE( lpStartAddress), lpParameter, eCreationFlags, & m_dwID);
#endif
}

void Thread :: CreateThread( PUIThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
{
	m_bSuspended = (eCreationFlags & eCreateSuspended ? true : false);
#if CASTOR_USE_MULTITHREADING
	m_thread = new boost::thread( CASTOR_THREAD_FUNCTOR( lpStartAddress, lpParameter));
//	m_thread = ::CreateThread( (LPSECURITY_ATTRIBUTES)lpThreadAttributes, dwStackSize, LPTHREAD_START_ROUTINE( lpStartAddress), lpParameter, eCreationFlags, & m_dwID);
#endif
}

void Thread :: ResumeThread()
{
#if CASTOR_USE_MULTITHREADING
	if (m_thread != NULL && m_bSuspended)
	{
		(( boost::thread *)m_thread)->detach();
//		::ResumeThread( m_thread);
	}
#endif
	m_bSuspended = false;
}

void Thread :: SuspendThread()
{
#if CASTOR_USE_MULTITHREADING
	if (m_thread != NULL && ! m_bSuspended)
	{
		(( boost::thread *)m_thread)->yield();
//		::SuspendThread( m_thread);
	}
#endif
	m_bSuspended = true;
}

int Thread :: GetThreadPriority()
{
	int iReturn = 0;

#if CASTOR_USE_MULTITHREADING
	if (m_thread != NULL)
	{
//		::GetThreadPriority( m_thread);
	}
#endif

	return iReturn;
}

bool Thread :: SetThreadPriority( int iPriority)
{
	bool bReturn = false;

#if CASTOR_USE_MULTITHREADING
	if (m_thread != NULL)
	{
//		::SetThreadPriority( m_thread, iPriority);
	}
#endif

	return bReturn;
}

bool Thread :: Wait( unsigned long dwMilliseconds)
{
	bool l_bReturn = true;

#if CASTOR_USE_MULTITHREADING
	if (m_thread != NULL)
	{
//		try//		{
			boost::posix_time::time_duration time = boost::posix_time::microsec( dwMilliseconds);
			l_bReturn = (( boost::thread *)m_thread)->timed_join( time);
//			l_bReturn = ::WaitForSingleObject( m_thread, dwMilliseconds) != WAIT_FAILED;//		}
//		catch ( ... )//		{//			l_bReturn = false;//		}
	}
#endif

	return l_bReturn;
}

//*********************************************************************************************

Thread * MultiThreading :: CreateThread( MultiThreading::PDWThreadStartFunction lpStartAddress,
										 void * lpParameter, void * lpThreadAttributes,
										 size_t dwStackSize, eTHREAD_STATE dwCreationFlags,
										 unsigned long * lpThreadId)
{
	Thread * pReturn = new Thread;
	pReturn->CreateThread( lpStartAddress, lpParameter, lpThreadAttributes, dwStackSize, dwCreationFlags);

	if (lpThreadId != NULL)
	{
		*lpThreadId = pReturn->GetID();
	}	return pReturn;}

MultiThreading::Thread * MultiThreading :: CreateThread( PUIThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE dwCreationFlags, unsigned long * lpThreadId)
{
	Thread * pReturn = new Thread;
	pReturn->CreateThread( lpStartAddress, lpParameter, lpThreadAttributes, dwStackSize, dwCreationFlags);

	if (lpThreadId != NULL)
	{
		*lpThreadId = pReturn->GetID();
	}	return pReturn;}

bool MultiThreading :: WaitForMultipleThreads( bool bWaitAll, unsigned long dwMilliseconds, size_t p_uiNbThreads, ...)
{
	bool l_bReturn = true;
	std::vector <void *> pHandles;
	va_list l_vaArgs;
	va_start( l_vaArgs, p_uiNbThreads);

	for (size_t i = 0 ; i < p_uiNbThreads ; i++)
	{
		MultiThreading::Thread * pThread = va_arg( l_vaArgs, MultiThreading::Thread *);
		CASTOR_ASSERT( pThread != NULL && pThread->GetHandle() != NULL);
		pHandles.push_back( pThread->GetHandle());
	}

	va_end( l_vaArgs);

	boost::posix_time::time_duration time = boost::posix_time::microsec( dwMilliseconds);
	for (size_t i = 0 ; i < pHandles.size() && l_bReturn ; i++)
	{
		l_bReturn = ((boost::thread *)pHandles.at( 0))->timed_join( time);
	}/*	try	{
		l_bReturn = ::WaitForMultipleObjects( p_uiNbThreads, reinterpret_cast <const HANDLE *>( & pHandles.at( 0)), bWaitAll, dwMilliseconds) != WAIT_FAILED;
	}
	catch ( ... )
	{
		l_bReturn = false;
	}
*/
	return l_bReturn;
}

bool MultiThreading :: WaitForSingleThread( Thread * pThread, unsigned long dwMilliseconds)
{
	return pThread->Wait( dwMilliseconds);
}
