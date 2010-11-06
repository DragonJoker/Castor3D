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

#	if CASTOR_MT_USE_BOOST
#		include <boost/thread/recursive_mutex.hpp>
#		include <boost/thread/mutex.hpp>
#	endif

namespace Castor
{ namespace MultiThreading
{
	template <class Locker>
	class ScopedLock
	{
	private:
		const Locker & m_locker;
		bool m_bLocked;

	public:
		ScopedLock( const Locker & p_bLocker, bool p_bStartLocked = true)
			:	m_locker	( p_bLocker),
				m_bLocked	( false)
		{
			if (p_bStartLocked)
			{
				Lock();
			}
		}
		~ScopedLock()
		{
			if (m_bLocked)
			{
				Unlock();
			}
		}
		void Lock()
		{
			m_locker.Lock();
			m_bLocked = true;
		}
		bool TryLock()
		{
			m_bLocked = m_locker.TryLock()
			return m_bLocked;
		}
		bool TimeLock( unsigned long p_ulTimeOutMilliseconds)
		{
			m_bLocked = m_locker.TimeLock( p_ulTimeOutMilliseconds)
			return m_bLocked;
		}
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

	class Mutex
	{
	private:
		mutable void * m_pSemaphore;

	public:
		Mutex();
		~Mutex();

		void Lock()const;
		bool TryLock()const;
		bool TimeLock( unsigned long p_ulTimeOutMilliseconds)const;
		void Unlock()const;

		typedef ScopedLock<Mutex> ScopedLock;
	};

	class RecursiveMutex
	{
	private:
		mutable void * m_pMutex;

	public:
		RecursiveMutex();
		~RecursiveMutex();

		void Lock()const;
		bool TryLock()const;
		bool TimeLock( unsigned long p_ulTimeOutMilliseconds)const;
		void Unlock()const;

		typedef ScopedLock<RecursiveMutex> ScopedLock;
	};
/*
	class CriticalSection
	{
	private:
		mutable void * m_pSection;

	public:
		CriticalSection();
		~CriticalSection();

		void Lock()const;
		bool TryLock()const;
		bool TimeLock( unsigned long p_ulTimeOutMilliseconds)const;
		void Unlock()const;

		typedef ScopedLock<CriticalSection> ScopedLock;
	};
*/
}
}

#	define CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_mutex)	Castor::MultiThreading::RecursiveMutex::ScopedLock l_scopedLockPRMutex( p_mutex)
#	define CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK()	Castor::MultiThreading::RecursiveMutex::ScopedLock l_scopedLockMRMutex( m_mutex)
#	define CASTOR_MUTEX_SCOPED_LOCK( p_mutex)			Castor::MultiThreading::Mutex::ScopedLock l_scopedLockPMutex( p_mutex)
#	define CASTOR_MUTEX_AUTO_SCOPED_LOCK()				Castor::MultiThreading::Mutex::ScopedLock l_scopedLockMMutex( m_mutex)
/*
#	define CASTOR_SECTION_SCOPED_LOCK( p_section)		Castor::MultiThreading::CriticalSection::ScopedLock l_scopedLockPSection( p_section)
#	define CASTOR_SECTION_AUTO_SCOPED_LOCK()			Castor::MultiThreading::CriticalSection::ScopedLock l_scopedLockMSection( m_criticalSection)
*/
#	define CASTOR_SCOPED_LOCK( p_locker)				Locker::ScopedLock l_scopedLockPLocker( p_locker)
#	define CASTOR_AUTO_SCOPED_LOCK()					Locker::ScopedLock l_scopedLockMLocker( m_locker)

#endif
