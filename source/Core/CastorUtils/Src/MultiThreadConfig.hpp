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
#ifndef ___CASTOR_MULTITHREAD_CONFIG_H___
#define ___CASTOR_MULTITHREAD_CONFIG_H___

#include "Macros.hpp"

#if CASTOR_HAS_STDTHREAD
#	include <thread>
#	include <mutex>
#	include <condition_variable>
#else
#	include <boost/thread/thread.hpp>
#	include <boost/thread/recursive_mutex.hpp>
#	include <boost/thread/mutex.hpp>
#	include <boost/thread/condition_variable.hpp>
namespace std
{
	using boost::thread;
	using boost::mutex;
	using boost::recursive_mutex;
	using boost::unique_lock;
	using boost::condition_variable;
	using namespace boost;
}
#endif

#define CASTOR_MUTEX_AUTO_SCOPED_LOCK() std::unique_lock< std::mutex > l_mutexAutoLock( m_mutex );
#define CASTOR_MUTEX_SCOPED_LOCK( p_mutex ) std::unique_lock< std::mutex > l_mutexLock( p_mutex );

#define CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK() std::unique_lock< std::recursive_mutex > l_recMutexAutoLock( m_mutex );
#define CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_mutex ) std::unique_lock< std::recursive_mutex > l_recMutexLock( p_mutex );

#endif
