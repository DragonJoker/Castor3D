/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CastorTest_TestPrerequisites___
#define ___CastorTest_TestPrerequisites___

#if defined( _WIN32 )
#	include <process.h>
#else
#	include <sys/types.h>
#	include <unistd.h>
#endif

#include <Miscellaneous/StringUtils.hpp>
#include <Log/Logger.hpp>
#include <Miscellaneous/PreciseTimer.hpp>

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
