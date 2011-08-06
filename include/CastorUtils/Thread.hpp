/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#include "MultiThreadConfig.hpp"

#ifndef CASTOR_THREAD_INIT
#	define CASTOR_THREAD_INIT()
#endif

namespace boost
{
	class thread;
}

namespace Castor
{ namespace MultiThreading
{
	typedef enum
	{	eTHREAD_STATE_SUSPENDED	= 1,
	}	eTHREAD_STATE;

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
			(m_instance->* m_function)();
		}
	};

	typedef unsigned long DWThreadStartFunction( void *);
	typedef DWThreadStartFunction * PDWThreadStartFunction;
	typedef unsigned int UIThreadStartFunction( void *);
	typedef UIThreadStartFunction * PUIThreadStartFunction;

	class ThreadFunctor
	{
	public:
		PDWThreadStartFunction m_pDWThreadFunction;
		PUIThreadStartFunction m_pUIThreadFunction;
		void * m_pParameter;

	public:
		ThreadFunctor( PDWThreadStartFunction p_pFunction, void * p_pParameter)
			:	m_pDWThreadFunction( p_pFunction)
			,	m_pUIThreadFunction( NULL)
			,	m_pParameter( p_pParameter)
		{
		}
		ThreadFunctor( PUIThreadStartFunction p_pFunction, void * p_pParameter)
			:	m_pDWThreadFunction( NULL)
			,	m_pUIThreadFunction( p_pFunction)
			,	m_pParameter( p_pParameter)
		{
		}
		unsigned long operator()()const
		{
			if (m_pDWThreadFunction)
			{
				return m_pDWThreadFunction( m_pParameter);
			}
			else if (m_pUIThreadFunction)
			{
				return m_pUIThreadFunction( m_pParameter);
			}

			return 0;
		}
	};

	//! Creates a ClassFunctor
#define CASTOR_THREAD_CLASS_FUNCTOR( p_instance, p_class, p_function)		Castor::MultiThreading::ClassFunctor <p_class> ( p_instance, & p_class::p_function)
#define CASTOR_THREAD_FUNCTOR( p_function, p_instance)						Castor::MultiThreading::ThreadFunctor( p_function, p_instance)

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
		boost::thread * m_pThread;
		bool m_bSuspended;
		unsigned long m_dwID;

	public:
		/**@name Constructors */
		//@{
		Thread();
		Thread( PDWThreadStartFunction p_lpStartAddress, void * p_lpParameter, void * p_lpThreadAttributes = nullptr, size_t p_dwStackSize = 0, eTHREAD_STATE p_eCreationFlags = eTHREAD_STATE_SUSPENDED);
		Thread( PUIThreadStartFunction p_lpStartAddress, void * p_lpParameter, void * p_lpThreadAttributes = nullptr, size_t p_dwStackSize = 0, eTHREAD_STATE p_eCreationFlags = eTHREAD_STATE_SUSPENDED);
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
		void CreateThread( PDWThreadStartFunction p_lpStartAddress, void * p_lpParameter, void * p_lpThreadAttributes = nullptr, size_t p_dwStackSize = 0, eTHREAD_STATE p_eCreationFlags = eTHREAD_STATE_SUSPENDED);
		/**
		 * Creates a thread from a function
		 *@param p_lpStartAddress : [in] The function that will be threaded
		 *@param p_lpParameter : [in] An optional parameter to the given function
		 *@param p_lpThreadAttributes : [in] The thread attributes
		 *@param p_dwStackSize : [in] The thread max stack size
		 *@param p_eCreationFlags : [in] The initial state of the thread
		 */
		void CreateThread( PUIThreadStartFunction p_lpStartAddress, void * p_lpParameter, void * p_lpThreadAttributes = nullptr, size_t p_dwStackSize = 0, eTHREAD_STATE p_eCreationFlags = eTHREAD_STATE_SUSPENDED);
		/**
		 * Waits for the thread to end
		 *@param p_ulMilliseconds : [in] The maximum time to wait, in milliseconds
		 *@return true if the max time has not been reached before end
		 */
		bool Wait( unsigned long p_ulMilliseconds);
		/**
		 * Kills the thread
		 */
		void Kill();
		/**@name Accessors */
		//@{
		inline boost::thread	*	GetHandle	()const	{ return m_pThread; }
		inline unsigned long		GetID		()const	{ return m_dwID; }
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
	Thread * CreateThread( PDWThreadStartFunction p_threadFunction, void * p_lpParameter, void * p_lpThreadAttributes = nullptr, size_t p_dwStackSize = 0, eTHREAD_STATE p_eCreationFlags = eTHREAD_STATE_SUSPENDED, unsigned long * p_lpThreadId = nullptr);
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
	Thread * CreateThread( PUIThreadStartFunction p_threadFunction, void * p_lpParameter, void * p_lpThreadAttributes = nullptr, size_t p_dwStackSize = 0, eTHREAD_STATE p_eCreationFlags = eTHREAD_STATE_SUSPENDED, unsigned long * p_lpThreadId = nullptr);
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
