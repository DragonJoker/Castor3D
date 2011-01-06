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
	}
	eTHREAD_STATE;

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

	typedef unsigned long DWThreadStartFunction( void *);
	typedef DWThreadStartFunction * PDWThreadStartFunction;
	typedef unsigned int UIThreadStartFunction( void *);
	typedef UIThreadStartFunction * PUIThreadStartFunction;

//! Creates a ClassFunctor
#define CASTOR_THREAD_CLASS_FUNCTOR( p_instance, p_class, p_function)		Castor::MultiThreading::ClassFunctor <p_class> ( p_instance, & p_class::p_function)

	//! Thread class wrapper
	/*!
	Wraps the chosen thread class (Win32, MFC or boost)
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class Thread : public MemoryTraced<Thread>
	{
	private:
		void * m_thread;
		bool m_bSuspended;
		unsigned long m_dwID;

	public:
		/**@name Constructors */
		//@{
		Thread();
		Thread( PDWThreadStartFunction p_lpStartAddress, void * p_lpParameter, void * p_lpThreadAttributes = NULL, size_t p_dwStackSize = 0, eTHREAD_STATE p_eCreationFlags = eCreateSuspended);
		Thread( PUIThreadStartFunction p_lpStartAddress, void * p_lpParameter, void * p_lpThreadAttributes = NULL, size_t p_dwStackSize = 0, eTHREAD_STATE p_eCreationFlags = eCreateSuspended);
		~Thread();
		//@}
		/**
		 * Creates a thread from a function
		 *@param p_lpStartAddress : [in] The function that will be threaded
		 *@param p_lpParameter : [in] An optional parameter to the given function
		 *@param p_lpThreadAttributes : [in] The thread attributes
		 *@param p_dwStackSize : [in] The thread max stack size
		 *@param p_eCreationFlags : [in] The initial state of the thread
		 */
		void CreateThread( PDWThreadStartFunction p_lpStartAddress, void * p_lpParameter, void * p_lpThreadAttributes = NULL, size_t p_dwStackSize = 0, eTHREAD_STATE p_eCreationFlags = eCreateSuspended);
		/**
		 * Creates a thread from a function
		 *@param p_lpStartAddress : [in] The function that will be threaded
		 *@param p_lpParameter : [in] An optional parameter to the given function
		 *@param p_lpThreadAttributes : [in] The thread attributes
		 *@param p_dwStackSize : [in] The thread max stack size
		 *@param p_eCreationFlags : [in] The initial state of the thread
		 */
		void CreateThread( PUIThreadStartFunction p_lpStartAddress, void * p_lpParameter, void * p_lpThreadAttributes = NULL, size_t p_dwStackSize = 0, eTHREAD_STATE p_eCreationFlags = eCreateSuspended);
		/**
		 * Resumes a paused thred
		 */
		void ResumeThread();
		/**
		 * Suspends a running thread
		 */
		void SuspendThread();
		/**
		 * Returns the thread priority
		 */
		int GetThreadPriority();
		/**
		 * Defines the thread priority
		 */
		bool SetThreadPriority( int iPriority);
		/**
		 * Waits for the thread to end
		 *@param p_ulMilliseconds : [in] The maximum time to wait, in milliseconds
		 *@return true if the max time has not been reached before end
		 */
		bool Wait( unsigned long p_ulMilliseconds);
		/**@name Accessors */
		//@{
		inline void *			GetHandle	()const	{ return m_thread; }
		inline unsigned long	GetID		()const	{ return m_dwID; }
		//@}

	};

	/**
	 * Creates a thread from a function
	 *@param p_threadFunction : [in] The function that will be threaded
	 *@param p_lpParameter : [in] An optional parameter to the given function
	 *@param p_lpThreadAttributes : [in] The thread attributes
	 *@param p_dwStackSize : [in] The thread max stack size
	 *@param p_eCreationFlags : [in] The initial state of the thread
	 *@param p_lpThreadId : [out] Receives th thread id
	 *@return The created thread
	 */
	Thread * CreateThread( PDWThreadStartFunction p_threadFunction, void * p_lpParameter, void * p_lpThreadAttributes = NULL, size_t p_dwStackSize = 0, eTHREAD_STATE p_eCreationFlags = eCreateSuspended, unsigned long * p_lpThreadId = NULL);
	/**
	 * Creates a thread from a function
	 *@param p_threadFunction : [in] The function that will be threaded
	 *@param p_lpParameter : [in] An optional parameter to the given function
	 *@param p_lpThreadAttributes : [in] The thread attributes
	 *@param p_dwStackSize : [in] The thread max stack size
	 *@param p_eCreationFlags : [in] The initial state of the thread
	 *@param p_lpThreadId : [out] Receives th thread id
	 *@return The created thread
	 */
	Thread * CreateThread( PUIThreadStartFunction p_threadFunction, void * p_lpParameter, void * p_lpThreadAttributes = NULL, size_t p_dwStackSize = 0, eTHREAD_STATE p_eCreationFlags = eCreateSuspended, unsigned long * p_lpThreadId = NULL);
	/**
	 * Waits for multiple threads
	 *@param p_bWaitAll : [in] Tells if we wait for all given threads (true) or just one (false)
	 *@param p_ulMilliseconds : [in] The maximum time to wait, in milliseconds
	 *@param p_uiNbThreads : [in] The number of threads to wait for
	 *@param ... : [in] The threads to wait for
	 */
	bool WaitForMultipleThreads( bool p_bWaitAll, unsigned long p_ulMilliseconds, size_t p_uiNbThreads, ...);
	/**
	 * Waits for one thread
	 *@param p_pThread : [in] The thread to wait for
	 *@param p_ulMilliseconds : [in] The maximum time to wait, in milliseconds
	 */
	bool WaitForSingleThread( Thread * p_pThread, unsigned long p_ulMilliseconds);
}
}

#endif
