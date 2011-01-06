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
#ifndef ___Castor_MultiThreadConfig___
#define ___Castor_MultiThreadConfig___

#include "config.h"

#if CASTOR_USE_MULTITHREADING

//! Tells whether or not to use boost
#	ifndef CASTOR_MT_USE_BOOST
#		define CASTOR_MT_USE_BOOST 0
#	endif

//! Tells whether or not to use MFC
#	ifndef CASTOR_MT_USE_MFC
#		define CASTOR_MT_USE_MFC 0
#	endif

//! Tells whether or not to use a mutex manager (for debug only, it costs a lot of time, but is useful to detect locks or cross-locks)
#	ifndef CASTOR_USE_MUTEX_MANAGER
#		define CASTOR_USE_MUTEX_MANAGER 0
#	endif

//! Defines the max boost function arguments number
#	ifndef CASTOR_BOOST_FUNCTION_MAX_ARGS
#		define CASTOR_BOOST_FUNCTION_MAX_ARGS 2
#	endif

//! If none is defined and under Win32, use Win32
#	if ! CASTOR_MT_USE_WIN32 && ! CASTOR_MT_USE_BOOST && ! CASTOR_MT_USE_MFC && defined _WIN32
#		define CASTOR_MT_USE_WIN32 1
#	else
#		define CASTOR_MT_USE_WIN32 0
#	endif

//! If both are defined there is an error
#	if CASTOR_MT_USE_BOOST && CASTOR_MT_USE_MFC
#		error Definition of CASTOR_MT_USE_MFC and CASTOR_MT_USE_BOOST. Choose only one
#	endif

#	define CASTOR_RECURSIVE_MUTEXES_AVAILABLE 1

#	if CASTOR_USE_MUTEX_MANAGER
#		include "MutexManager.h"
#	endif

#else

#	undef CASTOR_MT_USE_BOOST
#	define CASTOR_MT_USE_BOOST 0

#	undef CASTOR_MT_USE_MFC
#	define CASTOR_MT_USE_MFC 0

#	undef CASTOR_USE_MUTEX_MANAGER
#	define CASTOR_USE_MUTEX_MANAGER 0

#	undef CASTOR_BOOST_FUNCTION_MAX_ARGS
#	define CASTOR_BOOST_FUNCTION_MAX_ARGS 2

#	undef CASTOR_MT_USE_WIN32
#	define CASTOR_MT_USE_WIN32 0

#	define CASTOR_RECURSIVE_MUTEXES_AVAILABLE 0

#endif //CASTOR_USE_MULTITHREADING

#endif
