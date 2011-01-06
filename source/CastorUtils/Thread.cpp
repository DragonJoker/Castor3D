#include "CastorUtils/PrecompiledHeader.h"

#include "CastorUtils/Thread.h"
#include "CastorUtils/Assertion.h"

#if CHECK_MEMORYLEAKS
#	include "CastorUtils/Memory.h"
using namespace Castor::Utils;
#endif

using namespace Castor;
using namespace Castor::MultiThreading;

#if CASTOR_MT_USE_BOOST
#	include <boost/function.hpp>
#	include <boost/thread/thread.hpp>
#elif CASTOR_MT_USE_MFC
#	ifndef WINVER
#		define WINVER 0x0501
#	endif
#	define _AFXDLL
#	include <afxwin.h>
#endif

//*********************************************************************************************

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
#if CASTOR_MT_USE_MFC

Thread :: ~Thread()
{
	if (m_thread != NULL)
	{
		((CWinThread *)m_thread)->Delete();
	}
}

void Thread :: CreateThread( PDWThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
{
	m_bSuspended = (eCreationFlags & eCreateSuspended ? TRUE : FALSE);
	m_thread = AfxBeginThread( (AFX_THREADPROC)lpStartAddress, lpParameter, THREAD_PRIORITY_NORMAL, dwStackSize, eCreationFlags, (LPSECURITY_ATTRIBUTES)lpThreadAttributes);
	((CWinThread *)m_thread)->m_bAutoDelete = FALSE;
}

void Thread :: CreateThread( PUIThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
{
	m_bSuspended = (eCreationFlags & eCreateSuspended ? TRUE : FALSE);
	m_thread = AfxBeginThread( (AFX_THREADPROC)lpStartAddress, lpParameter, THREAD_PRIORITY_NORMAL, dwStackSize, eCreationFlags, (LPSECURITY_ATTRIBUTES)lpThreadAttributes);
	((CWinThread *)m_thread)->m_bAutoDelete = FALSE;
}

void Thread :: ResumeThread()
{
	if (m_thread != NULL && m_bSuspended)
	{
		m_bSuspended = FALSE;
		((CWinThread *)m_thread)->ResumeThread();
	}
}

void Thread :: SuspendThread()
{
	if (m_thread != NULL && ! m_bSuspended)
	{
		m_bSuspended = true;
		((CWinThread *)m_thread)->SuspendThread();
	}
}

int Thread :: GetThreadPriority()
{
	int iReturn = 0;

	if (m_thread != NULL)
	{
		((CWinThread *)m_thread)->GetThreadPriority();
	}

	return iReturn;
}

bool Thread :: SetThreadPriority( int iPriority)
{
	bool bReturn = false;

	if (m_thread != NULL)
	{
		((CWinThread *)m_thread)->SetThreadPriority( iPriority);
	}

	return bReturn;
}

bool Thread :: Wait( unsigned long dwMilliseconds)
{
	bool l_bReturn = true;

	if (m_thread != NULL)
	{
		try		{
			l_bReturn = ::WaitForSingleObject( ((CWinThread *)m_thread)->m_hThread, dwMilliseconds) != WAIT_FAILED;		}
		catch ( ... )		{			l_bReturn = false;		}
	}

	return l_bReturn;
}

//*********************************************************************************************
#elif CASTOR_MT_USE_BOOST

Thread :: ~Thread()
{
	if (m_thread != NULL)
	{
		::TerminateThread( m_thread, 0);
	}
}

void Thread :: CreateThread( PDWThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
{
	m_bSuspended = (eCreationFlags & eCreateSuspended ? true : false);
	m_thread = ::CreateThread( (LPSECURITY_ATTRIBUTES)lpThreadAttributes, dwStackSize, LPTHREAD_START_ROUTINE( lpStartAddress), lpParameter, eCreationFlags, & m_dwID);
}

void Thread :: CreateThread( PUIThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
{
	m_bSuspended = (eCreationFlags & eCreateSuspended ? true : false);
	m_thread = ::CreateThread( (LPSECURITY_ATTRIBUTES)lpThreadAttributes, dwStackSize, LPTHREAD_START_ROUTINE( lpStartAddress), lpParameter, eCreationFlags, & m_dwID);
}

void Thread :: ResumeThread()
{
	if (m_thread != NULL && m_bSuspended)
	{
		m_bSuspended = false;
		::ResumeThread( m_thread);
	}
}

void Thread :: SuspendThread()
{
	if (m_thread != NULL && ! m_bSuspended)
	{
		m_bSuspended = true;
		::SuspendThread( m_thread);
	}
}

int Thread :: GetThreadPriority()
{
	int iReturn = 0;

	if (m_thread != NULL)
	{
		::GetThreadPriority( m_thread);
	}

	return iReturn;
}

bool Thread :: SetThreadPriority( int iPriority)
{
	bool bReturn = false;

	if (m_thread != NULL)
	{
		::SetThreadPriority( m_thread, iPriority);
	}

	return bReturn;
}

bool Thread :: Wait( unsigned long dwMilliseconds)
{
	bool l_bReturn = true;

	if (m_thread != NULL)
	{
		try		{
			l_bReturn = ::WaitForSingleObject( m_thread, dwMilliseconds) != WAIT_FAILED;		}
		catch ( ... )		{			l_bReturn = false;		}
	}

	return l_bReturn;
}

//*********************************************************************************************
#elif CASTOR_MT_USE_WIN32
#	include <Windows.h>

Thread :: ~Thread()
{
	if (m_thread != NULL)
	{
		::TerminateThread( m_thread, 0);
	}
}

void Thread :: CreateThread( PDWThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
{
	m_bSuspended = (eCreationFlags & eCreateSuspended ? true : false);
	m_thread = ::CreateThread( (LPSECURITY_ATTRIBUTES)lpThreadAttributes, dwStackSize, LPTHREAD_START_ROUTINE( lpStartAddress), lpParameter, (DWORD)eCreationFlags, & m_dwID);
}

void Thread :: CreateThread( PUIThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
{
	m_bSuspended = (eCreationFlags & eCreateSuspended ? true : false);
	m_thread = ::CreateThread( (LPSECURITY_ATTRIBUTES)lpThreadAttributes, dwStackSize, LPTHREAD_START_ROUTINE( lpStartAddress), lpParameter, (DWORD)eCreationFlags, & m_dwID);
}

void Thread :: ResumeThread()
{
	if (m_thread != NULL && m_bSuspended)
	{
		m_bSuspended = false;
		::ResumeThread( m_thread);
	}
}

void Thread :: SuspendThread()
{
	if (m_thread != NULL && ! m_bSuspended)
	{
		m_bSuspended = true;
		::SuspendThread( m_thread);
	}
}

int Thread :: GetThreadPriority()
{
	int iReturn = 0;

	if (m_thread != NULL)
	{
		::GetThreadPriority( m_thread);
	}

	return iReturn;
}

bool Thread :: SetThreadPriority( int iPriority)
{
	bool bReturn = false;

	if (m_thread != NULL)
	{
		::SetThreadPriority( m_thread, iPriority);
	}

	return bReturn;
}

bool Thread :: Wait( unsigned long dwMilliseconds)
{
	bool l_bReturn = true;

	if (m_thread != NULL)
	{
		try		{
			l_bReturn = ::WaitForSingleObject( m_thread, dwMilliseconds) != WAIT_FAILED;		}
		catch ( ... )		{			l_bReturn = false;		}
	}

	return l_bReturn;
}

//*********************************************************************************************
#else

Thread :: ~Thread()
{
}

void Thread :: CreateThread( PDWThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
{
	m_bSuspended = (eCreationFlags & eCreateSuspended ? true : false);
}

void Thread :: CreateThread( PUIThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes, size_t dwStackSize, eTHREAD_STATE eCreationFlags)
{
	m_bSuspended = (eCreationFlags & eCreateSuspended ? true : false);
}

void Thread :: ResumeThread()
{
	if (m_bSuspended)
	{
		m_bSuspended = false;
	}
}

void Thread :: SuspendThread()
{
	if ( ! m_bSuspended)
	{
		m_bSuspended = true;
	}
}

int Thread :: GetThreadPriority()
{
	int iReturn = 0;
	return iReturn;
}

bool Thread :: SetThreadPriority( int iPriority)
{
	bool bReturn = true;
	return bReturn;
}

bool Thread :: Wait( unsigned long dwMilliseconds)
{
	bool l_bReturn = true;
	return l_bReturn;
}

#endif
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

#if CASTOR_MT_USE_BOOST
	boost::posix_time::time_duration time = boost::posix_time::microsec( dwMilliseconds);
	for (size_t i = 0 ; i < pHandles.size() && l_bReturn ; i++)
	{
		l_bReturn = ((boost::thread *)pHandles.at( 0))->timed_join( time);
	}#elif CASTOR_MT_USE_MFC	try	{
		l_bReturn = ::WaitForMultipleObjects( dwNbThreads, reinterpret_cast <const HANDLE *>( & pHandles.at( 0)), bWaitAll, dwMilliseconds) != WAIT_FAILED;
	}
	catch ( ... )
	{
		l_bReturn = false;
	}#elif CASTOR_MT_USE_WIN32	try	{
		l_bReturn = ::WaitForMultipleObjects( dwNbThreads, reinterpret_cast <const HANDLE *>( & pHandles.at( 0)), bWaitAll, dwMilliseconds) != WAIT_FAILED;
	}
	catch ( ... )
	{
		l_bReturn = false;
	}
#else
	l_bReturn = true;
#endif

	return l_bReturn;
}

bool MultiThreading :: WaitForSingleThread( Thread * pThread, unsigned long dwMilliseconds)
{
	return pThread->Wait( dwMilliseconds);
}