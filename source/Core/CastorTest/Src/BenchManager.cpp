#include "BenchManager.hpp"
#include "Benchmark.hpp"
#include "UnitTest.hpp"

#include <algorithm>

namespace Testing
{
	std::vector< BenchCaseSPtr > BenchManager::m_arrayBenchs;
	std::vector< TestCaseSPtr > BenchManager::m_arrayTests;

	BenchManager::BenchManager()
	{
	}

	BenchManager::~BenchManager()
	{
		m_arrayBenchs.clear();
		m_arrayTests.clear();
	}

	void BenchManager::Register( BenchCaseSPtr p_pBench )
	{
		m_arrayBenchs.push_back( p_pBench );
	}

	void BenchManager::Register( TestCaseSPtr p_pCode )
	{
		p_pCode->RegisterTests();
		m_arrayTests.push_back( p_pCode );
	}

	void BenchManager::ExecuteBenchs()
	{
		std::cout << "*********************************************************************************************" << std::endl;
		std::stringstream l_benchSep;
		l_benchSep.width( BENCH_TITLE_WIDTH );
		l_benchSep.fill( '*' );
		l_benchSep << '*';

		if ( m_arrayBenchs.size() )
		{
			std::cout << std::endl;
			std::cout << "Benchmarks - Begin" << std::endl;
			std::cout << l_benchSep.str() << std::endl;

			for ( auto l_bench : m_arrayBenchs )
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

		for ( auto l_bench : m_arrayBenchs )
		{
			std::cout << l_bench->GetSummary ().c_str () << std::endl;
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

		if ( m_arrayTests.size() )
		{
			std::cout << std::endl;
			std::cout << "Tests - Begin" << std::endl;
			std::cout << l_testSep.str() << std::endl;
			uint32_t l_testCount = 0;

			for ( auto l_testCase : m_arrayTests )
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

	bool Register( BenchCaseSPtr p_pBenchmark )
	{
		BenchManager::Register( p_pBenchmark );
		return true;
	}

	bool Register( TestCaseSPtr p_pCode )
	{
		BenchManager::Register( p_pCode );
		return true;
	}
}
