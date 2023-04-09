/* See LICENSE file in root folder */
#ifndef ___CastorTest_TestPrerequisites___
#define ___CastorTest_TestPrerequisites___

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#if defined( _WIN32 )
#	include <process.h>
#else
#	include <sys/types.h>
#	include <unistd.h>
#endif

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

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

	using BenchCasePtr = std::unique_ptr< BenchCase >;

	using TestCasePtr = std::unique_ptr< TestCase >;
}

#endif
