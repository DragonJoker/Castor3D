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
#ifndef ___CastorTest_Benchmark___
#define ___CastorTest_Benchmark___

#include "CastorTestPrerequisites.hpp"

#include <chrono>

namespace Testing
{
	///
	/// \func DoNotOptimizeAway
	///
	/// From Andrei Alexandrescu
	///
	template< class T >
	static inline void DoNotOptimizeAway( T && datum )
	{
#if defined( _WIN32 )

		if ( _getpid() == 1 )
#else
		if ( getpid() == 1 )
#endif
		{
			const void * p = &datum;
			putchar( *static_cast< const char * >( p ) );
		}
	}

	class BenchCase
	{
		typedef std::function< void() > CallbackBench;

	public:
		explicit BenchCase( std::string const & p_name );
		virtual ~BenchCase();
		virtual void Execute() = 0;
		inline std::string const & GetSummary()const
		{
			return m_summary;
		}

	protected:
		void DoBench( std::string p_name, CallbackBench p_bench, uint64_t p_ui64Calls );

	private:
		using clock = std::chrono::high_resolution_clock;
		clock::time_point m_saved;
		std::string m_name;
		std::chrono::nanoseconds m_cumulativeTimes;
		uint64_t m_totalExecutions;
		std::string m_summary;
	};

#	define BENCHMARK( Name, Calls ) DoBench( #Name, [&](){ Name(); }, Calls )
}

#endif
