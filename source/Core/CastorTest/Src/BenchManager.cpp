#include "BenchManager.hpp"
#include "Benchmark.hpp"
#include "UnitTest.hpp"

#include <algorithm>

namespace Testing
{
	std::vector< BenchCaseUPtr > BenchManager::m_benchs;
	std::vector< TestCaseUPtr > BenchManager::m_cases;

	BenchManager::BenchManager()
	{
	}

	BenchManager::~BenchManager()
	{
		m_benchs.clear();
		m_cases.clear();
	}

	void BenchManager::Register( BenchCaseUPtr p_bench )
	{
		m_benchs.push_back( std::move( p_bench ) );
	}

	void BenchManager::Register( TestCaseUPtr p_case )
	{
		p_case->RegisterTests();
		m_cases.push_back( std::move( p_case ) );
	}

	void BenchManager::ExecuteBenchs()
	{
		std::cout << "*********************************************************************************************" << std::endl;
		std::stringstream l_benchSep;
		l_benchSep.width( BENCH_TITLE_WIDTH );
		l_benchSep.fill( '*' );
		l_benchSep << '*';

		if ( m_benchs.size() )
		{
			std::cout << std::endl;
			std::cout << "Benchmarks - Begin" << std::endl;
			std::cout << l_benchSep.str() << std::endl;

			for ( auto & l_bench : m_benchs )
			{
				l_bench->Execute();
			}

			std::cout << std::endl;
			std::cout << "Benchmarks - End" << std::endl;
		}
		else
		{
			std::cout << std::endl;
			std::cout << "No bench" << std::endl;
		}

		std::cout << "*********************************************************************************************" << std::endl;
	}

	void BenchManager::BenchsSummary()
	{
		std::stringstream l_benchSep;
		l_benchSep.width( BENCH_TITLE_WIDTH );
		l_benchSep.fill( '*' );
		l_benchSep << '*';
		std::cout << l_benchSep.str() << std::endl;

		for ( auto & l_bench : m_benchs )
		{
			std::cout << l_bench->GetSummary().c_str() << std::endl;
		}
	}

	uint32_t BenchManager::ExecuteTests()
	{
		uint32_t l_errCount = 0;
		std::cout << "*********************************************************************************************" << std::endl;
		std::stringstream l_testSep;
		l_testSep.width( BENCH_TITLE_WIDTH );
		l_testSep.fill( '*' );
		l_testSep << '*';

		if ( m_cases.size() )
		{
			std::cout << std::endl;
			std::cout << "Tests - Begin" << std::endl;
			std::cout << l_testSep.str() << std::endl;
			uint32_t l_testCount = 0;

			for ( auto & l_testCase : m_cases )
			{
				l_testCase->Execute( l_errCount, l_testCount );
				std::cout << l_testSep.str() << std::endl;
			}

			if ( l_errCount )
			{
				std::cout << std::endl;
				std::cout << "Tests - End, " << l_errCount << " errors detected out of " << l_testCount << " tests" << std::endl;
			}
			else
			{
				std::cout << std::endl;
				std::cout << "Tests - End, no error detected out of " << l_testCount << " tests" << std::endl;
			}
		}
		else
		{
			std::cout << std::endl;
			std::cout << "No test" << std::endl;
		}

		std::cout << "*********************************************************************************************" << std::endl;
		return l_errCount;
	}

	//*************************************************************************************************

	bool Register( BenchCaseUPtr p_bench )
	{
		BenchManager::Register( std::move( p_bench ) );
		return true;
	}

	bool Register( TestCaseUPtr p_case )
	{
		BenchManager::Register( std::move( p_case ) );
		return true;
	}
}
