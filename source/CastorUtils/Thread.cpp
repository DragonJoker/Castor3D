#include "CastorUtils/PrecompiledHeader.hpp"

#include "CastorUtils/Thread.hpp"
#include "CastorUtils/Assertion.hpp"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.hpp"
using namespace Castor::Utils;
#endif

using namespace Castor;
using namespace Castor::MultiThreading;

//*********************************************************************************************

#include <boost/function.hpp>
#include <boost/thread/thread.hpp>

Thread :: Thread()
	:	m_pThread( NULL)
	,	m_bSuspended( false)
	,	m_dwID( 0)
{
}

Thread :: Thread( PDWThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
	:	m_pThread( NULL)
	,	m_bSuspended( false)
	,	m_dwID( 0)
{
	CreateThread( lpStartAddress, lpParameter, lpThreadAttributes, dwStackSize, eCreationFlags);
}

Thread :: Thread( PUIThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
	:	m_pThread( NULL)
	,	m_bSuspended( false)
	,	m_dwID( 0)
{
	CreateThread( lpStartAddress, lpParameter, lpThreadAttributes, dwStackSize, eCreationFlags);
}

Thread :: ~Thread()
{
	delete m_pThread;
}

void Thread :: CreateThread( PDWThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
{
	m_bSuspended = (eCreationFlags & eTHREAD_STATE_SUSPENDED ? true : false);
#if CASTOR_USE_MULTITHREADING
	m_pThread = new boost::thread( CASTOR_THREAD_FUNCTOR( lpStartAddress, lpParameter));
#endif
}

void Thread :: CreateThread( PUIThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
{
	m_bSuspended = (eCreationFlags & eTHREAD_STATE_SUSPENDED ? true : false);
#if CASTOR_USE_MULTITHREADING
	m_pThread = new boost::thread( CASTOR_THREAD_FUNCTOR( lpStartAddress, lpParameter));
#endif
}

bool Thread :: Wait( unsigned long dwMilliseconds)
{
	bool l_bReturn = true;

#if CASTOR_USE_MULTITHREADING
	if (m_pThread)
	{
		boost::posix_time::time_duration time = boost::posix_time::microsec( dwMilliseconds);
		l_bReturn = m_pThread->timed_join( time);

		if (l_bReturn)
		{
			delete m_pThread;
			m_pThread = NULL;
		}
	}
#endif

	return l_bReturn;
}

void Thread :: Kill()
{
	if (m_pThread != NULL)
	{
		m_pThread->join();
		delete m_pThread;
		m_pThread = NULL;
	}
}

//*********************************************************************************************

Thread * MultiThreading :: CreateThread( MultiThreading::PDWThreadStartFunction lpStartAddress,
										 void * lpParameter, void * lpThreadAttributes,
										 size_t dwStackSize, eTHREAD_STATE dwCreationFlags,
										 unsigned long * lpThreadId)
{
	Thread * l_pReturn = new Thread;
	l_pReturn->CreateThread( lpStartAddress, lpParameter, lpThreadAttributes, dwStackSize, dwCreationFlags);

	if (lpThreadId)
	{
		*lpThreadId = l_pReturn->GetID();
	}	return l_pReturn;}

MultiThreading::Thread * MultiThreading :: CreateThread( PUIThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE dwCreationFlags, unsigned long * lpThreadId)
{
	Thread * l_pReturn = new Thread;
	l_pReturn->CreateThread( lpStartAddress, lpParameter, lpThreadAttributes, dwStackSize, dwCreationFlags);

	if (lpThreadId)
	{
		*lpThreadId = l_pReturn->GetID();
	}	return l_pReturn;}

bool MultiThreading :: WaitForMultipleThreads( bool bWaitAll, unsigned long dwMilliseconds, size_t p_uiNbThreads, ...)
{
	bool l_bReturn = true;
#if CASTOR_USE_MULTITHREADING
	std::vector <boost::thread *> l_arrayHandles;
	va_list l_vaArgs;
	va_start( l_vaArgs, p_uiNbThreads);

	for (size_t i = 0 ; i < p_uiNbThreads ; i++)
	{
		MultiThreading::Thread * l_pThread = va_arg( l_vaArgs, MultiThreading::Thread *);
		CASTOR_ASSERT( l_pThread && l_pThread->GetHandle());
		l_arrayHandles.push_back( l_pThread->GetHandle());
	}

	va_end( l_vaArgs);

	boost::posix_time::time_duration time = boost::posix_time::microsec( dwMilliseconds);
	for (size_t i = 0 ; i < l_arrayHandles.size() && l_bReturn ; i++)
	{
		l_bReturn = l_arrayHandles[i]->timed_join( time);
	}
#endif
	return l_bReturn;
}

bool MultiThreading :: WaitForSingleThread( Thread * pThread, unsigned long dwMilliseconds)
{
	return pThread->Wait( dwMilliseconds);
}
