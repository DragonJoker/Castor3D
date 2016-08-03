#include "Benchmark.hpp"

namespace Testing
{
	BenchCase::BenchCase( std::string const & p_name )
		: m_name( p_name )
		, m_dCumulativeTimes( 0.0 )
		, m_uiTotalExecutions( 0 )
	{
	}

	BenchCase::~BenchCase()
	{
	}

	void BenchCase::DoBench( std::string p_name, CallbackBench p_bench, uint64_t p_ui64Calls )
	{
		std::stringstream l_benchSep;
		l_benchSep.width( BENCH_TITLE_WIDTH );
		l_benchSep.fill( '*' );
		l_benchSep << '*';

		try
		{
			m_dCumulativeTimes = 0;
			m_uiTotalExecutions = 0;

			for ( uint64_t i = 0; i < p_ui64Calls; i++ )
			{
				m_saved = clock::now();
				p_bench();
				m_dCumulativeTimes += std::chrono::duration_cast< std::chrono::milliseconds >( clock::now() - m_saved ).count() / 1000.0;
				m_uiTotalExecutions++;
			}

			std::stringstream l_stream;
			l_stream.precision( 4 );
			l_stream << "*	" << p_name << " global results :" << std::endl;
			l_stream << "*		- Executed " << p_ui64Calls << " times" << std::endl;
			l_stream << "*		- Total time : %" << m_dCumulativeTimes << "s" << std::endl;
			l_stream << "*		- Average time : %" << ( 1000.0 * m_dCumulativeTimes / m_uiTotalExecutions ) << "ms" << std::endl;
			l_stream << l_benchSep.rdbuf() << std::endl;
			m_strSummary += l_stream.str();
			std::cout << "*	Bench ended for: " << p_name.c_str() << std::endl;
			std::cout << "*		- Executed " << p_ui64Calls << " times" << std::endl;
			std::cout << "*		- Total time: " << m_dCumulativeTimes << "ms" << std::endl;
			std::cout << "*		- Average time: " << ( m_dCumulativeTimes / m_uiTotalExecutions ) << "ms" << std::endl;
			std::cout << l_benchSep.str() << std::endl;
		}
		catch ( ... )
		{
			std::cout << "*	Bench " << p_name.c_str() << " failed" << std::endl;
			std::cout << l_benchSep.str() << std::endl;
		}
	}
}
