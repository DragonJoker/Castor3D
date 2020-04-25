/* See LICENSE file in root folder */
#ifndef ___CastorTest_Benchmark___
#define ___CastorTest_Benchmark___

#include "CastorTestPrerequisites.hpp"

#include <chrono>

namespace Testing
{
	///
	/// \func doNotOptimizeAway
	///
	/// From Andrei Alexandrescu
	///
	template< class T >
	static inline void doNotOptimizeAway( T && datum )
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
		inline std::string const & getSummary()const
		{
			return m_summary;
		}

	protected:
		void doBench( std::string p_name, CallbackBench p_bench, uint64_t p_ui64Calls );

	private:
		using clock = std::chrono::high_resolution_clock;
		clock::time_point m_saved;
		std::string m_name;
		std::chrono::nanoseconds m_cumulativeTimes{};
		uint64_t m_totalExecutions;
		std::string m_summary;
	};

#	define BENCHMARK( Name, Calls ) doBench( #Name, [&](){ Name(); }, Calls )
}

#endif
