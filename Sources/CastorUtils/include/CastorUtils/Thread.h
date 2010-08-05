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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GENERAL_LIB_THREAD_H___
#define ___GENERAL_LIB_THREAD_H___

#include "MultiThreadConfig.h"

#if GENLIB_MT_USE_STL

#include <functional>
#include <thread>

namespace General
{
	namespace MultiThreading
	{
		//! to ease and uniformise the write of a thread
		typedef std::thread Thread;

#	ifndef GENLIB_THREAD_INIT
#		define GENLIB_THREAD_INIT()
#	endif

		//! Pointer over a class function
		template <class TClass>
		class ClassFunctor
		{
		public:
			TClass * m_instance;
			void( TClass::* m_function )( );

		public:
			ClassFunctor( TClass * p_instance, void( TClass::* p_function )( ) )
				: m_instance( p_instance ),
				m_function( p_function )
			{
			}
			void operator()()const
			{
				GENLIB_THREAD_INIT();
				( m_instance->*m_function )( );
			}
		};
	}
}

//! Creates a ClassFunctor
#	define GENLIB_THREAD_CLASS_FUNCTOR( p_instance, p_class, p_function)		General :: MultiThreading :: ClassFunctor <p_class> ( p_instance, & p_class::p_function)
//! Declares the BeginThread class function (to put in a class declaration)
#	define GENLIB_THREAD_DECLARE_THREAD_FUNC									void BeginThread()
//! Defines the BeginThread class function
#	define GENLIB_THREAD_DEFINE_THREAD_FUNC( p_class)							void p_class::BeginThread()
//! Creates a threaded class
#	define GENLIB_THREAD_CREATE_CLASS_THREAD( p_class, p_instance)				new General :: MultiThreading :: Thread( GENLIB_THREAD_CLASS_FUNCTOR( p_instance, p_class, BeginThread))
//! Creates a memeber threaded function
#	define GENLIB_THREAD_CREATE_MEMBER_FUNC_THREAD( p_class, p_instance, p_func)new General :: MultiThreading :: Thread( GENLIB_THREAD_CLASS_FUNCTOR( p_instance, p_class, p_func))
//! Deletes a thread
#	define GENLIB_THREAD_DELETE_THREAD( p_thread)								delete p_thread
//! Waits for a thread end
#	define GENLIB_THREAD_WAIT_FOR_END_OF( p_thread)								p_thread->join()

#	define GENLIB_AUTO_SHARED_MUTEX mutable std::recursive_mutex *				m_mutex;
#	define GENLIB_SET_AUTO_SHARED_MUTEX_NULL									m_mutex = NULL;
#	define GENLIB_NEW_AUTO_SHARED_MUTEX											m_mutex = new std::recursive_mutex();
#	define GENLIB_MUTEX_CONDITIONAL( mutex)										if (mutex)

#elif GENLIB_MT_USE_BOOST || defined( __GNUG__)

#include <boost/function.hpp>
#include <boost/thread/thread.hpp>

namespace General
{ namespace MultiThreading
{
	//! to ease and uniformise the write of a thread
	typedef boost::thread Thread;

#	ifndef GENLIB_THREAD_INIT
#		define GENLIB_THREAD_INIT()
#	endif

	//! Pointer over a class function
	template <class TClass>
	class ClassFunctor
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
			GENLIB_THREAD_INIT();
			(m_instance->* m_function)();
		}
	};
}
}

//! Creates a ClassFunctor
#	define GENLIB_THREAD_CLASS_FUNCTOR( p_instance, p_class, p_function)		General :: MultiThreading :: ClassFunctor <p_class> ( p_instance, & p_class::p_function)
//! Declares the BeginThread class function (to put in a class declaration)
#	define GENLIB_THREAD_DECLARE_THREAD_FUNC									void BeginThread()
//! Defines the BeginThread class function
#	define GENLIB_THREAD_DEFINE_THREAD_FUNC( p_class)							void p_class::BeginThread()
//! Creates a threaded class
#	define GENLIB_THREAD_CREATE_CLASS_THREAD( p_class, p_instance)				new General :: MultiThreading :: Thread( GENLIB_THREAD_CLASS_FUNCTOR( p_instance, p_class, BeginThread))
//! Creates a memeber threaded function
#	define GENLIB_THREAD_CREATE_MEMBER_FUNC_THREAD( p_class, p_instance, p_func)new General :: MultiThreading :: Thread( GENLIB_THREAD_CLASS_FUNCTOR( p_instance, p_class, p_func))
//! Deletes a thread
#	define GENLIB_THREAD_DELETE_THREAD( p_thread)								delete p_thread
//! Waits for a thread end
#	define GENLIB_THREAD_WAIT_FOR_END_OF( p_thread)								p_thread->join()

#	define GENLIB_AUTO_SHARED_MUTEX mutable boost::recursive_mutex *			m_mutex;
#	define GENLIB_SET_AUTO_SHARED_MUTEX_NULL									m_mutex = NULL;
#	define GENLIB_NEW_AUTO_SHARED_MUTEX											m_mutex = new boost::recursive_mutex();
#	define GENLIB_MUTEX_CONDITIONAL( mutex)										if (mutex)

#elif GENLIB_MT_USE_MFC

#	ifndef WINVER
#		define WINVER 0x0501
#	endif

#	include <afxwin.h>

namespace General
{ namespace MultiThreading
{
	//! to ease and uniformise the write of a thread
	typedef CWinThread Thread;
}
}

//! Declares the BeginThread class function (to put in a class declaration)
#	define GENLIB_THREAD_DECLARE_THREAD_FUNC						static unsigned int BeginThread( void * p_instance)
//! Defines the BeginThread class function
#	define GENLIB_THREAD_DEFINE_THREAD_FUNC( p_class)				unsigned int  p_class::BeginThread ( void * p_instance)
//! Creates a threaded class
#	define GENLIB_THREAD_CREATE_CLASS_THREAD( p_class, p_instance)	AfxBeginThread( p_class::BeginThread, static_cast <void *> ( p_instance), 0, 0, 0, 0)
//! Creates a threaded function
#	define GENLIB_THREAD_CREATE_FUNC_THREAD( p_func)				AfxBeginThread( p_func, 0, 0, 0, 0, 0)
//! Deletes a thread
#	define GENLIB_THREAD_DELETE_THREAD( p_thread)					delete ( p_thread)

#endif

#endif
