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

	void BenchManager::registerType( BenchCaseUPtr p_bench )
	{
		m_benchs.push_back( std::move( p_bench ) );
	}

	void BenchManager::registerType( TestCaseUPtr p_case )
	{
		p_case->registerTests();
		m_cases.push_back( std::move( p_case ) );
	}

	void BenchManager::ExecuteBenchs()
	{
		std::cout << "*********************************************************************************************" << std::endl;
		std::stringstream benchSep;
		benchSep.width( BENCH_TITLE_WIDTH );
		benchSep.fill( '*' );
		benchSep << '*';

		if ( m_benchs.size() )
		{
			std::cout << std::endl;
			std::cout << "Benchmarks - Begin" << std::endl;
			std::cout << benchSep.str() << std::endl;

			for ( auto & bench : m_benchs )
			{
				bench->Execute();
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
		std::stringstream benchSep;
		benchSep.width( BENCH_TITLE_WIDTH );
		benchSep.fill( '*' );
		benchSep << '*';
		std::cout << benchSep.str() << std::endl;

		for ( auto & bench : m_benchs )
		{
			std::cout << bench->getSummary().c_str() << std::endl;
		}
	}

	uint32_t BenchManager::ExecuteTests()
	{
		uint32_t errCount = 0;
		std::cout << "*********************************************************************************************" << std::endl;
		std::stringstream testSep;
		testSep.width( BENCH_TITLE_WIDTH );
		testSep.fill( '*' );
		testSep << '*';

		if ( m_cases.size() )
		{
			std::cout << std::endl;
			std::cout << "Tests - Begin" << std::endl;
			std::cout << testSep.str() << std::endl;
			uint32_t testCount = 0;

			for ( auto & testCase : m_cases )
			{
				testCase->execute( errCount, testCount );
				std::cout << testSep.str() << std::endl;
			}

			if ( errCount )
			{
				std::cout << std::endl;
				std::cout << "Tests - End, " << errCount << " errors detected out of " << testCount << " tests" << std::endl;
			}
			else
			{
				std::cout << std::endl;
				std::cout << "Tests - End, no error detected out of " << testCount << " tests" << std::endl;
			}
		}
		else
		{
			std::cout << std::endl;
			std::cout << "No test" << std::endl;
		}

		std::cout << "*********************************************************************************************" << std::endl;
		return errCount;
	}

	//*************************************************************************************************

	bool registerType( BenchCaseUPtr p_bench )
	{
		BenchManager::registerType( std::move( p_bench ) );
		return true;
	}

	bool registerType( TestCaseUPtr p_case )
	{
		BenchManager::registerType( std::move( p_case ) );
		return true;
	}
}
