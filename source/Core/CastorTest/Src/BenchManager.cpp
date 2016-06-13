#include "BenchManager.hpp"
#include "Benchmark.hpp"
#include "UnitTest.hpp"

#include <algorithm>

using namespace Castor;

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
		Logger::LogInfo( cuT( "*********************************************************************************************" ) );
		StringStream l_benchSep;
		l_benchSep.width( BENCH_TITLE_WIDTH );
		l_benchSep.fill( cuT( '*' ) );
		l_benchSep << cuT( '*' );

		if ( m_arrayBenchs.size() )
		{
			Logger::LogInfo( cuT( "\nBenchmarks - Begin\n" ) );
			Logger::LogInfo( l_benchSep );

			for ( auto l_bench : m_arrayBenchs )
			{
				l_bench->Execute();
			}

			Logger::LogInfo( cuT( "\nBenchmarks - End\n" ) );
		}
		else
		{
			Logger::LogInfo( cuT( "\nNo bench\n" ) );
		}

		Logger::LogInfo( cuT( "*********************************************************************************************" ) );
	}

	void BenchManager::BenchsSummary()
	{
		StringStream l_benchSep;
		l_benchSep.width( BENCH_TITLE_WIDTH );
		l_benchSep.fill( cuT( '*' ) );
		l_benchSep << cuT( '*' );
		Logger::LogInfo( l_benchSep );

		for ( auto l_bench : m_arrayBenchs )
		{
			Logger::LogInfo( l_bench->GetSummary().c_str() );
		}
	}

	uint32_t BenchManager::ExecuteTests()
	{
		uint32_t l_errCount = 0;
		Logger::LogInfo( cuT( "*********************************************************************************************" ) );
		StringStream l_testSep;
		l_testSep.width( BENCH_TITLE_WIDTH );
		l_testSep.fill( cuT( '*' ) );
		l_testSep << cuT( '*' );

		if ( m_arrayTests.size() )
		{
			Logger::LogInfo( cuT( "\nTests - Begin\n" ) );
			Logger::LogInfo( l_testSep );
			uint32_t l_testCount = 0;

			for ( auto l_testCase : m_arrayTests )
			{
				l_testCase->Execute( l_errCount, l_testCount );
				Logger::LogInfo( l_testSep );
			}

			if ( l_errCount )
			{
				Logger::LogInfo( StringStream() << ( "\nTests - End, " ) << l_errCount << cuT( " errors detected out of " ) << l_testCount << cuT( " tests\n" ) );
			}
			else
			{
				Logger::LogInfo( StringStream() << cuT( "\nTests - End, no error detected out of " ) << l_testCount << cuT( " tests\n" ) );
			}
		}
		else
		{
			Logger::LogInfo( cuT(	"\nNo test\n" ) );
		}

		Logger::LogInfo( cuT(	"*********************************************************************************************" ) );
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
