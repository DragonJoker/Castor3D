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
#ifndef ___Castor_MutexManager___
#define ___Castor_MutexManager___

#include "ThreadSpecific.hpp"
#include "Mutex.hpp"

#include <map>

#define CASTOR_THREAD_INIT() Castor::MultiThreading::MutexManager::GetSingleton().RegisterThread()

namespace Castor
{ namespace MultiThreading
{
	//! Lock result enumerator
	/*!
	Enumerates all lock results
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	enum LockResult
	{
		LR_SUCCESS,					//!< lock successful
		LR_FAILURE_DOUBLE_LOCK,		//!< lock failed : double lock
		LR_FAILURE_DEADLOCK,		//!< lock failed : dead lock
		LR_FAILURE_NOT_LOCKED,		//!< lock failed : cannot be locked
		LR_FAILURE_WRONG_THREAD		//!< lock failed : wrong thread tries to lock
	};

	//! Defines the mutex state, to check it with the MutexManager
	class MutexState : public MemoryTraced<MutexState>
	{
	private:
		Mutex const * m_mutexPtr;

	public:
		unsigned int m_lockCount;
		unsigned int m_ownerThreadID;
		bool m_recursive;

	public:
		MutexState( Mutex const * p_mutexPtr, unsigned int p_currentThread)
			:	m_mutexPtr		(p_mutexPtr),
				m_lockCount		(1),
				m_ownerThreadID	(p_currentThread)
		{
		}

		LockResult Lock( unsigned int p_currentThreadID)
		{
			if (m_lockCount == 0)
			{
				m_lockCount = 1;
				m_ownerThreadID = p_currentThreadID;
				return LR_SUCCESS;
			}

			if (p_currentThreadID == m_ownerThreadID)
			{
				m_lockCount ++;
				return LR_FAILURE_DOUBLE_LOCK;
			}

			return LR_FAILURE_DEADLOCK;
		}
		LockResult Unlock( unsigned int p_currentThreadID)
		{
			if (m_lockCount == 0)
			{
				return LR_FAILURE_NOT_LOCKED;
			}

			if (p_currentThreadID != m_ownerThreadID)
			{
				return LR_FAILURE_WRONG_THREAD;
			}

			m_lockCount --;
			return LR_SUCCESS;
		}
	};

	//! Mutex manager
	/*!
	Used to check locks and unlocks, to trace dead locks, cross locks and others
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class MutexManager : public AutoSingleton <MutexManager>
	{
	private:
		typedef std::map<Mutex const *, MutexState> MutexStateMap;

	public:
		ThreadSpecificUIntPtr	m_threadID;
		unsigned int			m_nextThreadID;
		Mutex					m_mutex;
		MutexStateMap			m_mutexeMap;

	public:
		MutexManager()
			: m_nextThreadID (0)
		{}
		~MutexManager(){}

	public:
		void RegisterThread()
		{
			boost::mutex::scoped_lock scoped_lock( m_mutex);

			m_threadID.reset( new unsigned int);
			(* m_threadID.get()) = m_nextThreadID ++;
		}

		bool SimpleLock( Mutex const * p_mutexPtr)
		{
			boost::mutex::scoped_lock scoped_lock( m_mutex);
			const MutexStateMap::iterator & ifind = m_mutexeMap.find( p_mutexPtr);

			if (ifind == m_mutexeMap.end())
			{
				m_mutexeMap.insert( MutexStateMap::value_type( p_mutexPtr, MutexState( p_mutexPtr, (* m_threadID.get()))));
				return true;
			}

			LockResult l_result = ifind->second.Lock( (* m_threadID.get()));

			if (l_result != LR_SUCCESS)
			{
				return false;
			}

			return true;
		}
		bool SimpleUnlock( Mutex const * p_mutexPtr)
		{
			boost::mutex::scoped_lock scoped_lock( m_mutex);
			const MutexStateMap::iterator & ifind = m_mutexeMap.find( p_mutexPtr);

			if (ifind == m_mutexeMap.end())
			{
				return false;
			}

			LockResult l_result = ifind->second.Unlock( (* m_threadID.get()));

			if (l_result != LR_SUCCESS)
			{
				return false;
			}

			return true;		
		}
	};

	//! Scoped lock for Mutex, using MutexManager to lock and unlock
	struct MutexLock
	{
	private:
		Mutex & m_mutex;
		bool m_dead;

	public:
		MutexLock( Mutex & p_mutex)
			: m_mutex (p_mutex)
		{
			if( MutexManager::GetSingleton().SimpleLock( & m_mutex))
			{
				m_dead = false;
				boost::detail::thread::lock_ops<Mutex>::lock( m_mutex);
			}
			else
			{
				m_dead = true;
			}
		}
		~MutexLock()
		{
			if ( ! m_dead)
			{
				if (MutexManager::GetSingleton().SimpleUnlock( & m_mutex))
				{
					boost::detail::thread::lock_ops <Mutex> ::unlock( m_mutex);
				}
			}
		}

	private:
		MutexLock( MutexLock const &);
	};
}
}

#ifdef CASTOR_SCOPED_LOCK
#	undef CASTOR_SCOPED_LOCK
#	undef CASTOR_SCOPED_RECURSIVE_LOCK
#	undef CASTOR_LOCK_MUTEX
#	undef CASTOR_UNLOCK_MUTEX
#endif

#define CASTOR_SCOPED_LOCK( p_mutex)			Castor::MultiThreading::MutexLock l_mutex_lock_ ## p_mutex( p_mutex)
#define CASTOR_SCOPED_RECURSIVE_LOCK( p_mutex)	Castor::MultiThreading::MutexLock l_mutex_lock_ ## p_mutex( p_mutex)
#define CASTOR_LOCK_MUTEX( p_mutex)				Castor::MultiThreading::MutexManager::GetSingleton().SimpleLock( & p_mutex)
#define CASTOR_UNLOCK_MUTEX( p_mutex)			Castor::MultiThreading::MutexManager::GetSingleton().SimpleUnlock( & p_mutex)

#endif
