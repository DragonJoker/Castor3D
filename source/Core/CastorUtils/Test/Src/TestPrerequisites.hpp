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
#ifndef ___CUT_TestPrerequisites___
#define ___CUT_TestPrerequisites___

#if defined( _WIN32 )
#	include <process.h>
#else
#	include <sys/types.h>
#	include <unistd.h>
#endif

#include <StringUtils.hpp>
#include <Logger.hpp>
#include <PreciseTimer.hpp>

namespace Testing
{
#if defined( NDEBUG )
	static const uint32_t NB_TESTS = 10000000;
#else
	static const uint32_t NB_TESTS = 1000000;
#endif
	static const uint32_t BENCH_TITLE_WIDTH = 60;
	static const uint32_t TEST_TITLE_WIDTH = 60;

	class BenchCase;
	class TestCase;
	class BenchManager;

	DECLARE_SMART_PTR( BenchCase );
	DECLARE_SMART_PTR( TestCase );
}

#endif
