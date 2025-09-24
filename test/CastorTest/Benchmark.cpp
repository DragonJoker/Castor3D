#include "Benchmark.hpp"

namespace Testing
{
	BenchCase::BenchCase( std::string const & name )
		: m_name( name )
		, m_totalExecutions( 0 )
	{
	}

	void BenchCase::doBench( std::string const & name, CallbackBench const & bench, uint64_t ui64Calls )
	{
		std::stringstream benchSep;
		benchSep.width( BENCH_TITLE_WIDTH );
		benchSep.fill( '*' );
		benchSep << '*';

		try
		{
			m_cumulativeTimes = std::chrono::nanoseconds{};
			m_totalExecutions = 0;

			for ( uint64_t i = 0; i < ui64Calls; i++ )
			{
				m_saved = clock::now();
				bench();
				m_cumulativeTimes += std::chrono::duration_cast< std::chrono::nanoseconds >( clock::now() - m_saved );
				m_totalExecutions++;
			}

			std::stringstream stream;
			stream.precision( 4 );
			stream << "*\t" << name << " global results :" << std::endl;
			stream << "*\t\t- Executed " << ui64Calls << " times" << std::endl;
			stream << "*\t\t- Total time : " << float( std::chrono::duration_cast< std::chrono::milliseconds >( m_cumulativeTimes ).count() ) / 1000.0 << "s" << std::endl;
			stream << "*\t\t- Average time : " << std::chrono::duration_cast< std::chrono::milliseconds >( m_cumulativeTimes / m_totalExecutions ).count() << "ms" << std::endl;
			stream << benchSep.rdbuf() << std::endl;
			m_summary += stream.str();
			std::cout << "*\tBench ended for: " << name.c_str() << std::endl;
			std::cout << "*\t\t- Executed " << ui64Calls << " times" << std::endl;
			std::cout << "*\t\t- Total time : " << float( std::chrono::duration_cast< std::chrono::milliseconds >( m_cumulativeTimes ).count() ) / 1000.0 << "s" << std::endl;
			std::cout << "*\t\t- Average time : " << std::chrono::duration_cast< std::chrono::milliseconds >( m_cumulativeTimes / m_totalExecutions ).count() << "ms" << std::endl;
			std::cout << benchSep.str() << std::endl;
		}
		catch ( ... )
		{
			std::cout << "*\tBench " << name.c_str() << " failed" << std::endl;
			std::cout << benchSep.str() << std::endl;
		}
	}
}
