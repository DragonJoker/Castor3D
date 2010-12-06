/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___Castor_Thread___
#define ___Castor_Thread___

#include "MultiThreadConfig.h"

#ifndef CASTOR_THREAD_INIT
#	define CASTOR_THREAD_INIT()
#endif
/*
	//! to ease and uniformise the write of a thread
	typedef boost::thread Thread;

//! Declares the BeginThread class function (to put in a class declaration)
#		define CASTOR_THREAD_DECLARE_THREAD_FUNC									void BeginThread()
//! Defines the BeginThread class function
#		define CASTOR_THREAD_DEFINE_THREAD_FUNC( p_class)							void p_class::BeginThread()
//! Creates a threaded class
#		define CASTOR_THREAD_CREATE_CLASS_THREAD( p_class, p_instance)				new Castor::MultiThreading::Thread( CASTOR_THREAD_CLASS_FUNCTOR( p_instance, p_class, BeginThread))
//! Creates a memeber threaded function
#		define CASTOR_THREAD_CREATE_MEMBER_FUNC_THREAD( p_class, p_instance, p_func)new Castor::MultiThreading::Thread( CASTOR_THREAD_CLASS_FUNCTOR( p_instance, p_class, p_func))
//! Deletes a thread
#		define CASTOR_THREAD_DELETE_THREAD( p_thread)								delete p_thread
//! Waits for a thread end
#		define CASTOR_THREAD_WAIT_FOR_END_OF( p_thread)								p_thread->join()

#		define CASTOR_AUTO_SHARED_MUTEX mutable boost::recursive_mutex *			m_mutex;
#		define CASTOR_SET_AUTO_SHARED_MUTEX_NULL									m_mutex = NULL;
#		define CASTOR_NEW_AUTO_SHARED_MUTEX											m_mutex = new boost::recursive_mutex();
#		define CASTOR_MUTEX_CONDITIONAL( mutex)										if (mutex)
*/

namespace Castor
{ namespace MultiThreading
{
	typedef enum
	{
		eCreatedRunning		= 0,
#	if CASTOR_MT_USE_BOOST
		eCreateSuspended	= 1,
#	elif CASTOR_MT_USE_MFC
		eCreateSuspended	= 4,
#	elif CASTOR_MT_USE_WIN32
		eCreateSuspended	= 4,
#	else
		eCreateSuspended	= 1,
#	endif
	} eTHREAD_STATE;

	//! Pointer over a class function
	template <class TClass>
	class ClassFunctor : public MemoryTraced< ClassFunctor<TClass> >
	{
	public:
		TClass * m_instance;
		void( TClass::* m_function)();

	public:
		ClassFunctor( TClass * p_instance, void( TClass::* p_function)())
			:	m_instance (p_instance),
				m_function (p_function)
		{
		}
		void operator()()const
		{
			CASTOR_THREAD_INIT();
			(m_instance->* m_function)();
		}
	};

	typedef unsigned long (__cdecl * DWThreadStartFunction)( void *);
	typedef DWThreadStartFunction PDWThreadStartFunction;
	typedef unsigned int (__cdecl * UIThreadStartFunction)( void *);
	typedef UIThreadStartFunction PUIThreadStartFunction;

//! Creates a ClassFunctor
#define CASTOR_THREAD_CLASS_FUNCTOR( p_instance, p_class, p_function)		Castor::MultiThreading::ClassFunctor <p_class> ( p_instance, & p_class::p_function)

	class Thread : public MemoryTraced<Thread>
	{
	private:
		void * m_thread;
		bool m_bSuspended;
		unsigned long m_dwID;

	public:
		Thread();
		Thread( PDWThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes = NULL, size_t dwStackSize = 0, eTHREAD_STATE eCreationFlags = eCreateSuspended);
		Thread( PUIThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes = NULL, size_t dwStackSize = 0, eTHREAD_STATE eCreationFlags = eCreateSuspended);
		~Thread();

		void CreateThread( PDWThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes = NULL, size_t dwStackSize = 0, eTHREAD_STATE eCreationFlags = eCreateSuspended);
		void CreateThread( PUIThreadStartFunction lpStartAddress, void * lpParameter, void * lpThreadAttributes = NULL, size_t dwStackSize = 0, eTHREAD_STATE eCreationFlags = eCreateSuspended);
		void ResumeThread();
		void SuspendThread();
		int GetThreadPriority();
		bool SetThreadPriority( int iPriority);

		bool Wait( unsigned long p_ulMilliseconds);
		inline void *			GetHandle	()const	{ return m_thread; }
		inline unsigned long	GetID		()const	{ return m_dwID; }
	};

	Thread * CreateThread( PDWThreadStartFunction threadFunction, void * lpParameter, void * lpThreadAttributes = NULL, size_t dwStackSize = 0, eTHREAD_STATE eCreationFlags = eCreateSuspended, unsigned long * lpThreadId = NULL);
	Thread * CreateThread( PUIThreadStartFunction threadFunction, void * lpParameter, void * lpThreadAttributes = NULL, size_t dwStackSize = 0, eTHREAD_STATE eCreationFlags = eCreateSuspended, unsigned long * lpThreadId = NULL);
	bool WaitForMultipleThreads( bool bWaitAll, unsigned long dwMilliseconds, size_t p_uiNbThreads, ...);
	bool WaitForSingleThread( Thread * pThread, unsigned long dwMilliseconds);
}
}

#endif
