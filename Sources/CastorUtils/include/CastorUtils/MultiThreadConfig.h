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
#ifndef ___MULTITHREAD_CONFIG_H___
#define ___MULTITHREAD_CONFIG_H___

//! Tells whether or not to use boost
#ifndef GENLIB_MT_USE_BOOST
#	define GENLIB_MT_USE_BOOST 1
#endif

//! Tells whether or not to use MFC
#ifndef GENLIB_MT_USE_MFC
#	define GENLIB_MT_USE_MFC 0
#endif

//! Tells whether or not to use a mutex manager (for debug only, it costs a lot of time, but is useful to detect locks or cross-locks)
#ifndef USE_MUTEX_MANAGER
#	define USE_MUTEX_MANAGER 0
#endif

//! Defines the max boost function arguments number
#ifndef BOOST_FUNCTION_MAX_ARGS
#	define BOOST_FUNCTION_MAX_ARGS 2
#endif

#if ! GENLIB_MT_USE_MFC && ! GENLIB_MT_USE_BOOST
#	error Define either GENLIB_MT_USE_MFC or GENLIB_MT_USE_BOOST
#elif GENLIB_MT_USE_BOOST && GENLIB_MT_USE_MFC
#	error Definition of both GENLIB_MT_USE_MFC and GENLIB_MT_USE_BOOST. Choose only one
#elif GENLIB_MT_USE_BOOST
#	define RECURSIVE_MUTEXES_AVAILABLE 1
#else
#	define RECURSIVE_MUTEXES_AVAILABLE 0
#endif

#if USE_MUTEX_MANAGER
#	include "MutexManager.h"
#endif

#endif
