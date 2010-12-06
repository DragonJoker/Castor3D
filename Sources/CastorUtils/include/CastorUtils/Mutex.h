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
#ifndef ___Castor_Mutex___
#define ___Castor_Mutex___

#include "MultiThreadConfig.h"
#include "Memory.h"

#	if CASTOR_MT_USE_BOOST
#		include <boost/thread/recursive_mutex.hpp>
#		include <boost/thread/mutex.hpp>
#	endif

namespace Castor
{ namespace MultiThreading
{
	/*!
	Class managing a lock/unlock of a mutex/recursive mutex inside a scope.
	It's the only way to lock/unlock a mutex/recursive mutex.
	This way of managing lockers allows to avoid most of dead lock cases
	template argument Locker is the class to manage (actually Mutex or RecursiveMutex)
	*/
	template <class Locker>
	class ScopedLock : public MemoryTraced< ScopedLock<Locker> >
	{
	private:
		const Locker & m_locker;
		bool m_bLocked;

	public:
		/**
		 * Constructor, from a locker
		 *@param p_locker : [in] The instance of Locker to lock
		 *@param p_bStartLocked : [in] Tells whether the constructor locks the locker, or not
		 */
		ScopedLock( const Locker & p_locker, bool p_bStartLocked = true)
			:	m_locker	( p_locker),
				m_bLocked	( false)
		{
			if (p_bStartLocked)
			{
				Lock();
			}
		}
		/**
		 * Destructor, unlocks the locker
		 */
		~ScopedLock()
		{
			if (m_bLocked)
			{
				Unlock();
			}
		}
		/**
		 * Locker lock function
		 */
		void Lock()
		{
			m_locker.Lock();
			m_bLocked = true;
		}
		/**
		 * Locker lock try function
		 */
		bool TryLock()
		{
			m_bLocked = m_locker.TryLock()
			return m_bLocked;
		}
		/**
		 * Locker timed lock
		 */
		bool TimeLock( unsigned long p_ulTimeOutMilliseconds)
		{
			m_bLocked = m_locker.TimeLock( p_ulTimeOutMilliseconds)
			return m_bLocked;
		}
		/**
		 * Locker unlocking function
		 */
		void Unlock()
		{
			if (m_bLocked)
			{
				m_bLocked = false;
				m_locker.Unlock();
			}
		}

		bool IsLocked()const			{ return m_bLocked; }
	};
#	define CASTOR_SCOPED_LOCK( p_locker)	Locker::ScopedLock l_scopedLockPLocker( p_locker)
#	define CASTOR_AUTO_SCOPED_LOCK()		Locker::ScopedLock l_scopedLockMLocker( m_locker)

	/*!
	Class used to restrict access of ScopedLock to lock and unlock functions
	*/
	template <class T>
	class Lockable
	{
	public:
		typedef ScopedLock< Lockable<T> > ScopedLock;

	private:
		friend class ScopedLock;

	private:
		virtual void Lock		()const											= 0;
		virtual bool TryLock	()const											= 0;
		virtual bool TimeLock	( unsigned long p_ulTimeOutMilliseconds)const	= 0;
		virtual void Unlock		()const											= 0;
	};

	//! Non reentrant mutex locker, can only be used with ScopedLock<Mutex>
	class Mutex : public Lockable<Mutex>, public MemoryTraced<Mutex>
	{
	private:
		mutable void * m_pSemaphore;

	public:
		/**
		 * Constructor
		 */
		Mutex();
		/**
		 * Destructor
		 */
		~Mutex();

	private:
		virtual void Lock()const;
		virtual bool TryLock()const;
		virtual bool TimeLock( unsigned long p_ulTimeOutMilliseconds)const;
		virtual void Unlock()const;
	};
#	define CASTOR_MUTEX_SCOPED_LOCK( p_mutex)			Castor::MultiThreading::Mutex::ScopedLock l_scopedLockPMutex( p_mutex)
#	define CASTOR_MUTEX_AUTO_SCOPED_LOCK()				Castor::MultiThreading::Mutex::ScopedLock l_scopedLockMMutex( m_mutex)

	//! Reentrant mutex locker, can only be used with ScopedLock<RecursiveMutex>
	class RecursiveMutex : public Lockable<RecursiveMutex>, public MemoryTraced<RecursiveMutex>
	{
	private:
		mutable void * m_pMutex;

	public:
		/**
		 * Constructor
		 */
		RecursiveMutex();
		/**
		 * Destructor
		 */
		~RecursiveMutex();

	private:
		virtual void Lock()const;
		virtual bool TryLock()const;
		virtual bool TimeLock( unsigned long p_ulTimeOutMilliseconds)const;
		virtual void Unlock()const;
	};
#	define CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_mutex)	Castor::MultiThreading::RecursiveMutex::ScopedLock l_scopedLockPRMutex( p_mutex)
#	define CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK()	Castor::MultiThreading::RecursiveMutex::ScopedLock l_scopedLockMRMutex( m_mutex)
/*
	//! Critical section locker, can only be used with ScopedLock<CriticalSection>
	class CriticalSection : public Lockable<CriticalSection>, public MemoryTraced<CriticalSection>
	{
	private:
		mutable void * m_pSection;

	public:
		CriticalSection();
		~CriticalSection();

	private:
		virtual void Lock()const;
		virtual bool TryLock()const;
		virtual bool TimeLock( unsigned long p_ulTimeOutMilliseconds)const;
		virtual void Unlock()const;
	};
#	define CASTOR_SECTION_SCOPED_LOCK( p_section)		Castor::MultiThreading::CriticalSection::ScopedLock l_scopedLockPSection( p_section)
#	define CASTOR_SECTION_AUTO_SCOPED_LOCK()			Castor::MultiThreading::CriticalSection::ScopedLock l_scopedLockMSection( m_criticalSection)
*/
}
}

#endif
