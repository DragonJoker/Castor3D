#include "Benchmark.hpp"

namespace Testing
{
	BenchCase::BenchCase( std::string const & p_name )
		: m_name( p_name )
		, m_totalExecutions( 0 )
	{
	}

	BenchCase::~BenchCase()
	{
	}

	void BenchCase::DoBench( std::string p_name, CallbackBench p_bench, uint64_t p_ui64Calls )
	{
		std::stringstream benchSep;
		benchSep.width( BENCH_TITLE_WIDTH );
		benchSep.fill( '*' );
		benchSep << '*';

		try
		{
			m_cumulativeTimes = std::chrono::nanoseconds{};
			m_totalExecutions = 0;

			for ( uint64_t i = 0; i < p_ui64Calls; i++ )
			{
				m_saved = clock::now();
				p_bench();
				m_cumulativeTimes += std::chrono::duration_cast< std::chrono::nanoseconds >( clock::now() - m_saved );
				m_totalExecutions++;
			}

			std::stringstream stream;
			stream.precision( 4 );
			stream << "*	" << p_name << " global results :" << std::endl;
			stream << "*		- Executed " << p_ui64Calls << " times" << std::endl;
			stream << "*		- Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( m_cumulativeTimes ).count() / 1000.0 << "s" << std::endl;
			stream << "*		- Average time : " << std::chrono::duration_cast< std::chrono::milliseconds >( m_cumulativeTimes / m_totalExecutions ).count() << "ms" << std::endl;
			stream << benchSep.rdbuf() << std::endl;
			m_summary += stream.str();
			std::cout << "*	Bench ended for: " << p_name.c_str() << std::endl;
			std::cout << "*		- Executed " << p_ui64Calls << " times" << std::endl;
			std::cout << "*		- Total time : " << std::chrono::duration_cast< std::chrono::milliseconds >( m_cumulativeTimes ).count() / 1000.0 << "s" << std::endl;
			std::cout << "*		- Average time : " << std::chrono::duration_cast< std::chrono::milliseconds >( m_cumulativeTimes / m_totalExecutions ).count() << "ms" << std::endl;
			std::cout << benchSep.str() << std::endl;
		}
		catch ( ... )
		{
			std::cout << "*	Bench " << p_name.c_str() << " failed" << std::endl;
			std::cout << benchSep.str() << std::endl;
		}
	}
}
