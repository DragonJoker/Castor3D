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
#ifndef ___GENERAL_LIB_MUTEX_H___
#define ___GENERAL_LIB_MUTEX_H___

#include "MultiThreadConfig.h"

#if GENLIB_MT_USE_STL

#include <mutex>

namespace General
{
	namespace MultiThreading
	{
		typedef std::recursive_mutex RecursiveMutex;
		typedef std::mutex Mutex;
		typedef std::unique_lock< RecursiveMutex > RecursiveMutexLock;
		typedef std::unique_lock< Mutex > MutexLock;
	}
}

#	define GENLIB_AUTO_SCOPED_LOCK()				std::unique_lock< std::mutex > scoped_lock_m_mutex( m_mutex)
#	define GENLIB_SCOPED_LOCK( p_mutex)				std::unique_lock< std::mutex > scoped_lock_ ## p_mutex( p_mutex)
#	define GENLIB_SCOPED_RECURSIVE_LOCK( p_mutex)	std::unique_lock< std::recursive_mutex> scoped_lock_ ## p_mutex( p_mutex)

#	define GENLIB_LOCK_MUTEX( p_mutex)				(p_mutex).lock()
#	define GENLIB_UNLOCK_MUTEX( p_mutex)			(p_mutex).unlock()
#	define GENLIB_RECURSIVE_LOCK_MUTEX( p_mutex)	(p_mutex).lock()
#	define GENLIB_RECURSIVE_UNLOCK_MUTEX( p_mutex)	(p_mutex).unlock()

#elif GENLIB_MT_USE_BOOST
#	include <boost/thread/recursive_mutex.hpp>
#	include <boost/thread/mutex.hpp>

namespace General
{ namespace MultiThreading
{
	typedef boost::recursive_mutex RecursiveMutex;
	typedef boost::mutex Mutex;
}
}

#	define GENLIB_AUTO_SCOPED_LOCK()				boost::mutex::scoped_lock scoped_lock_m_mutex( m_mutex)
#	define GENLIB_SCOPED_LOCK( p_mutex)				boost::mutex::scoped_lock scoped_lock_ ## p_mutex( p_mutex)
#	define GENLIB_SCOPED_RECURSIVE_LOCK( p_mutex)	boost::recursive_mutex::scoped_lock scoped_lock_ ## p_mutex( p_mutex)

#	if BOOST_VERSION >= 103500

#		define GENLIB_LOCK_MUTEX( p_mutex)				(p_mutex).lock()
#		define GENLIB_UNLOCK_MUTEX( p_mutex)			(p_mutex).unlock()
#		define GENLIB_RECURSIVE_LOCK_MUTEX( p_mutex)	(p_mutex).lock()
#		define GENLIB_RECURSIVE_UNLOCK_MUTEX( p_mutex)	(p_mutex).unlock()

#	else

#		define GENLIB_LOCK_MUTEX( p_mutex)					boost::detail::thread::lock_ops <Mutex> ::lock( p_mutex)
#		define GENLIB_UNLOCK_MUTEX( p_mutex)				boost::detail::thread::lock_ops <Mutex> ::unlock( p_mutex)
#		define GENLIB_RECURSIVE_LOCK_MUTEX( p_mutex)		boost::detail::thread::lock_ops <RecursiveMutex> ::lock( p_mutex)
#		define GENLIB_RECURSIVE_UNLOCK_MUTEX( p_mutex)	boost::detail::thread::lock_ops <RecursiveMutex> ::unlock( p_mutex)

#	endif

#elif GENLIB_MT_USE_MFC && ! defined( __GNUG__)

#	ifndef WINVER
#		define WINVER 0x0501
#	endif

#	include <afxmt.h>

namespace General
{ namespace MultiThreading
{
	typedef CMutex Mutex;

	class MutexScopedLock
	{
	private:
		Mutex & m_mutex;
		bool m_locked;

	public:
		MutexScopedLock( Mutex & p_mutex, bool p_startLocked = true);
		~MutexScopedLock();

		void Lock();
		void Unlock();

		bool IsLocked()const			{ return m_locked; }
		operator const void *()const	{ return m_locked ? this : 0; }
	};
}
}

#	define GENLIB_LOCK_MUTEX( p_mutex)	p_mutex.Lock()
#	define GENLIB_UNLOCK_MUTEX( p_mutex)	p_mutex.Unlock()
#	define GENLIB_SCOPED_LOCK( p_mutex)	MutexScopedLock scoped_lock_ ## p_mutex( p_mutex)

#endif  //GENLIB_MT_USE_BOOST || GENLIB_MT_USE_MFC

#endif
